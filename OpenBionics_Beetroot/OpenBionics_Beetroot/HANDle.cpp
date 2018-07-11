/*	Open Bionics - Beetroot
*	Author - Olly McBride
*	Date -January 2017
*
*	This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
*	To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
*
*	Website - http://www.openbionics.com/
*	GitHub - https://github.com/Open-Bionics
*	Email - ollymcbride@openbionics.com
*
*	HANDle.cpp
*
*/
#include <Wire.h>

#include "Globals.h"
#include "HANDle.h"

#include "Grips.h"
#include "Initialisation.h"

HANDLE_CLASS HANDle;

// CLASS INSTANCE
HANDLE_CLASS::HANDLE_CLASS()
{

}

// check whether HANDle mode is enabled
bool HANDLE_CLASS::enabled(void)
{
	return _en;
}

// enable HANDle mode
void HANDLE_CLASS::enable(void)
{
	enable_disable(true);
}

// disable HANDle mode
void HANDLE_CLASS::disable(void)
{
	enable_disable(false);
}

// toggle HANDle mode between enable and disable
bool HANDLE_CLASS::toggleEnable(void)
{
	_en = !_en;
	return _en;
}

// set HANDle mode to be enabled or disabled
void HANDLE_CLASS::enable_disable(bool en)
{
	_en = en;
}

// toggle whether to 
bool HANDLE_CLASS::toggleSerial(void)
{
	const char *disabled_enabled[2] = { "Disabled", "Enabled" };

	_serialFlag = !_serialFlag;
	MYSERIAL_PRINT_PGM("HANDle Serial ");
	MYSERIAL_PRINTLN(disabled_enabled[_serialFlag]);

	return _serialFlag;
}

void HANDLE_CLASS::begin(void)
{
	// initialise the variables to 0
	_exp = 0;
	_pos = 0;

	// set headphone jack to I2C
	setHeadphoneJack(JACK_I2C);			

	// initialise the Nunchuck
	Wire.beginTransmission(HANDLE_ADDR);
	Wire.write(0xF0);
	delay(HANDLE_I2C_DEL);
	Wire.write(0x55);
	delay(HANDLE_I2C_DEL);
	Wire.endTransmission();

	Wire.beginTransmission(HANDLE_ADDR);
	Wire.write(0xFB);
	delay(HANDLE_I2C_DEL);
	Wire.write(0x00);
	delay(HANDLE_I2C_DEL);
	Wire.endTransmission();

	delay(10);

	poll();					// read data and discard the first readings from the Nunchuck
	calibrate();			// calibrate the Nunchuck with fresh data

	
	// print current grip name and open the hand
	MYSERIAL_PRINTLN(Grip.getGripName());	
	Grip.open();


	//Grip.setSpeed(MAX_FINGER_PWM);
	//Grip.open();
	//delay(700);
	//Grip.setSpeed(MAX_FINGER_PWM);
	//Grip.run();
}

void HANDLE_CLASS::run(void)
{
	static bool init = false;

	// if the HANDle has not been initialised
	if (!init)
	{	
		begin();				// initialise the HANDle
		init = true;
	}

	// if Nunchuck data is available
	if (poll())					
	{
		checkJoy();				// check joystick, perform movement or change group
		checkButtons();			// check buttons, if pressed calibrate/change grip 

		// if the 
		if (_serialFlag)
		{
			print();
		}
	}
}

uint8_t HANDLE_CLASS::poll(void)
{
	const uint8_t numI2CVals = 6;

	uint8_t values[numI2CVals];
	uint8_t count = 0;

	// read I2C values
	Wire.requestFrom((uint8_t)HANDLE_ADDR, (uint8_t)numI2CVals);
	while (Wire.available())
	{
		values[count] = Wire.read();
		count++;
		delay(HANDLE_I2C_DEL);
	}

	// decode and store received values
	raw.joy.x = values[0];
	raw.joy.y = values[1];
	raw.accel.x = (values[2] << 2) | ((values[5] >> 2) & 0x03);
	raw.accel.y = (values[3] << 2) | ((values[5] >> 4) & 0x03);
	raw.accel.z = (values[4] << 2) | ((values[5] >> 6) & 0x03);
	raw.btn.z = !((values[5] >> 0) & 0x01);
	raw.btn.c = !((values[5] >> 1) & 0x01);

	// if the device is not calibrating, subtract the calibration values
	if (!_calibrating)
	{
		raw.joy.x -= calib.joy.x;
		raw.joy.y -= calib.joy.y;
		raw.accel.x -= calib.accel.x;
		raw.accel.y -= calib.accel.y;
		raw.accel.z -= calib.accel.z;
	}

	Wire.beginTransmission(HANDLE_ADDR);
	Wire.write(0x00);
	delay(HANDLE_I2C_DEL);
	Wire.write(0x00);
	delay(HANDLE_I2C_DEL);
	Wire.endTransmission();

	delay(5);				// allow time for the Nunchuck to process data



#ifdef PRINT_RAW_VALUES
	MYSERIAL_PRINT(raw.joy.x);
	MYSERIAL_PRINT_PGM(" ");
	MYSERIAL_PRINT(raw.joy.y);
	MYSERIAL_PRINT_PGM(" ");
	MYSERIAL_PRINT(raw.accel.x);
	MYSERIAL_PRINT_PGM(" ");
	MYSERIAL_PRINT(raw.accel.y);
	MYSERIAL_PRINT_PGM(" ");
	MYSERIAL_PRINT(raw.accel.z);
	MYSERIAL_PRINT_PGM(" ");
	MYSERIAL_PRINT(raw.btn.z);
	MYSERIAL_PRINT_PGM(" ");
	MYSERIAL_PRINTLN(raw.btn.c);
#endif


	return count;
}

