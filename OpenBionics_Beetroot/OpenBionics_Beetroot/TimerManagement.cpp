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
		_seconds++;


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

//// delay for a number of milliSeconds
//bool customDelay(double delVal)
//{
//	static double _prevTime = 0;
//
//	_prevTime = customMillis();
//
//	while ((customMillis() - _prevTime) <= delVal)
//	{
//		delay(1);
//	}
//
//	return true;
//}


////////////////////// NON_BLOCKING DELAY CLASS //////////////////////////

NB_DELAY::NB_DELAY()
{

}

// start non-blocking delay
void NB_DELAY::start(long delVal)
{
	_startTime = customMillis();
	_interval = delVal;
	_started = true;
}

// returns true if delVal has elapsed
bool NB_DELAY::finished(void)
{
	if ((customMillis() - _startTime) >= _interval)
	{
		_started = false;
		return 1;
	}
	else
		return 0;
}

// returns true if timer is running
bool NB_DELAY::started(void)
{
	return _started;
}

// returns elapsed time, if not started return 0
long NB_DELAY::now(void)
{
	if (!_started)
		return 0;
	else
		return (customMillis() - _startTime);
}

// stop the timer and return elapsed time
long NB_DELAY::stop(void)
{
	long timeEllapsed;

	timeEllapsed = now();
	_started = false;

	return timeEllapsed;
}

// run the timer constantly, return true every time the delVal has elapsed
bool NB_DELAY::timeEllapsed(long delVal)
{
	if (!started())			// if timer is not started (first run of timer)
	{
		start(delVal);		// start timer
		return false;		// duration has not elapsed, so return false
	}

	if (finished())			// if timer is finished
	{
		start(delVal);		// restart timer
		return true;		// duration has elapsed, so restart timer and return true
	}
	else
	{
		return false;
	}
}

////////////////////// NON_BLOCKING TIMER CLASS //////////////////////////
NB_TIMER::NB_TIMER()
{

}

// start non-blocking timer
void NB_TIMER::start(void)
{
	_startTime = customMillis();
	_started = true;
}

// returns true if delay timer is running	
bool NB_TIMER::started(void)
{
	return _started;
}

// returns how much time has elapsed since the start, if not started return 0
long NB_TIMER::now(void)
{
	if (!_started)
		return 0;
	else
		return (customMillis() - _startTime);
}

// stop the timer and return elapsed time
long NB_TIMER::stop(void)
{
	long timeEllapsed;

	timeEllapsed = now();
	_started = false;

	return timeEllapsed;
}

// check whether a certain interval has elapsed, keep timer running
bool NB_TIMER::timeEllapsed(long interval)
{
	if (now() >= interval)
		return true;
	else
		return false;
}