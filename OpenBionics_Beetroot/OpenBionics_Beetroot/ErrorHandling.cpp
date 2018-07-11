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
#include "Watchdog.h"


////////////////////////////// Constructors/Destructors //////////////////////////////
ERROR_HANDLING::ERROR_HANDLING()
{
	// TODO: store all error descriptions is PROGMEM

	// NO ERROR
	_errorList[ERROR_NONE].num = 0;
	_errorList[ERROR_NONE].type = ERROR_NONE;
	_errorList[ERROR_NONE].level = LEVEL_NONE;
	_errorList[ERROR_NONE].LED.c1 = LED_GREEN_DIM;
	_errorList[ERROR_NONE].LED.blinkFreq = 0.5;				// 0.5Hz

	// Error 001 - UNKNOWN ERROR
	_errorList[ERROR_UNKNOWN].num = 1;
	_errorList[ERROR_UNKNOWN].type = ERROR_UNKNOWN;
	_errorList[ERROR_UNKNOWN].level = LEVEL_ERROR;
	_errorList[ERROR_UNKNOWN].description = "Unknown error occurred";
	_errorList[ERROR_UNKNOWN].LED.c1 = LED_YELLOW_DIM;

	// Error 002 - Initialisation sequence
	_errorList[ERROR_INIT].num = 2;
	_errorList[ERROR_INIT].type = ERROR_INIT;
	_errorList[ERROR_INIT].level = LEVEL_INFO;
	_errorList[ERROR_INIT].LED.c1 = LED_ORANGE_DIM;
	_errorList[ERROR_INIT].LED.blinkFreq = 2.5;				// 2.5Hz

	// Error 003 - EEPROM fails to respond to ping()
	_errorList[ERROR_EEPROM_INIT].num = 3;
	_errorList[ERROR_EEPROM_INIT].type = ERROR_EEPROM_INIT;
	_errorList[ERROR_EEPROM_INIT].level = LEVEL_FATAL;
	_errorList[ERROR_EEPROM_INIT].description = "EEPROM is not detected during initialisation";
	_errorList[ERROR_EEPROM_INIT].LED.c1 = LED_RED_DIM;
	_errorList[ERROR_EEPROM_INIT].LED.blinkFreq = 10;

	// Error 004 -  Finger pins fail to initialise
	_errorList[ERROR_FINGER_INIT].num = 4;
	_errorList[ERROR_FINGER_INIT].type = ERROR_FINGER_INIT;
	_errorList[ERROR_FINGER_INIT].level = LEVEL_FATAL;
	_errorList[ERROR_FINGER_INIT].description = "Finger pins failed to initialise";
	_errorList[ERROR_FINGER_INIT].LED.c1 = LED_OFF;

	// Error 005 - EEPROM settings overflow
	_errorList[ERROR_EEPROM_SETTINGS].num = 5;
	_errorList[ERROR_EEPROM_SETTINGS].type = ERROR_EEPROM_SETTINGS;
	_errorList[ERROR_EEPROM_SETTINGS].level = LEVEL_FATAL;
	_errorList[ERROR_EEPROM_SETTINGS].description = "Not enough EEPROM space for board settings";
	_errorList[ERROR_EEPROM_SETTINGS].LED.c1 = LED_OFF;

	// Error 006 - Serial buffer overflow
	_errorList[ERROR_S_BUFF_OVFLOW].num = 6;
	_errorList[ERROR_S_BUFF_OVFLOW].type = ERROR_S_BUFF_OVFLOW;
	_errorList[ERROR_S_BUFF_OVFLOW].level = LEVEL_WARN;
	_errorList[ERROR_S_BUFF_OVFLOW].description = "Serial buffer overflow";
	_errorList[ERROR_S_BUFF_OVFLOW].LED.c1 = LED_BLUE_DIM;
	_errorList[ERROR_S_BUFF_OVFLOW].duration = 5;					// display for 5s before clearing

	// Error 007 - Warning CPU temperature has been reached
	_errorList[ERROR_TEMP_WARNING].num = 7;
	_errorList[ERROR_TEMP_WARNING].type = ERROR_TEMP_WARNING;
	_errorList[ERROR_TEMP_WARNING].level = LEVEL_WARN;
	_errorList[ERROR_TEMP_WARNING].description = "Hand temperature is high";
	_errorList[ERROR_TEMP_WARNING].LED.c1 = LED_YELLOW;
	_errorList[ERROR_TEMP_WARNING].LED.c2 = LED_BLUE;
	_errorList[ERROR_TEMP_WARNING].LED.blinkFreq = 2;				// 2Hz

	// Error 008 - Maximum CPU temperature has been reached
	_errorList[ERROR_TEMP_MAX].num = 8;
	_errorList[ERROR_TEMP_MAX].type = ERROR_TEMP_MAX;
	_errorList[ERROR_TEMP_MAX].level = LEVEL_FATAL;
	_errorList[ERROR_TEMP_MAX].description = "Hand has reached maximum temperature";
	_errorList[ERROR_TEMP_MAX].LED.c1 = LED_RED_DIM;
	_errorList[ERROR_TEMP_MAX].LED.c2 = LED_YELLOW;
	_errorList[ERROR_TEMP_MAX].LED.blinkFreq = 5;					// 5Hz

	_tempLvlLED = 0;		// value of the LED brightness before the override 
}


