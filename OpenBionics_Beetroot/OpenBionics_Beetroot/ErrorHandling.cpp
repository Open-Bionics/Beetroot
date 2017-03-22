/*	Open Bionics - Beetroot
*	Author - Olly McBride
*	Date - October 2016
*
*	This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
*	To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
*
*	Website - http://www.openbionics.com/
*	GitHub - https://github.com/Open-Bionics
*	Email - ollymcbride@openbionics.com
*
*	ErrorHandling.cpp
*
*/

#include "Globals.h"
#include "ErrorHandling.h"

#include "I2C_EEPROM.h"
#include "LED.h"


////////////////////////////// Constructors/Destructors //////////////////////////////
ERROR_HANDLING::ERROR_HANDLING()
{
	// TODO: store all error descriptions is PROGMEM

	// NO ERROR
	_errorList[ERROR_NONE].num = 0;
	_errorList[ERROR_NONE].type = ERROR_NONE;
	_errorList[ERROR_NONE].level = LEVEL_NONE;
	_errorList[ERROR_NONE].LED.colour = LED_GREEN_DIM;
	_errorList[ERROR_NONE].LED.blinkFreq = 0.5;				// 0.5Hz

	// Error 001 - UNKNOWN ERROR
	_errorList[ERROR_UNKNOWN].num = 1;
	_errorList[ERROR_UNKNOWN].type = ERROR_UNKNOWN;
	_errorList[ERROR_UNKNOWN].level = LEVEL_ERROR;
	_errorList[ERROR_UNKNOWN].description = "Unknown error occurred";
	_errorList[ERROR_UNKNOWN].LED.colour = LED_YELLOW_DIM;

	// Error 002 - Initialisation sequence
	_errorList[ERROR_INIT].num = 2;
	_errorList[ERROR_INIT].type = ERROR_INIT;
	_errorList[ERROR_INIT].level = LEVEL_INFO;
	_errorList[ERROR_INIT].LED.colour = LED_ORANGE_DIM;
	_errorList[ERROR_INIT].LED.blinkFreq = 2.5;				// 2.5Hz

	// Error 003 - EEPROM fails to respond to ping()
	_errorList[ERROR_EEPROM_INIT].num = 3;
	_errorList[ERROR_EEPROM_INIT].type = ERROR_UNKNOWN;
	_errorList[ERROR_EEPROM_INIT].level = LEVEL_FATAL;
	_errorList[ERROR_EEPROM_INIT].description = "EEPROM is not detected during initialisation";
	_errorList[ERROR_EEPROM_INIT].LED.colour = LED_RED_DIM;

	// Error 004 -  Finger pins fail to initialise
	_errorList[ERROR_FINGER_INIT].num = 4;
	_errorList[ERROR_FINGER_INIT].type = ERROR_FINGER_INIT;
	_errorList[ERROR_FINGER_INIT].level = LEVEL_FATAL;
	_errorList[ERROR_FINGER_INIT].description = "Finger pins failed to initialise";
	_errorList[ERROR_FINGER_INIT].LED.colour = LED_AQUA;

	// Error 005 - EEPROM settings overflow
	_errorList[ERROR_EEPROM_SETTINGS].num = 5;
	_errorList[ERROR_EEPROM_SETTINGS].type = ERROR_EEPROM_SETTINGS;
	_errorList[ERROR_EEPROM_SETTINGS].level = LEVEL_FATAL;
	_errorList[ERROR_EEPROM_SETTINGS].description = "Not enough EEPROM space for board settings";
	_errorList[ERROR_EEPROM_SETTINGS].LED.colour = LED_PURPLE_DIM;

	// Error 006 - Serial buffer overflow
	_errorList[ERROR_S_BUFF_OVFLOW].num = 6;
	_errorList[ERROR_S_BUFF_OVFLOW].type = ERROR_S_BUFF_OVFLOW;
	_errorList[ERROR_S_BUFF_OVFLOW].level = LEVEL_WARN;
	_errorList[ERROR_S_BUFF_OVFLOW].description = "Serial buffer overflow";
	_errorList[ERROR_S_BUFF_OVFLOW].LED.colour = LED_BLUE_DIM;

	// Error 007 - Warning CPU temperature has been reached
	_errorList[ERROR_TEMP_WARNING].num = 7;
	_errorList[ERROR_TEMP_WARNING].type = ERROR_TEMP_WARNING;
	_errorList[ERROR_TEMP_WARNING].level = LEVEL_WARN;
	_errorList[ERROR_TEMP_WARNING].description = "CPU temperature is high";
	_errorList[ERROR_TEMP_WARNING].LED.colour = LED_PINK_DIM;

	// Error 008 - Maximum CPU temperature has been reached
	_errorList[ERROR_TEMP_MAX].num = 8;
	_errorList[ERROR_TEMP_MAX].type = ERROR_TEMP_MAX;
	_errorList[ERROR_TEMP_MAX].level = LEVEL_FATAL;
	_errorList[ERROR_TEMP_MAX].description = "CPU has reached maximum temperature";
	_errorList[ERROR_TEMP_MAX].LED.colour = LED_PINK_DIM;
	_errorList[ERROR_TEMP_MAX].LED.blinkFreq = 2.5;				// 2.5Hz

}


////////////////////////////// Public Methods //////////////////////////////

