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
*	ROS.cpp
*
*/




#include "ROS.h"

#ifdef USE_ROS

// ROS NODE
ros::NodeHandle ob_node;

void ros_targ_cb(const ob_hand::FingerMsg &msg)
{

    if (msg.thumb.speed != (int16_t)BLANK)
      finger[0].writeSpeed(msg.thumb.speed);

    if (msg.thumb.pos != (int16_t)BLANK)
      finger[0].writePos(msg.thumb.pos);

    if (msg.thumb.dir != (int16_t)BLANK)
      finger[0].writeDir(msg.thumb.dir);

    if (msg.thumb.force != (int16_t)BLANK)
      finger[0].writeForce(msg.thumb.force, CLOSE);



    if (msg.index.speed != (int16_t)BLANK)
      finger[1].writeSpeed(msg.index.speed);

    if (msg.index.pos != (int16_t)BLANK)
      finger[1].writePos(msg.index.pos);

    if (msg.index.dir != (int16_t)BLANK)
      finger[1].writeDir(msg.index.dir);

    if (msg.index.force != (int16_t)BLANK)
      finger[1].writeForce(msg.index.force, CLOSE);



    if (msg.middle.speed != (int16_t)BLANK)
      finger[2].writeSpeed(msg.middle.speed);

    if (msg.middle.pos != (int16_t)BLANK)
      finger[2].writePos(msg.middle.pos);

    if (msg.middle.dir != (int16_t)BLANK)
      finger[2].writeDir(msg.middle.dir);

    if (msg.middle.force != (int16_t)BLANK)
      finger[2].writeForce(msg.middle.force, CLOSE);



    if (msg.ring.speed != (int16_t)BLANK)
      finger[3].writeSpeed(msg.ring.speed);

    if (msg.ring.pos != (int16_t)BLANK)
      finger[3].writePos(msg.ring.pos);

    if (msg.ring.dir != (int16_t)BLANK)
      finger[3].writeDir(msg.ring.dir);

    if (msg.ring.force != (int16_t)BLANK)
      finger[3].writeForce(msg.ring.force, CLOSE);




  
//	for (int i = 0; i < msg.finger_length; i++)
//	{
//		if (msg.finger[i].speed != (int16_t)BLANK)
//			finger[i].writeSpeed(msg.finger[i].speed);
//
//		if (msg.finger[i].pos != (int16_t)BLANK)
//			finger[i].writePos(msg.finger[i].pos);
//
//		if (msg.finger[i].dir != (int16_t)BLANK)
//			finger[i].writeDir(msg.finger[i].dir);
//
//		if (msg.finger[i].force != (int16_t)BLANK)
//			finger[i].writeForce(msg.finger[i].force, CLOSE);
//	}
}

// ROS TOPIC - targ
ros::Subscriber<ob_hand::FingerMsg> ros_targ_topic("ob_hand/targ", ros_targ_cb);

// ROS TOPIC - curr
ob_hand::FingerMsg ros_curr_msg;
ros::Publisher ros_curr_topic("ob_hand/curr", &ros_curr_msg);

// initialise ros node and topics
void ros_setup(void)
{
	ob_node.initNode();
	ob_node.advertise(ros_curr_topic);
	ob_node.subscribe(ros_targ_topic);
}

// manage ros topics
void ros_run(void)
{
	ros_setup();
	while (1)
	{
		ros_curr_msg = ros_generate_curr_msg();
		ros_curr_topic.publish(&ros_curr_msg);

		ob_node.spinOnce();

		delay(50);
	}
}

// generate curr_msg with all of the finger data
ob_hand::FingerMsg& ros_generate_curr_msg(void)
{
	ob_hand::FingerMsg msg;

    msg.thumb.speed = finger[0].readSpeed();
    msg.thumb.pos = finger[0].readPos();
    msg.thumb.dir = finger[0].readDir();
    msg.thumb.force = finger[0].readForce();

    msg.index.speed = finger[1].readSpeed();
    msg.index.pos = finger[1].readPos();
    msg.index.dir = finger[1].readDir();
    msg.index.force = finger[1].readForce();

    msg.middle.speed = finger[2].readSpeed();
    msg.middle.pos = finger[2].readPos();
    msg.middle.dir = finger[2].readDir();
    msg.middle.force = finger[2].readForce();

    msg.ring.speed = finger[3].readSpeed();
    msg.ring.pos = finger[3].readPos();
    msg.ring.dir = finger[3].readDir();
    msg.ring.force = finger[3].readForce();

//	for (int i = 0; i < NUM_FINGERS; i++)
//	{
//		msg.finger[i].speed = finger[i].readSpeed();
//		msg.finger[i].pos = finger[i].readPos();
//		msg.finger[i].dir = finger[i].readDir();
//		msg.finger[i].force = finger[i].readForce();
//	}

	return msg;
}

























#endif // USE_ROS
