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

// ERRORS
#define MAX_NUM_ERRORS				15			// maximum number of error types

// WARNING
#define WARNING_DURATION			5000		// number of ms to display warning status on the LED

// EEPROM
#define EEPROM_LOC_STORED_ERROR		1023		// location within EEPROM to store the last error state

// LED
#define NO_LED_COLOUR				((uint32_t)(-1))
#define OVRIDE_LVL_LED				100			// LED brightness override level

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
	ERROR_TEMP_MAX,			// 8 maximum CPU temperature has been reached
	ERROR_WATCHDOG,			// 9 Watchdog timer triggered
} ErrorType;

typedef enum _ErrorLevel
{
	LEVEL_NONE = 0,			// no error level
	LEVEL_DEBUG,			// debug level
	LEVEL_INFO,				// info level
	LEVEL_WARN,				// warning level
	LEVEL_ERROR,			// error level
	LEVEL_FATAL,			// fatal error level
} ErrorLevel;


typedef struct _ErrorLED
{
	uint32_t c1 = NO_LED_COLOUR;	// LED error state colour1
	uint32_t c2 = NO_LED_COLOUR;	// LED error state colour2
	double blinkFreq = 0.0;			// blink frequency (0.0 = no blink)
} ErrorLED;

typedef struct _ErrorState
{
	uint16_t num;					// error number
	ErrorType type;					// error state
	ErrorLevel level;				// error/message level
	char *description = nullptr;	// error description
	ErrorLED LED;					// LED c1 and blink rate
	double duration = 0.0;			// duration (in seconds) to display error (0 = constant)
} ErrorState;




class ERROR_HANDLING
{
	public:
		ERROR_HANDLING();

		void begin(void);					// initialise error handling and display any stored errors

		void set(ErrorType error);			// set an error state and perform the appropriate actions
		//void clear(void);					// clear the error state
		bool clear(ErrorType error);		// only clear the error state if it is the same as the one passed to the function


		void run(void);						// clear any warnings after a set period and check for fatal errors

		ErrorType get(void);				// get the current error state
		bool isSet(ErrorType error);		// check whether a specific error state is set
		void printCurrent(bool nl = true);	// print the current error state

		void checkPrevError(void);			// check EEPROM for previous errors

		void enableLEDcontrol(bool en);		// enable error handling to control the tri-c1 LED (NeoPixel)

		ErrorState loadError(void);							// read the stored error from EEPROM
		void storeError(ErrorType error);					// store the error in EEPROM

	private:
		ErrorState *_currError = NULL;						// current error
		ErrorState _errorList[MAX_NUM_ERRORS];				// list of all errors

		uint8_t _tempLvlLED;								// value of the LED brightness before the override 

		MS_NB_DELAY errorDuration;							// timer used to clear warning after a set duration

		bool _ctrlLED;										// flag to determine whether err handling has control over the LED

		void printErrorDescr(ErrorState *error, bool nl = true);	// print the error number and description
};

extern ERROR_HANDLING ERROR;



#endif // ERROR_HANDLING_H_