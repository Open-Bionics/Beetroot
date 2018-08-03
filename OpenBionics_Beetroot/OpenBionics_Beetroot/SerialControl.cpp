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
*	SerialControl.cpp
*
*/

#include "Globals.h"
#include "SerialControl.h"

#include "Demo.h"					// DEMO
#include "EMGControl.h"				// EMG
#include "ErrorHandling.h"			// ERROR
#include "Grips.h"					// NUM_GRIPS
#include "HANDle.h"					// HANDle
#include "I2C_IMU_LSM9DS1.h"		// IMU
#include "Initialisation.h"			// settings

SerialCode serialCodes[NUM_SERIAL_CODES];

char serialBuff[SERIAL_BUFF_SIZE];

// assign the char codes for each serial command
void initSerialCharCodes(void)
{
	// code - the character used for each serial command (e.g. G for grip control)
	// limit - used to constrain the value that follows the char code
	// func - attached function for command codes. Some codes are only used as modifiers and do not have an attached func (e.g. C, O, P, S)

	// The notes below have not been tested. It may be safe to use the following chars, but noting here just in case
	// Note. Do not use an 'e' or 'E' char, as strtod() reads these chars as an exponent modifier
	// Note. Do not use a '+' or '-' char, as strtod() reads these chars as a sign modifier
	// Note. Do not use a '0x' or '0X' char, as strtod() reads these chars as a hex modifier
	

	serialCodes[SERIAL_CODE_A].code = 'A';		// Advanced settings
	serialCodes[SERIAL_CODE_A].limit = NUM_ADV_SETTINGS;
	serialCodes[SERIAL_CODE_A].func = serial_AdvancedSettings;

	serialCodes[SERIAL_CODE_C].code = 'C';		// Close
	serialCodes[SERIAL_CODE_C].limit = LIMIT_FOR_BOOLEAN;
	// no attached func as C is a modifier

	serialCodes[SERIAL_CODE_D].code = 'D';		// Demo mode
	serialCodes[SERIAL_CODE_D].limit = LIMIT_FOR_BOOLEAN;
	serialCodes[SERIAL_CODE_D].func = serial_ToggleDemoMode;

	serialCodes[SERIAL_CODE_F].code = 'F';		// Finger number
	serialCodes[SERIAL_CODE_F].limit = NUM_FINGERS;
	serialCodes[SERIAL_CODE_F].func = serial_FingerControl;

	serialCodes[SERIAL_CODE_G].code = 'G';		// Grip number
	serialCodes[SERIAL_CODE_G].limit = NUM_GRIPS;
	serialCodes[SERIAL_CODE_G].func = serial_GripControl;

	serialCodes[SERIAL_CODE_H].code = 'H';		// Set hand to left/right
	serialCodes[SERIAL_CODE_H].limit = NUM_HAND_TYPES;
	serialCodes[SERIAL_CODE_H].func = serial_SetHandType;

	serialCodes[SERIAL_CODE_M].code = 'M';		// EMG mode
	serialCodes[SERIAL_CODE_M].limit = NUM_EMG_MODES;
	serialCodes[SERIAL_CODE_M].func = serial_MuscleControlMode;

	serialCodes[SERIAL_CODE_O].code = 'O';		// Open
	serialCodes[SERIAL_CODE_O].limit = LIMIT_FOR_BOOLEAN;
	// no attached func as O is a modifier

	serialCodes[SERIAL_CODE_P].code = 'P';		// Finger position
	serialCodes[SERIAL_CODE_P].limit = 100;		// 0 - 100
	// no attached func as P is a modifier

	serialCodes[SERIAL_CODE_R].code = 'R';		// Reset to defaults
	serialCodes[SERIAL_CODE_R].limit = LIMIT_FOR_BOOLEAN;
	serialCodes[SERIAL_CODE_R].func = serial_ResetToDefaults;

	serialCodes[SERIAL_CODE_S].code = 'S';		// Finger speed
	serialCodes[SERIAL_CODE_S].limit = 255;		// max 8-bit value
	// no attached func as S is a modifier

	serialCodes[SERIAL_CODE_T].code = 'T';		// Muscle hold time
	serialCodes[SERIAL_CODE_T].limit = 5000;	// 5000ms
	serialCodes[SERIAL_CODE_T].func = serial_HoldTime;

	serialCodes[SERIAL_CODE_U].code = 'U';		// Muscle peak threshold
	serialCodes[SERIAL_CODE_U].limit = 1024;	// max 10-bit value
	serialCodes[SERIAL_CODE_U].func = serial_PeakThresh;

	serialCodes[SERIAL_CODE_X].code = 'X';		// Exit mode
	serialCodes[SERIAL_CODE_X].limit = LIMIT_FOR_BOOLEAN;
	serialCodes[SERIAL_CODE_X].func = serial_ExitMode;

	serialCodes[SERIAL_CODE_HASH].code = '#';	// Print system diagnostics
	serialCodes[SERIAL_CODE_HASH].limit = LIMIT_FOR_BOOLEAN;
	serialCodes[SERIAL_CODE_HASH].func = serial_systemDiagnostics;

	serialCodes[SERIAL_CODE_QMARK].code = '?';	// Print serial instructions
	serialCodes[SERIAL_CODE_QMARK].limit = LIMIT_FOR_BOOLEAN;
	serialCodes[SERIAL_CODE_QMARK].func = serial_SerialInstructions;

}

