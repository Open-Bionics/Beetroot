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
*	Utils.cpp
*
*/

#include "Utils.h"

// returns true if n is even
bool isEven(int n)
{
	if (n % 2 == 0)
		return 1;
	else
		return 0;
}

// get the number of digits in an unsigned int
unsigned int getNumberOfDigits(unsigned int i)
{
	return i > 0 ? (int)log10((double)i) + 1 : 1;
}


// converts a number (len) of integer variables from valArray to a CSV string (outString)
void convertToCSV(int *valArray, int len, char* outString)
{
	const char* CSV_seperator = ",";  // char used to separate values

	for (int i = 0; i < len; i++)     // count through values from valArray
	{
		itoa(valArray[i], outString, 10);              // convert array value to string and store in CSV string
		outString += getNumberOfDigits(valArray[i]);   // increment pointer location by the number of digits stored
		*outString++ = *CSV_seperator;                 // add CSV separator
	}
	*outString = NULL;                 // terminate string

}

// converts a CSV line from string to individual values within int array, returns number of variables
int convertFromCSV(char *inString, int *valArray, int len)
{
	const char* CSV_seperator = ",";
	char *tmpVal, *tmpLoc;
	int tokenIndex = 0;
		
	if (*inString == NULL)		// if char is NULL, return
		return 0;

	tmpVal = strtok_r(inString, CSV_seperator, &tmpLoc);	// get pointer of first token

	// while there are still unread tokens in the CSV string
	while (tmpVal)
	{
		valArray[tokenIndex++] = atoi(tmpVal);				// convert the token string to an int and store in the array

		if (tokenIndex >= len)								// if the array has now been filled with values
			break;											// stop searching for more values

		tmpVal = strtok_r(NULL, CSV_seperator, &tmpLoc);	// continue searching through string
	}
	return tokenIndex;										// return number of CSV values stored
}



//// read the CPU temperature, in °C (inaccurate)
//float readCPUTemp(void)
//{
//	float deltaT = CPU_TEMP_TMAX - CPU_TEMP_TMIN;
//	float deltaV = CPU_TEMP_VMAX - CPU_TEMP_VMIN;
//	float Vmes;
//	uint32_t ADCval;
//
//	// prevent other ADC reads from interrupting this temperature read
//	noInterrupts();
//
//	syncADC();
//
//	// Enable the temperature sensor
//	SYSCTRL->VREF.bit.TSEN = 1;
//	syncADC();
//
//	// Set Mux to temperature sensor
//	ADC->INPUTCTRL.bit.MUXPOS = 0x18;
//
//	// Enable ADC
//	syncADC();
//	ADC->CTRLA.bit.ENABLE = 0x01;
//
//	// Start conversion
//	syncADC();
//	ADC->SWTRIG.bit.START = 1;
//
//	// Clear the Data Ready flag
//	ADC->INTFLAG.bit.RESRDY = 1;
//
//	// Start conversion again, since The first conversion after the reference is changed must not be used.
//	syncADC();
//	ADC->SWTRIG.bit.START = 1;
//
//	// Store the value
//	while (ADC->INTFLAG.bit.RESRDY == 0);   // Waiting for conversion to complete
//	ADCval = ADC->RESULT.reg;
//
//	// Disable ADC
//	syncADC();
//	ADC->CTRLA.bit.ENABLE = 0x00;
//
//	syncADC();
//
//	Vmes = (CPU_TEMP_AREF / pow(2, CPU_TEMP_ARES)) * ADCval;
//
//	// re-enable interrupts
//	interrupts();
//
//	return CPU_TEMP_CMIN + (Vmes - CPU_TEMP_VOUTMAX) * (deltaT / deltaV);
//}

// print time in dd:hh:mm:ss format when passed the time in ms
void printTime_ms(uint32_t ms)
{
	uint32_t s;
	uint8_t d, h, m;

	// convert ms to s
	s = ms / 1000;

	// days
	d = s / 86400;
	s = s % 86400;

	// hours
	h = s / 3600;
	s = s % 3600;

	// minutes
	m = s / 60;

	// seconds
	s = s % 60;


	if (d < 10)
	{
		MYSERIAL_PRINT_PGM("0");
	}
	MYSERIAL_PRINT(d);
	MYSERIAL_PRINT_PGM(":");

	if (h < 10)
	{
		MYSERIAL_PRINT_PGM("0");
	}
	MYSERIAL_PRINT(h);
	MYSERIAL_PRINT_PGM(":");

	if (m < 10)
	{
		MYSERIAL_PRINT_PGM("0");
	}
	MYSERIAL_PRINT(m);
	MYSERIAL_PRINT_PGM(":");

	if (s < 10)
	{
		MYSERIAL_PRINT_PGM("0");
	}
	MYSERIAL_PRINT(s);
}