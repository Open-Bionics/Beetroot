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
*	EMGControl.cpp
*
*/

#include "Globals.h"
#include "EMGControl.h"

#include "Grips.h"
#include "Initialisation.h"
#include "TimerManagement.h"

////////////////////////////// Constructors/Destructors //////////////////////////////

EMG_CONTROL::EMG_CONTROL()
{
	_mode = EMG_OFF;
	_printVals = false;
}

////////////////////////////// Public Methods //////////////////////////////

// for each channel, initialise the noise floor buffer with default value, to prevent initial false positives
void EMG_CONTROL::begin(void)
{
	for (int c = 0; c < NUM_EMG_CHANNELS; c++)
	{
		_channel[c].noiseFloor.begin(NOISE_BUFFER_SIZE);

		for (int i = 0; i < NOISE_BUFFER_SIZE; i++)
		{
			_channel[c].noiseFloor.write(BUFFER_DEFAULT_VAL);
		}
	}


#if defined(USE_I2C_EMG)
	setHeadphoneJack(JACK_I2C);		// set comms switch to I2C over the headphone jack
#else
	setHeadphoneJack(JACK_ADC);		// set comms switch to ADC over the headphone jack

	attachPin(0, A4);		// if using analogue EMG, attach analogue pins
	#if (NUM_EMG_CHANNELS > 1 )
		attachPin(1, A5);	// if using analogue EMG, attach analogue pins
	#endif
#endif
}

// check whether EMG mode is enabled
bool EMG_CONTROL::enabled(void)
{
	if ((_mode == MODE_EMG_SIMPLE) || (_mode == MODE_EMG_PROP))
		return true;
	else
		return false;
}

// run EMG acquisition, analysis and control
void EMG_CONTROL::run(void)
{
	if (_mode == EMG_OFF)
		return;

	// read the raw sample data and remove the noise floor
	getSample();

	// detect PEAK or HOLD on each EMG channel
	analyseSignal();

	// print ADC values if M3 is enabled
	if (_printVals)
		printEMGData();

	// control hand using either simple or proportional mode
	control();
}

// assign ADC EMG pin to EMG channel
void EMG_CONTROL::attachPin(int ch, int pin)
{
	if (ch < NUM_EMG_CHANNELS)
		_channel[ch].pin = pin;
}

// enable/disable ADC vals printing over serial
bool EMG_CONTROL::toggleADCVals(void)
{
	_printVals = !_printVals;

	return _printVals;
}

// set EMG to off, simple or proportional mode 
void EMG_CONTROL::setMode(EMGMode mode)
{
	if (IS_BETWEEN(mode, EMG_OFF, EMG_PROPORTIONAL))
	{
		_mode = mode;
	}
}

// set EMG mode to EMG_OFF
void EMG_CONTROL::off(void)
{
	setMode(EMG_OFF);
}

// set EMG mode to EMG_SIMPLE
void EMG_CONTROL::simple(void)
{
	setMode(EMG_SIMPLE);
}

// set EMG mode to EMG_PROPORTIONAL
void EMG_CONTROL::proportional(void)
{
	setMode(EMG_PROPORTIONAL);
}


////////////////////////////// Private Methods //////////////////////////////

// read an EMG sample, add to noise floor, store active component as signal
void EMG_CONTROL::getSample(void)
{
	for (uint8_t c = 0; c < NUM_EMG_CHANNELS; c++)
	{
		// read raw EMG sample
#ifdef USE_I2C_ADC
		_channel[c].sample = ADC2.read(c);
#else
		noInterrupts();
		_channel[c].sample = analogRead(_channel[c].pin);
		interrupts();
#endif

		// add to noise floor if muscle is NOT active
		calcNoiseFloor(c, _channel[c].sample);

		// store the previous signal for signal analysis
		_channel[c].prevSignal = _channel[c].signal;

		// signal is the size of the active signal (signal > noise floor), and is always >= 0
		_channel[c].signal = _channel[c].sample - _channel[c].noiseFloor.readMean();
		if (_channel[c].signal < 0)
			_channel[c].signal = 0;

		// check signal activation
		if (_channel[c].signal > 0)
			_channel[c].active = true;
		else
			_channel[c].active = false;
	}
}

