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
*	Watchdog.h
*
*/


#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#if defined(ARDUINO_ARCH_SAMD)

class WDT_CLASS
{
	public:
		WDT_CLASS();

		// Enable the watchdog timer to reset the machine after a period of time
		// without any calls to reset().  The passed in period (in milliseconds)
		// is just a suggestion and a lower value might be picked if the hardware
		// does not support the exact desired value.
		// User code should NOT set the 'isForSleep' argument either way --
		// it's used internally by the library, but your sketch should leave this
		// out when calling enable(), just let the default have its way.
		//
		// The actual period (in milliseconds) before a watchdog timer reset is
		// returned.
		int begin(int maxPeriodMS = 0);

		// enable/disable the early warning interrupt and attach a function
		int enInterrupt(bool en, int period = 0, void(*f)(void) = nullptr);

		// Reset or 'kick' the watchdog timer to prevent a reset of the device.
		void reset();

		// Completely disable the watchdog timer.
		void disable();

		// pointer to function to call if the EW interrupt is enabled
		void(*_EWcallbackFunc)(void);

	private:
		void initialiseWDT(void);

		bool _init;

};

extern WDT_CLASS Watchdog;

#endif // WATCHDOG_H_

#endif // ARDUINO_ARCH_SAMD