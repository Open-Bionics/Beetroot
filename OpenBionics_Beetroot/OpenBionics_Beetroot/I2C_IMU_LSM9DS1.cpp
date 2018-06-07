/*	Open Bionics - Carrot HC
*	Author - Olly McBride
*	Date - February 2017
*
*	This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
*	To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
*
*	Website - http://www.openbionics.com/
*	GitHub - https://github.com/Open-Bionics
*	Email - ollymcbride@openbionics.com
*
*	I2C_IMU_LSM9DS1.cpp
*
*/



/*	LSM9DS1 - 9 axis IMU, 3D accelerometer, 3D gyroscope, 3D magnetometer
*
*/


#include <Arduino.h>
#include <Wire.h>

#include "I2C_IMU_LSM9DS1.h"

// SCALE VALS FOR RESOLUTION CALC
float IMU_ACCEL_SENSITIVITY[4] = { 0.000061, 0.000732, 0.000122, 0.000244 };	// g/LSB
float IMU_GYRO_SENSITIVITY[3] = { 0.00875, 0.0175, 0.07 };						// dps
float IMU_MAG_SENSITIVITY[4] = { 0.00014, 0.00029, 0.00043, 0.00058 };			// gauss/LSB
float IMU_TEMP_SENSITIVITY = 16;												// LSB/°C

////////////////////////////// Constructors/Destructors //////////////////////////////
LSM9DS1::LSM9DS1()
{
	// ACCEL
	settings.accel.addr = IMU_ACCEL_ADDR;
	settings.accel.en = true;
	settings.accel.enX = true;
	settings.accel.enY = true;
	settings.accel.enZ = true;
	settings.accel.enHighRes = false;
	settings.accel.scale = IMU_ACCEL_SCALE;
	settings.accel.sRate = IMU_ACCEL_S_RATE;
	settings.accel.bWidth = IMU_ACCEL_B_WIDTH;			// actual bandwidth determined by sample rate	
														//settings.accel.bWidthHighRes = 

														// GYRO
	settings.gyro.addr = IMU_GYRO_ADDR;
	settings.gyro.en = true;
	settings.gyro.enX = true;
	settings.gyro.enY = true;
	settings.gyro.enZ = true;
	settings.gyro.enHighRes = false;
	settings.gyro.enLowPower = false;
	settings.gyro.enHighPass = false;
	settings.gyro.scale = IMU_GYRO_SCALE;
	settings.gyro.sRate = IMU_GYRO_S_RATE;
	settings.gyro.bWidth = IMU_GYRO_B_WIDTH;			// actual bandwidth determined by sample rate	
														//settings.gyro.bWidthHighRes = 
														//settings.gyro.cutOffHighPass = 
	settings.gyro.orientation = 0;
	settings.gyro.flipX = false;
	settings.gyro.flipY = false;
	settings.gyro.flipZ = false;

	// MAGNETOMETER
	settings.mag.addr = IMU_MAG_ADDR;
	settings.mag.en = true;
	settings.mag.enLowPower = false;
	settings.mag.enTempComp = false;
	settings.mag.scale = IMU_MAG_SCALE;
	settings.mag.sRate = IMU_MAG_S_RATE;
	settings.mag.mode = 0;
	settings.mag.performanceXY = IMU_MAG_PERFOMANCE_XY;
	settings.mag.performanceZ = IMU_MAG_PERFOMANCE_Z;

	// TEMP
	settings.temp.en = true;


	// CLEAR VARIABLES
	for (uint8_t i = 0; i < LSM9DS1_NUM_AXIS; i++)
	{
		_accelBias[i] = 0;
		_gyroBias[i] = 0;
	}
}

LSM9DS1::~LSM9DS1()
{

}


////////////////////////////// Public Methods //////////////////////////////

// initialise accelerometer, gyro and magnetometer
bool LSM9DS1::begin(void)
{
	// if IMU responds to ping
	if (ping())
	{
		// initialise with presets
		initAccel();
		initGyro();
		initMag();

		return true;
	}
	else
	{
		return false;		// if accel/gyro or mag don't respond, return fail
	}
}

// check if IMU is responding
bool LSM9DS1::ping(void)
{
	return (pingAccelGyro() && pingMag());
}