// detect whether EMG is in a PEAK or HOLD
void EMG_CONTROL::analyseSignal(void)
{
	for (int c = 0; c < NUM_EMG_CHANNELS; c++)
	{

		//if (!_channel[c].active)		// if the muscle is not active
		//	break;						// end channel analysis

		if (!_channel[c].active)			// if the muscle is not active
		{
			_channel[c].PEAK = false;		// clear PEAK flag	
			_channel[c].HOLD = false;		// set HOLD flag
			_channel[c].HOLD_timer.stop();	// stop HOLD timer

			break;							// end channel analysis
		}

		if (_channel[c].HOLD_timer.started())			// if HOLD timer is running
		{
			if (_channel[c].HOLD_timer.timeElapsed(settings.emg.holdTime) && !_channel[c].HOLD)	// if PEAK has been HELD and HOLD flag is not set
			{
				_channel[c].HOLD = true;				// set HOLD flag		
				_channel[c].HOLD_timer.stop();			// stop HOLD timer
			}
		}

		if (detect_peakStart(_channel[c].signal, _channel[c].prevSignal))							// if at PEAK_START
		{
			_channel[c].HOLD_timer.start();				// start HOLD timer
#if (NUM_EMG_CHANNELS == 2)
			_channel[c].PEAK = true;					// set single PEAK flag	
#endif
		}
		else if (detect_peakEnd(_channel[c].signal, _channel[c].prevSignal))						// if at PEAK_END
		{
			_channel[c].HOLD_timer.stop();				// stop HOLD timer

#if (NUM_EMG_CHANNELS == 1)
			// if the muscle is not held, then trigger a PEAK
			if (!_channel[c].HOLD)
				_channel[c].PEAK = true;				// set single PEAK flag	

#else
			_channel[c].PEAK = false;					// clear PEAK flag	
#endif
		}
	}
}

// print EMG signal, raw value, noise floor, peak flag and hold flag
void EMG_CONTROL::printEMGData(void)
{
	const char* open_close[2] = { "Open", "Close" };

	for (int c = 0; c < NUM_EMG_CHANNELS; c++)
	{
#if (NUM_EMG_CHANNELS == 2)
		MYSERIAL_PRINT(open_close[c]);
		MYSERIAL_PRINT_PGM("  ");
#endif
		MYSERIAL_PRINT_PGM("M");
		MYSERIAL_PRINT(c);
		MYSERIAL_PRINT_PGM(": ");
		MYSERIAL_PRINT(_channel[c].signal);
#if defined(PRINT_MORE_EMG_DETAIL)
		MYSERIAL_PRINT_PGM("\tR");
		MYSERIAL_PRINT(c);
		MYSERIAL_PRINT_PGM(": ");
		MYSERIAL_PRINT(_channel[c].sample);
		MYSERIAL_PRINT_PGM("\tN");
		MYSERIAL_PRINT(c);
		MYSERIAL_PRINT_PGM(": ");
		MYSERIAL_PRINT(_channel[c].noiseFloor.readMean());
		MYSERIAL_PRINT_PGM("\tP");
		MYSERIAL_PRINT(c);
		MYSERIAL_PRINT_PGM(": ");
		MYSERIAL_PRINT(_channel[c].PEAK);
		MYSERIAL_PRINT_PGM("\tH");
		MYSERIAL_PRINT(c);
		MYSERIAL_PRINT_PGM(": ");
		MYSERIAL_PRINT(_channel[c].HOLD);
#endif

		MYSERIAL_PRINT_PGM("\t");
	}

	MYSERIAL_PRINT_PGM("\n");
}

// change grip if HOLD, and run EMG control mode (simple or proportional)
void EMG_CONTROL::control(void)
{
	// if the OPEN muscle has been HELD
	if (_channel[0].HOLD)			
	{
		// reset HOLD flag
		_channel[0].HOLD = false;

		// cycle to next grip
		Grip.nextGrip();
		Grip.open();
		Grip.run();

		MYSERIAL_PRINTLN_PGM("Grip Change");
		MYSERIAL_PRINT_PGM("Grip ");
		MYSERIAL_PRINTLN(Grip.getGripName());
	}

	// run EMG control mode
	switch (settings.mode)
	{
	case MODE_EMG_SIMPLE:
		control_simple();
		break;
	case MODE_EMG_PROP:
		control_proportional();
		break;
	default:
		break;
	}
}

