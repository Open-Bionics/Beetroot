/*	Open Bionics - Beetroot
*	Author - Olly McBride
*	Date - December 2016
*
*	This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
*	To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
*
*	Website - http://www.openbionics.com/
*	GitHub - https://github.com/Open-Bionics
*	Email - ollymcbride@openbionics.com
*
*	LED.cpp
*
*/


#include "LED.h"

////////////////////////////// Constructors/Destructors //////////////////////////////
LED_CLASS::LED_CLASS()
{
	_index = 0;				// position of current LED details within _LEDHistory[] list
	_interruptEn = true;	// flag to prevent race condition
	_brightness = 1;		// default to max brightness

	// initialise the default LED details
	_tempLED.mode = LED_MODE_SOLID;
	_tempLED.c1.c = LED_BLACK;
	_tempLED.c2.c = LED_BLACK;
	_tempLED.pulsePer_ms = 1000;		// default to 1Hz
	_tempLED.pulseDir = HIGH;
	_tempLED.fadePer_ms = 0;
	_tempLED.fadeStep = 1;
	_tempLED.durPer_ms = 0;
}

////////////////////////////// Public Methods //////////////////////////////
// initialise the NeoPixel
void LED_CLASS::begin(void)
{
	pixel.begin();

	// save default LED details as the first LED history
	moveToLEDIndex(0, true);			// save temp LED to index 0 and point currLED to it
	_index = 0;
}

// set the LED display mode (SOLID, FLASH, FADE)
void LED_CLASS::setMode(LEDMode mode)
{
	_tempLED.mode = mode;
}

// set the two flash/fade LED colours
void LED_CLASS::setColour(uint32_t c1, uint32_t c2)
{
	_tempLED.c1.c = c1;
	_tempLED.c2.c = c2;

	// calculate the brightness values using the _brightness modifier (calc here so it doesn't have to eb calcualted in run())
	_tempLED.dim1 = calcBrightness(_tempLED.c1);
	_tempLED.dim2 = calcBrightness(_tempLED.c2);
}

// set the one solid/flash/fade c1 using RGB components
void LED_CLASS::setColour(uint8_t r, uint8_t g, uint8_t b)
{
	_tempLED.c1.rgb.r = r;
	_tempLED.c1.rgb.g = g;
	_tempLED.c1.rgb.b = b;

	// calculate the brightness values using the _brightness modifier (calc here so it doesn't have to eb calcualted in run())
	_tempLED.dim1 = calcBrightness(_tempLED.c1);
}

// set the global brightness modifier (0 - 100)
void LED_CLASS::setBrightness(uint8_t brightness)
{
	_brightness = (float)brightness / 100;		// scale brightness from 0-100 to 0-1

	// calculate the brightness values using the _brightness modifier (calc here so it doesn't have to eb calcualted in run())
	_tempLED.dim1 = calcBrightness(_tempLED.c1);
	_tempLED.dim2 = calcBrightness(_tempLED.c2);
	show();

	// if the LED is being turned off, make sure the LED is turned off here
	if (_brightness == 0.0)
	{
		runMode(LED_MODE_SOLID);
	}
}

// get the global brightness modifier (0 - 100)
uint8_t LED_CLASS::getBrightness(void)
{
	return _brightness * 100;		// scale brightness from 0-1 to 0-100
}

// set the flash/fade frequency
void LED_CLASS::setFreq(float freq)
{
	// convert the freq to period in ms
	if (freq == 0.0)
	{
		_tempLED.pulsePer_ms = 0.0;
	}
	else
	{
		_tempLED.pulsePer_ms = (1000.0 / freq);
	}
}

// duration to display the current mode before returning to previous (0 is constant)
void LED_CLASS::setDuration(uint16_t dur)
{
	_tempLED.durPer_ms = dur;
}

// set the number of flash/fades
void LED_CLASS::setNumCycles(uint8_t nCycles)
{
	_tempLED.durPer_ms = nCycles * _tempLED.pulsePer_ms * 2;
}

void LED_CLASS::show(void)
{
	// if _currLED is not pointing to any LED details, return
	if (_currLED == nullptr)
	{
		return;
	}

	// if the new values are equal to the current values, do not bother showing new values
	if (*_currLED == _tempLED)
	{
		return;
	}

	pauseInterrupt();					// pause 'run()' interrupt to prevent a race condition

										// if the previous duration was running, store the time remaining
	if (_currLED->durTimer.started())
	{
		_currLED->durPer_ms -= _currLED->durTimer.stop();	// subtract the elapsed time from the total duration
	}

	// if the previous pulse timer was running
	if (_currLED->stepTimer.started())
	{
		_currLED->stepTimer.stop();	// stop the timer
	}

	resumeInterrupt();					// resume 'run()' interrupt to prevent a race condition


										// if there is more space to save the LED details to the history
	if (_index < LED_MAX_HISTORY - 1)
	{
		_index++;	// move to the new history location
	}

	moveToLEDIndex(_index, true);		// move to index and save LED details to history
}

