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
*	Initialisation.cpp
*
*/


#include <FingerLib.h>
#include <Wire.h>

#include "Globals.h"
#include "Initialisation.h"

#include "I2C_IMU_LSM9DS1.h"				// IMU
#include "Demo.h"							// DEMO
#include "EMGControl.h"						// EMG
#include "ErrorHandling.h"					// ERROR
#include "Grips.h"							// Grip
#include "HANDle.h"							// HANDle
#include "I2C_EEPROM.h"						// EEPROM
#include "LED.h"							// NeoPixel
#include "SerialControl.h"					// init char codes

Settings settings;		// board settings

// board initialisation sequence
void deviceSetup(void)
{
#if defined (SERIAL_JACK_CONTROL)
	setHeadphoneJack(JACK_SERIAL);	// configure headphone jack to Serial
#else
	setHeadphoneJack(JACK_ADC);		// configure headphone jack to ADC so as to not affect I2C lines
#endif

	MYSERIAL.begin(SERIAL_BAUD_RATE);		// initialise Serial

	Wire.begin();				// initialise I2C

	LED.begin();				// initialise NeoPixel

	timerSetup();				// initialise customMillis() and LED pulsing

	ERROR.begin();				// initialise the error handler
	ERROR.checkPrevError();		// check for previous errors (using EEPROM)
	ERROR.set(ERROR_INIT);		// set error state during initialisation, and set LED to orange	

	readEEPROM();				// load settings from EEPROM, if no settings, use defaults

	initFingerPins();			// attach finger pins and start ms timer

	initSerialCharCodes();		// assign the char codes and functions to char codes

	Grip.begin();				// initialise the grips

	EMG.begin();				// initialise EMG control

	IMU.begin();				// initialise IMU

	setModes();					// start EMG or Demo mode

	detectSerialConnection();	// wait for serial connection if flag is set

	ERROR.clear();				// clear error state and set LED to green
}

// wait for serial connection if flag is set
void detectSerialConnection(void)
{
	if (settings.waitForSerial)			// if flag is set
	{
		while (!MYSERIAL);					// wait for serial connection
		MYSERIAL_PRINTLN_PGM("Started");
	}
}

// read the settings from EEPROM
void loadSettings(void)
{
	static bool firstLoad = true;

	// if this is the first time the settings are being loaded from EEPROM
	if (firstLoad)
	{
		firstLoad = false;

		// if size of settings will overflow EEPROM, set error and halt program
		if ((EEPROM_LOC_BOARD_SETTINGS + sizeof(settings)) > EEPROM_TOTAL_SIZE - 1)
		{
			ERROR.set(ERROR_EEPROM_SETTINGS);
		}
	}

	//MYSERIAL.print("loadSettings() settings size: ");
	//MYSERIAL.println(sizeof(settings));

	//MYSERIAL.print("From: ");
	//MYSERIAL.print(EEPROM_LOC_BOARD_SETTINGS);
	//MYSERIAL.print(" to: ");
	//MYSERIAL.println(EEPROM_LOC_BOARD_SETTINGS+sizeof(settings));


	EEPROM_readStruct(EEPROM_LOC_BOARD_SETTINGS, settings);
}

// store the settings in EEPROM
void storeSettings(void)
{
	//MYSERIAL.print("storeSettings() settings size: ");
	//MYSERIAL.println(sizeof(settings));
	EEPROM_writeStruct(EEPROM_LOC_BOARD_SETTINGS, settings);
}


// set default settings
void resetToDefaults(void)
{
	ERROR.storeError(ERROR_NONE);			// reset stored error state

	settings.handType = HAND_TYPE_RIGHT;	// right hand
	settings.mode = MODE_NONE;				// normal mode (not demo or EMG)

	settings.emg.holdTime = 300;			// 300ms
	settings.emg.peakThresh = 600;			// 600/1023

	settings.waitForSerial = true;			// wait for serial connection to start
	settings.motorEn = true;				// enable all motors
	settings.printInstr = true;				// print serial instructions

  settings.motorReorder = false;

	settings.init = EEPROM_INIT_CODE;		// store the unique initialisation code to indicate that EEPROM has been initialised with values

	storeSettings();						// store the settings in EEPROM
}

// start hand in a particular mode depending on EEPROM settings
void setModes(void)
{
	// initialise hand in specific mode, read from EEPROM
	switch (settings.mode)
	{
	case MODE_DEMO:
		DEMO.toggleConstant();
		break;
	case MODE_EMG_SIMPLE:
		EMG.simple();
		break;
	case MODE_EMG_PROP:
		EMG.proportional();
		break;
	case MODE_HANDLE:
		HANDle.enable();
		break;
	default:
		break;
	}
}

