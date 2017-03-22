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
*	CircleBuff.h
*
*/

#ifndef CIRCLE_BUFFER_H_
#define CIRCLE_BUFFER_H_

#include <Arduino.h>

// MUST BE A POWER OF 2
//#define C_BUFF_MAX_SIZE		64				// default buffer size  (2^6)
#define C_BUFF_MAX_SIZE		128				// default buffer size  (2^7)
//#define C_BUFF_MAX_SIZE		255				  // default buffer size  (2^8)
//#define C_BUFF_MAX_SIZE		512				// default buffer size  (2^9)
//#define C_BUFF_MAX_SIZE		1024			// default buffer size  (2^10)
//#define C_BUFF_MAX_SIZE		2048			// default buffer size  (2^11)
//#define C_BUFF_MAX_SIZE		4096			// default buffer size  (2^12)
//#define C_BUFF_MAX_SIZE		8192			// default buffer size  (2^13)

// uncomment the following to enable this library to print
#define ENABLE_CBUFF_PRINT

class CIRCLE_BUFFER
{
	public:
		CIRCLE_BUFFER(uint16_t bufferSize);		// create an instance of a circular buffer of size 'size' (size must be a power or 2)

		uint16_t read(void);					// read the first (oldest) value from the buffer and increment read index
		uint16_t readGlimpse(void);				// same as above but don't increment read index
		uint16_t readElement(uint16_t index);	// read a specific element within the buffer
		uint32_t readMean(void);				// read the mean of the buffer
		uint32_t readTotal(void);				// read the total/sum of the buffer
		uint16_t readBufferSize(void);			// read the actual buffer size (power of 2)
		
		uint16_t getCurrentReadIndex(void);
		uint16_t getCurrentWriteIndex(void);
		
		void write(uint16_t value);				// write to the buffer, recalculate the total and the mean (and std dev if enabled)
		void destroy(void);						// destroy the buffer by freeing the memory

#ifdef ENABLE_CBUFF_PRINT
		void printBuff(void);					// print the contents of the buffer
#endif
			
	private:
		// buffer metrics
		uint16_t buffSize;
		uint16_t buffSizeMask;
		uint16_t buff[C_BUFF_MAX_SIZE] = {0};

		// read and write index
		uint16_t readIndex;
		uint16_t writeIndex;

		// averaging buffers
		uint32_t total = 0;		// running total
		uint32_t mean = 0;		// running mean
		
		bool isPowerOfTwo (uint16_t x);				// return 1 if 'x' is a power of 2
};


#endif // CIRCLE_BUFFER_H_
