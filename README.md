# Open Bionics - Beetroot (BETA)

Open Bionics Robotics is the Open Source branch of the Open Bionics company, in 
Included in this repositry;

- Beetroot V1.0 - The firmware release for Open Bionics Brunel hand & Chestnut PCB 

## Quick Start
1. Install [Arduino](https://www.arduino.cc)
* Navigate to the downloads page at [Arduino.cc](https://www.arduino.cc/en/Main/Software)
* Download the latest version of the IDE (1.8.1 at time of writing)
* Install Arduino
2. Download [Beetroot](https://github.com/Open-Bionics/Beetroot) 
* Navigate to the [Beetroot repository](https://github.com/Open-Bionics/Beetroot) 
* Download and extract the zip folder (Clone or download -> Download Zip)
* Open the arduino file at **Beetroot\OpenBionics_Beetroot\OpenBionics_Beetroot\OpenBionics_Beetroot.ino**
3. Install [FingerLib.h](https://github.com/Open-Bionics/FingerLib)
* Within Arduino, go to **Sketch -> Include Library -> Manage Libraries...**
* Search for **FingerLib**
* Install **FingerLib**
4. Install Chesnut board
* Within Arduino, go to **File -> Preferences**
* Copy the following URL https://open-bionics.github.io/package_openbionics_index.json
* Paste URL in **Additional Boards Manager URLs** box
* Open **Tools -> Board -> Boards Manager..**
* Search for **Open Bionics**
* Install the latest verison of the **Open Bionics Boards**
5. Upload to the Brunel hand
* Plug the hand into the supplied 12V DC jack
* Plug the USB micro into the hand and the computer
* Within Arduino, select **Tools -> Port -> COM## (Chestnut)** (see [here](https://www.arduino.cc/en/guide/troubleshooting#toc16) for more details)
* Select **Sketch -> Upload**
6. Use Beetroot
* After a successful upload, the hand will be flashing Orange (near the USB port)
* Within Arduino, select **Tools -> Serial Monitor** (the hand should now be flashing Green)
* Make sure the baud rate is set to **115200 baud** and to append **Carriage Return**
* Enter **?** over the serial monitor to view the list of serial instructions

This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/


## Beetroot Release Notes

	Version	|	Date		|	Notes
	V1.0.0	|	22/03/17	|	Initial release for Brunel hand and Chestnut board using SAMD21G18A


	Beetroot Description

		- Simple hand control software designed to run on the Open Bionics Chestnut hand controller
		- Uses FingerLib.h for low level finger control, which allows fingers to be used as servos
		- Can be controlled via the following methods:
			- Serial control (baud 115200)
			- Muscle control (EMG)
		- Uses either inbuilt ADC or external I2C ADC for muscle sensing and hand control
		- Enter '?' to view available serial commands
