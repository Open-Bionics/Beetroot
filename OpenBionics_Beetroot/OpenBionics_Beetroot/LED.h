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
#define LED_FADE_RES			32		// fade resolution, i.e. number of steps per c1 gradient change
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
#define LED_WHITE_DIM	CONVERT_RGB(90, 90, 90)
#define LED_BLACK		0
#define LED_OFF			0

// LED MODE
typedef enum _LEDMode
{
	LED_MODE_SOLID = 0,		// solid on/off
	LED_MODE_FLASH,			// flashing between two colours
	LED_MODE_FADE			// fading between two colours
} LEDMode;

// COLOUR TYPE
typedef struct _RGBComp_t
{
	uint8_t b;			// blue
	uint8_t g;			// green
	uint8_t r;			// red
	uint8_t w;			// white (unused)
} RGBComp_t;

typedef struct _Colour_t
{
	union
	{
		RGBComp_t rgb;	// RGBW components
		uint32_t c = 0;	// 32-bit colour value
	};

	// compare Colour_t structs
	bool operator==(const _Colour_t& comp) const
	{
		return c == comp.c;		// only compare the c values, as they are a union of RGBW values
	}
} Colour_t;

// LED MODE, COLOURS & RATE
typedef struct _LEDDetails_t
{
	LEDMode mode;			// SOLID, FLASH, FADE

	Colour_t c1;			// 32-bit or RGBW colour
	Colour_t dim1;			// 32-bit or RGBW colour adjusted using the _brightness modifier

	Colour_t c2;			// 32-bit or RGBW colour 
	Colour_t dim2;			// 32-bit or RGBW colour adjusted using the _brightness modifier

	long pulsePer_ms = 0;	// ms. flash/fade period (period for complete on & off cycle)
	bool pulseDir = 0;		// LED flashing/fading on/up or off/down

	long fadePer_ms = 0;	// ms. time between the colour step changes during a fade
	MS_NB_DELAY stepTimer;	// timer between colour fade steps
	uint16_t fadeStep = 1;	// current fade step between the two colours

	long durPer_ms = 0;		// ms. run duration (0 = run constantly)
	MS_NB_DELAY durTimer;	// timer used to stop a pattern if it is set to only run for a set duration

							// compare LEDDetails_t structs (mode, colour, periods)
	bool operator==(const _LEDDetails_t& comp) const
	{
		return	mode == comp.mode		&&
			c1 == comp.c1			&&
			dim1 == comp.dim1		&&
			c2 == comp.c2			&&
			dim2 == comp.dim2		&&
			pulsePer_ms == comp.pulsePer_ms	&&
			fadePer_ms == comp.fadePer_ms	&&
			durPer_ms == comp.durPer_ms;
	}
} LEDDetails_t;



// LED CONTROL CLASS
class LED_CLASS
{
	public:
		LED_CLASS();

		void begin(void);										// initialise the NeoPixel

		void setMode(LEDMode mode);								// set the LED display mode (SOLID, FLASH, FADE)

		void setColour(uint32_t c1, uint32_t c2 = LED_OFF);		// set the two LED colours
		void setColour(uint8_t r, uint8_t g, uint8_t b);		// set the one LED c1 using RGB components

		void setBrightness(uint8_t brightness);					// set the global brightness modifier (0 - 100)
		uint8_t getBrightness(void);							// get the global brightness modifier (0 - 100)

		void setFreq(float freq);								// set the flash/fade frequency

		void setDuration(uint16_t dur);							// duration to display the current mode before returning to previous (0 is constant)

		void setNumCycles(uint8_t nCycles);						// set the number of flash/fades

		void show(void);				// show the 'temporary' LED details and store in history (_LEDHistory[])
		void showPrev(void);			// set LED to previous mode/colours/freq
		void resetHistory(void);		// reset the LED history

		void pauseInterrupt(void);		// prevent the run() interrupt from using _currLED, so that it can be modified
		void resumeInterrupt(void);		// re-enable access to _currLED
		void run(void);					// run the LED solid/flash/fade

		// DEBUG
		//void printDetails(void);
		//void printStruct(LEDDetails_t *details);
		//void printMode(LEDMode mode);
		//void printColour(int cNum, Colour_t *c, Colour_t *dim);
		//void printTimer(NB_DELAY_CLASS *timer);

	private:
		Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NEOPIXEL_NUM_PIXELS, NEOPIXEL_PIN);

		const int _pNum = 0;			// pixel number
		uint8_t _index;					// position of current LED details within _LEDHistory[] list
		float _brightness;				// global brightness modifier (0 - 1)

		bool _interruptEn;				// flag to prevent race condition

		LEDDetails_t _tempLED;						// variable to store the new/unsaved LED details
		LEDDetails_t _LEDHistory[LED_MAX_HISTORY];	// list of a number of previous LED details
		LEDDetails_t *_currLED = nullptr;			// pointer to the current set of LED details

		void moveToLEDIndex(uint8_t i, bool save = false);

		void runMode(LEDMode mode);		// select a mode to run
		void runSolid(void);			// set the LED to a solid colour
		void runFlash(void);			// flash the LED between two colours
		void runFade(void);				// fade the LED between two colours


		Colour_t calcBrightness(Colour_t &c1);												// calculate the colour with all RGB values modified by the _brightness modifier
		Colour_t calcFade(Colour_t &c1, Colour_t &c2, uint8_t step, uint8_t maxSteps);		// calculate the c1 at a particular point between two colours
};

extern LED_CLASS LED;


#endif // LED_H_