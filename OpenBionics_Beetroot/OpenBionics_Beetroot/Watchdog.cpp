/*	Open Bionics - Carrot HC
*	Author - Olly McBride
*	Date - October 2017
*
*	This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
*	To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
*
*	Website - http://www.openbionics.com/
*	GitHub - https://github.com/Open-Bionics
*	Email - ollymcbride@openbionics.com
*
*	Watchdog.cpp
*
*/

#include <Arduino.h>

#if defined(ARDUINO_ARCH_SAMD)

#include "Globals.h"
#include "LED.h"
#include "Watchdog.h"


WDT_CLASS::WDT_CLASS()
{
	_init = false;
	_EWcallbackFunc = nullptr;
}

int WDT_CLASS::begin(int maxPeriodMS)
{
	int cycles, actualMS;
	uint8_t bits;

	if (!_init)
	{
		initialiseWDT();
	}

	WDT->CTRL.reg = 0; // Disable watchdog for config
	while (WDT->STATUS.bit.SYNCBUSY);

	// You'll see some occasional conversion here compensating between
	// milliseconds (1000 Hz) and WDT clock cycles (~1024 Hz).  The low-
	// power oscillator used by the WDT ostensibly runs at 32,768 Hz with
	// a 1:32 prescale, thus 1024 Hz, though probably not super precise.

	if ((maxPeriodMS >= 16000) || !maxPeriodMS) {
		cycles = 16384;
		bits = 0xB;
	}
	else {
		cycles = (maxPeriodMS * 1024L + 500) / 1000; // ms -> WDT cycles
		if (cycles >= 8192) 
		{
			cycles = 8192;
			bits = 0xA;
		}
		else if (cycles >= 4096) 
		{
			cycles = 4096;
			bits = 0x9;
		}
		else if (cycles >= 2048) 
		{
			cycles = 2048;
			bits = 0x8;
		}
		else if (cycles >= 1024) 
		{
			cycles = 1024;
			bits = 0x7;
		}
		else if (cycles >= 512) 
		{
			cycles = 512;
			bits = 0x6;
		}
		else if (cycles >= 256) 
		{
			cycles = 256;
			bits = 0x5;
		}
		else if (cycles >= 128) 
		{
			cycles = 128;
			bits = 0x4;
		}
		else if (cycles >= 64) 
		{
			cycles = 64;
			bits = 0x3;
		}
		else if (cycles >= 32) 
		{
			cycles = 32;
			bits = 0x2;
		}
		else if (cycles >= 16) 
		{
			cycles = 16;
			bits = 0x1;
		}
		else 
		{
			cycles = 8;
			bits = 0x0;
		}
	}

	WDT->CONFIG.bit.PER = bits;					// Set period for chip reset
	WDT->CTRL.bit.WEN = 0;						// Disable window mode
	while (WDT->STATUS.bit.SYNCBUSY);			// Sync CTRL write

	actualMS = (cycles * 1000L + 512) / 1024;	// WDT cycles -> ms

	reset();									// Clear watchdog interval
	WDT->CTRL.bit.ENABLE = 1;					// Start watchdog 
	while (WDT->STATUS.bit.SYNCBUSY);

	return actualMS;
}

