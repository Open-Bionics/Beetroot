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
*	LED.h
*
*/


#ifndef LED_H_
#define LED_H_

#include "Adafruit_NeoPixel.h"
#include "TimerManagement.h"

// NEOPIXEL
#define NEOPIXEL_PIN			3		// NeoPixel pin (Chestnut 3)
#define NEOPIXEL_NUM_PIXELS		1		// number of NeoPixels attached

// LED CONTROL SETTINGS
#define LED_FADE_RES			32		// fade resolution, i.e. number of steps per colour gradient change
#define LED_MAX_HISTORY			8		// maximum number of previous LED details to store

// COLOUR MACRO
#define CONVERT_RGB(R,G,B)        (((uint32_t)(R) << 16) | ((uint32_t)(G) <<  8) | (B))     // converts RGB values to 32 bit colour values

// NEOPIXEL COLOURS
#define LED_RED			CONVERT_RGB(255, 0, 0)
#define LED_RED_DIM		CONVERT_RGB(128, 0, 0)
#define LED_GREEN		CONVERT_RGB(0, 255, 0)
#define LED_GREEN_DIM	CONVERT_RGB(0, 128, 0)
#define LED_BLUE		CONVERT_RGB(0, 0, 255)
#define LED_BLUE_DIM	CONVERT_RGB(0, 0, 128)     
#define LED_PINK		CONVERT_RGB(255, 20, 147)
#define LED_PINK_DIM	CONVERT_RGB(129, 10, 72)
#define LED_PURPLE		CONVERT_RGB(255, 0, 255)
#define LED_PURPLE_DIM	CONVERT_RGB(51, 0, 51) 
#define LED_YELLOW		CONVERT_RGB(255, 255, 0)
#define LED_YELLOW_DIM	CONVERT_RGB(128, 128, 0)
#define LED_ORANGE		CONVERT_RGB(255, 128, 0)
#define LED_ORANGE_DIM	CONVERT_RGB(192, 64, 0)
#define LED_AQUA		CONVERT_RGB(0, 255, 255)
#define LED_AQUA_DIM	CONVERT_RGB(0, 128, 128)
#define LED_WHITE		CONVERT_RGB(255, 255, 255)
#define LED_WHITE_DIM	CONVERT_RGB(128, 128, 128)
#define LED_BLACK		0
#define LED_OFF			0

// LED MODE
typedef enum _LEDMode
{
	LED_MODE_SOLID = 0,		// solid on/off
	LED_MODE_BLINK,			// blinking between two colours
	LED_MODE_FADE			// fading between two colours
} LEDMode;


// COLOUR TYPE
typedef  struct _RGBComp
{
	uint8_t b;			// blue
	uint8_t g;			// green
	uint8_t r;			// red
	uint8_t w;			// white (unused)
} RGBComp_t;

typedef struct _Colour
{
	union
	{
		RGBComp_t rgb;	// RGBW components
		uint32_t c;		// 32-bit colour value
	};
} Colour_t;

// LED MODE, COLOURS & RATE
typedef struct _LEDDetails
{
	LEDMode mode;		// SOLID, BLINK, FADE

	Colour_t c1;		// 32-bit or RGBW colour
	Colour_t c2;		// 32-bit or RGBW colour 

	double period_ms;	// ms. blink period
	double dur_ms = 0;	// ms. run duration (0 is constant)

	NB_DELAY runTimer;				// duration timer
	NB_DELAY pulseTimer;			// time to pulse
	bool pulseState;				// pulsing up/down
} LEDDetails_t;


// LED CONTROL CLASS
class LED_CLASS
{
public:
	LED_CLASS();
	~LED_CLASS();

	void begin(void);					// initialise the NeoPixel

	void setColour(uint32_t c1, uint32_t c2);				// set the two blink/fade LED colours
	void setColour(uint32_t c1);							// set the one solid/blink/fade colour
	void setColour(uint8_t r, uint8_t g, uint8_t b);		// set the one solid/blink/fade colour using RGB components

	uint32_t getColour(void);								// get the one solid/blink/fade colour

	void setColourPercent(uint32_t c1, uint32_t c2, uint8_t percent);	// set the colour to be a percentage between 2 colours (0 - 100)

	void setSolid(void);
	void setBlink(uint32_t c1, uint32_t c2 = LED_BLACK);				// set two blink colours
	void setFade(uint32_t c1, uint32_t c2 = LED_BLACK);					// set two fade colours

	void setFlash(uint32_t c1, uint8_t nFlash, float freq = 4.0);		// flash a colour (c1 - BLACK) a number of times at freq

	void setBlinkFreq(float freq);		// set blink frequency
	void setFadeFreq(float freq);		// set fade frequency
	void setDuration(uint16_t dur);		// duration to display the current mode before returning to previous (0 is constant)

	void show(void);					// show the 'temporary' LED details and store in history (_prevLED[])
	void showPrev(void);				// set LED to previous mode/colours/freq
	void resetHistory(void);			// reset the LED history

	void off(void);						// turn off the LED

	void run(void);						// run the LED solid/blink/fade

private:
	Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NEOPIXEL_NUM_PIXELS, NEOPIXEL_PIN);

	uint8_t _index = 0;				// position of current LED details within _prevLED[] list
	const int _pNum = 0;			// pixel number

	LEDDetails_t _tempLED;					// variable to store the new/unsaved LED details
	LEDDetails_t _prevLED[LED_MAX_HISTORY];	// list of a number of previous LED details
	LEDDetails_t *_currLED = nullptr;		// pointer to the current set of LED details

	void setMode(LEDMode mode);		// set the LED display mode (SOLID, BLINK, FADE)
	void setFreq(float freq);		// set the blink/fade frequency

	void runSolid(void);			// set the LED to a solid colour
	void runBlink(void);			// blink the LED between two colours
	void runFade(void);				// fade the LED between two colours

	uint32_t calcFade(Colour_t c1, Colour_t c2, uint8_t step, uint8_t maxSteps);		// calculate the colour at a particular point between two colours
};

extern LED_CLASS LED;


#endif // LED_H_