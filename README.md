# Open Bionics - Beetroot

Open Bionics Robotics is the Open Source branch of the Open Bionics company, in 
Included in this repositry;

- Beetroot V1.0 - The firmware release for Open Bionics robotic hand kits, requires 

## Required Software

- [FingerLib.h] ( https://github.com/Open-Bionics/FingerLib )
- Chesnut PCB board file 
	-- Copy the following URL https://open-bionics.github.io/package_openbionics_index.json
	-- Open Arduino -> Preferences
	-- Paste URL in 'Additional Boards Manager URLs' box
	-- Open 'Tools -> Board -> Boards Manager'
	-- Search for and install the latest verison of the 'Open Bionics Boards'


This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/


## Artichoke Release Notes

	Version	|	Date		|	Notes
	V1.0.0	|	08/01/16	|	Initial release for Brunel hand and Chestnut board using SAMd21G18A


	Beetroot Description

		- Simple hand control software designed to run on the Open Bionics Chestnut hand controller
		- Uses FingerLib.h for low level finger control, which allows fingers to be used as servos
		- Can be controlled via the following methods:
			- Serial control (baud 115200)
			- Muscle control (EMG)
		- Uses either inbuilt ADC or external I2C ADC for muscle sensing and hand control
		- Enter '?' to view available serial commands