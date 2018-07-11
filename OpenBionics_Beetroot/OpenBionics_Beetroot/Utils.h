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
*	Utils.h
*
*/

#ifndef UTILS_H_
#define UTILS_H_

#include "Globals.h"
#include "I2C_EEPROM.h"

///////////////////////////////////// EEPROM WRITE STRUCT ///////////////////////////////////////
// write any data type to EEPROM
template <class T> int EEPROM_writeStruct(int ee, const T& value)
{
	const byte* p = (const byte*)(const void*)&value;

	EEPROM.writeMany(ee, (uint8_t*)p, sizeof(value));
	//EEPROM.writeMany(ee, (int*)p, sizeof(value));

	return true;
}

// read any data type from EEPROM
template <class T> int EEPROM_readStruct(int ee, const T& value)
{
	byte* p = (byte*)(void*)&value;

	EEPROM.readMany(ee, (uint8_t*)p, sizeof(value));
	//EEPROM.readMany(ee, (int*)p, sizeof(value));

	return true;
}


///////////////////////////////////// SERIAL PRINT FROM PROGMEM ///////////////////////////////////////
// STORES SERIAL STRINGS IN PROGMEM, SAVES 3KB RAM

/* If defined, use SerialUSB for comms, otherwise Serial (untested). */
/* The app uses only the MYSERIAL_funcion macros so that all accesses can be
 * intercepted here to avoid blocking behaviour. */

#if defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_UNO)
#define MYSERIAL Serial
#define SERIALNONBLOCKCHECK			(1)
#elif defined(ARDUINO_ARCH_SAMD)
#if defined(SERIAL_USB_CONTROL)
#define MYSERIAL SerialUSB
#define SERIALNONBLOCKCHECK			(MYSERIAL.dtr())
#elif defined(SERIAL_PINS_CONTROL)
#define MYSERIAL SerialPins
#define SERIALNONBLOCKCHECK			(1)
#elif defined(SERIAL_JACK_CONTROL)
#define MYSERIAL SerialJack
#define SERIALNONBLOCKCHECK			(1)
#else
#define MYSERIAL SerialUSB		// default to SerialUSB control
#define SERIALNONBLOCKCHECK			(MYSERIAL.dtr())
#endif
#else
#error "Board not supported. Serial not able to be configured
#endif

#define	FORCE_INLINE __attribute__((always_inline)) inline
FORCE_INLINE void serialprintPGM(const char *str)
{
	if( SERIALNONBLOCKCHECK )
	{
		char ch = pgm_read_byte(str);
		while (ch)
		{
			MYSERIAL.write(ch);
			ch = pgm_read_byte(++str);
		}
	}
}

#define MYSERIAL_PRINT(x)\
	if( SERIALNONBLOCKCHECK )\
	{\
		MYSERIAL.print(x);\
	}
#define MYSERIAL_PRINT_F(x,y)\
	if( SERIALNONBLOCKCHECK )\
	{\
		MYSERIAL.print(x,y);\
	}
#define MYSERIAL_PRINTLN(x) \
	if( SERIALNONBLOCKCHECK )\
	{\
		MYSERIAL.print(x);\
		MYSERIAL.write('\n');\
	}

#define MYSERIAL_PRINT_PGM(x) serialprintPGM(PSTR(x));
#define MYSERIAL_PRINTLN_PGM(x) do{serialprintPGM(PSTR(x));serialprintPGM("\n");} while(0)

#define MYSERIAL_BEGIN(reate) MYSERIAL.begin(rate)
#define MYSERIAL_AVAILABLE() MYSERIAL.available()
#define MYSERIAL_READ() MYSERIAL.read()



///////////////////////////////////// NUMBER & DIGITS ///////////////////////////////////////
bool isEven(int n);								// returns true if n is even
unsigned int getNumberOfDigits(unsigned int i);	// get the number of digits in an unsigned int
				

///////////////////////////////////// CSV ///////////////////////////////////////
void convertToCSV(int *valArray, int len, char* outString);	// converts a number (len) of integer variables from valArray to a CSV string (outString)
int convertFromCSV(char *inString, int *valArray, int len);	// converts a CSV line from string to individual values within int array, returns number of variables


///////////////////////////////////// TIME CONVERSION ///////////////////////////////////////
void printTime_ms(uint32_t ms);		// print time in dd:hh:mm:ss format when passed the time in ms


#endif // UTILS_H_
