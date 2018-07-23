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

#include <FingerLib.h>		// MS_NB_DELAY

void timerSetup(void);				// attach millisecond function to millisecond interrupt from FingerLib
void milliSecInterrupt(void);		// called every 1ms, increments milliseconds and second counters			
long customMillis(void);			// return number of milliseconds since power on
long customSeconds(void);			// return number of seconds since power on

#endif // TIMER_MANAGEMENT_H_