// check and read serial, then perform appropriate actions
void pollSerial(void)
{
	// read and store serial chars, return true if end of line char is received
	if (checkSerial())
	{
		// if the current mode is not CSV mode
		if (settings.mode != MODE_CSV)
		{
			// print received serial string
			MYSERIAL_PRINT_PGM("\n");
			MYSERIAL_PRINTLN(serialBuff);
		}

		extractCodesFromSerial();		// extract char codes from serialBuff and store values in serialCodes[]

		processCodes();					// use the extracted codes & values to control the hand
	}
}

// read and store serial chars, return true if end of line char is received
bool checkSerial(void)
{
	static uint8_t buffIndex = 0;

	// if there is data in the serial buffer
	if (MYSERIAL.available())
	{
		char rxChar = MYSERIAL.read();		// store char in temporary variable

		// if the received char is an end of line character, reset buffer index and return true to indicate the message is ready to read
		if ((rxChar == SERIAL_EOL_CHAR_NL) || (rxChar == SERIAL_EOL_CHAR_CR))
		{
			serialBuff[buffIndex] = NULL;	// terminate string

			if (buffIndex == 0)				// if there is nothing in the buffer
				return false;				// return false to indicate no buffer has been received

			buffIndex = 0;					// reset buffer index fur future strings
			return true;					// return string is complete


		}
		else	// if the received char is a normal char
		{
			// if buffer overflow
			if (buffIndex >= (SERIAL_BUFF_SIZE))
			{
				ERROR.set(ERROR_S_BUFF_OVFLOW);			// set serial buffer overflow error state
			}
			else	// if there is space in the buffer
			{
				
				// check if the char is valid
				if (isValidChar(rxChar))
				{
					serialBuff[buffIndex++] = rxChar;		// store received char in buffer
				}
				else
				{
					MYSERIAL_PRINT_PGM("WARNING - '");
					MYSERIAL_PRINT(rxChar);
					MYSERIAL_PRINT_PGM("' (0x");
					MYSERIAL_PRINT_F((int)rxChar,HEX);
					MYSERIAL_PRINTLN_PGM(") IS NOT A VALID CHARACTER");
				}

			}

			return false;		// do not process received string until an EOL char has been detected
		}
	}

	return false;		// either no new serial char or no end of line char
}