// initialise error handling and display any stored errors
void ERROR_HANDLING::begin(void)
{
	enableLEDcontrol();					// enable error handling to control the LED

	//_error = ERROR_NONE;				// clear the current error flag

	_currError = &_errorList[ERROR_NONE];
}

// set an error state and perform the appropriate actions
void ERROR_HANDLING::set(ErrorType error)
{
	//// if the new error is lower severity than the current error and is not being cleared, return
	//if ((_errorList[error].severity < _currError->severity) && (error != ERROR_NONE))
	//	return;

	// if the new error is the same as the current error, do not set again
	if (_currError->type == _errorList[error].type)
		return;

	// store error
	_currError = &_errorList[error];
	storeError(_currError->type);

	// if error handling has control of the NeoPixel
	if (_ctrlLED)
	{
		LED.setColour(_currError->LED.colour);				// set LED colour
		LED.setFadeFreq(_currError->LED.blinkFreq);			// set LED to be solid or to fade

		if (_currError->level == LEVEL_WARN)				// if error is a WARNING
			warnTimer.start(WARNING_DURATION);				// start timer to that the warning can be cleared
	}

	if(error != ERROR_NONE)
		printErrorDescr(_currError);

	// if the error is of high severity, halt the program
	if (_currError->level == LEVEL_FATAL)
	{
		for (int i = 0; i < 5; i++)
		{
			MYSERIAL_PRINTLN_PGM("******* Halting Program *******");
		}

		// disable all of the motors
		for (int i = 0; i < NUM_FINGERS; i++)
		{
			if(finger[i].attached())
				finger[i].disableMotor();
		}

		// halt program
		while (1);
	}
};

// clear the error state
void ERROR_HANDLING::clear(void)
{
	set(ERROR_NONE);
}

// clear any warnings after a set period and check for fatal errors
void ERROR_HANDLING::run(void)
{
	// if warning is set & duration has passed
	if (warnTimer.started() && warnTimer.finished() && (_currError->level == LEVEL_WARN))
	{
		LED.setPrev();		// set LED to previous colour
		clear();			// clear warning flag

		MYSERIAL.println("Clearing Warning");
	}
}

// get the current error state
ErrorType ERROR_HANDLING::get(void)
{
	return _currError->type;
}

// check whether a specific error state is set
bool ERROR_HANDLING::isSet(ErrorType error)
{
	return (error == _currError->type);
}

// print the current error state
void ERROR_HANDLING::printCurrent(void)
{
	if (_currError->type == ERROR_NONE)
	{
		MYSERIAL_PRINTLN_PGM("No Errors");
	}
	else
	{
		printErrorDescr(_currError);
	}
}

// check EEPROM for previous errors
void ERROR_HANDLING::checkPrevError(void)
{
	// if EEPROM is available, check for previous errors
	if (EEPROM.ping())
	{
		ErrorState error = loadError();		// load the previously saved error

		if (error.type != ERROR_NONE)			// if there was an error
		{
			MYSERIAL_PRINT_PGM("Previous Error State - ");
			printErrorDescr(&error);			// print the error description
			MYSERIAL_PRINTLN_PGM("Error Cleared\n");

			storeError(ERROR_NONE);			// clear the stored error
		}
	}
	// if EEPROM is not responding, set an error state
	else
	{
		set(ERROR_EEPROM_INIT);
	}
}

// enable error handling to control the tri-colour LED (NeoPixel)
void ERROR_HANDLING::enableLEDcontrol()
{
	_ctrlLED = true;
}

// disable error handling to control the tri-colour LED (NeoPixel)
void ERROR_HANDLING::disableLEDcontrol()
{
	_ctrlLED = false;
}

ErrorState ERROR_HANDLING::loadError(void)
{
	ErrorType errorNum = (ErrorType)EEPROM.read(EEPROM_LOC_STORED_ERROR);
	
	if (!IS_BETWEEN(errorNum, 0, MAX_NUM_ERRORS))
		errorNum = ERROR_NONE;

	return _errorList[errorNum];
}

// store the error in EEPROM
void ERROR_HANDLING::storeError(ErrorType error)
{
	EEPROM.write(EEPROM_LOC_STORED_ERROR, error);		// reset the stored error
}


////////////////////////////// Private Methods //////////////////////////////
// print the error number and description
void ERROR_HANDLING::printErrorDescr(ErrorState *error)
{
	if (!IS_BETWEEN(error->num, 0, MAX_NUM_ERRORS))
	{
		MYSERIAL_PRINT_PGM("Error - Error not an error");
		return;
	}
	
	// print error state
	if (error->level == LEVEL_FATAL)
	{
		MYSERIAL_PRINT_PGM("Fatal Error ");
	}
	else if(error->level == LEVEL_ERROR)
	{
		MYSERIAL_PRINT_PGM("Error ");
	}
	else if(error->level == LEVEL_WARN)
	{
		MYSERIAL_PRINT_PGM("Warning ");
	}
	else if(error->level == LEVEL_DEBUG)
	{
		MYSERIAL_PRINT_PGM("Debug ");
	}

	// if there is an error description
	if (error->description)
	{
		// print error number
		if (error->num < 100)
			MYSERIAL_PRINT_PGM("0");
		if (error->num < 10)
			MYSERIAL_PRINT_PGM("0");
		MYSERIAL_PRINT(error->num);

		MYSERIAL_PRINT_PGM(" - ");

		// print error description
		MYSERIAL_PRINT(error->description);
	}
}

ERROR_HANDLING ERROR;