// check whether accelerometer & gyro is responding
bool LSM9DS1::pingAccelGyro(void)
{
	uint8_t status;

	
	Wire.beginTransmission(settings.accel.addr);
	status = Wire.endTransmission();
	

	// if return status is 'SUCCESS'
	if (status == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// check whether magnetometer is responding
bool LSM9DS1::pingMag(void)
{
	uint8_t status;

	
	Wire.beginTransmission(settings.mag.addr);
	status = Wire.endTransmission();
	

	// if return status is 'SUCCESS'
	if (status == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// read the latest IMU data from the IMU
bool LSM9DS1::poll(void)
{
	// if new data is available, read and store the data
	if (availAccel())
		updateAccel();

	if (availGyro())
		updateGyro();

	if (availMag())
		updateMag();

	if (availTemp())
		updateTemp();
}

// calculate roll, pitch & yaw
void LSM9DS1::calcRollPitchYaw(void)
{

	_roll = atan2((double)getAccelY(), (double)getAccelZ());
	_pitch = atan2((double)-getAccelX(), sqrt(pow((double)getAccelY(), 2) + pow((double)getAccelZ(), 2)));


	float heading;
	if (getMagY() == 0)
		_yaw = (getMagX() < 0) ? 180.0 : 0;
	else
		_yaw = atan2((double)getMagX(), (double)getMagY());

	_yaw -= IMU_DECLINATION * PI / 180;

	if (_yaw > PI) _yaw -= (2 * PI);
	else if (_yaw < -PI) _yaw += (2 * PI);
	else if (_yaw < 0) _yaw += 2 * PI;

	// Convert everything from radians to degrees:
	_yaw *= 180.0 / PI;
	_pitch *= 180.0 / PI;
	_roll *= 180.0 / PI;
}

// get the latest accelerometer data along the X axis
float LSM9DS1::getAccelX(void)
{
	return (_accelRaw[X_AXIS] - _accelBias[X_AXIS]) * _accelRes;
}

// get the latest accelerometer data along the Y axis
float LSM9DS1::getAccelY(void)
{
	return (_accelRaw[Y_AXIS] - _accelBias[Y_AXIS]) * _accelRes;
}

// get the latest accelerometer data along the Z axis
float LSM9DS1::getAccelZ(void)
{
	return (_accelRaw[Z_AXIS] - _accelBias[Z_AXIS]) * _accelRes;
}

// get the latest gyro data along the X axis
float LSM9DS1::getGyroX(void)
{
	return (_gyroRaw[X_AXIS] - _gyroBias[X_AXIS]) * _gyroRes;
}

// get the latest gyro data along the Y axis
float LSM9DS1::getGyroY(void)
{
	return (_gyroRaw[Y_AXIS] - _gyroBias[Y_AXIS]) * _gyroRes;
}

// get the latest gyro data along the Z axis
float LSM9DS1::getGyroZ(void)
{
	return (_gyroRaw[Z_AXIS] - _gyroBias[Z_AXIS]) * _gyroRes;
}

// get the latest mag data along the X axis
float LSM9DS1::getMagX(void)
{
	return _magRaw[X_AXIS] * _magRes;
}

// get the latest mag data along the Y axis
float LSM9DS1::getMagY(void)
{
	return _magRaw[Y_AXIS] * _magRes;
}

// get the latest mag data along the Z axis
float LSM9DS1::getMagZ(void)
{
	return _magRaw[Z_AXIS] * _magRes;
}

// get the latest temp data
float LSM9DS1::getTemp(void)
{
	return LSM9DS1_DEFAULT_TEMP + (_tempRaw / _tempRes);
	//return  LSM9DS1_DEFAULT_TEMP + _tempRaw;
}

// get the roll value
float LSM9DS1::getRoll(void)
{
	return _roll;
}

// get the pitch value
float LSM9DS1::getPitch(void)
{
	return _pitch;
}

// get the yaw value
float LSM9DS1::getYaw(void)
{
	return _yaw;
}


////////////////////////////// Private Methods //////////////////////////////

// INITIALISE THE DEVICE

// initialise the accel with the presets
void LSM9DS1::initAccel(void)
{
	uint8_t regVal = 0;

	// if accel is disabled, do not init
	if (!settings.accel.en)
		return;

	// enable axis
	regVal = (settings.accel.enX << 5) | (settings.accel.enY << 4) | (settings.accel.enZ << 3) | 0;			// | 0 ?

	writeAccel(LSM9DS1_CTRL_REG5_XL, regVal);

	// output data rate, scale and bandwidth
	regVal = (settings.accel.sRate << 5) | (settings.accel.scale << 3) | 0;

	// if a bandwidth is selected
	if (settings.accel.bWidth > 0)
		regVal |= (0x01 << 2) | (settings.accel.bWidth);	// set BW_SCAL_ODR and BW_XL

	writeAccel(LSM9DS1_CTRL_REG6_XL, regVal);

	// if high resolution mode is enabled
	if (settings.accel.enHighRes)
		regVal = (0x01 << 7) | (settings.accel.bWidthHighRes << 5) | 0;	// set HR and DCF

	writeAccel(LSM9DS1_CTRL_REG7_XL, regVal);

}

// initialise the gyro with the presets
void LSM9DS1::initGyro(void)
{
	uint8_t regVal = 0;

	// if gyro is disabled, do not init
	if (!settings.gyro.en)
		return;

	// output data rate, scale and bandwidth
	regVal = (settings.gyro.sRate << 5) | (settings.gyro.scale << 3) | (settings.gyro.bWidth);

	writeGyro(LSM9DS1_CTRL_REG1_G, regVal);

	// power modes, high-pass filter settings
	regVal = (settings.gyro.enLowPower << 7) | (settings.gyro.enHighPass << 6) | (settings.gyro.cutOffHighPass);

	writeGyro(LSM9DS1_CTRL_REG3_G, regVal);

	// enable axis & interrupt settings
	regVal = (settings.gyro.enX << 5) | (settings.gyro.enY << 4) | (settings.gyro.enZ << 3) | 0;	// | 0 ?

	writeGyro(LSM9DS1_CTRL_REG4, regVal);

	// invert axis and orientation
	regVal = (settings.gyro.flipX << 5) | (settings.gyro.flipY << 4) | (settings.gyro.flipZ << 3) | (settings.gyro.orientation);

	writeGyro(LSM9DS1_ORIENT_CONFIG_G, regVal);
}

// initialise the mag with the presets
void LSM9DS1::initMag(void)
{
	uint8_t regVal = 0;

	// if mag is disabled, do not init
	if (!settings.mag.en)
		return;

	// temperature compensation, X & Y operating performance, output data rate
	regVal = (settings.mag.enTempComp << 7) | (settings.mag.performanceXY << 5) | (settings.mag.sRate << 2) | 0;

	writeMag(LSM9DS1_CTRL_REG1_M, regVal);

	// full-scale configuration
	regVal = (settings.mag.scale << 5) | 0;

	writeMag(LSM9DS1_CTRL_REG2_M, regVal);

	// power and operating modes
	regVal = (settings.mag.enLowPower << 5) | (settings.mag.mode);

	writeMag(LSM9DS1_CTRL_REG3_M, regVal);

	// Z-axis operating performance
	regVal = (settings.mag.performanceZ << 2) | 0;

	writeMag(LSM9DS1_CTRL_REG4_M, regVal);
}



// CHECK IF NEW DATA IS AVAILABLE

// check status register to determine whether new data is available
bool LSM9DS1::availAccel(void)
{
	uint8_t status = readAccel(LSM9DS1_STATUS_REG);

	return (status & 0x01);
}

// check status register to determine whether new data is available
bool LSM9DS1::availGyro(void)
{
	uint8_t status = readGyro(LSM9DS1_STATUS_REG);

	return ((status & 0x02) >> 1);
}

// check status register to determine whether new data is available
bool LSM9DS1::availMag(void)
{
	uint8_t status = readMag(LSM9DS1_STATUS_REG_M);		// Note: checks if  X, Y and Z-axis new data available

	return ((status & 0x08) >> 3);
}

// check status register to determine whether new data is available
bool LSM9DS1::availTemp(void)
{
	uint8_t status = readAccel(LSM9DS1_STATUS_REG);

	return ((status & 0x04) >> 2);
}


// READ THE LATEST ACCEL, GYRO, MAG & TEMP VALUES

// read and store the latest accel data
void LSM9DS1::updateAccel(void)
{
	uint8_t vals[LSM9DS1_NUM_AXIS * 2];

	readAccel(LSM9DS1_OUT_X_L_XL, vals, (LSM9DS1_NUM_AXIS * 2));	// read the output data from LSM9DS1_OUT_X_L_XL - LSM9DS1_OUT_Z_H_XL

	_accelRes = IMU_ACCEL_SENSITIVITY[settings.accel.scale];		// calculate the resolution

																	// store the raw accelerometer values for each axis
	for (uint8_t i = 0; i < LSM9DS1_NUM_AXIS; i++)
	{
		_accelRaw[i] = (int32_t)((vals[(i * 2) + 1] << 8) | (vals[i * 2]));

		// if the number is negative (2's complement) store as a negative number
		if (_accelRaw[i] & (0x01 << 15))
		{
			// invert value & add 1 to convert from 2's complement
			_accelRaw[i] = -((uint16_t)~(_accelRaw[i]) + 1);
		}
	}
}

// read and store the latest gyro data
void LSM9DS1::updateGyro(void)
{
	uint8_t vals[LSM9DS1_NUM_AXIS * 2];

	readGyro(LSM9DS1_OUT_X_L_G, vals, (LSM9DS1_NUM_AXIS * 2));	// read the output data from LSM9DS1_OUT_X_L_G - LSM9DS1_OUT_Z_H_G

	_gyroRes = IMU_GYRO_SENSITIVITY[settings.gyro.scale];		// calculate the resolution

																// store the raw gyroscope values for each axis
	for (uint8_t i = 0; i < LSM9DS1_NUM_AXIS; i++)
	{
		_gyroRaw[i] = (float)((vals[(i * 2) + 1] << 8) | (vals[i * 2]));

		// if the number is negative (2's complement) store as a negative number
		if (_gyroRaw[i] & (0x01 << 15))
		{
			// invert value & add 1 to convert from 2's complement
			_gyroRaw[i] = -((uint16_t)~(_gyroRaw[i]) + 1);
		}
	}
}

// read and store the latest mag data
void LSM9DS1::updateMag(void)
{
	uint8_t vals[LSM9DS1_NUM_AXIS * 2];

	readMag(LSM9DS1_OUT_X_L_M, vals, (LSM9DS1_NUM_AXIS * 2));	// read the output data from LSM9DS1_OUT_X_L_M - LSM9DS1_OUT_Z_H_M

	_magRes = IMU_MAG_SENSITIVITY[settings.mag.scale];  		// calculate the resolution

																// store the raw magnetometer values for each axis
	for (uint8_t i = 0; i < LSM9DS1_NUM_AXIS; i++)
	{
		_magRaw[i] = (float)((vals[(i * 2) + 1] << 8) | (vals[i * 2]));

		// if the number is negative (2's complement) store as a negative number
		if (_magRaw[i] & (0x01 << 15))
		{
			// invert value & add 1 to convert from 2's complement
			_magRaw[i] = -((uint16_t)~(_magRaw[i]) + 1);
		}
	}
}

// read and store the latest temp data
void LSM9DS1::updateTemp(void)
{
	uint8_t vals[2];

	_tempRes = IMU_TEMP_SENSITIVITY;

	readAccel(LSM9DS1_OUT_TEMP_L, vals, 2);			// read the output data from LSM9DS1_OUT_TEMP_L - LSM9DS1_OUT_TEMP_H

	_tempRaw = ((vals[1] << 8) | (vals[0]));		// store the temperature

													// if the number is negative (2's complement) store as a negative number
	if (_tempRaw & (0x01 << 15))
	{
		// invert value & add 1 to convert from 2's complement
		_tempRaw = -((uint16_t)~(_tempRaw)+1);
	}

}


// READ THE ACCEL, GYRO & MAG CONFIG

// read multiple bytes from the accelerometer
void LSM9DS1::readAccel(uint8_t reg, uint8_t *val, uint8_t size)
{
	read(settings.accel.addr, reg, val, size);
}

// read one byte from the accelerometer
uint8_t LSM9DS1::readAccel(uint8_t reg)
{
	uint8_t val;

	read(settings.accel.addr, reg, &val, 1);

	return val;
}

// read multiple bytes from the gyro
void LSM9DS1::readGyro(uint8_t reg, uint8_t *val, uint8_t size)
{
	read(settings.gyro.addr, reg, val, size);
}

// read one byte from the gyro
uint8_t LSM9DS1::readGyro(uint8_t reg)
{
	uint8_t val;

	read(settings.gyro.addr, reg, &val, 1);

	return val;
}

// read multiple bytes from the magnetometer
void LSM9DS1::readMag(uint8_t reg, uint8_t *val, uint8_t size)
{
	read(settings.mag.addr, reg, val, size);
}

// read one byte from the magnetometer
uint8_t LSM9DS1::readMag(uint8_t reg)
{
	uint8_t val;

	read(settings.mag.addr, reg, &val, 1);

	return val;
}

// read multiple bytes from the IMU
void LSM9DS1::read(uint8_t addr, uint8_t reg, uint8_t *val, uint8_t size)
{
	
	Wire.beginTransmission(addr);
	Wire.write(reg);
	Wire.endTransmission(REPEATED_START);
	Wire.requestFrom(addr, size);
	for (uint8_t i = 0; i < size; i++)
	{
		val[i] = Wire.read();
	}
	
}


// WRITE TO ACCEL, GYRO & MAG CONFIG

// write multiple bytes to the accelerometer
void LSM9DS1::writeAccel(uint8_t reg, uint8_t *val, uint8_t size)
{
	write(settings.accel.addr, reg, val, 1);
}

// write one byte to the accelerometer
void LSM9DS1::writeAccel(uint8_t reg, uint8_t val)
{
	write(settings.accel.addr, reg, &val, 1);
}

// write multiple bytes to the gyro
void LSM9DS1::writeGyro(uint8_t reg, uint8_t *val, uint8_t size)
{
	write(settings.gyro.addr, reg, val, 1);
}

// write one byte to the gyro
void LSM9DS1::writeGyro(uint8_t reg, uint8_t val)
{
	write(settings.gyro.addr, reg, &val, 1);
}

// write multiple bytes to the magnetometer
void LSM9DS1::writeMag(uint8_t reg, uint8_t *val, uint8_t size)
{
	write(settings.mag.addr, reg, val, size);
}

// write one byte to the magnetometer
void LSM9DS1::writeMag(uint8_t reg, uint8_t val)
{
	write(settings.mag.addr, reg, &val, 1);
}

// write multiple bytes to the IMU
void LSM9DS1::write(uint8_t addr, uint8_t reg, uint8_t *val, uint8_t size)
{
	
	Wire.beginTransmission(addr);
	Wire.write(reg);
	for (uint8_t i = 0; i < size; i++)
	{
		Wire.write(val[i]);
	}
	Wire.endTransmission();
	
}

// enable/disable the FIFO buffer
void LSM9DS1::enableFIFO(bool en)
{
	uint8_t reg;

	// read CTRL_REG9
	reg = readAccel(LSM9DS1_CTRL_REG9);

	// set or clear FIFO_EN bit
	if (en)
		reg |= (0x01 << 1);
	else
		reg &= ~(0x01 << 1);

	// write CTRL_REG9
	writeAccel(LSM9DS1_CTRL_REG9, reg);
}

// set FIFO mode and size (max size = 32 samples)
void LSM9DS1::setFIFOMode(FIFOMode mode, uint8_t size)
{
	uint8_t reg;

	// limit the FIFO size to 31 samples
	size = constrain(size, 0, 31);

	reg = ((mode & 0x07) << 5) | (size);

	// write to FIFO_CTRL
	writeAccel(LSM9DS1_FIFO_CTRL, reg);
}

// get the number of unread FIFO samples
uint8_t LSM9DS1::unreadFIFOSamples(void)
{
	uint8_t numSamples;

	// read the number of unread FIFO samples
	numSamples = readAccel(LSM9DS1_FIFO_SRC) & 0x3F;

	return numSamples;
}

//void LSM9DS1::calibrate(void)
//{
//	const uint8_t FIFOsize = 31;		// 0 - 31 samples
//	uint32_t totalAccel[LSM9DS1_NUM_AXIS] = { 0 }, totalGyro[LSM9DS1_NUM_AXIS] = { 0 };
//
//	// enable FIFO mode and stop collecting when FIFO is full
//	setFIFOMode(FIFO_ONCE, FIFOsize);
//	enableFIFO(true);
//
//	// wait for FIFO to fill with samples
//	while (unreadFIFOSamples() < FIFOsize);
//
//	// find the total of all the data in the FIFO
//	for (uint8_t s = 0; s < FIFOsize; s++)
//	{
//		updateAccel();
//		// store the raw accelerometer values for each axis
//		for (uint8_t a = 0; a < LSM9DS1_NUM_AXIS; a++)
//		{
//			totalAccel[a] += _accelRaw[a];
//		}
//		updateGyro();
//		// store the raw gyroscope values for each axis
//		for (uint8_t g = 0; g < LSM9DS1_NUM_AXIS; g++)
//		{
//			totalGyro[g] += _gyroRaw[g];
//		}
//	}
//
//	// calculate the mean of all of the data in the FIFO
//	for (uint8_t i = 0; i < LSM9DS1_NUM_AXIS; i++)
//	{
//		_accelBias[i] = (totalAccel[i] / (float) FIFOsize) * _accelRes;
//		_gyroBias[i] = (totalGyro[i] / (float) FIFOsize) * _gyroRes;
//	}
//
//	_accelBias[Z_AXIS] -= 1.0;		// assume the IMU is on a flat surface
//
//	// disable FIFO mode
//	enableFIFO(false);
//	setFIFOMode(FIFO_OFF, 0);
//}

LSM9DS1 IMU;