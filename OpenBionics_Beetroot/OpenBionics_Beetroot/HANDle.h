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

#include "Wiichuck.h"			// for Wiichuck class

#define HANDLE_JOY_MAX			100

class HANDLE
{
	public:
		HANDLE();

		bool enabled(void);
		void begin(void);
		void run(void);

		void enable(void);
		void disable(void);
		bool toggleEnable(void);
		void enable_disable(bool en);


		bool toggleSerial(void);
		void print(void);

	private:
		bool _en;

		Wiichuck wii;

		bool _init;
		bool _serialFlag;

		double exp;
		double _pos;

		int16_t _yJoy, _xJoy;

		void checkButtons(void);
		void checkJoy(void);

		double calcPosChange(void);
		
};

extern HANDLE HANDle;


#endif // HANDLE_H_
