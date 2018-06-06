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

}

LED_CLASS::~LED_CLASS()
{

}


////////////////////////////// Public Methods //////////////////////////////
// initialise the NeoPixel
void LED_CLASS::begin(void)
{
	Adafruit_NeoPixel(NEOPIXEL_NUM_PIXELS, NEOPIXEL_PIN);

	pixel.begin();

	// initialise the default LED details
	_tempLED.mode = LED_MODE_SOLID;
	_tempLED.c1.c = LED_BLACK;
	_tempLED.c2.c = LED_BLACK;
	_tempLED.period_ms = 0;
	_tempLED.dur_ms = 0;
	_tempLED.pulseState = HIGH;

	_index = 0;							// save the default LED details as the first member of the array 

	_prevLED[_index] = _tempLED;		// save new LED details to next position within array
	_currLED = &_prevLED[_index];		// set the pointer to point to the current LED details
}

// set the two blink/fade LED colours
void LED_CLASS::setColour(uint32_t c1, uint32_t c2)
{
	_tempLED.c1.c = c1;
	_tempLED.c2.c = c2;
}

// set the one solid/blink/fade colour
void LED_CLASS::setColour(uint32_t c1)
{
	setColour(c1, LED_BLACK);			// colour1, colour2
}

// set the one solid/blink/fade colour using RGB components
void LED_CLASS::setColour(uint8_t r, uint8_t g, uint8_t b)
{
	_tempLED.c1.rgb.r = r;
	_tempLED.c1.rgb.g = g;
	_tempLED.c1.rgb.b = b;
}

// get the one solid/blink/fade colour
uint32_t LED_CLASS::getColour(void)
{
	return _currLED->c1.c;
}

// set the colour to be a percentage between 2 colours (0 - 100)
void LED_CLASS::setColourPercent(uint32_t c1, uint32_t c2, uint8_t percent)
{
	Colour_t cMin, cMax;
	uint32_t cCalc;

	// store the colour as Colour_t type to quickly extract RGB components
	cMin.c = c1;
	cMax.c = c2;

	// calculate 'in-between' colour
	cCalc = calcFade(cMin, cMax, percent, 100);

	// set calculated colour
	setColour(cCalc);
}

void LED_CLASS::setSolid(void)
{
	setMode(LED_MODE_SOLID);
}

// set two blink colours
void LED_CLASS::setBlink(uint32_t c1, uint32_t c2)
{
	setColour(c1, c2);

	setMode(LED_MODE_BLINK);
}

// set two fade colours
void LED_CLASS::setFade(uint32_t c1, uint32_t c2)
{
	setColour(c1, c2);

	setMode(LED_MODE_FADE);
}

// flash a colour (c1 - BLACK) a number of times at freq
void LED_CLASS::setFlash(uint32_t c1, uint8_t nFlash, float freq)
{
	if (freq == 0.0)
		setFreq(1);			// if no frequency is set, default to 1Hz
	else
		setFreq(freq);

	// set at least 1 flash
	if (nFlash == 0)
		nFlash = 1;

	setDuration(2 * nFlash * _tempLED.period_ms);
	setBlink(c1);
}


// set blink frequency
void LED_CLASS::setBlinkFreq(float freq)
{
	setFreq(freq);

	setMode(LED_MODE_BLINK);
}

// set fade frequency
void LED_CLASS::setFadeFreq(float freq)
{
	setFreq(freq);

	setMode(LED_MODE_FADE);
}

// duration to display the current mode before returning to previous (0 is constant)
void LED_CLASS::setDuration(uint16_t dur)
{
	_tempLED.dur_ms = dur;

}

void LED_CLASS::show(void)
{
	// if the previous duration was running, store the remaining time
	if (_currLED)
	{
		// if the previous duration was running, store the remaining time
		if (_currLED->runTimer.started())
		{
			_currLED->dur_ms -= _currLED->runTimer.stop();
		}
	}

	if (_index < LED_MAX_HISTORY - 1)
		_index++;

	_prevLED[_index] = _tempLED;		// save new LED details to next position within array
	_currLED = &_prevLED[_index];		// set the pointer to point to the current LED details

										// if the LED current LED has a duration assigned
										// TODO, also check if timer is already running?
	if (_currLED->dur_ms)
		_currLED->runTimer.start(_currLED->dur_ms);	// start the run timer	
}

// set LED to previous mode/colours/freq
void LED_CLASS::showPrev(void)
{
	// step back to the previous set of LED details
	if (_index > 0)
		_index--;

	_currLED = &_prevLED[_index];		// set the pointer to point to the current LED details

										// if the new LED details have a duration setting
	if (_currLED->dur_ms)
		_currLED->runTimer.start(_currLED->dur_ms);	// start/resume the duration timer
}