// toggle direction between open & close when a PEAK is detected
void EMG_CONTROL::control_simple(void)
{
	const char *open_close[2] = { "OPEN","CLOSE" };

#if (NUM_EMG_CHANNELS == 1)

	// if channel has triggered a PEAK, and hand is not already OPEN
	if (_channel[0].PEAK)
	{
		_channel[0].PEAK = false;                // reset PEAK trigger

		Grip.setDir(!Grip.getDir());
		Grip.run();

		MYSERIAL_PRINT_PGM("Arm ");
		MYSERIAL_PRINTLN(open_close[Grip.getDir()]);
	}

#elif (NUM_EMG_CHANNELS == 2)
	// if OPEN channel has triggered a PEAK, and hand is not already OPEN
	if (_channel[0].PEAK && (Grip.getDir() != OPEN))
	{
		_channel[0].PEAK = false;                // reset PEAK trigger
		MYSERIAL_PRINTLN_PGM("Arm OPEN");

		Grip.setDir(OPEN);

	}
	else if (_channel[1].PEAK && (Grip.getDir() != CLOSE))
	{
		_channel[1].PEAK = false;                // reset PEAK trigger
		MYSERIAL_PRINTLN_PGM("Arm CLOSE");
		
		Grip.setDir(CLOSE);
	}

	Grip.run();

#endif
}

// open/close the hand at a speed proportional to the magnitude of the EMG signal
void EMG_CONTROL::control_proportional(void)
{
#if (NUM_EMG_CHANNELS == 1)

	static double pos = 0;

#if defined(TENSE_CLOSE_RELAX_OPEN)
	if (EMG[0].signal > 0)
	{
		pos -= calcPosChange(EMG[0].signal);
	}
	else
	{
		pos += calcPosChange(900);
	}
#elif defined(TENSE_OPEN_RELAX_CLOSE)
	if (EMG[0].signal > 0)
	{
		pos += calcPosChange(EMG[0].signal);
	}
	else
	{
		pos -= calcPosChange(900);
	}
#endif

	//MYSERIAL_PRINT_PGM("pos: ");
	//MYSERIAL_PRINTLN_PGM(pos);

	pos = constrain(pos, 0, 100);

	Grip.setPos(pos);
	Grip.run();

#elif (NUM_EMG_CHANNELS == 2)
	static double pos = 0;

	if (_channel[0].signal > _channel[1].signal)
	{
		pos += calcPosChange(_channel[0].signal);
	}
	else if (_channel[0].signal < _channel[1].signal)
	{
		pos -= calcPosChange(_channel[1].signal);
	}

	//MYSERIAL_PRINT_PGM("pos: ");
	//MYSERIAL_PRINTLN_PGM(pos);

	pos = constrain(pos, 0, 100);

	Grip.setPos(pos);
	Grip.run();

#endif
}

// generate noise floor, only when muscle is inactive 
void EMG_CONTROL::calcNoiseFloor(int muscleNum, int  muscleVal)
{
	const int dampening = 3;		// used to reduce the noise floor creeping upwards too quickly when the muscle is held tensed

	// if muscle value is less than the noise floor + sensitivity offset / dampening
	// (this reduces upwards creep of the noise floor when the muscle is held tensed)
	if (muscleVal < (_channel[muscleNum].noiseFloor.readMean() + (settings.emg.peakThresh / dampening)))
	{
		_channel[muscleNum].noiseFloor.write(muscleVal);		// add to noise floor buffer
	}
}

// calculate the 'speed' (change in pos) of movement by using the magnitude of the EMG signal
double EMG_CONTROL::calcPosChange(int signal)
{
	bool invert = false;
	double _exp;

	double  power = 2;				// expo 
	double sensitivity = 2;			// gradient 

	static double k = pow((1024 / sensitivity), power);		// normalise, then modify expo sensitivity

	if (isEven(power))				// fix issue that -x^2 = x?^2 
		invert = true;

	//MYSERIAL_PRINT_PGM("signal: ");
	//MYSERIAL_PRINT_PGM(signal);
	//MYSERIAL_PRINT_PGM("\tpower: ");
	//MYSERIAL_PRINT_PGM(power);
	//MYSERIAL_PRINT_PGM("\tk: ");
	//MYSERIAL_PRINT_PGM(k);


	// calculate position change as exponent (sensitivity is x^power proportional to _yPos)
	_exp = pow(signal, power) / k;

	//MYSERIAL_PRINT_PGM("_exp: ");
	//MYSERIAL_PRINTLN_PGM(_exp);

	if (invert)						// fix issue that -x^2 = x?^2
		_exp = -_exp;

	return _exp;
}

// detect whether a peak has just crossed the peak threshold (sensitivity offset)
bool EMG_CONTROL::detect_peakStart(int currVal, int prevVal)
{
	return ((prevVal < settings.emg.peakThresh) && (currVal >= settings.emg.peakThresh));
}

// detect whether a peak has just fallen below the peak threshold (sensitivity offset)
bool EMG_CONTROL::detect_peakEnd(int currVal, int prevVal)
{
	return ((prevVal >= settings.emg.peakThresh) && (currVal < settings.emg.peakThresh));
}


EMG_CONTROL EMG;