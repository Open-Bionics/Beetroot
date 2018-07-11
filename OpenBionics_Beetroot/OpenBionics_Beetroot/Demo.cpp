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
*	Demo.cpp
*
*/

#include "Globals.h"
#include "Demo.h"

#include "Grips.h"

////////////////////////////// Constructors/Destructors //////////////////////////////

DEMO_CLASS::DEMO_CLASS()
{
	setGripDuration(DEMO_DEFAULT_GRIP_DURATION);	
}

////////////////////////////// Public Methods //////////////////////////////

// check whether demo mode is enabled
bool DEMO_CLASS::enabled(void)
{
	if ((_mode == DEMO_ONCE) || (_mode == DEMO_CONSTANT))
		return true;
	else
		return false;
}

// run the demo mode
void DEMO_CLASS::run(void)
{
	// if time between last step and current step has elapsed
	if (_stepTimer.timeElapsed(_stepDuration))
	{
		// if grip has fully opened and closed
		if ((_stepNum == 0) && _stepDir)
		{
			_stepDir = 0;

			Grip.nextGrip();			// cycle to next grip
			
			MYSERIAL_PRINTLN(Grip.getGripName());		// print current grip name

			// if the grips have wrapped around to the beginning and is only set to run once
			if ((Grip.getGrip() == 0) && (_mode == DEMO_ONCE))
			{
				_mode = DEMO_STOP;		// disable demo mode
				MYSERIAL_PRINTLN_PGM("End of Demo Mode");
				return;
			}
		}

		// count up/down depending on _stepDir
		_stepNum += 1 + (_stepDir * (-2));

		// if one end has been reached, flip the direction
		if (_stepNum == GRIP_MAX_COUNT_VAL)
			_stepDir = 1;

		Grip.setPos(_stepNum);
		Grip.setSpeed(MAX_FINGER_PWM);
		Grip.run();
	}
}

// set the duration of each grip
void DEMO_CLASS::setGripDuration(uint16_t duration)
{
	_stepDuration = duration / (GRIP_MAX_COUNT_VAL * 2);		// calculate the time per step (2 directions so COUNTx 2)
	_stepNum = 0;			// reset the step number for the new grip
	_stepDir = 0;			// reset the step direction to count up
}

// set demo to stop, run once or run constantly
void DEMO_CLASS::setMode(DemoMode mode)
{
	if (IS_BETWEEN(mode, DEMO_STOP, DEMO_CONSTANT))
	{
		_mode = mode;
	}
}

// set demo mode to stop
void DEMO_CLASS::stop(void)
{
	setMode(DEMO_STOP);
}

// set demo mode to once
void DEMO_CLASS::once(void)
{
	setMode(DEMO_ONCE);
}

// set demo mode to constant
void DEMO_CLASS::constant(void)
{
	setMode(DEMO_CONSTANT);
}

// toggle demo mode between once and stop
void DEMO_CLASS::toggleOnce(void)
{
	if (_mode == DEMO_STOP)
	{
		_mode = DEMO_ONCE;
		MYSERIAL_PRINTLN(Grip.getGripName());		// print current grip name
	}
	else
	{
		_mode = DEMO_STOP;
	}
}

// toggle demo mode between constant and stop
void DEMO_CLASS::toggleConstant(void)
{
	if (_mode == DEMO_STOP)
	{
		_mode = DEMO_CONSTANT;
		MYSERIAL_PRINTLN(Grip.getGripName());		// print current grip name
	}
	else
	{
		_mode = DEMO_STOP;
	}
}

DEMO_CLASS DEMO;