/*	Open Bionics - Beetroot
*	Author - Olly McBride
*	Date - November 2016
*
*	This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
*	To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
*
*	Website - http://www.openbionics.com/
*	GitHub - https://github.com/Open-Bionics
*	Email - ollymcbride@openbionics.com
*
*	EMGControl.h
*
*/

#ifndef EMG_CONTROL_H_
#define EMG_CONTROL_H_

//#include "CircleBuff.h"
#include "TimerManagement.h"

#define NUM_EMG_CHANNELS	2
#define PRINT_MORE_EMG_DETAIL			// uncomment this line to view more EMG details

#define NOISE_BUFFER_SIZE	128
#define BUFFER_DEFAULT_VAL	925

typedef enum _EMGMode
{
	EMG_OFF = 0,		// EMG MODE NOT RUNNING
	EMG_SIMPLE,			// SIMPLE EMG MODE
	EMG_PROPORTIONAL	// PROPORTIONAL EMG MODE
} EMGMode;

typedef struct _EMGChannel
{
	int pin;

	CIRCLE_BUFFER <int>noiseFloor;

	MS_NB_TIMER HOLD_timer;

	bool active;
	int sample;
	int signal;
	int prevSignal;

	bool PEAK;
	bool HOLD;
} EMGchannel;

class EMG_CONTROL
{
	public:
		EMG_CONTROL();

		void begin(void);				// for each channel, initialise the noise floor buffer with default value, to prevent initial false positives
		bool enabled(void);				// check whether EMG mode is enabled
		void run(void);					// run EMG acquisition, analysis and control

		void attachPin(int ch, int pin);		// assign ADC EMG pin to EMG channel
		bool toggleADCVals(void);				// toggle whether to print ADC vals over serial, return _printVals state							
		
		void setMode(EMGMode mode);		// set EMG to off, simple or proportional mode 
		void off(void);					// set EMG mode to EMG_OFF
		void simple(void);				// set EMG mode to EMG_SIMPLE
		void proportional(void);		// set EMG mode to EMG_PROPORTIONAL

	private:
		EMGchannel _channel[NUM_EMG_CHANNELS];	// EMG channel struct
		bool _printVals;						// flag to determine whether to print ADC values
		EMGMode _mode;							// current EMG mode

		void getSample(void);			// read an EMG sample, add to noise floor, store active component as signal 
		void analyseSignal(void);		// detect whether EMG is in a PEAK or HOLD

		void control(void);				// change grip if HOLD, and run EMG control mode (simple or proportional)
		void control_simple(void);			// toggle direction between open & close when a PEAK is detected
		void control_proportional(void);	// open/close the hand at a speed proportional to the magnitude of the EMG signal

		void printEMGData(void);		// print EMG signal, raw value, noise floor, peak flag and hold flag

		void calcNoiseFloor(int muscleNum, int  muscleVal);		// generate noise floor, only when muscle is inactive 
		double calcPosChange(int signal);						// calculate the 'speed' (change in pos) of movement by using the magnitude of the EMG signal

		bool detect_peakStart(int currVal, int prevVal);		// detect whether a peak has just crossed the peak threshold (sensitivity offset)
		bool detect_peakEnd(int currVal, int prevVal);			// detect whether a peak has just fallen below the peak threshold (sensitivity offset)

};

extern EMG_CONTROL EMG;

#endif // EMG_CONTROL_H_