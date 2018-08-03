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
*	SerialControl.h
*
*/


#ifndef SERIAL_CONTROL_H_
#define SERIAL_CONTROL_H_

// SERIAL BUFFER
#define SERIAL_BUFF_SIZE	64		// number of serial chars to store for each serial string

// END OF LINE CHARS
#define SERIAL_EOL_CHAR_NL	'\n'	// end of line characters (new line)
#define SERIAL_EOL_CHAR_CR	'\r'	// end of line characters (carriage return)

// VALID SERIAL CHARS
#define ASCII_SPACE			0x20	// space character
#define ASCII_HASH			0x23	// # character
#define ASCII_COMMA			0x2C	// , character
#define ASCII_QMARK			0x3F	// ? character
#define ASCII_0				0x30	// 0 digit
#define ASCII_9				0x39	// 9 digit
#define ASCII_A				0x41	// A character
#define ASCII_z				0x7A	// z character

// CHAR CODES
#define NUM_SERIAL_CODES	16		// Number of different char codes (e.g. A, C, D, F, G ...)
#define SERIAL_CODE_A		0		// Advanced settings
#define SERIAL_CODE_C		1		// Close
#define SERIAL_CODE_D		2		// Demo mode
#define SERIAL_CODE_F		3		// Finger number
#define SERIAL_CODE_G		4		// Grip number
#define SERIAL_CODE_H		5		// Set hand to left/right
#define SERIAL_CODE_M		6		// EMG mode
#define SERIAL_CODE_O		7		// Open
#define SERIAL_CODE_P		8		// Finger position
#define SERIAL_CODE_R		9		// Reset to defaults
#define SERIAL_CODE_S		10		// Finger speed
#define SERIAL_CODE_T		11		// Muscle hold time
#define SERIAL_CODE_U		12		// Muscle peak threshold
#define SERIAL_CODE_X		13		// Exit mode
#define	SERIAL_CODE_HASH	14		// Print system diagnostics
#define SERIAL_CODE_QMARK	15		// Print serial instructions

// CODE VAL CONTRAINTS
#define NUM_ADV_SETTINGS	6		// number of advanced settings
#define NUM_EMG_MODES		3		// number of EMG modes
#define NUM_HAND_TYPES		3		// None, Left, Right
#define LIMIT_FOR_BOOLEAN	1		// either 0 or 1


typedef void(*serialFuncPtr)(int);

typedef struct _SerialCode
{
	char code;					// char code used as the prefix for each command value (i.e. G0)
	int val;					// value that is read after the char code

	bool newVal = false;		// flag to indicate if a new value has been detected
	int limit;					// max value, to constrain val
	serialFuncPtr func = NULL;	// function to run if a newVal is received

} SerialCode;


void initSerialCharCodes(void);			// assign the char codes for each serial command	

void pollSerial(void);					// check and read serial, then perform appropriate actions	
bool checkSerial(void);					// read and store serial chars, return true if end of line char is received
bool isValidChar(char rxChar);			// check if rxChar is alphanumeric, or a permitted character	
void extractCodesFromSerial(void);		// process serial buff and extract any char codes with their appended values
bool codeDetected(char code, char **codeLoc);	// search through serial buffer and look for char code, if found return true and store pointer to loc
int codeVal(char *codeLoc);						// get the numerical value following the char code at codeLoc

void processCodes(void);				// use the extracted codes & values to control the hand

void sendCSV(void);						// if CSV mode is enabled, send CSV of all finger positions
void receiveCSV(char *buff);			// if string contains CSV data, write received positions to the fingers





// CHAR CODE FUNCTIONS (the following functions are attached to the char codes)
void serial_AdvancedSettings(int setting);		// configure the advanced settings
void serial_ToggleDemoMode(int val);			// toggle demo mode
void serial_FingerControl(int fNum);			// finger control
void serial_GripControl(int gNum);				// grip control
void serial_SetHandType(int hType);				// set hand type (NONE, LEFT, RIGHT)
void serial_MuscleControlMode(int mMode);		// muscle control mode
void serial_HoldTime(int hTime);				// muscle hold time
void serial_PeakThresh(int pThresh);			// muscle peak threshold
void serial_ResetToDefaults(int val);			// reset to defaults
void serial_ExitMode(int val);					// exit modes
void serial_systemDiagnostics(int val);			// system diagnostics
void serial_SerialInstructions(int val = NULL);	// serial instructions

void printCurrentMode(void);					// print the current mode and the exit command

#endif // SERIAL_CONTROL_H_