// set LED to previous mode/colours/freq
void LED_CLASS::showPrev(void)
{
	// step back to the previous set of LED details
	if (_index > 0)
	{
		_index--;
	}

	moveToLEDIndex(_index);
}

// reset the LED history
void LED_CLASS::resetHistory(void)
{
	// index 0  - BLACK
	// index 1  - WHITE
	// index 2+ - everything else
	moveToLEDIndex(1);
}

// prevent the run() interrupt from using _currLED, so that it can be modified
void LED_CLASS::pauseInterrupt(void)
{
	_interruptEn = false;
}

// re-enable access to _currLED
void LED_CLASS::resumeInterrupt(void)
{
	_interruptEn = true;
}

// run the LED solid/flash/fade
void LED_CLASS::run(void)
{
	// if the interrupt has been disabled (to prevent race condition), return without running
	if (!_interruptEn)
	{
		return;
	}

	// if the LED is set to only run for a particular duration
	if (_currLED->durPer_ms != 0.0)
	{
		// if the run duration has elapsed
		if (_currLED->durTimer.finished())
		{
			showPrev();			// set LED to previous settings
		}
	}

	// if no flash/fade period, set the LED to be solid
	if (_currLED->pulsePer_ms == 0.0)
	{
		_currLED->mode = LED_MODE_SOLID;
	}

	// if the LED is disabled, do not bother running the LED
	if (_brightness == 0.0)
	{
		return;
	}

	// select a mode to run
	runMode(_currLED->mode);

}

// select a mode to run
void LED_CLASS::runMode(LEDMode mode)
{
	switch (mode)
	{
		case LED_MODE_SOLID:
			runSolid();
			break;
		case LED_MODE_FLASH:
			runFlash();
			break;
		case LED_MODE_FADE:
			runFade();
			break;
		default:
			break;
	}
}


void LED_CLASS::moveToLEDIndex(uint8_t i, bool save)
{
	pauseInterrupt();					// pause 'run()' interrupt to prevent a race condition

										// if we are to save new details to LED history
	if (save)
	{
		_LEDHistory[_index] = _tempLED;		// save new LED details to new position within history array
	}

	_currLED = &_LEDHistory[i];			// set the pointer to point to the current LED details
	_tempLED = _LEDHistory[i];			// load previous LED details into temp

										// if the new (prev) LED details have a duration setting
	if (_currLED->durPer_ms)
	{
		_currLED->durTimer.start(_currLED->durPer_ms);	// start/resume the duration timer
	}

	// calculate time between colour step changes (calc here so it doesn't have to be calculated in runFade() )
	_currLED->fadePer_ms = (_currLED->pulsePer_ms / LED_FADE_RES);

	_currLED->fadeStep = 1;				// reset the fade step number
	_currLED->pulseDir = 1;				// reset the pulse direction

	resumeInterrupt();					// resume 'run()' interrupt to prevent a race condition
}

// set the LED to a solid c1
void LED_CLASS::runSolid(void)
{
	// if the LED is already the correct c1
	if (pixel.getPixelColor(_pNum) == _currLED->dim1.c)
	{
		return;
	}

	pixel.setPixelColor(_pNum, _currLED->dim1.c);
	pixel.show();
}

// flash the LED between two colours
void LED_CLASS::runFlash(void)
{
	// if no flash/fade frequency, return
	if (_currLED->pulsePer_ms == 0.0)
	{
		return;
	}

	if (_currLED->stepTimer.timeElapsed(_currLED->pulsePer_ms))
	{
		if (_currLED->pulseDir)
		{
			pixel.setPixelColor(_pNum, _currLED->dim1.c);
		}
		else
		{
			pixel.setPixelColor(_pNum, _currLED->dim2.c);
		}

		pixel.show();

		_currLED->pulseDir = !_currLED->pulseDir;
	}
}

