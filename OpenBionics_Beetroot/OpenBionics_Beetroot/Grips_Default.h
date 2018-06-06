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
*	Grips_Default.h
*
*/

#ifndef GRIPS_DEFAULT_H_
#define GRIPS_DEFAULT_H_

#include <FingerLib.h>

#include "Globals.h"			// NUM_FINGERS

// GRIP SETTINGS
#define NUM_GRIPS		7
#define G0	0
#define G1	1
#define G2	2
#define G3	3
#define G4	4
#define G5	5
#define G6	6
#define NUM_GRIP_STEPS	6

// FINGER POSITIONS
#define FULLY_OPEN		MIN_FINGER_POS					// fully open position for the hand, from FingerLib.h
#define FULLY_CLOSED	MAX_FINGER_POS					// fully closed position for the hand, from FingerLib.h
#define BLANK			(-1)							// no position entered (ignored)
#define MID_POS			((FULLY_CLOSED - FULLY_OPEN)/2)	// mid position between fully open and fully closed

// GRIP NAMES AND POSITIONS
extern const char* default_GripNames[NUM_GRIPS + 1];								// grip names + empty string
extern uint16_t default_GripPos[NUM_GRIPS][NUM_GRIP_STEPS][NUM_FINGERS + 1];		// array to hold the finger positions for each grip




#endif // GRIPS_DEFAULT_H_