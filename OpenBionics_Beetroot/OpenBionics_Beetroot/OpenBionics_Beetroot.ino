/*	Open Bionics - Beetroot
 *	Author - Olly McBride
 *	Date - October 2016
 *
 *	This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
 *	To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
 *
 *	Website - http://www.openbionics.com/
 *	GitHub - https://github.com/Open-Bionics
 *	Email - ollymcbride@openbionics.com
 *
 *	OpenBionics_Beetroot.ino
 *
 */

#include <Wire.h>
#include <FingerLib.h>

#include "Globals.h"

#include "Demo.h"							// DEMO
#include "EMGControl.h"						// EMG
#include "Grips.h"							// Grip
#include "HANDle.h"							// HANDle
#include "Initialisation.h"					// settings, deviceSetup, systemMonitor 
#include "SerialControl.h"					// pollSerial
#include "Watchdog.h"						// Watchdog


// DEBUG
#include "ErrorHandling.h"
volatile int debugVal = 0;
bool runTestFlag = false;
bool firstRun = true;
long startTime = 0;



void setup() 
{
	deviceSetup();							// initialise the board

	if (settings.printInstr)
	{
		delay(100);							// allow time for serial to connect
		serial_SerialInstructions();		// print serial instructions
	}	

	runTest(true);
}


void loop() 
{
#if defined(USE_ROS)
	ros_run();
#endif

	// monitor system temp
	systemMonitor();

	// if demo mode is enabled, run demo mode
	if (DEMO.enabled())
	{
		DEMO.run();
	}

	// if EMG mode is enabled, run EMG mode
	if (EMG.enabled())
	{
		EMG.run();
	}

	// if HANDle mode is enabled, run HANDle mode
	if (HANDle.enabled())
	{
		HANDle.run();
	}

	// process any received serial characters
	pollSerial();

#if defined(ARDUINO_ARCH_SAMD)
	Watchdog.reset();



	if (runTestFlag)
	{
		runImpededFingerTest();
	}

}







void runTest(bool en)
{
	runTestFlag = en;

	if (runTestFlag)
	{
		firstRun = true;

	}
}

void runImpededFingerTest(void)
{
	const int nCycles = 5000;
	static MS_NB_DELAY moveTimer;
	static MS_NB_DELAY tempTimer;
	static int cycleCount = 0;

	if (firstRun)
	{
		Grip.open();
		Grip.run();
		startTime = millis();
		firstRun = false;
	}

	//if (tempTimer.timeElapsed(500))
	//{
	//	MYSERIAL.print((millis() - startTime)/1000);
	//	MYSERIAL.print(",");
	//	MYSERIAL.println(readTemperature());
	//	//MYSERIAL.println("'C");
	//}


	if (moveTimer.timeElapsed(4000))
	{
		//MYSERIAL.println("Toggle");

		MYSERIAL.print(cycleCount);
		MYSERIAL.print(",");
		MYSERIAL.print((millis() - startTime) / 1000);
		MYSERIAL.print(",");
		MYSERIAL.println(readTemperature());

		Grip.toggleDir();
		Grip.run();

		if (cycleCount++ > nCycles)
		{
			runTestFlag = false;
		}
	}

	
}

