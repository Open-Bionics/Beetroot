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
*	Initialisation.h
*
*/

#ifndef INITIALISATION_H_
#define INITIALISATION_H_

// WATCHDOG SETTINGS
#define WATCHDOG_RESET_PER			16000		// ms. cause a device reset at 16s

// EEPROM
#define EEPROM_LOC_BOARD_SETTINGS	1010		// location within EEPROM of settings
#define EEPROM_INIT_CODE			7			// EEPROM init verification code

/////////////////////////////////////// BOARD SETTINGS ///////////////////////////////////
typedef enum _HandType
{
	HAND_TYPE_NONE = 0,		// no hand configuration
	HAND_TYPE_RIGHT,		// right hand
	HAND_TYPE_LEFT			// left hand
} HandType;

#define NUM_MODES	5		// number of modes (used for modeNames[] in SerialControl.cpp->serial_systemDiagnostics() )
typedef enum _OperatingMode
{
	MODE_NONE = 0,			// normal operating mode
	MODE_DEMO,				// demo mode
	MODE_EMG_SIMPLE,		// EMG simple control mode
	MODE_EMG_PROP,			// EMG proportional control mode
	MODE_CSV,				// CSV control mode
	MODE_HANDLE				// HANDle mode (Nunchuck)
} OperatingMode;

typedef enum _HeadphoneJackMode
{
	JACK_I2C = 0,			// I2C over the headphone jack (set switch low)
	JACK_ADC = 1,			// ADC over the headphone jack (switch in default pos)
	JACK_DIGITAL = 1,		// Digital over the headphone jack (switch in default pos)
	JACK_SERIAL = 1			// SerialJack over the headphone jack (switch in default pos)
} HeadphoneJackMode;



typedef struct _EMGSettings
{
	uint16_t peakThresh;	// threshold above which the EMG signal is a PEAK
	uint16_t holdTime;		// amount of time a PEAK needs to be held to be recognised as a HOLD
} EMGSettings;

typedef struct _Settings
{
	HandType handType;		// NONE, RIGHT, LEFT
	OperatingMode mode;		// NONE, DEMO, EMG SIMPLE, EMG PROPORTIONAL

	EMGSettings emg;		// EMG settings

	uint8_t waitForSerial = true;	// wait for serial connection before the programme runs
	uint8_t motorEn = true;			// motor enable
	uint8_t printInstr = true;		// print serial instructions

	uint8_t init = false;			// if the EEPROM has been initialised for the first time
} Settings;








void deviceSetup(void);				// board initialisation sequence

void initFingerPins(void);			// attach the finger pins for a left/right hand
void resetToDefaults(void);			// set default settings
void setModes(void);				// start hand in a particular mode depending on EEPROM settings
		
void readEEPROM(void);				// load settings from EEPROM, if no settings, use defaults
void loadSettings(void);			// read the settings from EEPROM
void storeSettings(void);			// store the settings in EEPROM
				
void detectSerialConnection(void);	// initialise SerialUSB and wait for serial connection if flag is set
void setHeadphoneJack(HeadphoneJackMode mode);			// configure headphone jack to I2C, ADC, Digital or SerialJack
void printDeviceInfo(void);			// print board & firmware version, hand type and motor enabled/disabled

void systemMonitor(void);			// monitor system status

float readTemperature(void);		// read the device temperature, in °C
float readMinTemp(void);			// read the minimum temperature reached since power-on
float readMaxTemp(void);			// read the maximum temperature reached since power-on
void monitorTemperature(void);		// check the temperature, throw warnings/errors or shutdown if too high
			

void watchdogCallback(void);		// set the WDT error and shutdown safely



extern Settings settings;			// board settings


#endif // INITIALISATION_H_