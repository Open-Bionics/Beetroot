/*	Open Bionics - Beetroot
*	Author - Olly McBride
*	Date - November 2016
*
*	This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
*	To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
*
*	Website - http://www.openbionics.com/
*	GitHub - https://github.com/Open-Bionics
*	Email - ollymcbride@openbionics.com
*
*	CircleBuff.cpp
*
*/

#include <Arduino.h>			// for type definitions


#include "CircleBuff.h"

#ifdef ENABLE_CBUFF_PRINT
#include <FingerLib.h>
#include "Globals.h"
#endif

////////////////////////////// Constructors/Destructors //////////////////////////////

// create an instance of a circular buffer of size 'size' (size must be a power or 2)
CIRCLE_BUFFER::CIRCLE_BUFFER(uint16_t size)
{
	// if buffer size is not a power of 2, increase size until it is
	while(!isPowerOfTwo(size))
		size ++;
				
	// limit array size to maximum allocated size
	if(size > C_BUFF_MAX_SIZE)
		size = C_BUFF_MAX_SIZE;
		
	// set circle buffer size
	buffSize = size;
	buffSizeMask = (buffSize-1);

	// initialise read/write index
 	readIndex = 0;
 	writeIndex = 0;
	
	// clear maths variables
	total = 0;
	mean = 0;
}


////////////////////////////// Public Methods //////////////////////////////

// read the first (oldest) value from the buffer and increment read index
uint16_t CIRCLE_BUFFER::read(void)
{
	return buff[readIndex++ & buffSizeMask];
}

// same as above but don't increment read index (take a small peek, but don't affect the index)
uint16_t CIRCLE_BUFFER::readGlimpse(void)
{
	return buff[readIndex & buffSizeMask];
}

// read a specific element within the buffer
uint16_t CIRCLE_BUFFER::readElement(uint16_t index)
{
	return buff[index];
}

// read the mean of the buffer
uint32_t CIRCLE_BUFFER::readMean(void)
{
	return mean;
}

// read the total/sum of the buffer
uint32_t CIRCLE_BUFFER::readTotal(void)
{
	return total;
}

// read the actual buffer size (power of 2)
uint16_t CIRCLE_BUFFER::readBufferSize(void)
{
	return buffSize;
}

// write to the buffer, recalculate the total and the mean
void CIRCLE_BUFFER::write(uint16_t value)
{
	// remove first value from total, then add new value and calculate mean

	total -= (uint32_t)buff[writeIndex & buffSizeMask];				// remove last value from total
	total += (uint32_t)value;										// add new value to total

	mean = (uint32_t)total / (uint32_t)buffSize;					// calculate new mean

	buff[writeIndex++ & buffSizeMask] = value;						// add new value to buffer	
}

uint16_t CIRCLE_BUFFER::getCurrentReadIndex(void)
{
	return readIndex;
}

uint16_t CIRCLE_BUFFER::getCurrentWriteIndex(void)
{
	return writeIndex;
}

#ifdef ENABLE_CBUFF_PRINT
// print the contents of the buffer
void CIRCLE_BUFFER::printBuff(void)
{
	int i;
	
	MYSERIAL_PRINT_PGM("Printing buffer of size ");
	MYSERIAL_PRINT(buffSize);
	MYSERIAL_PRINTLN_PGM(" bytes");
	for(i=0;i<buffSize;i++)
	{
		MYSERIAL_PRINT_PGM("index ");
		MYSERIAL_PRINT(i);
		MYSERIAL_PRINT_PGM("  \tval ");
		MYSERIAL_PRINTLN(buff[i]);
	}
	
	MYSERIAL_PRINT_PGM("\nTotal ");
	MYSERIAL_PRINT(total);
	MYSERIAL_PRINT_PGM(" \tMean ");
	MYSERIAL_PRINTLN(mean);
	
	
	MYSERIAL_PRINTLN_PGM("******* COMPLETE ********");
}
#endif

// destroy the buffer by freeing the memory
void CIRCLE_BUFFER::destroy(void)
{
	free(buff);
}

////////////////////////////// Private Methods //////////////////////////////

// return 1 if 'x' is a power of 2
bool CIRCLE_BUFFER::isPowerOfTwo(uint16_t x)
{
	return ((x & (x - 1)) == 0);
}