////////////////////////////// Public Methods //////////////////////////////

// initialise error handling and display any stored errors
void ERROR_HANDLING::begin(void)
{
	enableLEDcontrol(true);					// enable error handling to control the LED

	_currError = &_errorList[ERROR_NONE];	// start pointer with no error
	ERROR.set(ERROR_NONE);					// start with no errors (LED, etc)
}

// set an error state and perform the appropriate actions
void ERROR_HANDLING::set(ErrorType error)
{
	// if the enterred error number is out of bounds, set unknown error state
	if (error >= MAX_NUM_ERRORS)
	{
		error = ERROR_UNKNOWN;
	}

	// if the new error has a lower severity/level than the current level, and error isn't being cleared 
	if ((_errorList[error].level <= _currError->level) && (error != ERROR_NONE))
	{
		return;						// do not run the lower error level
	}
	// else if the new error has a greater severity/level than the current level
	else if (_errorList[error].level > _currError->level)
	{
		clear(_currError->type);	// clear the error state (and roll back the LED history)
	}

	// point to error properties
	_currError = &_errorList[error];
	//storeError(_currError->type);			// WARNING. THIS CAUSES THE PROGRAM TO HANG

	// if error handling has control of the NeoPixel and there is a colour to show
	if (_ctrlLED && (_currError->LED.c1 != NO_LED_COLOUR))
	{
		if (_currError->LED.c2 == NO_LED_COLOUR)
		{
			_currError->LED.c2 = LED_BLACK;
		}

		// if the error is of a high enough severity, override the LED brightness
		if (_currError->level >= LEVEL_ERROR)
		{
			_tempLvlLED = LED.getBrightness();		// save the current LED brightness level
			LED.setBrightness(OVRIDE_LVL_LED);
		}

		LED.setMode(LED_MODE_FADE);
		LED.setColour(_currError->LED.c1, _currError->LED.c2);	// set LED flashing colours
		LED.setDuration(0);										// clear duration, as the ERROR.run() will clear the LED after a duration
		LED.setFreq(_currError->LED.blinkFreq);					// set LED to be solid or to fade
		LED.show();												// show on the LED
	}

	if (_currError->duration)								// if error has a duration 
	{
		errorDuration.start(_currError->duration * 1000.0);	// start timer to that the warning can be cleared (convert s to ms)
	}

	// print the error description
	if (error != ERROR_NONE)
	{
		printErrorDescr(_currError);
	}

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
			if (finger[i].attached())
			{
				finger[i].motorEnable(false);
			}
		}

		// halt program (if still running)
		while (1)
		{
#if defined(ARDUINO_ARCH_SAMD)
			Watchdog.reset();
#endif
		}
	}
};

