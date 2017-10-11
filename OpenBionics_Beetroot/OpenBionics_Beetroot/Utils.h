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

#include "I2C_EEPROM.h"

///////////////////////////////////// CPU TEMP CONSTANTS ///////////////////////////////////////
#define CPU_TEMP_CMIN		25.0	// °C   Nominal operating temperature
#define CPU_TEMP_TMIN		-40.0	// °C	Minimum operating temperature
#define CPU_TEMP_TMAX		85.0	// °C	Maximum operating temperature
#define CPU_TEMP_VMAX		3.63	// V	Minimum operating voltage
#define CPU_TEMP_VMIN		1.62	// V	Maximum operating voltage
#define CPU_TEMP_VOUTMAX	0.667	// V	Temperature sensor output voltage
#define CPU_TEMP_AREF		3.3		// V	Analogue reference
#define CPU_TEMP_ARES		10		// bit	ADC resolution

float readCPUTemp(void);			// read the CPU temperature, in °C


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
#define	FORCE_INLINE __attribute__((always_inline)) inline
#define MYSERIAL			SerialUSB

FORCE_INLINE void serialprintPGM(const char *str)
{
	char ch = pgm_read_byte(str);
	while (ch)
	{
		MYSERIAL.write(ch);
		ch = pgm_read_byte(++str);
	}
}


#define MYSERIAL_PRINT(x)\
  if(MYSERIAL.dtr())\
  {\
	MYSERIAL.print(x);\
  }
#define MYSERIAL_PRINT_F(x,y)\
  if(MYSERIAL.dtr())\
  {\
    MYSERIAL.print(x,y);\
  }
#define MYSERIAL_PRINTLN(x) \
  if(MYSERIAL.dtr())\
  {\
	MYSERIAL.print(x);\
	MYSERIAL.write('\n');\
  }

#define MYSERIAL_PRINT_PGM(x) serialprintPGM(PSTR(x));
#define MYSERIAL_PRINTLN_PGM(x) do{serialprintPGM(PSTR(x));MYSERIAL.write('\n');} while(0)


///////////////////////////////////// NUMBER & DIGITS ///////////////////////////////////////
bool isEven(int n);								// returns true if n is even
unsigned int getNumberOfDigits(unsigned int i);	// get the number of digits in an unsigned int
				

///////////////////////////////////// CSV ///////////////////////////////////////
void convertToCSV(int *valArray, int len, char* outString);	// converts a number (len) of integer variables from valArray to a CSV string (outString)
int convertFromCSV(char *inString, int *valArray, int len);	// converts a CSV line from string to individual values within int array, returns number of variables


///////////////////////////////////// TIME CONVERSION ///////////////////////////////////////
void printTime_ms(uint32_t ms);		// print time in dd:hh:mm:ss format when passed the time in ms


#endif // UTILS_H_