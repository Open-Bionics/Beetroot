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
*	TimerManagement.cpp
*
*/

#include "Globals.h"
#include "TimerManagement.h"



#include "ErrorHandling.h"
#include "LED.h"

static long _milliSeconds = 0;			// number of milliSeconds since power on
static long _seconds = 0;				// number of seconds since power on

// attach millisecond function to millisecond interrupt from FingerLib
void timerSetup(void)
{
	_attachFuncToTimer(milliSecInterrupt);		// attach function to 1ms FingerLib timer interrupt
}

// called every 1ms, increments milliseconds and second counters
void milliSecInterrupt(void)
{
	_milliSeconds++;

	// if 1000ms has passed
	if (!(_milliSeconds % 1000))
	{
		_seconds++;
	}


	// clear any warnings after a set period
	ERROR.run();

	// run LED class to manage blinking and fading
	LED.run();
}

// return number of milliseconds since power on
long customMillis(void)
{
	return _milliSeconds;
}

// return number of seconds since power on
long customSeconds(void)
{
	return _seconds;
}
