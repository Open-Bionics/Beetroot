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

#include "Globals.h"
#include "HANDle.h"

#include "Grips.h"
#include "Initialisation.h"
#include "Wiichuck.h"


HANDLE HANDle;

// CLASS INSTANCE
HANDLE::HANDLE()
{

}

// check whether demo mode is enabled
bool HANDLE::enabled(void)
{
	return _en;
}

void HANDLE::begin(void)
{
	setHeadphoneJack(JACK_I2C);
	delay(100);

	_en = false;

	_init = false;
	_serialFlag = false;

	exp = _pos = 0;
	_yJoy = _xJoy = 0;


	wii.init();
	MYSERIAL_PRINTLN_PGM("Calibrating mid position of joystick");
	wii.poll();
	delay(100);
	wii.poll();
	wii.calibrate();  // calibration (needs a poll before hand to work)
	delay(100);
	MYSERIAL_PRINTLN_PGM("Complete");

	_pos = 0;
	Grip.setPos(_pos);
	Grip.run();

	MYSERIAL_PRINTLN(Grip.getGripName());		// print current grip name
	_init = true;

}

void HANDLE::run(void)
{
	if (!_init)
		begin();

	delay(50);

	if (wii.poll())				// if Nunchuck data is available
	{
		checkJoy();					// check joystick, perform movement or change group
		checkButtons();			// check buttons, if pressed calibrate/change grip 

		Grip.run();

		if(_serialFlag)
			print();
	}
}


void HANDLE::enable(void)
{
	enable_disable(true);
}

void HANDLE::disable(void)
{
	enable_disable(false);
}

bool HANDLE::toggleEnable(void)
{
	_en = !_en;
	return _en;
}

void HANDLE::enable_disable(bool en)
{
	_en = en;
}




bool HANDLE::toggleSerial(void)
{
	const char *disabled_enabled[2] = { "Disabled", "Enabled" };

	_serialFlag = !_serialFlag;
	MYSERIAL_PRINT_PGM("HANDle Serial ");
	MYSERIAL_PRINTLN(disabled_enabled[_serialFlag]);

	return _serialFlag;
}

void HANDLE::print(void)
{
	MYSERIAL_PRINT_PGM("Grip ");
	MYSERIAL_PRINT(Grip.getGripName());
	MYSERIAL_PRINT_PGM("  \tyPos ");
	MYSERIAL_PRINT(_yJoy);
	MYSERIAL_PRINT_PGM("  \txPos ");
	MYSERIAL_PRINT(_xJoy);

	MYSERIAL_PRINT_PGM("  \texp ");
	MYSERIAL_PRINT(exp);
	MYSERIAL_PRINT_PGM("  \tHand pos ");
	MYSERIAL_PRINTLN((int)_pos);
}


void HANDLE::checkButtons(void)
{
	bool btn_c, btn_z;

	// get button states
	btn_c = wii.buttonC();
	btn_z = wii.buttonZ();

	// if no buttons are pressed, return
	if (!btn_c && !btn_z)
		return;

	// if both buttons are pressed
	if (btn_c && btn_z)
	{
		MYSERIAL_PRINTLN_PGM("Both Pressed - Calibrating mid position of joystick");
		wii.poll();
		delay(100);
		wii.poll();
		wii.calibrate();	// calibration (needs a poll beforehand to work)
		
		return;				// return after calibration to prevent false grip change
	}

	// change grip on a button press
	if (btn_c)
		Grip.nextGrip();
	if (btn_z)
		Grip.prevGrip();

	MYSERIAL_PRINTLN(Grip.getGripName());
	delay(200);				// add delay to prevent too many sequential grip changes
}


void HANDLE::checkJoy(void)
{
	// add new grip position to current grip positions
	_pos += calcPosChange();
	_pos = constrain(_pos, 0, HANDLE_JOY_MAX);
	Grip.setPos(_pos);

#ifdef USE_GRIP_GROUPS
	checkGroupChange();
#endif
}

double HANDLE::calcPosChange(void)
{
	bool invert = false;
	//uint8_t  power = 3;
	//static double k = pow(HANDLE_JOY_MAX / 3, power);			// normalise, then modify expo sensitivity

	uint8_t  power = 2;
	static double k = pow((HANDLE_JOY_MAX / 3), power);			// normalise, then modify expo sensitivity

	// get joystick position
	_yJoy = wii.joyY();

	if (isEven(power) && (_yJoy < 0))				// fix issue that -x^2 = x?^2 
		invert = true;

	// calculate position change as exponent (sensitivity is x^power proportional to _yPos)
	exp = pow(_yJoy, power) / k;

	if (invert)						// fix issue that -x^2 = x?^2
		exp = -exp;
	
	return exp;
}