// reset the LED history
void LED_CLASS::resetHistory(void)
{
	_index = 1;
	_currLED = &_prevLED[_index];		// set the pointer to point to the current LED details
}

// turn off the LED
void LED_CLASS::off(void)
{
	setMode(LED_MODE_SOLID);
	setColour(LED_BLACK);
	setDuration(0);
	setFade(0);
}

// run the LED solid/blink/fade
void LED_CLASS::run(void)
{
	// if the LED is set to only run for a particular duration
	if (_currLED->dur_ms != 0)
	{
		// if the run duration has elapsed
		if (_currLED->runTimer.finished())
		{
			showPrev();			// set LED to previous settings
		}
	}

	// if no blink/fade period, set the LED to be solid
	if (_currLED->period_ms == 0.0)
		_currLED->mode = LED_MODE_SOLID;

	switch (_currLED->mode)
	{
	case LED_MODE_SOLID:
		runSolid();
		break;
	case LED_MODE_BLINK:
		runBlink();
		break;
	case LED_MODE_FADE:
		runFade();
		break;
	default:
		break;
	}
}


////////////////////////////// Private Methods //////////////////////////////

// set the LED display mode (SOLID, BLINK, FADE)
void LED_CLASS::setMode(LEDMode mode)
{
	_tempLED.mode = mode;

}

// set the blink/fade frequency
void LED_CLASS::setFreq(float freq)
{
	// convert the freq to period in ms
	if (freq == 0.0)
		_tempLED.period_ms = 0;
	else
		_tempLED.period_ms = 1000 / freq;
}

// set the LED to a solid colour
void LED_CLASS::runSolid(void)
{
	// if the LED is already the correct colour
	if (pixel.getPixelColor(_pNum) == _currLED->c1.c)
		return;

	pixel.setPixelColor(_pNum, _currLED->c1.c);
	pixel.show();
}

// blink the LED between two colours
void LED_CLASS::runBlink(void)
{
	// if no blink/fade frequency, return
	if (_currLED->period_ms == 0)
		return;

	if (_currLED->pulseTimer.timeElapsed(_currLED->period_ms))
	{
		if (_currLED->pulseState)
			pixel.setPixelColor(_pNum, _currLED->c1.c);
		else
			pixel.setPixelColor(_pNum, _currLED->c2.c);

		pixel.show();

		_currLED->pulseState = !_currLED->pulseState;
	}
}

// fade the LED between two colours
void LED_CLASS::runFade(void)
{
	// if no blink/fade frequency, return
	if (_currLED->period_ms == 0)
		return;

	static uint8_t stepNum = 0;		// current fade step between the two colours

	uint32_t stepPeriod;			// time between the colour step changes
	Colour_t stepColour;			// colour of the current fade step

									// calculate time between 'colour step' changes
	stepPeriod = _currLED->period_ms / LED_FADE_RES;

	// if period between steps has elapsed
	if (_currLED->pulseTimer.timeElapsed(stepPeriod))
	{
		stepColour.c = calcFade(_currLED->c1, _currLED->c2, stepNum, LED_FADE_RES);

		pixel.setPixelColor(_pNum, stepColour.c);
		pixel.show();

		// if fade is complete, change direction
		if ((stepNum < 1) || (stepNum >= LED_FADE_RES))
			_currLED->pulseState = !_currLED->pulseState;

		// increment/decrement step number
		stepNum += 1 + (_currLED->pulseState * -2);

	}
}


// calculate the colour at a particular point between two colours
uint32_t LED_CLASS::calcFade(Colour_t c1, Colour_t c2, uint8_t step, uint8_t maxSteps)
{
	Colour_t cf;		// fade colour

	float rs, gs, bs;                   // RGB increments
	uint8_t rd = 0, gd = 0, bd = 0;     // RGB increment direction

										// determine count direction of colour change for each RGB component
	if (c1.rgb.r < c2.rgb.r) rd = 1;
	if (c1.rgb.g < c2.rgb.g) gd = 1;
	if (c1.rgb.b < c2.rgb.b) bd = 1;

	// determine step increments
	rs = (float)abs(c1.rgb.r - c2.rgb.r) / maxSteps;
	gs = (float)abs(c1.rgb.g - c2.rgb.g) / maxSteps;
	bs = (float)abs(c1.rgb.b - c2.rgb.b) / maxSteps;

	// calculate new colour components at the desired step
	cf.rgb.r = (uint8_t)c1.rgb.r - (rs*step) + (rd * 2 * (rs*step));
	cf.rgb.g = (uint8_t)c1.rgb.g - (gs*step) + (gd * 2 * (gs*step));
	cf.rgb.b = (uint8_t)c1.rgb.b - (bs*step) + (bd * 2 * (bs*step));

	// return the fade colour
	return cf.c;
}

LED_CLASS LED;