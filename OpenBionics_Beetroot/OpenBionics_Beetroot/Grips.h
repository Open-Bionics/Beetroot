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
*	Grips.h
*
*/

#ifndef GRIPS_H_
#define GRIPS_H_

#include "Grips_Default.h"

#define GRIP_MAX_COUNT_VAL	100		// a grip is divided up into 0 - 100 count values

#define GRIP_OPEN			0
#define GRIP_CLOSE			GRIP_MAX_COUNT_VAL


// a single step used to store the default grip
typedef struct	_GripStep
{
	uint16_t count;			// the count value for each grip movement (0 - 100, open - close)
	uint16_t *pos;			// pointer to the finger positions for each grip
} GripStep;

// a struct to store a single grip's num, name and finger positions
typedef struct _GripType
{
	uint8_t num;
	const char *name;

	GripStep step[NUM_GRIP_STEPS];
} GripType;



class GRIP_CLASS
{
	public:
		GRIP_CLASS();

		void begin(void);					// store all the default grips (default_GripPos) in _allGrips
		
		void setGrip(int gNum);				// set the current grip to the grip number gNum
		int getGrip(void);					// get the number of the current grip							
		char* getGripName(void);			// get the name of the current grip
		char* getGripName(int gNum);		// get the name of grip gNum
						
		int nextGrip(void);					// cycle to the next grip
		int prevGrip(void);					// cycle to the previous grip

		void open(void);					// open using the current grip
		void close(void);					// close using the current grip

		void setPos(int pos);				// set the target grip position (in steps)
		int getPos(void);					// get the target grip position (in steps)

		void setDir(int dir);				// set the grip to either be fully open or fully closed
		int getDir(void);					// get the grip direction
		int toggleDir(void);				// toggle the grip direction between open and close

		void setSpeed(int speed);			// set the speed at which the fingers move
		int getSpeed(void);					// get the target speed of the fingers

		void run(void);						// calculate the target position for each finger depending on the target step number (_pos)



		// DEBUG
		uint16_t _pos;						// target grip position (in steps)
		uint16_t _dir;						// target grip direction
	private:
		GripType _allGrips[NUM_GRIPS];		// all grip numbers, names and finger positions
		GripType *_currGrip = NULL;			// pointer to the current grip

		//uint16_t _pos;						// target grip position (in steps)
		//uint16_t _dir;						// target grip direction
		uint16_t _speed;					// target grip speed



};

extern GRIP_CLASS Grip;


#endif // GRIPS_H_