// fade the LED between two colours
void LED_CLASS::runFade(void)
{
	// if no flash/fade frequency, return
	if (_currLED->pulsePer_ms == 0.0)
	{
		return;
	}

	Colour_t stepColour;			// c1 of the current fade step

	// if period between steps has elapsed
	if (_currLED->stepTimer.timeElapsed(_currLED->fadePer_ms))
	{
		// use the colour levels adjusted using the brightness
		stepColour = calcFade(_currLED->dim1, _currLED->dim2, _currLED->fadeStep, LED_FADE_RES);

		pixel.setPixelColor(_pNum, stepColour.c);
		pixel.show();

		// if fade is complete, change direction
		if ((_currLED->fadeStep < 1) || (_currLED->fadeStep >= LED_FADE_RES - 1))
		{
			_currLED->pulseDir = !_currLED->pulseDir;
		}

		// increment/decrement step number
		_currLED->fadeStep += 1 + (_currLED->pulseDir * -2);
	}
}
// calculate the colour with all RGB values modified by the _brightness modifier
Colour_t LED_CLASS::calcBrightness(Colour_t &c1)
{
	Colour_t cb;		// brightness colour

	// calculate new c1 components at the desired brightness
	cb.rgb.r = c1.rgb.r * _brightness;
	cb.rgb.g = c1.rgb.g * _brightness;
	cb.rgb.b = c1.rgb.b * _brightness;

	return cb;
}

// calculate the colour at a particular point between two colours
Colour_t LED_CLASS::calcFade(Colour_t &c1, Colour_t &c2, uint8_t step, uint8_t maxSteps)
{
	Colour_t cf;		// fade colour

	// calculate new c1 components at the desired step
	cf.rgb.r = ((c1.rgb.r * (maxSteps - step)) + (c2.rgb.r * step)) / maxSteps;
	cf.rgb.g = ((c1.rgb.g * (maxSteps - step)) + (c2.rgb.g * step)) / maxSteps;
	cf.rgb.b = ((c1.rgb.b * (maxSteps - step)) + (c2.rgb.b * step)) / maxSteps;

	// return the fade c1
	return cf;
}





//// DEBUG
//
//// print details of _currLED, _tempLED & _LEDHistory[] for debugging
//void LED_CLASS::printDetails(void)
//{
//	char *LEDMode_Str[3] = { "Solid","Flash","Fade" };
//
//	Comms.println("LED.printDetails()\n");
//
//	// print _currLED
//	Comms.println("_currLED");
//	printStruct(_currLED);
//
//	// print _tempLED
//	Comms.println("_tempLED");
//	printStruct(&_tempLED);
//
//	// print all LED history
//	for (int i = _index; i >= 0; i--)
//	{
//		Comms.print("_LEDHistory[");
//		Comms.print(i);
//		Comms.print("]");
//		if (i == _index)
//		{
//			Comms.print(" - CURRENT");
//		}
//		Comms.print("\n");
//
//		printStruct(&_LEDHistory[i]);
//	}
//}
//
//void LED_CLASS::printStruct(LEDDetails_t *details)
//{
//	printMode(details->mode);
//	printColour(1, &details->c1, &details->dim1);
//	printColour(2, &details->c2, &details->dim2);
//
//	Comms.print("pulsePer_ms:\t");
//	Comms.println(details->pulsePer_ms);
//	Comms.print("pulseDir:\t");
//	Comms.println(details->pulseDir);
//
//	Comms.print("fadePer_ms:\t");
//	Comms.println(details->fadePer_ms);
//	Comms.print("stepTimer:\t");
//	printTimer(&details->stepTimer);
//	Comms.print("fadeStep:\t");
//	Comms.println(details->fadeStep);
//
//	Comms.print("durPer_ms:\t");
//	Comms.println(details->durPer_ms);
//	Comms.print("durTimer:\t");
//	printTimer(&details->durTimer);
//
//	Comms.println("*************************************\n");
//}
//
//void LED_CLASS::printMode(LEDMode mode)
//{
//	char *modeNames[3] = { "Solid","Flash","Fade" };
//
//	Comms.print("mode:\t\t");
//	Comms.print(modeNames[mode]);
//	Comms.print(" (");
//	Comms.print(mode);
//	Comms.println(")");
//}
//
//void LED_CLASS::printColour(int cNum, Colour_t *c, Colour_t *dim)
//{
//	Comms.print("c");
//	Comms.print(cNum);
//	Comms.print(":\t\t");
//	Comms.print(c->rgb.r);
//	Comms.print(", ");
//	Comms.print(c->rgb.g);
//	Comms.print(", ");
//	Comms.print(c->rgb.b);
//
//	Comms.print("\t(dim: ");
//	Comms.print(dim->rgb.r);
//	Comms.print(", ");
//	Comms.print(dim->rgb.g);
//	Comms.print(", ");
//	Comms.print(dim->rgb.b);
//	Comms.println(")");
//}
//
//void LED_CLASS::printTimer(NB_DELAY_CLASS *timer)
//{
//	Comms.print(timer->now());
//	Comms.print("/");
//	Comms.println(timer->getInterval());
//}

LED_CLASS LED;