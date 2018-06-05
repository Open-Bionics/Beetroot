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
*	Demo.h
*
*/


#ifndef DEMO_H_
#define DEMO_H_

#include <Arduino.h>

#include "TimerManagement.h"


#define DEMO_DEFAULT_GRIP_DURATION	2500	// ms


typedef enum _DemoMode
{
	DEMO_STOP = 0,		// DEMO MODE NOT RUNNING
	DEMO_ONCE,			// DEMO MODE RUNNING ONCE
	DEMO_CONSTANT		// DEMO MODE RUNNING CONSTANTLY
} DemoMode;


class DEMO_CLASS
{
public:
	DEMO_CLASS();

	bool enabled(void);							// check whether demo mode is enabled
	void run(void);								// run the demo mode

	void setGripDuration(uint16_t duration);	// set the duration of each grip
	void setMode(DemoMode mode);				// set demo to stop, run once or run constantly

	void stop(void);							// set demo mode to stop
	void once(void);							// set demo mode to once
	void constant(void);						// set demo mode to constant

	void toggleOnce(void);						// toggle demo mode between once and stop
	void toggleConstant(void);					// toggle demo mode between constant and stop

private:
	DemoMode _mode;				// STOP, ONCE, CONSTANT
	
	float _stepNum;				// current step (may be a decimal)
	bool _stepDir;				// direction of step count
	double _stepDuration;		// amount of time between each step
	MS_NB_DELAY _stepTimer;		// duration of each step


};

extern DEMO_CLASS DEMO;

#endif // DEMO_H_