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
*	Grips_Default.cpp
*
*/

#include "Grips_Default.h"

// grip names + empty string
const char* default_GripNames[NUM_GRIPS + 1] = {
	"Fist", \
	"Hook", \
	"Point", \
	"Pinch", \
	"Tripod", \
	"Finger Roll", \
	"Thumb Roll"
	""		// EMPTY STR
};  


#define OPPOSE_PINCH	680	// 480
#define OPPOSE_TRIPOD	770	// 520


// array to hold the finger positions for each grip
uint16_t default_GripPos[NUM_GRIPS][NUM_GRIP_STEPS][NUM_FINGERS + 1] = {		 
	{	// FIST
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//OPEN
		{ 2,		750,			BLANK,			BLANK,			BLANK },		//MIDPOINTA
		{ 20,		BLANK,			FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//MIDPOINTB
		{ 40,		750,			BLANK,			BLANK,			BLANK },		//MIDPOINTC
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTD
		{ 100,		800,			FULLY_CLOSED,	FULLY_CLOSED,	850 },			//CLOSED
	},
	{	// HOOK
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//OPEN
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTA
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTB
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTC
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTD
		{ 100,		FULLY_OPEN,		FULLY_CLOSED,	FULLY_CLOSED,	850 },			//CLOSED
	},
	{	// POINT
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		800,			FULLY_OPEN,		FULLY_CLOSED,	850 },			//OPEN
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTA
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTB
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTC
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTD
		{ 100,		800,			FULLY_CLOSED,	FULLY_CLOSED,	850 },			//CLOSED
	},
	{	// PINCH
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//OPEN
		{ 10,		OPPOSE_PINCH,	BLANK,			BLANK,			BLANK },		//MIDPOINTA
		{ 79,		OPPOSE_PINCH,	BLANK,			BLANK,			BLANK },		//MIDPOINTB
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTC
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTD
		{ 100,		OPPOSE_PINCH,	FULLY_CLOSED,	FULLY_OPEN,		FULLY_OPEN },	//CLOSED
	},

	{	// TRIPOD
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//OPEN
		{ 2,		OPPOSE_TRIPOD,	BLANK,			BLANK,			BLANK },		//MIDPOINTA
		{ 85,		OPPOSE_TRIPOD,	BLANK,			BLANK,			BLANK },		//MIDPOINTB
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTC
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTD
		{ 100,		OPPOSE_TRIPOD,	FULLY_CLOSED,	FULLY_CLOSED,	FULLY_OPEN },	//CLOSED
	},
	{	// FINGER ROLL
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//OPEN
		{ 20,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		BLANK },		//MIDPOINTA
		{ 40,		FULLY_OPEN,		FULLY_OPEN,		BLANK,			850 },			//MIDPOINTB
		{ 60,		FULLY_OPEN,		BLANK,			FULLY_CLOSED,	850 },			//MIDPOINTC
		{ 80,		BLANK,			FULLY_CLOSED,	FULLY_CLOSED,	850 },			//MIDPOINTD
		{ 100,		800,			FULLY_CLOSED,	FULLY_CLOSED,	850 },			//CLOSED
	},
	{	// THUMB ROLL
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//OPEN
		{ 20,		800,			FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//MIDPOINTA
		{ 40,		800,			FULLY_CLOSED,	FULLY_OPEN,		FULLY_OPEN },	//MIDPOINTB
		{ 60,		800,			FULLY_CLOSED,	FULLY_CLOSED,	FULLY_OPEN },	//MIDPOINTC
		{ 80,		800,			FULLY_CLOSED,	FULLY_CLOSED,	850 },			//MIDPOINTD
		{ 100,		800,			FULLY_CLOSED,	FULLY_CLOSED,	850 },			//CLOSED
	},
};