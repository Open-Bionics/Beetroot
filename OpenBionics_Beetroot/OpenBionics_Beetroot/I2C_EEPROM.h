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
*	I2C_EEPROM.h
*
*/

// 24AA08 8Kb (1KB) I2C EEPROM

#ifndef I2C_EEPROM_H_
#define I2C_EEPROM_H_

#include <Arduino.h>

// EEPROM SETTINGS
#define EEPROM_ADDR			0x50	// first address of device (occupies addresses 0x50 -> 0x53)
#define EEPROM_BLOCK_SIZE	256		// bytes
#define EEPROM_NUM_BLOCKS	4		// number of memory blocks (of size EEPROM_BLOCK_SIZE)
#define EEPROM_PAGE_SIZE	16		// bytes

#define EEPROM_TOTAL_SIZE	(EEPROM_BLOCK_SIZE * EEPROM_NUM_BLOCKS)		// total size of EEPROM

// WIRE FLAG
#define RESTART				0		// send RESTART instead of STOP

// CLASS 
class I2C_EEPROM
{	
	public:
		I2C_EEPROM();

		bool begin(void);			// initialise Wire and ping the device to confirm operation, return false if fails
		bool ping(void);			// check whether EEPROM is responding, returns false if no response

		int read(int loc);										// read a single value from EEPROM
		int readMany(int loc, int* val, int totalToRead);		// read many pieces of data from the EEPROM (by reading as a page)
		int readMany(int loc, uint8_t* val, int totalToRead);	// read many pieces of data from the EEPROM (by reading as a page), with the value passed as an int pointer

		int write(int addr, int val);							// write a single value to EEPROM
		int writeMany(int loc, int* val, int totalToWrite);		// write many pieces of data to EEPROM (by writing as a page)
		int writeMany(int loc, uint8_t* val, int totalToWrite);	// write many pieces of data to EEPROM (by writing as a page), with the value passed as an int pointer
		void writeAll(int loc, int val, int totalToWrite);		// set all values from 'start' to 'stop' to be 'val'
		void clearAll(int loc, int totalToClear);				// clear all values from 'start' to 'stop'

		int update(int loc, int val);							// only write the value if the value has been changed (return 0 = no update, 1 = update, -1 = error)

	private:

		uint8_t generateAddress(int loc);						// generate the EEPROM address, including block select bits (returns 0 if loc is not valid)
		int generateChunkSize(int currValLoc, int size);		// generate the size of the chunk, taking into account the page, block and end boundaries

};

extern I2C_EEPROM EEPROM;

#endif // I2C_EEPROM_H_