int WDT_CLASS::enInterrupt(bool en, int period, void(*f)(void))
{
	int cycles, actualMS;
	uint8_t reg;

	// assign pointer to function to run when EW interrupt is triggered
	_EWcallbackFunc = f;

	WDT->CTRL.reg = 0; // Disable watchdog for config
	while (WDT->STATUS.bit.SYNCBUSY);

	if ((period >= 16000) || !period) 
	{
		cycles = 16384;
		//reg = WDT_EWCTRL_EWOFFSET_11;
		reg = 0x0B;

	}
	else 
	{
		cycles = (period * 1024L + 500) / 1000; // ms -> WDT cycles
		if (cycles >= 8192) {
			cycles = 8192;
			//reg = WDT_EWCTRL_EWOFFSET_10;
			reg = 0x0A;
		}
		else if (cycles >= 4096) 
		{
			cycles = 4096;
			//reg = WDT_EWCTRL_EWOFFSET_9;
			reg = 0x09;
		}
		else if (cycles >= 2048) 
		{
			cycles = 2048;
			//reg = WDT_EWCTRL_EWOFFSET_8;
			reg = 0x08;
		}
		else if (cycles >= 1024) 
		{
			cycles = 1024;
			//reg = WDT_EWCTRL_EWOFFSET_7;
			reg = 0x07;
		}
		else if (cycles >= 512) 
		{
			cycles = 512;
			//reg = WDT_EWCTRL_EWOFFSET_6;
			reg = 0x06;
		}
		else if (cycles >= 256) 
		{
			cycles = 256;
			//reg = WDT_EWCTRL_EWOFFSET_5;
			reg = 0x05;
		}
		else if (cycles >= 128) 
		{
			cycles = 128;
			//reg = WDT_EWCTRL_EWOFFSET_4;
			reg = 0x04;
		}
		else if (cycles >= 64) 
		{
			cycles = 64;
			//reg = WDT_EWCTRL_EWOFFSET_3;
			reg = 0x03;
		}
		else if (cycles >= 32) 
		{
			cycles = 32;
			//reg = WDT_EWCTRL_EWOFFSET_2;
			reg = 0x02;
		}
		else if (cycles >= 16) 
		{
			cycles = 16;
			//reg = WDT_EWCTRL_EWOFFSET_1;
			reg = 0x01;
		}
		else 
		{
			cycles = 8;
			//reg = WDT_EWCTRL_EWOFFSET_0;
			reg = 0x00;
		}
	}

	if (en)
	{
		WDT->EWCTRL.reg = reg;					// Set EW interrupt offset time
		WDT->INTENSET.bit.EW = 1;				// Enable early warning interrupt
	}
	else
	{
		WDT->INTENSET.bit.EW = 0;				// Disable early warning interrupt
	}

	actualMS = (cycles * 1000L + 512) / 1024;	// WDT cycles -> ms

	reset();									// Clear watchdog interval
	WDT->CTRL.bit.ENABLE = 1;					// Start watchdog now!
	while (WDT->STATUS.bit.SYNCBUSY);

	return actualMS;
}

void WDT_CLASS::reset()
{
	// Write the watchdog clear key value (0xA5) to the watchdog
	// clear register to clear the watchdog timer and reset it.
	WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
	while (WDT->STATUS.bit.SYNCBUSY);
}

void WDT_CLASS::disable()
{
	WDT->CTRL.bit.ENABLE = 0;				// Disable watchdog
	WDT->INTENSET.bit.EW = 0;				// Disable early warning interrupt
	while (WDT->STATUS.bit.SYNCBUSY);
}


void WDT_CLASS::initialiseWDT(void)
{
	// One-time initialization of watchdog timer.
	// Insights from rickrlh and rbrucemtl in Arduino forum!

	// Generic clock generator 2, divisor = 32 (2^(DIV+1))
	GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(4);
	// Enable clock generator 2 using low-power 32KHz oscillator.
	// With /32 divisor above, this yields 1024Hz(ish) clock.
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) |
		GCLK_GENCTRL_GENEN |
		GCLK_GENCTRL_SRC_OSCULP32K |
		GCLK_GENCTRL_DIVSEL;
	while (GCLK->STATUS.bit.SYNCBUSY);
	// WDT clock = clock gen 2
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT |
		GCLK_CLKCTRL_CLKEN |
		GCLK_CLKCTRL_GEN_GCLK2;

	// Enable WDT early-warning interrupt
	NVIC_DisableIRQ(WDT_IRQn);
	NVIC_ClearPendingIRQ(WDT_IRQn);
	//NVIC_SetPriority(WDT_IRQn, 0); // Top priority
	NVIC_SetPriority(WDT_IRQn, 1); // Medium-high priority
	NVIC_EnableIRQ(WDT_IRQn);

	_init = true;
}


void WDT_Handler(void)
{
	// ISR for watchdog early warning, DO NOT RENAME!
	//WDT->CTRL.bit.ENABLE = 0;        // Disable watchdog
	//while (WDT->STATUS.bit.SYNCBUSY); // Sync CTRL write
	WDT->INTFLAG.bit.EW = 1;        // Clear interrupt flag

	//Comms.println("WDT");
	delay(50);
	if (Watchdog._EWcallbackFunc != nullptr)
	{
		Watchdog._EWcallbackFunc();
	}
}

WDT_CLASS Watchdog;

#endif // ARDUINO_ARCH_SAMD