// load settings from EEPROM, if no previous EEPROM settings are stored, store defaults
void readEEPROM(void)
{
	loadSettings();				// load settings from EEPROM

	// if the EEPROM is not currently loaded with the correct values
	if (settings.init != EEPROM_INIT_CODE)
	{
		resetToDefaults();		// set the default values
		storeSettings();		// store settings in EEPROM

		deviceSetup();			// restart device setup
	}
}

// attach the finger pins for a left/right hand
void initFingerPins(void)
{
	// if no hand type is detected
	if ((settings.handType != HAND_TYPE_RIGHT) && (settings.handType != HAND_TYPE_LEFT))
	{
		settings.handType = HAND_TYPE_RIGHT;			// default to a right hand
		storeSettings();								// store the settings in EEPROM
	}

  // Allow motor order to be reversed for the new Brunel design.
  static const int fingerOrderSets[][4] = { {0,1,2,3}, {3,2,1,0} };
  const int* fingerOrder = fingerOrderSets[ settings.motorReorder ? 1 : 0 ];

	// attach the finger pins
	if (settings.handType == HAND_TYPE_RIGHT)
	{
		finger[fingerOrder[0]].attach(4, 8, A2, A8, false);		// attach the thumb	
		finger[fingerOrder[1]].attach(1, 2, A0, A6, true);		// attach the index (finger is inverted)     
		finger[fingerOrder[2]].attach(7, 5, A1, A9, true);		// attach the middle (finger is inverted)
		finger[fingerOrder[3]].attach(0, 9, A3, A7, true);		// attach the ring & pinky (fingers are inverted)

		//finger[fingerOrder[0]].attach(1, 2, A0, A6, true);		// M0     
		//finger[fingerOrder[1]].attach(7, 5, A1, A9, true);		// M1
		//finger[fingerOrder[2]].attach(4, 8, A2, A8, true);		// M2
		//finger[fingerOrder[3]].attach(0, 9, A3, A7, true);		// M3

	}
	else if (settings.handType == HAND_TYPE_LEFT)
	{
		finger[fingerOrder[0]].attach(7, 5, A1, A9, false);		// attach the thumb	
		finger[fingerOrder[1]].attach(0, 9, A3, A7, true);		// attach the index (finger is inverted) 
		finger[fingerOrder[2]].attach(4, 8, A2, A8, true);		// attach the middle (finger is inverted)
		finger[fingerOrder[3]].attach(1, 2, A0, A6, true);		// attach the ring & pinky (fingers are inverted)     
			
		//finger[fingerOrder[0]].attach(1, 2, A0, A6, true);		// M0     
		//finger[fingerOrder[1]].attach(7, 5, A1, A9, true);		// M1
		//finger[fingerOrder[2]].attach(4, 8, A2, A8, false);	// M2
		//finger[fingerOrder[3]].attach(0, 9, A3, A7, true);		// M3
	}


	// enable all fingers
	for (int i = 0; i < NUM_FINGERS; i++)
	{
		if (!finger[i].attached())				// if finger failed to attach
		{
			ERROR.set(ERROR_FINGER_INIT);		// set error
			break;
		}

		finger[i].motorEnable(settings.motorEn);	// set motor to be enabled/disabled depending on EEPROM setting
#ifdef FORCE_SENSE
		finger[i].enableForceSense();				// enable force sense on the finger
#endif
	}

		
}

// configure headphone jack to I2C, ADC, Digital or SerialJack
void setHeadphoneJack(HeadphoneJackMode mode)
{
	const int commSwitchPin = 10;

	pinMode(commSwitchPin, OUTPUT);

	digitalWrite(commSwitchPin, mode);			// JACK_I2C, JACK_ADC, JACK_DIGITAL, JACK_SERIAL
	delay(100);									// allow time for switch to settle
}

// print board & firmware version, hand type and motor enabled/disabled
void printDeviceInfo(void)
{
	const char* handTypeNames[3] = { "NONE", "Right", "Left" };
	
	// print firmware version
	MYSERIAL_PRINT("FW:\tBeetroot V");
	MYSERIAL_PRINTLN(FW_VERSION);

	// print board name
	MYSERIAL_PRINTLN_PGM("Board:\tChestnut");

	// print hand type
	MYSERIAL_PRINT_PGM("Hand:\t");
	MYSERIAL_PRINTLN(handTypeNames[settings.handType]);

	// print motor reordering 
	MYSERIAL_PRINT_PGM("Motor order:\t");
	MYSERIAL_PRINTLN(settings.motorReorder ? "Normal" : "Reversed");
}

// monitor system status
void systemMonitor(void)
{
	static NB_DELAY timer_1s;

	if (timer_1s.timeEllapsed(1000))
	{
		// monitor CPU temperature
		if (readCPUTemp() >= CPU_TEMP_MAX)
		{
			ERROR.set(ERROR_TEMP_MAX);
		}
		else if (readCPUTemp() >= CPU_TEMP_WARNING)
		{
			ERROR.set(ERROR_TEMP_WARNING);
		}

	}
}