// check if rxChar is alphanumeric, or a permitted character
bool isValidChar(char rxChar)
{
	// if char is a permitted special character or an alphanumeric char, return true, else return false
	if ((rxChar == ASCII_SPACE) || 
		(rxChar == ASCII_HASH)	||
		(rxChar == ASCII_COMMA) ||
		(rxChar == ASCII_QMARK) ||
		IS_BETWEEN(rxChar, ASCII_0, ASCII_9) || 
		IS_BETWEEN(rxChar, ASCII_A, ASCII_z))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// process serial buff and extract any char codes with their appended values
void extractCodesFromSerial(void)
{
	char *codeLocPtr = NULL;	// pointer to location of code within serial buffer
	bool detected = false;		// flag to indicate if a char code has been detected in the buffer

	for (int i = 0; i < NUM_SERIAL_CODES; i++)
	{
		if (codeDetected(serialCodes[i].code, &codeLocPtr))	// if the char code is detected in serialBuff
		{
			serialCodes[i].val = codeVal(codeLocPtr);		// get the value after the char code

			serialCodes[i].newVal = true;					// set flag to indicate a new value has been detected

			detected = true;								// set detected to true, to skip CSV check
		}
	}

	// if no char code was detected and currently in CSV mode 
	if (!detected && (settings.mode == MODE_CSV))
	{
		receiveCSV(serialBuff);		// check whether buffer contains CSV string
	}

	serialBuff[0] = NULL;			// clear serial buff once all codes have been looked for
}

// search through serial buffer and look for char code, if found return true and store pointer to loc of char code
bool codeDetected(char code, char **codeLoc)
{
	// get a pointer to the first occurrence of code in serialBuff
	*codeLoc = strchr(serialBuff, code);

	return (*codeLoc != NULL);	// return true if code found, else return false
}

// get the numerical value following the char code at codeLoc
int codeVal(char *codeLoc)
{
	codeLoc++;							// increment pointer to the value after the char code

	if (isDigit(*codeLoc))					// if there is a digit following the char
		return strtod(codeLoc, NULL);		// return the value of the digits
 	else									// if there is no digit 
		return BLANK;						// return BLANK
}

// use the extracted codes & values to control the hand
void processCodes(void)
{
	// count through all char codes
	for (int i = 0; i < NUM_SERIAL_CODES; i++)
	{
		// if a new value has been received
		if (serialCodes[i].newVal)
		{
			// constrain the code value
			serialCodes[i].val = constrain(serialCodes[i].val, BLANK, serialCodes[i].limit);

			// run the attached function with the constrained code value (if the code is a command code)
			if (serialCodes[i].func)
			{
				serialCodes[i].func(serialCodes[i].val);		// run the attached function

				serialCodes[i].newVal = false;					// clear newVal flag, as the value has been seen by the attached function
			}
		}
	}
}

// if CSV mode is enabled, send CSV of all finger positions
void sendCSV(void)
{
	int posArray[NUM_FINGERS];
	char CSVStr[64];

	for (int i = 0; i < NUM_FINGERS; i++)	// count through all fingers
	{
		posArray[i] = finger[i].readPos();	// store position in array
	}

	convertToCSV(posArray, NUM_FINGERS, CSVStr);	// convert position array to CSV string

	MYSERIAL_PRINTLN(CSVStr);				// send CSV string over serial
}

// if string contains CSV data, write received positions to the fingers
void receiveCSV(char *buff)
{
	int posArray[NUM_FINGERS];

	if (convertFromCSV(buff, posArray, NUM_FINGERS))
	{
		for (int i = 0; i < NUM_FINGERS; i++)
		{
			finger[i].writePos(posArray[i]);
		}
	}
}




// CHAR CODE FUNCTIONS (the following functions are attached to the char codes)

// configure the advanced settings
void serial_AdvancedSettings(int setting)
{
	const char *off_on[2] = { "OFF","ON" };
	const char *disabled_enabled[2] = { "DISABLED","ENABLED" };


	switch (setting)
	{
	case 0:			// toggle demo operating mode
		if (settings.mode == MODE_DEMO)
		{
			settings.mode = MODE_NONE;
			DEMO.stop();
		}
		else
		{
			settings.mode = MODE_DEMO;
			DEMO.constant();
		}

		storeSettings();
		MYSERIAL_PRINT_PGM("Demo mode ");
		MYSERIAL_PRINTLN(off_on[settings.mode]);

		break;

	case 1:			// toggle serial instructions
		settings.printInstr = !settings.printInstr;
		storeSettings();

		MYSERIAL_PRINT_PGM("Serial instructions ");
		MYSERIAL_PRINTLN(off_on[settings.printInstr]);

		if (settings.printInstr)
			serial_SerialInstructions(0);
		break;

	case 2:			// toggle wait for serial
		settings.waitForSerial = !settings.waitForSerial;
		storeSettings();

		MYSERIAL_PRINT_PGM("Wait for serial ");
		MYSERIAL_PRINTLN(disabled_enabled[settings.waitForSerial]);
		break;

	case 3:			// enable/disable motors
		settings.motorEn = !settings.motorEn;
		storeSettings();

		for (int i = 0; i < NUM_FINGERS; i++)
			finger[i].motorEnable(settings.motorEn);

		MYSERIAL_PRINT_PGM("Motors ");
		MYSERIAL_PRINTLN(disabled_enabled[settings.motorEn]);
		break;

	case 4:			// enable/disable CSV mode
		MYSERIAL_PRINT_PGM("CSV mode ");
		if (settings.mode == MODE_CSV)
		{
			settings.mode = MODE_NONE;
			MYSERIAL_PRINTLN(disabled_enabled[0]);
		}
		else 
		{
			settings.mode = MODE_CSV;
			MYSERIAL_PRINTLN(disabled_enabled[1]);
		}
		storeSettings();
		break;

	case 5:			// enable/disable HANDle mode
		MYSERIAL_PRINT_PGM("HANDle mode ");
		if (settings.mode == MODE_HANDLE)
		{
			settings.mode = MODE_NONE;
			HANDle.disable();
			MYSERIAL_PRINTLN(off_on[OFF]);
		}
		else
		{
			settings.mode = MODE_HANDLE;
			HANDle.enable();
			MYSERIAL_PRINTLN(off_on[ON]);
		}

		storeSettings();
		break;

	case 6:			// read finger positions as CSV			
		sendCSV();
		break;

	default:
		MYSERIAL_PRINTLN_PGM("Advanced Setting Not Valid");
		break;
	}
}

// toggle demo mode
void serial_ToggleDemoMode(int val)
{
	MYSERIAL_PRINTLN_PGM("Demo Mode");
	DEMO.toggleOnce();
}

// finger control
void serial_FingerControl(int fNum)
{
	const char* dirStr[2] = { "Open", "Close" };
	

	if (!IS_BETWEEN(fNum, 0, NUM_FINGERS)) 
	{
		MYSERIAL_PRINTLN_PGM("Finger Number Not Valid");
		return;
	}

	// detect direction (O or C)
	if (serialCodes[SERIAL_CODE_C].newVal)			// open direction
	{
		finger[fNum].writeDir(CLOSE);
		serialCodes[SERIAL_CODE_P].val = 100;
		serialCodes[SERIAL_CODE_C].newVal = false;
	}
	else if (serialCodes[SERIAL_CODE_O].newVal)		// close direction
	{
		finger[fNum].writeDir(OPEN);
		serialCodes[SERIAL_CODE_P].val = 0;
		serialCodes[SERIAL_CODE_O].newVal = false;
	}
	else if (serialCodes[SERIAL_CODE_P].newVal)		// detect position
	{
		finger[fNum].writePos(map(serialCodes[SERIAL_CODE_P].val, 0, 100, FULLY_OPEN, FULLY_CLOSED));	// map 0 - 100 to 0 - 1024
		serialCodes[SERIAL_CODE_P].newVal = false;
	}
	else                                          	// toggle direction
	{
		finger[fNum].writeDir(!finger[fNum].readDir());
		serialCodes[SERIAL_CODE_P].val = 100 * finger[fNum].readDir();
	}

	
	// detect speed
	if (serialCodes[SERIAL_CODE_S].newVal)
	{
		finger[fNum].writeSpeed(serialCodes[SERIAL_CODE_S].val);
		serialCodes[SERIAL_CODE_S].newVal = false;
	}

	// remember to clear O, C, P & S

	MYSERIAL_PRINT_PGM("Finger: \t");
	MYSERIAL_PRINTLN(fNum);

	MYSERIAL_PRINT_PGM("Direction:\t");
	MYSERIAL_PRINTLN(dirStr[finger[fNum].readDir()]);

	MYSERIAL_PRINT_PGM("Position:\t");
	MYSERIAL_PRINTLN(serialCodes[SERIAL_CODE_P].val);

	MYSERIAL_PRINT_PGM("Speed:\t\t");
	MYSERIAL_PRINTLN(finger[fNum].readTargetPWM());


	// if the hand is in a mode that means the fingers will not respond to serial control
	printCurrentMode();		// print the current mode and the exit command
}

// grip control
void serial_GripControl(int gNum)
{
	const char* dirStr[2] = { "Close" , "Open" };
	bool dir; // = Grip.getDir();

	if (gNum >= NUM_GRIPS)
	{
		MYSERIAL_PRINTLN_PGM("Grip Number Not Valid");
		return;
	}

	// only set grip if a grip number is entered (i.e. if G then don't set grip)
	if (gNum != BLANK)
	{
		Grip.setGrip(gNum);			// set grip number

		// detect direction (O or C)
		if (serialCodes[SERIAL_CODE_C].newVal)
		{
			Grip.setDir(CLOSE);
			serialCodes[SERIAL_CODE_C].newVal = false;
			dir = CLOSE;
		}
		else if (serialCodes[SERIAL_CODE_O].newVal)
		{
			Grip.setDir(OPEN);
			serialCodes[SERIAL_CODE_O].newVal = false;
			dir = OPEN;
		}
		else
		{
			dir = Grip.toggleDir();
		}

		// detect position
		if (serialCodes[SERIAL_CODE_P].newVal)
		{
			Grip.setPos(serialCodes[SERIAL_CODE_P].val);
			serialCodes[SERIAL_CODE_P].newVal = false;
		}

		// detect speed
		if (serialCodes[SERIAL_CODE_S].newVal)
		{
			Grip.setSpeed(serialCodes[SERIAL_CODE_S].val);
			serialCodes[SERIAL_CODE_S].newVal = false;
		}

		Grip.run();

		// remember to clear O, C, P & S

		MYSERIAL_PRINT_PGM("Grip:\t\t");
		MYSERIAL_PRINTLN(Grip.getGripName());

		MYSERIAL_PRINT_PGM("Direction:\t");
		MYSERIAL_PRINTLN(dirStr[dir]);

		MYSERIAL_PRINT_PGM("Position:\t");
		MYSERIAL_PRINTLN(Grip.getPos());

		MYSERIAL_PRINT_PGM("Speed:\t\t");
		MYSERIAL_PRINTLN(Grip.getSpeed());

		// if the hand is in a mode that means the fingers will not respond to serial control
		printCurrentMode();		// print the current mode and the exit command
	}
	else
	{
		MYSERIAL_PRINTLN_PGM("Grip Number Not Valid");
	}
}

// set hand type (NONE, LEFT, RIGHT)
void serial_SetHandType(int hType)
{
	const char *right_left[2] = { "Right","Left" };

	if (hType == BLANK)
	{
		MYSERIAL_PRINT(right_left[settings.handType - 1]);
		MYSERIAL_PRINTLN_PGM(" Hand");
	}
	else if (IS_BETWEEN(hType, RIGHT, LEFT))
	{

		settings.handType = (HandType)hType;
		storeSettings();

		MYSERIAL_PRINT_PGM("Now configured as a ");
		initFingerPins();	
		MYSERIAL_PRINT(right_left[settings.handType - 1]);
		MYSERIAL_PRINTLN_PGM(" Hand");
	}
	else
	{
		MYSERIAL_PRINTLN_PGM("Hand Type Not Valid");
	}
	
}

// muscle control mode
void serial_MuscleControlMode(int mMode)
{
	const char *disabled_enabled[2] = { "DISABLED","ENABLED" };

	MYSERIAL_PRINT_PGM("Muscle mode - ");

	switch (mMode)
	{
	case 0:			// muscle mode off
		EMG.off();
		settings.mode = MODE_NONE;
		storeSettings();

		MYSERIAL_PRINTLN_PGM("OFF");

		break;
	case 1:			// simple muscle mode
		EMG.simple();
		settings.mode = MODE_EMG_SIMPLE;
		storeSettings();

		MYSERIAL_PRINTLN_PGM("Simple");
		break;
	case 2:			// proportional muscle mode
		EMG.proportional();
		settings.mode = MODE_EMG_PROP;
		storeSettings();

		MYSERIAL_PRINTLN_PGM("Proportional");
		break;
	case 3:
		MYSERIAL_PRINT_PGM("Print ADC vals ");
		MYSERIAL_PRINTLN(disabled_enabled[EMG.toggleADCVals()]);
		break;
	default:
		MYSERIAL_PRINTLN_PGM("Not Valid");
		break;
	}
}

// muscle hold time
void serial_HoldTime(int hTime)
{
	if (hTime != BLANK)
	{
		settings.emg.holdTime = (uint16_t)hTime;
		storeSettings();
	}
	
	MYSERIAL_PRINT_PGM("Hold time - ");
	MYSERIAL_PRINT(settings.emg.holdTime);
	MYSERIAL_PRINTLN_PGM("ms");
}

// muscle peak threshold
void serial_PeakThresh(int pThresh)
{
	if (pThresh != BLANK)
	{
		settings.emg.peakThresh = (uint16_t)pThresh;
		storeSettings();
	}
	
	MYSERIAL_PRINT_PGM("Peak threshold - ");
	MYSERIAL_PRINTLN(settings.emg.peakThresh);
}

// reset to defaults
void serial_ResetToDefaults(int val)
{
	MYSERIAL_PRINTLN_PGM("Resetting To Defaults");
	resetToDefaults();
	serial_SerialInstructions();		// print serial instructions
}

// exit modes
void serial_ExitMode(int val)
{
	// disable EMG mode
	EMG.off();
	settings.mode = MODE_NONE;
	storeSettings();

	// disable DEMO mode
	DEMO.stop();


	MYSERIAL_PRINTLN_PGM("Exit All Modes");

}

// system diagnostics
void serial_systemDiagnostics(int val)
{
	const char *disabled_enabled[2] = { "DISABLED","ENABLED" };
	const char *modeNames[NUM_MODES] = { "None","Demo","EMG - Simple","EMG - Proportional", "CSV" };

	MYSERIAL_PRINTLN_PGM("     System Diagnostics");
	for (uint8_t i = 0; i < 28; i++)
	{
		MYSERIAL_PRINT_PGM("_");
	}
	MYSERIAL_PRINT_PGM("\n\n");

	// print FW type/version, hand type and whether motors are enabled
	printDeviceInfo();	

	// print total 'on-time'
	MYSERIAL_PRINT_PGM("OnTime:\t");
	printTime_ms(millis());
	MYSERIAL_PRINT_PGM("\n");

	// print IMU temperature
	IMU.poll();
	MYSERIAL_PRINT_PGM("Temp:\t");
	MYSERIAL_PRINT(IMU.getTemp());
	MYSERIAL_PRINTLN_PGM("'C");

	// print current error state
	MYSERIAL_PRINT_PGM("Errors:\t");
	ERROR.printCurrent();

	// print current mode
	MYSERIAL_PRINT_PGM("Mode:\t");
	MYSERIAL_PRINTLN(modeNames[settings.mode]);

	// print whether motors are enabled/disabled
	MYSERIAL_PRINT_PGM("Motors:\t");
	MYSERIAL_PRINTLN(disabled_enabled[settings.motorEn]);
}


// serial instructions
void serial_SerialInstructions(int val)
{
	printDeviceInfo();					// print board & firmware info
	
	// TITLE
	MYSERIAL_PRINT_PGM("\n");
	MYSERIAL_PRINTLN_PGM("         Open Bionics Serial Commands - Beetroot");
	for (uint8_t i = 0; i < 60; i++)
	{
		MYSERIAL_PRINT_PGM("_");
	}
	MYSERIAL_PRINT_PGM("\n\n");

	// GRIPS
	MYSERIAL_PRINTLN_PGM("Grip Control (G#, K#)");
	MYSERIAL_PRINTLN_PGM("Command     Description");

	for (int gNum = 0; gNum < NUM_GRIPS; gNum++)
	{
		MYSERIAL_PRINT_PGM("G");
		MYSERIAL_PRINT(gNum);
		MYSERIAL_PRINT_PGM("         ");
		if (gNum < 10)						// fix alignment issue when digit only has one number (e.g. G3__### or G10_###)
		{
			MYSERIAL_PRINT_PGM(" ");
		}
		MYSERIAL_PRINT(Grip.getGripName(gNum));
		MYSERIAL_PRINT_PGM("\n");
	}
	MYSERIAL_PRINT_PGM("\n");

	// FINGER CONTROL
	MYSERIAL_PRINTLN_PGM("Finger Control (F#, P##, S##)");
	MYSERIAL_PRINTLN_PGM("Command     Description");
	MYSERIAL_PRINTLN_PGM("F#          Move finger, # is the finger number (F0 - F3)");
	MYSERIAL_PRINTLN_PGM("F# P50      Finger # to position 50 (P0 - P100)");
	MYSERIAL_PRINTLN_PGM("F# S255     Finger # at speed 255 (S0 - S255)");
	MYSERIAL_PRINT_PGM("\n");

	// MUSCLE CONTROL
	MYSERIAL_PRINTLN_PGM("Muscle Control (M#, U#, T#, N)");
	MYSERIAL_PRINTLN_PGM("Command     Description");
	MYSERIAL_PRINTLN_PGM("M0          Muscle control OFF");
	MYSERIAL_PRINTLN_PGM("M1          Standard muscle control ON");
	MYSERIAL_PRINTLN_PGM("M2          Muscle position control ON");
	MYSERIAL_PRINTLN_PGM("M3          Toggle whether to display SIMPLE muscle readings");
	MYSERIAL_PRINTLN_PGM("M4          Toggle whether to display DETAILED muscle readings");
	MYSERIAL_PRINT_PGM("\n");

	// ADVANCED SETTINGS
	MYSERIAL_PRINTLN_PGM("Advanced Settings (H#, A#, ?)");
	MYSERIAL_PRINTLN_PGM("Command     Description");
	MYSERIAL_PRINTLN_PGM("H           View hand configuration (LEFT or RIGHT)");
	MYSERIAL_PRINTLN_PGM("H1          Set hand to be RIGHT");
	MYSERIAL_PRINTLN_PGM("H2          Set hand to be LEFT");
	MYSERIAL_PRINTLN_PGM("A0          Toggle 'Automatic Demo Mode' ON/OFF");
	MYSERIAL_PRINTLN_PGM("A1          Toggle 'Serial Commands Instructions' ON/OFF");
	MYSERIAL_PRINTLN_PGM("A2          Toggle 'Wait For Serial Connection' ON/OFF");
	MYSERIAL_PRINTLN_PGM("A3          Enable/Disable motors");
	MYSERIAL_PRINTLN_PGM("A4          Enable/Disable CSV mode (fast control)");
	MYSERIAL_PRINTLN_PGM("A5          Enable/Disable HANDle mode (Wii Nunchuck)");
	MYSERIAL_PRINTLN_PGM("A6          Get the position of all fingers as a CSV string");
	MYSERIAL_PRINTLN_PGM("#           Display system diagnostics");
	MYSERIAL_PRINTLN_PGM("?           Display serial commands list");
	MYSERIAL_PRINT_PGM("\n");

	// EXAMPLES
	MYSERIAL_PRINT_PGM("\n\n");
	MYSERIAL_PRINTLN_PGM("Examples");
	MYSERIAL_PRINTLN_PGM("H2          Set hand to be configured as a LEFT hand");
	MYSERIAL_PRINTLN_PGM("G0 O        Fist Grip Open");
	MYSERIAL_PRINTLN_PGM("G2 C        Thumbs Up Close");
	MYSERIAL_PRINTLN_PGM("F0 O        Thumb Open");
	MYSERIAL_PRINTLN_PGM("F0          Toggle thumb direction (open/close)");
	MYSERIAL_PRINTLN_PGM("F1 P50      Index to position 50");
	MYSERIAL_PRINTLN_PGM("F2 P50 S80  Middle finger to position 50 at speed 80");
	MYSERIAL_PRINTLN_PGM("F0 O S200   Thumb Open at speed 200");
	MYSERIAL_PRINTLN_PGM("A0          Enable/Disable demo mode");
	MYSERIAL_PRINT_PGM("\n\n");

	// MODE ESCAPE TEXT
	if (!settings.motorEn)			// enable motors
		MYSERIAL_PRINTLN_PGM("Motors disabled, enter 'A3' to re-enable them");

	printCurrentMode();				// print the current mode and the exit command

	MYSERIAL_PRINT_PGM("\n");
}

// print the current mode and the exit command
void printCurrentMode(void)
{
	switch (settings.mode)
	{
	case MODE_DEMO:
		MYSERIAL_PRINTLN_PGM("Automatic Demo Mode, enter 'A0' to disable");
		break;
	case MODE_EMG_SIMPLE:
		MYSERIAL_PRINTLN_PGM("Simple Muscle Control Mode, enter 'M0' to disable");
		break;
	case MODE_EMG_PROP:
		MYSERIAL_PRINTLN_PGM("Proportional Muscle Control Mode, enter 'M0' to disable");
		break;
	case MODE_CSV:
		MYSERIAL_PRINTLN_PGM("CSV Control Mode, enter 'A4' to disable");
		break;
	case MODE_HANDLE:
		MYSERIAL_PRINTLN_PGM("HANDle Mode, enter 'A5' to disable");
		break;
	default:
		break;
	}

	MYSERIAL_PRINT_PGM("\n");
}

