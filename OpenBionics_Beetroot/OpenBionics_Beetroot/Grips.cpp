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
*	Grips.cpp
*
*/

#include <FingerLib.h>

#include "Globals.h"			// NUM_FINGERS
#include "Grips.h"

#include "Grips_Default.h"


////////////////////////////// Constructors/Destructors //////////////////////////////
GRIP_CLASS::GRIP_CLASS()
{

}

////////////////////////////// Public Methods //////////////////////////////

// store all the default grips (default_GripPos) in _allGrips
void GRIP_CLASS::begin(void)
{
	// store the default grips in the _allGrips struct
	for (int gNum = 0; gNum < NUM_GRIPS; gNum++)
	{
		_allGrips[gNum].num = gNum;
		_allGrips[gNum].name = default_GripNames[gNum];

		for (int sNum = 0; sNum < NUM_GRIP_STEPS; sNum++)
		{
			_allGrips[gNum].step[sNum].count = default_GripPos[gNum][sNum][0];	// store the count value
			_allGrips[gNum].step[sNum].pos = &default_GripPos[gNum][sNum][1];	// store a pointer to the pos values
		}
	}

	// clear all values
	_pos = 0;
	_dir = OPEN;
	_speed = MAX_FINGER_PWM;

	// set current grip to be the very first grip
	_currGrip = &_allGrips[0];
}


// set the current grip to the grip number gNum
void GRIP_CLASS::setGrip(int gNum)
{
	gNum = constrain(gNum, 0, NUM_GRIPS);
	_currGrip = &_allGrips[gNum];
}

// get the number of the current grip
int GRIP_CLASS::getGrip(void)
{
	return _currGrip->num;
}

// get the name of the current grip
char* GRIP_CLASS::getGripName(void)
{
	return (char*)_currGrip->name;
}

// get the name of grip gNum
char* GRIP_CLASS::getGripName(int gNum)
{
	return (char*)_allGrips[gNum].name;
}

// cycle to the next grip
int GRIP_CLASS::nextGrip(void)
{
	// if the last grip has been reached, wrap around
	if (_currGrip->num + 1 >= NUM_GRIPS)
	{
		_currGrip = &_allGrips[0];
	}
	else
	{
		_currGrip = &_allGrips[_currGrip->num + 1];
	}

	return _currGrip->num;
}

// cycle to the previous grip
int GRIP_CLASS::prevGrip(void)
{
	// if the first grip has been reached, wrap around
	if (_currGrip->num <= 0)
	{
		_currGrip = &_allGrips[NUM_GRIPS - 1];
	}
	else
	{
		_currGrip = &_allGrips[_currGrip->num - 1];
	}

	return _currGrip->num;
}

// open using the current grip
void GRIP_CLASS::open(void)
{
	setDir(OPEN);
	run();
}

// close using the current grip
void GRIP_CLASS::close(void)
{
	setDir(CLOSE);
	run();
}

// set the target grip position (in steps)
void GRIP_CLASS::setPos(int pos)
{
	_pos = constrain(pos, 0, 100);
}

// get the target grip position (in steps)
int GRIP_CLASS::getPos(void)
{
	return _pos;
}

// set the grip to either be fully open or fully closed
void GRIP_CLASS::setDir(int dir)
{
	_dir = constrain(dir, CLOSE, OPEN);

	if (_dir == OPEN)
	{
		setPos(GRIP_OPEN);
	}
	else if (_dir == CLOSE)
	{
		setPos(GRIP_CLOSE);
	}
}

// get the grip direction
int GRIP_CLASS::getDir(void)
{
	return _dir;
}

// toggle the grip direction between open and close
int GRIP_CLASS::toggleDir(void)
{
	setDir(!getDir());

	return getDir();
}

// set the speed at which the fingers move
void GRIP_CLASS::setSpeed(int speed)
{
	_speed = constrain(speed, OFF_FINGER_PWM, MAX_FINGER_PWM);
}

// get the target speed of the fingers
int GRIP_CLASS::getSpeed(void)
{
	return _speed;
}

// calculate the target position for each finger depending on the target step number (_pos)
void GRIP_CLASS::run(void)
{
	uint16_t fingerNum;

	uint16_t stepNum;
	uint16_t posA, posB;
	uint16_t countA, countB;
	uint16_t stepModA = 0, stepModB = 0;

	uint16_t currentCountVal, nextCountVal;

	for (stepNum = 0; stepNum < NUM_GRIP_STEPS; stepNum++)				// count through COUNT rows (0 - 5)
	{
		currentCountVal = _currGrip->step[stepNum].count;				// read first COUNT val
		nextCountVal = _currGrip->step[stepNum + 1].count;				// read next COUNT val

		if (IS_BETWEEN(_pos, currentCountVal, nextCountVal))			// if grip pos is between the two COUNT vals
		{
			for (fingerNum = 0; fingerNum < NUM_FINGERS; fingerNum++)		// count through each finger element
			{
				do														// search upwards for a finger position that isn't BLANK
				{
					posA = _currGrip->step[(stepNum - stepModA)].pos[fingerNum];		// read finger position from stored grip struct

					if (posA == (uint16_t)BLANK)						// if finger position is blank
					{
						stepModA++;										// look above for previous finger position
					}
				} while (posA == (uint16_t)BLANK);

				do														// search below for a finger position that isn't BLANK
				{
					posB = _currGrip->step[(stepNum + 1 + stepModB)].pos[fingerNum];	// read finger position from stored grip struct

					if (posB == (uint16_t)BLANK) 						// if finger position is blank
					{
						stepModB++;										// look below for next finger position
					}
				} while (posB == (uint16_t)BLANK);


				// stepMod - used to find the first finger pos (above or below) that isn't BLANK
				// stepNum - row number of COUNT (0-5) that grip position is in between
				countA = _currGrip->step[(stepNum - stepModA)].count;					// get below COUNT val
				countB = _currGrip->step[(stepNum + 1 + stepModB)].count;				// get above COUNT val

				finger[fingerNum].writePos(map(_pos, countA, countB, posA, posB));		// map finger pos using below and above COUNT vals

				finger[fingerNum].writeSpeed(_speed);									// set the finger speed

				stepModA = 0;
				stepModB = 0;

			}
			break;				// if grip pos is between the two COUNT vals, exit from the search as grip management is complete
		}
	}

	// set current direction of the grip by using the average of all finger positions
	if (_pos > (GRIP_CLOSE / 2))
	{
		_dir = CLOSE;
	}
	else
	{
		_dir = OPEN;
	}
}


GRIP_CLASS Grip;