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
	"Palm", \
	"Thumbs Up", \
	"Point", \
	"Pinch", \
	"Tripod", \
	"Finger Roll", \
	"Thumb Roll", \
	""		// EMPTY STR
};  


// array to hold the finger positions for each grip
uint16_t default_GripPos[NUM_GRIPS][NUM_GRIP_STEPS][NUM_FINGERS + 1] = {		 
	{	// FIST
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN},	//OPEN
		{ 10,		750,			BLANK,			BLANK,			BLANK },		//MIDPOINTA
		{ 40,		750,			BLANK,			BLANK,			BLANK },		//MIDPOINTB
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTC
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTD
		{ 100,		FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED },	//CLOSED
	},
	{	// PALM
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//OPEN
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTA
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTB
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTC
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTD
		{ 100,		FULLY_OPEN,		FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED },	//CLOSED
	},
	{
		// THUMBS
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED },	//OPEN
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTA
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTB
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTC
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTD
		{ 100,		FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED },	//CLOSED
	},
	{	// POINT
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_CLOSED,	FULLY_OPEN,		FULLY_CLOSED,	FULLY_CLOSED },	//OPEN
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTA
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTB
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTC
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTD
		{ 100,		FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED },	//CLOSED
	},
	{	// PINCH
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//OPEN
		{ 10,		620,			BLANK,			BLANK,			BLANK },		//MIDPOINTA
		{ 79,		620,			BLANK,			BLANK,			BLANK },		//MIDPOINTB
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTC
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTD
		{ 100,		700,			FULLY_CLOSED,	FULLY_OPEN,		FULLY_OPEN },	//CLOSED
	},
	{	// TRIPOD
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		700,			FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//OPEN
		{ 10,		700,			BLANK,			BLANK,			BLANK },		//MIDPOINTA
		{ 85,		700,			BLANK,			BLANK,			BLANK },		//MIDPOINTB
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTC
		{ BLANK,	BLANK,			BLANK,			BLANK,			BLANK },		//MIDPOINTD
		{ 100,		800,			FULLY_CLOSED,	FULLY_CLOSED,	FULLY_OPEN },	//CLOSED
	},
	{	// FINGER ROLL
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//OPEN
		{ 20,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		BLANK },		//MIDPOINTA
		{ 40,		FULLY_OPEN,		FULLY_OPEN,		BLANK,			FULLY_CLOSED },	//MIDPOINTB
		{ 60,		FULLY_OPEN,		BLANK,			FULLY_CLOSED,	FULLY_CLOSED },	//MIDPOINTC
		{ 80,		BLANK,			FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED },	//MIDPOINTD
		{ 100,		FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED },	//CLOSED
	},
	{	// FINGER ROLL 2
		//COUNT		F0				F1				F2				F3 & F4
		{ 0,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//OPEN
		{ 20,		FULLY_CLOSED,	FULLY_OPEN,		FULLY_OPEN,		FULLY_OPEN },	//MIDPOINTA
		{ 40,		FULLY_CLOSED,	FULLY_CLOSED,	FULLY_OPEN,		FULLY_OPEN },	//MIDPOINTB
		{ 60,		FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED,	FULLY_OPEN },	//MIDPOINTC
		{ 80,		FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED },	//MIDPOINTD
		{ 100,		FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED,	FULLY_CLOSED },	//CLOSED
	},
};