//// clear the error state
//void ERROR_HANDLING::clear(void)
//{
//	set(ERROR_NONE);
//}

// only clear the error state if it is the same as the one passed to the function
bool ERROR_HANDLING::clear(ErrorType error)
{
	if (error == _currError->type)
	{
		// if error handling has control of the NeoPixel and there is a colour to show
		if (_ctrlLED && (_currError->LED.c1 != NO_LED_COLOUR))
		{
			// if the error is of a high enough severity, revert the vibration intensity to the pre-override val
			if (_currError->level >= LEVEL_ERROR)
			{
				LED.setBrightness(_tempLvlLED);
			}

			LED.showPrev();

			// NOTE. THIS MAY RESET THE BRIGHTNESS
		}

		set(ERROR_NONE);	// set the error to none (moved to LED index 2)

							//clear();
		return true;
	}
	else
	{
		return false;
	}
}

// clear any warnings after a set period and check for fatal errors
void ERROR_HANDLING::run(void)
{
	// if error is set & duration has passed
	if (errorDuration.started() && errorDuration.finished())
	{
		//LED.showPrev();		// set LED to previous state
		clear(_currError->type);	// clear error flag
	}
}

// get the current error state
ErrorType ERROR_HANDLING::get(void)
{
	if (_currError == nullptr)
	{
		_currError = &_errorList[ERROR_UNKNOWN];
	}

	return _currError->type;
}

// check whether a specific error state is set
bool ERROR_HANDLING::isSet(ErrorType error)
{
	if (!_currError)
	{
		_currError = &_errorList[ERROR_UNKNOWN];
	}

	return (error == _currError->type);

}

// print the current error state
void ERROR_HANDLING::printCurrent(bool nl)
{
	if (!_currError)
	{
		_currError = &_errorList[ERROR_UNKNOWN];
	}

	printErrorDescr(_currError);
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

// enable error handling to control the tri-c1 LED (NeoPixel)
void ERROR_HANDLING::enableLEDcontrol(bool en)
{
	_ctrlLED = en;
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
void ERROR_HANDLING::printErrorDescr(ErrorState *error, bool nl)
{
	if (!IS_BETWEEN(error->num, 0, MAX_NUM_ERRORS))
	{
		MYSERIAL_PRINT_PGM("Error - Error not an error");

		if (nl)
		{
			MYSERIAL_PRINT_PGM("\n");
		}

		return;
	}

	// if there is an error description
	if (error->description)
	{
		// print error state
		switch (error->level)
		{
			case LEVEL_FATAL:
				MYSERIAL_PRINT_PGM("Fatal Error ");
				break;
			case LEVEL_ERROR:
				MYSERIAL_PRINT_PGM("Error ");
				break;
			case LEVEL_WARN:
				MYSERIAL_PRINT_PGM("Warning ");
				break;
			case LEVEL_INFO:
				MYSERIAL_PRINT_PGM("Info ");
				break;
			case LEVEL_DEBUG:
				MYSERIAL_PRINT_PGM("Debug ");
				break;
			default:
				break;
		}

		// print error number
		if (error->num < 100)
		{
			MYSERIAL_PRINT_PGM("0");
		}
		if (error->num < 10)
		{
			MYSERIAL_PRINT_PGM("0");
		}
		MYSERIAL_PRINT(error->num);

		MYSERIAL_PRINT_PGM(" - ");

		// print error description
		MYSERIAL_PRINT(error->description);

		if (nl)
		{
			MYSERIAL_PRINT_PGM("\n");
		}
	}


}

ERROR_HANDLING ERROR;