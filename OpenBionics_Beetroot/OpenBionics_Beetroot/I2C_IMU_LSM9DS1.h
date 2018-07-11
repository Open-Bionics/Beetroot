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
*	.I2C_IMU_LSM9DS1h
*
*/

#ifndef I2C_IMU_LSM9DS1_H
#define I2C_IMU_LSM9DS1_H

#include "I2C_IMU_LSM9DS1_Reg.h"

// ACCEL
#define IMU_ACCEL_ADDR		LSM9DS1_ADDR_XL_G
#define IMU_ACCEL_SCALE		ACCEL_SCALE_2
#define IMU_ACCEL_S_RATE	ACCEL_ODR_952
#define IMU_ACCEL_B_WIDTH	0		// actual bandwidth determined by sample rate

// GYRO
#define IMU_GYRO_ADDR		LSM9DS1_ADDR_XL_G
#define IMU_GYRO_SCALE		GYRO_SCALE_245
#define IMU_GYRO_S_RATE		GYRO_ODR_952
#define IMU_GYRO_B_WIDTH	0		// actual bandwidth determined by sample rate

// MAG
#define IMU_MAG_ADDR		LSM9DS1_ADDR_M
#define IMU_MAG_SCALE		MAG_SCALE_4
#define IMU_MAG_S_RATE		MAG_ODR_80
#define IMU_MAG_PERFOMANCE_XY	MAG_PERF_ULTRA
#define IMU_MAG_PERFOMANCE_Z	MAG_PERF_ULTRA
#define IMU_DECLINATION		-1.26		// in Bristol, UK (http://www.magnetic-declination.com/)


// SCALE VALS FOR RESOLUTION CALC
extern float IMU_ACCEL_SENSITIVITY[4];
extern float IMU_GYRO_SENSITIVITY[3];
extern float IMU_MAG_SENSITIVITY[4];

class LSM9DS1
{
public:
	LSM9DS1();
	~LSM9DS1();

	bool begin(void);				// initialise accelerometer, gyro and magnetometer

									// CHECK FOR COMMUNICATION
	bool ping(void);				// check if IMU is responding
	bool pingAccelGyro(void);		// check whether accelerometer & gyro is responding
	bool pingMag(void);				// check whether magnetometer is responding

	bool poll(void);				// read the latest IMU data from the IMU
	void calcRollPitchYaw(void);	// calculate roll, pitch & yaw

									// READ THE DATA
	float getAccelX(void);		// get the latest accelerometer data along the X axis
	float getAccelY(void);		// get the latest accelerometer data along the Y axis
	float getAccelZ(void);		// get the latest accelerometer data along the Z axis
	float getGyroX(void);		// get the latest gyro data along the X axis
	float getGyroY(void);		// get the latest gyro data along the Y axis
	float getGyroZ(void);		// get the latest gyro data along the Z axis
	float getMagX(void);		// get the latest mag data along the X axis
	float getMagY(void);		// get the latest mag data along the Y axis
	float getMagZ(void);		// get the latest mag data along the Z axis
	float getTemp(void);		// get the latest temp data

								// CALCULATED VALUES
	float getRoll(void);		// get the roll value
	float getPitch(void);		// get the pitch value
	float getYaw(void);			// get the yaw value

private:

	IMUSettings settings;

	// INITIALISE THE DEVICE
	void initAccel(void);				// initialise the accel with the presets
	void initGyro(void);				// initialise the gyro with the presets
	void initMag(void);					// initialise the mag with the presets

										// CHECK IF NEW DATA IS AVAILABLE
	bool availAccel(void);				// check status register to determine whether new data is available
	bool availGyro(void);				// check status register to determine whether new data is available
	bool availMag(void);				// check status register to determine whether new data is available
	bool availTemp(void);				// check status register to determine whether new data is available

										// READ THE LATEST ACCEL, GYRO, MAG & TEMP VALUES
	void updateAccel(void);				// read and store the latest accel data
	void updateGyro(void);				// read and store the latest gyro data
	void updateMag(void);				// read and store the latest mag data
	void updateTemp(void);				// read and store the latest temp data

										// READ THE ACCEL, GYRO & MAG CONFIG
	void readAccel(uint8_t reg, uint8_t *val, uint8_t size);			// read multiple bytes from the accelerometer
	uint8_t readAccel(uint8_t reg);										// read one byte from the accelerometer
	void readGyro(uint8_t reg, uint8_t *val, uint8_t size);				// read multiple bytes from the gyro
	uint8_t readGyro(uint8_t reg);										// read one byte from the gyro
	void readMag(uint8_t reg, uint8_t *val, uint8_t size);  			// read multiple bytes from the magnetometer
	uint8_t readMag(uint8_t reg);										// read one byte from the magnetometer
	void read(uint8_t addr, uint8_t reg, uint8_t *val, uint8_t size);	// read multiple bytes from the IMU


																		// WRITE TO ACCEL, GYRO & MAG CONFIG
	void writeAccel(uint8_t reg, uint8_t *val, uint8_t size);			// write multiple bytes to the accelerometer
	void writeAccel(uint8_t reg, uint8_t val);							// write one byte to the accelerometer
	void writeGyro(uint8_t reg, uint8_t *val, uint8_t size);			// write multiple bytes to the gyro
	void writeGyro(uint8_t reg, uint8_t val);							// write one byte to the gyro
	void writeMag(uint8_t reg, uint8_t *val, uint8_t size);				// write multiple bytes to the magnetometer
	void writeMag(uint8_t reg, uint8_t val);							// write one byte to the magnetometer
	void write(uint8_t addr, uint8_t reg, uint8_t *val, uint8_t size);	// write multiple bytes to the IMU

																		// EXTRA SETTINGS
	void enableFIFO(bool en);						// enable/disable the FIFO buffer
	void setFIFOMode(FIFOMode mode, uint8_t size);	// set FIFO mode and size (max size = 32 samples)
	uint8_t unreadFIFOSamples(void);				// get the number of unread FIFO samples
													//void calibrate(void);							// calibrate the IMU

													// RAW & CALCULATED VALUES
	int32_t _accelRaw[LSM9DS1_NUM_AXIS], _gyroRaw[LSM9DS1_NUM_AXIS], _magRaw[LSM9DS1_NUM_AXIS], _tempRaw;		// raw values (after 2's compliment)
	float  _accelBias[LSM9DS1_NUM_AXIS], _gyroBias[LSM9DS1_NUM_AXIS], _magBias[LSM9DS1_NUM_AXIS];				// bias values, for calibration
	float _accelRes, _gyroRes, _magRes, _tempRes;	// scale values
	float _roll, _pitch, _yaw;						// calculated roll, pitch & yaw values

};


extern LSM9DS1 IMU;

#endif  // I2C_IMU_LSM9DS1_H