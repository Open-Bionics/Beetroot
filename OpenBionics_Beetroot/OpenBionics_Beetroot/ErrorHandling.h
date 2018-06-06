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
*	ErrorHandling.h
*
*/

#ifndef ERROR_HANDLING_H_
#define ERROR_HANDLING_H_

#include <Arduino.h>

#include "TimerManagement.h"

#define MAX_NUM_ERRORS		15			// maximum number of error types
#define WARNING_DURATION	5000		// number of ms to display warning status on the LED

#define EEPROM_LOC_STORED_ERROR		1023			// location within EEPROM to store the last error state


typedef enum _ErrorType
{
	ERROR_NONE = 0,			// 0 no error
	ERROR_UNKNOWN,			// 1 unknown error occurred
	ERROR_INIT,				// 2 in this state during initialisation
	ERROR_EEPROM_INIT,		// 3 EEPROM failed to respond during getBoardVersion()
	ERROR_FINGER_INIT,		// 4 finger pins fail to initialise
	ERROR_EEPROM_SETTINGS,	// 5 EEPROM settings overflow
	ERROR_S_BUFF_OVFLOW,	// 6 serial buffer overflow
	ERROR_TEMP_WARNING,		// 7 warning CPU temperature has been reached
	ERROR_TEMP_MAX			// 8 maximum CPU temperature has been reached
} ErrorType;

typedef enum _ErrorLevel
{
	LEVEL_NONE = 0,			// no error level
	LEVEL_DEBUG,			// debug level
	LEVEL_INFO,				// info level
	LEVEL_WARN,				// warning level
	LEVEL_ERROR,			// error level
	LEVEL_FATAL				// fatal error level
} ErrorLevel;


typedef struct _ErrorLED
{
	uint32_t colour;			// LED error state colour
	double blinkFreq = 0.0;		// blink frequency (0.0 = no blink)
} ErrorLED;

typedef struct _ErrorState
{
	uint16_t num;				// error number
	ErrorType type;				// error state
	ErrorLevel level;			// error/message level
	char *description = NULL;	// error description
	ErrorLED LED;				// LED colour and blink rate
} ErrorState;




class ERROR_HANDLING
{
	public:
		ERROR_HANDLING();

		void begin(void);					// initialise error handling and display any stored errors

		void set(ErrorType error);			// set an error state and perform the appropriate actions
		void clear(void);					// clear the error state

		void run(void);						// clear any warnings after a set period and check for fatal errors

		ErrorType get(void);				// get the current error state
		bool isSet(ErrorType error);		// check whether a specific error state is set
		void printCurrent(void);			// print the current error state

		void checkPrevError(void);			// check EEPROM for previous errors

		void enableLEDcontrol();			// enable error handling to control the tri-colour LED (NeoPixel)
		void disableLEDcontrol();			// disable error handling to control the tri-colour LED (NeoPixel)

		ErrorState loadError(void);							// read the stored error from EEPROM
		void storeError(ErrorType error);					// store the error in EEPROM

	private:
		ErrorState *_currError = NULL;						// current error
		ErrorState _errorList[MAX_NUM_ERRORS];				// list of all errors

		MS_NB_DELAY warnTimer;									// timer used to clear warning after a set duration

		bool _ctrlLED;										// flag to determine whether err handling has control over the LED

		void printErrorDescr(ErrorState *error);			// print the error number and description
};

extern ERROR_HANDLING ERROR;



#endif // ERROR_HANDLING_H_