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

#define NEOPIXEL_PIN			3		// NeoPixel pin
#define NEOPIXEL_NUM_PIXELS		1		// number of NeoPixels attached
//#define NEOPIXEL_FADE_RES		8		// fade resolution, i.e. number of steps per colour gradient change
#define NEOPIXEL_FADE_RES		16		// fade resolution, i.e. number of steps per colour gradient change

// COLOUR MACRO
#define CONVERT_RGB(R,G,B)        (((uint32_t)(R) << 16) | ((uint32_t)(G) <<  8) | (B))     // converts RGB values to 32 bit colour values

// NEOPIXEL COLOURS
#define LED_RED         CONVERT_RGB(255, 0, 0)
#define LED_RED_DIM     CONVERT_RGB(128, 0, 0)
#define LED_GREEN       CONVERT_RGB(0, 255, 0)
#define LED_GREEN_DIM   CONVERT_RGB(0, 128, 0)
#define LED_BLUE        CONVERT_RGB(0, 0, 255)
#define LED_BLUE_DIM    CONVERT_RGB(0, 0, 128)     
#define LED_PINK		CONVERT_RGB(255, 20, 147)
#define LED_PINK_DIM	CONVERT_RGB(129, 10, 72)
#define LED_PURPLE      CONVERT_RGB(255, 0, 255)
#define LED_PURPLE_DIM  CONVERT_RGB(128, 0, 128)
#define LED_YELLOW      CONVERT_RGB(255, 255, 0)
#define LED_YELLOW_DIM  CONVERT_RGB(128, 128, 0)
#define LED_ORANGE      CONVERT_RGB(255, 128, 0)
#define LED_ORANGE_DIM  CONVERT_RGB(128, 64, 0)
#define LED_AQUA        CONVERT_RGB(0, 255, 255)
#define LED_WHITE       CONVERT_RGB(255, 255, 255)
#define LED_WHITE_DIM   CONVERT_RGB(128, 128, 128)
#define LED_BLACK       0

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
} RGBComp;

typedef struct _Colour_t
{
	union
	{
		RGBComp rgb;	// RGBW components
		uint32_t c;		// 32-bit colour value
	};
} Colour_t;

// LED MODE, COLOURS & RATE
typedef struct _LEDDetails
{
	LEDMode mode;		// SOLID, BLINK, FADE
	
	Colour_t c1;		// 32-bit or RGBW colour
	Colour_t c2;		// 32-bit or RGBW colour 

	double period_ms;	// blink period in ms
};


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

		void setBlink(uint32_t c1, uint32_t c2);				// set two blink colours
		void setBlink(uint32_t c1);								// set one blink colour, the other is defaulted to black

		void setFade(uint32_t c1, uint32_t c2);					// set two fade colours
		void setFade(uint32_t c1);								// set one fade colour, the other is defaulted to black

		void setBlinkFreq(float freq);		// set blink frequency
		void setFadeFreq(float freq);		// set fade frequency

		void setPrev(void);					// set LED to previous mode/colours/freq

		void run(void);						// run the LED solid/blink/fade

	private:
		Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NEOPIXEL_NUM_PIXELS, NEOPIXEL_PIN);

		_LEDDetails _led;				// led colour components and blink/fade rate
		_LEDDetails _prev;				// previous led details (used when ERROR_HANDLING takes control)

		const int _pNum = 0;			// pixel number

		NB_DELAY _pulseTimer;			// time to pulse
		bool _pulseState;				// pulsing up/down


		void setMode(LEDMode mode);		// set the LED display mode (SOLID, BLINK, FADE)
		void setFreq(float freq);		// set the blink/fade frequency

		void runSolid(void);	// set the LED to a solid colour
		void runBlink(void);	// blink the LED between two colours
		void runFade(void);		// fade the LED between two colours

		uint32_t calcFade(Colour_t c1, Colour_t c2, uint8_t step, uint8_t maxSteps);		// calculate the colour at a particular point between two colours
};

extern LED_CLASS LED;


#endif // LED_H_