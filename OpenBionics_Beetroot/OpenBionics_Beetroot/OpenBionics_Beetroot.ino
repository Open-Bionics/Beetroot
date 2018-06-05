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

#include "Demo.h"
#include "EMGControl.h"
#include "Grips.h"
#include "HANDle.h"
#include "Initialisation.h"
#include "SerialControl.h"


void setup() 
{
	deviceSetup();							// initialise the board

	if (settings.printInstr)
	{
		delay(100);							// allow time for serial to connect
		printDeviceInfo();					// print board & firmware info
		serial_SerialInstructions(NULL);	// print serial instructions
	}	
}


void loop() 
{
#if defined(USE_ROS)
	ros_run();
#endif


	//for (int i = 0; i < NUM_FINGERS; i++)
	for (int i = F1; i < F2; i++)
	{
		MYSERIAL.print(finger[i].debugVal);
		MYSERIAL.print("_");
		MYSERIAL.print(finger[i].readPos());
		MYSERIAL.print("/");
		MYSERIAL.print(finger[i].readTargetPos());
		MYSERIAL.print("_");
		MYSERIAL.print(finger[i].readSpeed());
		MYSERIAL.print("\t");
	}
	MYSERIAL.print("\n");



	// process any received serial characters
	pollSerial();

	// monitor system temp
	systemMonitor();

	//// if demo mode is enabled, run demo mode
	//if (DEMO.enabled())
	//{
	//	DEMO.run();
	//}

	//// if EMG mode is enabled, run EMG mode
	//if (EMG.enabled())
	//{
	//	EMG.run();
	//}

	//// if HANDle mode is enabled, run HANDle mode
	//if (HANDle.enabled())
	//{
	//	HANDle.run();
	//}
}


