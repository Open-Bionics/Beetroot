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
*	TimerManagement.h
*
*/


#ifndef TIMER_MANAGEMENT_H_
#define TIMER_MANAGEMENT_H_

void timerSetup(void);				// attach millisecond function to millisecond interrupt from FingerLib
void milliSecInterrupt(void);		// called every 1ms, increments milliseconds and second counters			
long customMillis(void);			// return number of milliseconds since power on
long customSeconds(void);			// return number of seconds since power on
//bool customDelay(double delVal);	// delay for a number of milliSeconds

// NON_BLOCKING DELAY CLASS (milliSeconds)
class NB_DELAY
{
	public:
		NB_DELAY();
		void start(long delVal);		// start non-blocking delay
		bool finished(void);			// returns true if delVal has elapsed
		bool started(void);				// returns true if delay timer is running
		long now(void);					// returns elapsed time, if not started return 0
		long stop(void);				// stop the timer and return elapsed time

		bool timeEllapsed(long delVal);	// run the timer constantly, return true every time the delVal has elapsed
	private:
		long _startTime;
		long _interval;
		bool _started = false;
};

// NON_BLOCKING TIMER CLASS (milliSeconds)
class NB_TIMER
{
	public:
		NB_TIMER();
		void start(void);				// start non-blocking timer
		bool started(void);				// returns true if delay timer is running	
		long now(void);					// returns how much time has elapsed since the start, if not started return 0
		long stop(void);				// stop the timer and return elapsed time

		bool timeEllapsed(long interval);	// check whether a certain interval has elapsed, keep timer running
	private:
		long _startTime;
		bool _started = false;

};

#endif // TIMER_MANAGEMENT_H_