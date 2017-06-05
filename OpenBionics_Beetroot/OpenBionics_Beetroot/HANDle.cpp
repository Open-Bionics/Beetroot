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

// check whether demo mode is enabled
bool HANDLE_CLASS::enabled(void)
{
	return _en;
}

void HANDLE_CLASS::enable(void)
{
	enable_disable(true);
}

void HANDLE_CLASS::disable(void)
{
	enable_disable(false);
}

bool HANDLE_CLASS::toggleEnable(void)
{
	_en = !_en;
	return _en;
}

void HANDLE_CLASS::enable_disable(bool en)
{
	_en = en;
}

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
	// set the headphone jack to be used for I2C
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

	exp = 0;
	_pos = 0;
	Grip.setPos(_pos);
	Grip.run();

	MYSERIAL_PRINTLN(Grip.getGripName());		// print current grip name
	_init = true;
}

void HANDLE_CLASS::run(void)
{
	if (!_init)
	{
		begin();
	}

	if (poll())					// if Nunchuck data is available
	{
		checkJoy();				// check joystick, perform movement or change group
		checkButtons();			// check buttons, if pressed calibrate/change grip 

		Grip.run();

		if (_serialFlag)
			print();
	}
}


uint8_t HANDLE_CLASS::poll(void)
{
	const uint8_t numI2CVals = 6;

	uint8_t values[numI2CVals];
	uint8_t count = 0;

	// read I2C values
	Wire.requestFrom(HANDLE_ADDR, numI2CVals);
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

	MYSERIAL.print(raw.joy.x);
	MYSERIAL.print(" ");
	MYSERIAL.print(raw.joy.y);
	MYSERIAL.print(" ");
	MYSERIAL.print(raw.accel.x);
	MYSERIAL.print(" ");
	MYSERIAL.print(raw.accel.y);
	MYSERIAL.print(" ");
	MYSERIAL.print(raw.accel.z);
	MYSERIAL.print(" ");
	MYSERIAL.print(raw.btn.z);
	MYSERIAL.print(" ");
	MYSERIAL.println(raw.btn.c);


	Wire.beginTransmission(HANDLE_ADDR);
	Wire.write(0x00);
	delay(HANDLE_I2C_DEL);
	Wire.write(0x00);
	delay(HANDLE_I2C_DEL);
	Wire.endTransmission();

	delay(5);

	return count;
}

void HANDLE_CLASS::calibrate(void)
{
	MYSERIAL.println("Calibrating...");
	_calibrating = true;

	poll();

	calib.joy.x = raw.joy.x - HANDLE_CAL_JOY_MID;
	calib.joy.y = raw.joy.y - HANDLE_CAL_JOY_MID;
	calib.accel.x = raw.accel.x;
	calib.accel.y = raw.accel.y;
	calib.accel.z = raw.accel.z;

	_calibrating = false;

	MYSERIAL.print("Calibration vals ");

	MYSERIAL.print(calib.joy.x);
	MYSERIAL.print(" ");
	MYSERIAL.print(calib.joy.y);
	MYSERIAL.print(" ");
	MYSERIAL.print(calib.accel.x);
	MYSERIAL.print(" ");
	MYSERIAL.print(calib.accel.y);
	MYSERIAL.print(" ");
	MYSERIAL.print(calib.accel.z);
	MYSERIAL.println(" Complete");
}




void HANDLE_CLASS::print(void)
{
	MYSERIAL_PRINT_PGM("Grip ");
	MYSERIAL_PRINT(Grip.getGripName());
	MYSERIAL_PRINT_PGM("  \tyPos ");
	MYSERIAL_PRINT(raw.joy.y);
	MYSERIAL_PRINT_PGM("  \txPos ");
	MYSERIAL_PRINT(raw.joy.x);

	MYSERIAL_PRINT_PGM("  \texp ");
	MYSERIAL_PRINT(exp);
	MYSERIAL_PRINT_PGM("  \tHand pos ");
	MYSERIAL_PRINTLN((int)_pos);
}


void HANDLE_CLASS::checkButtons(void)
{
	// if no buttons are pressed, return
	if (!raw.btn.c && !raw.btn.z)
		return;

	// if both buttons are pressed
	if (raw.btn.c && raw.btn.z)
	{
		MYSERIAL.println("Both Pressed - Calibrating mid position of joystick");
		calibrate();

		return;				// return after calibration to prevent false grip change
	}

	// change grip on a button press
	if (raw.btn.c)
		Grip.nextGrip();
	if (raw.btn.z)
		Grip.prevGrip();

	MYSERIAL_PRINTLN(Grip.getGripName());
	delay(200);				// add delay to prevent too many sequential grip changes
}


void HANDLE_CLASS::checkJoy(void)
{
	// add new grip position to current grip positions
	_pos += calcPosChange();
	_pos = constrain(_pos, 0, HANDLE_JOY_MAX);
	Grip.setPos(_pos);

#ifdef USE_GRIP_GROUPS
	checkGroupChange();
#endif
}

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
	exp = pow(raw.joy.y, power) / k;

	if (invert)						// fix issue that -x^2 = x?^2
		exp = -exp;

	return exp;
}


