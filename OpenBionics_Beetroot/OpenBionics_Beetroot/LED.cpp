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
	Adafruit_NeoPixel(NEOPIXEL_NUM_PIXELS, NEOPIXEL_PIN);

	setMode(LED_MODE_SOLID);			// default LED to be solid on/off
	setColour(0, 0);
	setFreq(1);

	_pulseState = HIGH;
	_led.period_ms = 0;
}

LED_CLASS::~LED_CLASS()
{

}


////////////////////////////// Public Methods //////////////////////////////
// initialise the NeoPixel
void LED_CLASS::begin(void)
{
	pixel.begin();
}

// set the two blink/fade LED colours
void LED_CLASS::setColour(uint32_t c1, uint32_t c2)
{
	_prev.c1.c = _led.c1.c;				// store previous state
	_prev.c2.c = _led.c2.c;				// store previous state

	_led.c1.c = c1;
	_led.c2.c = c2;
}

// set the one solid/blink/fade colour
void LED_CLASS::setColour(uint32_t c1)
{
	setColour(c1, LED_BLACK);			// colour1, colour2
}

// set the one solid/blink/fade colour using RGB components
void LED_CLASS::setColour(uint8_t r, uint8_t g, uint8_t b)
{
	_prev.c1.c = _led.c1.c;				// store previous state

	_led.c1.rgb.r = r;
	_led.c1.rgb.g = g;
	_led.c1.rgb.b = b;
}

// set two blink colours
void LED_CLASS::setBlink(uint32_t c1, uint32_t c2)
{
	setColour(c1, c2);

	setMode(LED_MODE_BLINK);
}

// set one blink colour, the other is defaulted to black
void LED_CLASS::setBlink(uint32_t c1)
{
	setColour(c1, LED_BLACK);

	setMode(LED_MODE_BLINK);
}

// set two fade colours
void LED_CLASS::setFade(uint32_t c1, uint32_t c2)
{
	setColour(c1, c2);

	setMode(LED_MODE_FADE);
}

// set one fade colour, the other is defaulted to black
void LED_CLASS::setFade(uint32_t c1)
{
	setColour(c1, LED_BLACK);
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

// set LED to previous mode/colours/freq
void LED_CLASS::setPrev(void)
{
	_led = _prev;
}


// run the LED solid/blink/fade
void LED_CLASS::run(void)
{
	// if no blink/fade period, set the LED to be solid
	if (_led.period_ms == 0.0)
		_led.mode = LED_MODE_SOLID;

	switch (_led.mode)
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
	_prev.mode = _led.mode;		// store previous state

	_led.mode = mode;
}

// set the blink/fade frequency
void LED_CLASS::setFreq(float freq)
{
	_prev.period_ms = _led.period_ms;		// store previous state

	// convert the freq to period in ms
	if (freq == 0.0)
		_led.period_ms = 0;
	else
		_led.period_ms = 1000/freq;				
}

// set the LED to a solid colour
void LED_CLASS::runSolid(void)
{
	// if the LED is already the correct colour
	if (pixel.getPixelColor(_pNum) == _led.c1.c)
		return;

	pixel.setPixelColor(_pNum, _led.c1.c);
	pixel.show();
}

// blink the LED between two colours
void LED_CLASS::runBlink(void)
{
	// if no blink/fade frequency, return
	if (_led.period_ms == 0)
		return;

	if (_pulseTimer.timeEllapsed(_led.period_ms))
	{
		if (_pulseState)
			pixel.setPixelColor(_pNum, _led.c1.c);
		else
			pixel.setPixelColor(_pNum, _led.c2.c);

		pixel.show();

		_pulseState = !_pulseState;
	}
}

// fade the LED between two colours
void LED_CLASS::runFade(void)
{
	// if no blink/fade frequency, return
	if (_led.period_ms == 0)
		return;

	static uint8_t stepNum = 0;		// current fade step between the two colours

	uint32_t stepPeriod;			// time between the colour step changes
	Colour_t stepColour;			// colour of the current fade step

	// calculate time between 'colour step' changes
	stepPeriod = _led.period_ms / NEOPIXEL_FADE_RES;

	// if period between steps has elapsed
	if (_pulseTimer.timeEllapsed(stepPeriod))
	{
		stepColour.c = calcFade(_led.c1, _led.c2, stepNum, NEOPIXEL_FADE_RES);

		pixel.setPixelColor(_pNum, stepColour.c);
		pixel.show();

		// if fade is complete, change direction
		if ((stepNum < 1) || (stepNum >= NEOPIXEL_FADE_RES))
			_pulseState = !_pulseState;

		// increment/decrement step number
		stepNum += 1 + (_pulseState * -2);

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