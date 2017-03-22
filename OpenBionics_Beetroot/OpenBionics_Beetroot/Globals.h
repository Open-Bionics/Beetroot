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
*	Globals.h
*
*/

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <Arduino.h>
#include <FingerLib.h>

#include "Utils.h"			// serial print from PROGMEM & other utilities

// uncomment the following to enable ROS control
//#define USE_ROS


///////////////////////////////////// MACROS ///////////////////////////////////////
#define IS_BETWEEN(x,a,b)	(((((x)>=(a))&&((x)<=(b)))||(((x)>=(b))&&((x)<=(a))))?(1):(0))		// check if value x is equal to or between values a and b

///////////////////////////////////// SERIAL SETTINGS ///////////////////////////////////////
#define SERIAL_BAUD_RATE	115200	// baud

////////////////////////////////////// TEMP SETTINGS ////////////////////////////////////////
#define CPU_TEMP_WARNING	45.0	// 'C
#define CPU_TEMP_MAX		50.0	// 'C			

///////////////////////////////////// FINGER SETTINGS ///////////////////////////////////////
#define NUM_FINGERS			4		// actuated fingers

extern Finger finger[NUM_FINGERS];

///////////////////////////////////// FIRMWARE VERSION ///////////////////////////////////////
#define FW_VERSION			1.01	// firmware version


#endif // GLOBALS_H_