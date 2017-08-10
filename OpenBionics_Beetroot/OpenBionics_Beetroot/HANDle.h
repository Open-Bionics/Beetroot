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
*	HANDle.h
*
*/

#ifndef HANDLE_H_
#define HANDLE_H_

#define HANDLE_ADDR			0x52

#define HANDLE_JOY_MAX		100
#define HANDLE_I2C_DEL		1		// ms

#define HANDLE_CAL_JOY_MID	0

// uncomment the following line to print the raw values from the HANDle
//#define PRINT_RAW_VALUES

typedef struct _Axis
{
	int16_t x;
	int16_t y;
	int16_t z;
} Axis;

typedef struct _Button
{
	bool z;
	bool c;
} Button;

typedef struct _HANDleVals
{
	Axis joy;
	Axis accel;
	Button btn;
} HANDleVals;

class HANDLE_CLASS
{
public:
	HANDLE_CLASS();

	bool enabled(void);
	void enable(void);
	void disable(void);
	bool toggleEnable(void);
	void enable_disable(bool en);
	bool toggleSerial(void);

	void begin(void);
	void run(void);


private:
	bool _en;
	bool _calibrating = false;
	bool _serialFlag = false;

	HANDleVals raw;
	HANDleVals calib;

	double _exp;
	double _pos;

	uint8_t poll(void);
	void calibrate(void);
	void print(void);

	void checkJoy(void);
	void checkButtons(void);

	double calcPosChange(void);
};

extern HANDLE_CLASS HANDle;


#endif // HANDLE_H_