void HANDLE_CLASS::calibrate(void)
{
	MYSERIAL_PRINT_PGM("Calibrating...");
	_calibrating = true;

	poll();

	calib.joy.x = raw.joy.x - HANDLE_CAL_JOY_MID;
	calib.joy.y = raw.joy.y - HANDLE_CAL_JOY_MID;
	calib.accel.x = raw.accel.x;
	calib.accel.y = raw.accel.y;
	calib.accel.z = raw.accel.z;

	_calibrating = false;

	MYSERIAL_PRINTLN_PGM(" Complete");
}



// GRIP POS CONTROL
void HANDLE_CLASS::checkJoy(void)
{
	// add new grip position to current grip positions
	_pos += calcPosChange();
	_pos = constrain(_pos, 0, 100);
	Grip.setSpeed(MAX_FINGER_PWM);
	Grip.setPos(_pos);
	Grip.run();
}


void HANDLE_CLASS::checkButtons(void)
{
	// if no buttons are pressed, return
	if (!raw.btn.c && !raw.btn.z)
		return;

	// if both buttons are pressed
	if (raw.btn.c && raw.btn.z)
	{
		MYSERIAL_PRINTLN_PGM("Both Pressed - Calibrating mid position of joystick");
		calibrate();

		return;				// return after calibration to prevent false grip change
	}

	// change grip on a button press
	if (raw.btn.c)
	{
		Grip.nextGrip();
	}
	if (raw.btn.z)
	{
		Grip.prevGrip();
	}


	MYSERIAL_PRINTLN(Grip.getGripName());
	Grip.setSpeed(MAX_FINGER_PWM);
	Grip.open();
	delay(500);
	Grip.setSpeed(0);
	Grip.run();


	delay(200);				// add delay to prevent too many sequential grip changes
}


//// GRIP SPEED CONTROL
//void HANDLE_CLASS::checkJoy(void)
//{
//	static int prevSp = 0;
//	int sp = map(raw.joy.y, -HANDLE_JOY_MAX, HANDLE_JOY_MAX, -255, 255);
//
//	if (sp != prevSp)
//	{
//		if (sp < 0)
//		{
//			Grip.setDir(CLOSE);
//		}
//		else if (sp > 0)
//		{
//			Grip.setDir(OPEN);
//		}
//
//		Grip.setSpeed(abs(sp));
//		Grip.run();
//	}
//	prevSp = sp;
//}





double HANDLE_CLASS::calcPosChange(void)
{
	bool invert = false;
	//uint8_t  power = 3;
	//static double k = pow(HANDLE_JOY_MAX / 3, power);			// normalise, then modify expo sensitivity

	uint8_t  power = 2;
	static double k = pow((HANDLE_JOY_MAX / 3), power);			// normalise, then modify expo sensitivity

	if (isEven(power) && (raw.joy.y < 0))				// fix issue that -x^2 = x?^2 
		invert = true;

	// calculate position change as exponent (sensitivity is x^power proportional to _yPos)
	_exp = pow(raw.joy.y, power) / k;

	if (invert)						// fix issue that -x^2 = x?^2
	{
		_exp = -_exp;
	}

	return _exp;
}



void HANDLE_CLASS::print(void)
{
	MYSERIAL_PRINT(Grip.getGripName());
	MYSERIAL_PRINT_PGM("  \tPos ");
	MYSERIAL_PRINT(raw.joy.y);

	MYSERIAL_PRINT_PGM("  \texp ");
	MYSERIAL_PRINT(_exp);
	MYSERIAL_PRINT_PGM("  \tHand pos ");
	MYSERIAL_PRINTLN((int)_pos);
}