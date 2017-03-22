/*	Open Bionics - Beetroot
*	Author - Olly McBride
*	Date - December 2016
*
*	This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
*	To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
*
*	Website - http://www.openbionics.com/
*	GitHub - https://github.com/Open-Bionics
*	Email - ollymcbride@openbionics.com
*
*	ROS.h
*
*/


#ifndef ROS_H_
#define ROS_H_

#include "Globals.h"

#ifdef USE_ROS

#include <FingerLib.h>
#include <ros.h>
#include <ArduinoHardware.h>

#include "ob_hand/FingerMsg.h"		// ROS MSG

#define BLANK (-1)


void ros_setup(void);			// initialise ros node and topics
void ros_run(void);				// manage ros topics
ob_hand::FingerMsg& ros_generate_curr_msg(void);		// generate curr_msg with all of the finger data

#endif

#endif // ROS_H_
