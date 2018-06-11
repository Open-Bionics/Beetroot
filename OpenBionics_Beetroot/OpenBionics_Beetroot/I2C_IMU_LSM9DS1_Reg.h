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
*	I2C_IMU_LSM9DS1_Reg.h
*
*/

#ifndef I2C_IMU_LSM9DS1_REG_H_
#define I2C_IMU_LSM9DS1_REG_H_

#include <Arduino.h>

#define LSM9DS1_NUM_AXIS		3			// X, Y, Z
#define LSM9DS1_DEFAULT_TEMP	25			// 'C. 25'C = 0

//////////////////////////////////////////////////////////////////
//							REGISTERS							//
//																//
//////////////////////////////////////////////////////////////////

/////////////////////////// ACCEL & GYRO REGISTERS ///////////////////////////
#define LSM9DS1_ACT_THS				0x04		// ACTIVITY THRESHOLD
#define LSM9DS1_ACT_DUR				0x05		// INACTIVITY DURATION
#define LSM9DS1_INT_GEN_CFG_XL		0x06		// ACCEL INTERRUPT CONFIG
#define LSM9DS1_INT_GEN_THS_X_XL	0x07		// ACCEL INTERRUPT X THRESHOLD
#define LSM9DS1_INT_GEN_THS_Y_XL	0x08		// ACCEL INTERRUPT Y THRESHOLD
#define LSM9DS1_INT_GEN_THS_Z_XL	0x09		// ACCEL INTERRUPT Z THRESHOLD
#define LSM9DS1_INT_GEN_DUR_XL		0x0A		// ACCEL INTERUPT DURATION
#define LSM9DS1_REFERENCE_G			0x0B		// GYRO HIGH-PASS REFERENCE
#define LSM9DS1_INT1_CTRL			0x0C		// ACCEL & GYRO INTERRUPT 1 CONTROL
#define LSM9DS1_INT2_CTRL			0x0D		// ACCEL & GYRO INTERRUPT 2 CONTROL
#define LSM9DS1_WHO_AM_I			0x0F		// WHO_AM_I
#define LSM9DS1_CTRL_REG1_G			0x10		// GYRO CONTROL REGISTER 1
#define LSM9DS1_CTRL_REG2_G			0x11		// GYRO CONTROL REGISTER 2
#define LSM9DS1_CTRL_REG3_G			0x12		// GYRO CONTROL REGISTER 3
#define LSM9DS1_ORIENT_CONFIG_G		0X13		// GYRO SIGN AND ORIENTATION
#define LSM9DS1_INT_GEN_SRC_G		0x14		// GYRO INTERRUPT SOURCE
#define LSM9DS1_OUT_TEMP_L			0x15		// TEMPERATURE DATA OUTPUT LOW BYTE
#define LSM9DS1_OUT_TEMP_H			0x16		// TEMPERATURE DATA OUTPUT HIGH BYTE
#define LSM9DS1_STATUS_REG			0x17		// STATUS REGISTER
#define LSM9DS1_OUT_X_L_G			0x18		// GYRO DATA OUTPUT X LOW BYTE
#define LSM9DS1_OUT_X_H_G			0x19		// GYRO DATA OUTPUT X HIGH BYTE
#define LSM9DS1_OUT_Y_L_G			0x1A		// GYRO DATA OUTPUT Y LOW BYTE
#define LSM9DS1_OUT_Y_H_G			0x1B		// GYRO DATA OUTPUT Y HIGH BYTE
#define LSM9DS1_OUT_Z_L_G			0x1C		// GYRO DATA OUTPUT Z LOW BYTE
#define LSM9DS1_OUT_Z_H_G			0x1D		// GYRO DATA OUTPUT Z HIGH BYTE
#define LSM9DS1_CTRL_REG4			0x1E		// CONTROL REGISTER 4
#define LSM9DS1_CTRL_REG5_XL		0x1F		// ACCEL CONTROL REGISTER 5
#define LSM9DS1_CTRL_REG6_XL		0x20		// ACCEL CONTROL REGISTER 6
#define LSM9DS1_CTRL_REG7_XL		0x21		// ACCEL CONTROL REGISTER 7
#define LSM9DS1_CTRL_REG8			0x22		// CONTROL REGISTER 8
#define LSM9DS1_CTRL_REG9			0x23		// CONTROL REGISTER 9
#define LSM9DS1_CTRL_REG10			0x24		// CONTROL REGISTER 10
#define LSM9DS1_INT_GEN_SRC_XL		0x26		// ACCEL INTERRUPT SOURCE
#define LSM9DS1_STATUS_REG			0x27		// STATUS REG
#define LSM9DS1_OUT_X_L_XL			0x28		// ACCEL DATA OUTPUT X LOW BYTE
#define LSM9DS1_OUT_X_H_XL			0x29		// ACCEL DATA OUTPUT X HIGH BYTE
#define LSM9DS1_OUT_Y_L_XL			0x2A		// ACCEL DATA OUTPUT Y LOW BYTE
#define LSM9DS1_OUT_Y_H_XL			0x2B		// ACCEL DATA OUTPUT Y HIGH BYTE
#define LSM9DS1_OUT_Z_L_XL			0x2C		// ACCEL DATA OUTPUT Z LOW BYTE
#define LSM9DS1_OUT_Z_H_XL			0x2D		// ACCEL DATA OUTPUT Z HIGH BYTE
#define LSM9DS1_FIFO_CTRL			0x2E		// FIFO CONTROL
#define LSM9DS1_FIFO_SRC			0x2F		// FIFO STATUS CONTROL
#define LSM9DS1_INT_GEN_CFG_G		0x30		// GYRO INTERRUPT CONFIG
#define LSM9DS1_INT_GEN_THS_XH_G	0x31		// GYRO INTERRUPT X THRESHOLD HIGH BYTE
#define LSM9DS1_INT_GEN_THS_XL_G	0x32		// GYRO INTERRUPT X THRESHOLD LOW BYTE
#define LSM9DS1_INT_GEN_THS_YH_G	0x33		// GYRO INTERRUPT Y THRESHOLD HIGH BYTE
#define LSM9DS1_INT_GEN_THS_YL_G	0x34		// GYRO INTERRUPT Y THRESHOLD LOW BYTE
#define LSM9DS1_INT_GEN_THS_ZH_G	0x35		// GYRO INTERRUPT Z THRESHOLD HIGH BYTE
#define LSM9DS1_INT_GEN_THS_ZL_G	0x36		// GYRO INTERRUPT Z THRESHOLD LOW BYTE
#define LSM9DS1_INT_GEN_DUR_G		0x37		// GYRO DURATION

//////////////////////////// MAGENETOMETER REGISTERS ////////////////////////////
#define LSM9DS1_OFFSET_X_REG_L_M	0x05		// MAGNETOMETER X OFFSET LOW BYTE
#define LSM9DS1_OFFSET_X_REG_H_M	0x06		// MAGNETOMETER X OFFSET HIGH BYTE
#define LSM9DS1_OFFSET_Y_REG_L_M	0x07		// MAGNETOMETER Y OFFSET LOW BYTE
#define LSM9DS1_OFFSET_Y_REG_H_M	0x08		// MAGNETOMETER Y OFFSET HIGH BYTE
#define LSM9DS1_OFFSET_Z_REG_L_M	0x09		// MAGNETOMETER Z OFFSET LOW BYTE
#define LSM9DS1_OFFSET_Z_REG_H_M	0x0A		// MAGNETOMETER Z OFFSET HIGH BYTE
#define LSM9DS1_WHO_AM_I_M			0x0F		// WHO_AM_I
#define LSM9DS1_CTRL_REG1_M			0x20		// MAGNETOMETER CONTROL REGISTER 1
#define LSM9DS1_CTRL_REG2_M			0x21		// MAGNETOMETER CONTROL REGISTER 2
#define LSM9DS1_CTRL_REG3_M			0x22		// MAGNETOMETER CONTROL REGISTER 3
#define LSM9DS1_CTRL_REG4_M			0x23		// MAGNETOMETER CONTROL REGISTER 4
#define LSM9DS1_CTRL_REG5_M			0x24		// MAGNETOMETER CONTROL REGISTER 5
#define LSM9DS1_STATUS_REG_M		0x27		// MAGNETOMETER STATUS
#define LSM9DS1_OUT_X_L_M			0x28		// MAGNETOMETER DATA OUTPUT X LOW BYTE
#define LSM9DS1_OUT_X_H_M			0x29		// MAGNETOMETER DATA OUTPUT X HIGH BYTE
#define LSM9DS1_OUT_Y_L_M			0x2A		// MAGNETOMETER DATA OUTPUT Y LOW BYTE
#define LSM9DS1_OUT_Y_H_M			0x2B		// MAGNETOMETER DATA OUTPUT Y HIGH BYTE
#define LSM9DS1_OUT_Z_L_M			0x2C		// MAGNETOMETER DATA OUTPUT Z LOW BYTE
#define LSM9DS1_OUT_Z_H_M			0x2D		// MAGNETOMETER DATA OUTPUT Z HIGH BYTE
#define LSM9DS1_INT_CFG_M			0x30		// MAGNETOMETER INTERRUPT CONFIG
#define LSM9DS1_INT_SRC_M			0x31		// MAGNETOMETER INTERRUPT SOURCE
#define LSM9DS1_INT_THS_L_M			0x32		// MAGNETOMETER INTERRUPT THRESHOLD LOW BYTE
#define LSM9DS1_INT_THS_H_M			0x33		// MAGNETOMETER INTERRUPT THRESHOLD HIGH BYTE

/////////////////////////////////// I2C ///////////////////////////////////
#define LSM9DS1_ADDR_XL_G			0x6B		// ACCEL & GYRO ADDRESS
#define LSM9DS1_ADDR_M				0x1E		// MAGNETOMETER ADDRESS
#define REPEATED_START				0x00		// send RESTART instead of STOP



//////////////////////////////////////////////////////////////////
//							TYPES/SETTINGS						//
//																//
//////////////////////////////////////////////////////////////////

// Axis names
typedef enum _AxisNames
{
	X_AXIS = 0x00,				// X-axis
	Y_AXIS,						// Y-axis
	Z_AXIS,						// Z-axis
};

///////////////////////////// MODE SELECTION /////////////////////////////

// FIFO mode selection
typedef enum _FIFOMode
{
	FIFO_OFF = 0x00,			// FIFO turned off (bypass mode)
	FIFO_ONCE = 0x01,			// FIFO mode, stop when FIFO is full
	FIFO_CONT_TRIGG = 0x03,		// Continuous mode until trigger, then FIFO mode
	FIFO_OFF_TRIGG = 0x04,		// Bypass mode until trigger, then continuous mode
	FIFO_CONT = 0x05			// Continuous mode, if full, wrap-around and overwrite
} FIFOMode;

// Accelerometer operating modes
typedef enum _AccelODR
{
	ACCEL_ODR_OFF = 0x00,		// Power down
	ACCEL_ODR_10 = 0x01,		// 10Hz (BW = 408Hz)
	ACCEL_ODR_50 = 0x02,		// 50Hz (BW = 408Hz)
	ACCEL_ODR_119 = 0x03,		// 119Hz (BW = 50Hz)
	ACCEL_ODR_238 = 0x04,		// 238Hz (BW = 105Hz)
	ACCEL_ODR_476 = 0x05,		// 476Hz (BW = 211Hz)
	ACCEL_ODR_952 = 0x06		// 952Hz (BW = 408Hz)
} AccelODR;

// Gyro operating modes
typedef enum _GyroODR
{
	GYRO_ODR_OFF = 0x00,		// Power down 
	GYRO_ODR_14_9 = 0x01,		// 14.9Hz, Low Power/Normal Mode (1.9mA)
	GYRO_ODR_59_5 = 0x02,		// 59.5Hz, Low Power/Normal Mode (2.4mA)
	GYRO_ODR_119 = 0x03,		// 119Hz, Low Power/Normal Mode (3.1mA)
	GYRO_ODR_238 = 0x04,		// 238Hz, Normal Mode (4.3mA)
	GYRO_ODR_476 = 0x05,		// 476Hz, Normal Mode (4.3mA)
	GYRO_ODR_952 = 0x06			// 952Hz, Normal Mode (4.3mA)
} GyroODR;

// Magnetometer operating modes
typedef enum _MagODR
{
	MAG_ODR_0_625 = 0x00,		// 0.0625Hz
	MAG_ODR_1_25 = 0x01,		// 1.25Hz
	MAG_ODR_2_5 = 0x02,			// 2.5Hz
	MAG_ODR_5 = 0x03,			// 5Hz
	MAG_ODR_10 = 0x04,			// 10Hz
	MAG_ODR_20 = 0x05,			// 20Hz
	MAG_ODR_40 = 0x06,			// 40Hz
	MAG_ODR_80 = 0x07			// 80Hz
};


/////////////////////////////// SCALES ///////////////////////////////

// Accel scale
typedef enum _AccelScale
{
	ACCEL_SCALE_2 = 0x00,		// ±2g
	ACCEL_SCALE_16 = 0x01,		// ±16g
	ACCEL_SCALE_4 = 0x02,		// ±4g
	ACCEL_SCALE_8 = 0x03,		// ±8g
} AccelScale;

// Gyro scale
typedef enum _GyroScale
{
	GYRO_SCALE_245 = 0x00,		// ±245dps
	GYRO_SCALE_500 = 0x01,		// ±500dps
	GYRO_SCALE_2000 = 0x02		// ±2000dps
} GyroScale;

// Magnetometer scale
typedef enum _MagScale
{
	MAG_SCALE_4 = 0x00,			// ±4Gs
	MAG_SCALE_8 = 0x01,			// ±8Gs
	MAG_SCALE_12 = 0x02,		// ±12Gs
	MAG_SCALE_16 = 0x03,		// ±16Gs
} MagScale;


//////////////////////////// FREQUENCY SELECTION ////////////////////////////

// Accel anti-aliasing filter bandwidth selection
typedef enum _AccelABWS
{
	ACCEL_ABWS_408 = 0x00,		// 408Hz
	ACCEL_ABWS_211 = 0x01,		// 211Hz
	ACCEL_ABWS_105 = 0x02,		// 105Hz
	ACCEL_ABWS_50 = 0x03,		// 50Hz
};

// Accel low pass cut-off frequency (in high resolution mode, HR = 1)
typedef enum _AccelLPCF
{
	ACCEL_LPCF_DIV_50 = 0x00,	// (ODR/50)Hz
	ACCEL_LPCF_DIV_100 = 0x01,	// (ODR/100)Hz
	ACCEL_LPCF_DIV_9 = 0x02,	// (ODR/9)Hz
	ACCEL_LPCF_DIV_400 = 0x03,	// (ODR/400)Hz
};

// Magnetometer performance mode
typedef enum _MagPerf
{
	MAG_PERF_LOW = 0x00,		// Low power mode
	MAG_PERF_MED = 0x01,		// Medium performance
	MAG_PERF_HIGH = 0x02,		// High performance
	MAG_PERF_ULTRA = 0x03		// Ultra-high performance
} MagPerf;


//////////////////////////////////////////////////////////////////
//							DEVICE SETTINGS						//
//																//
//////////////////////////////////////////////////////////////////

// accelerometer settings
typedef struct _AccelSettings
{
	uint8_t addr;				// address of the accel

	uint8_t en;					// enable the accelerometer
	uint8_t enX;				// enable the X-axis
	uint8_t enY;				// enable the Y-axis
	uint8_t enZ;				// enable the Z-axis
	uint8_t enHighRes;			// enable high resolution mode

	uint8_t scale;				// accelerometer scale
	uint8_t sRate;				// sample rate
	uint8_t bWidth;				// standard bandwidth
	uint8_t bWidthHighRes;		// bandwidth of high resolution mode
} AccelSettings;

// gyroscope settings
typedef struct _GyroSettings
{
	uint8_t addr;				// address of the gyro

	uint8_t en;					// enable the gyro
	uint8_t enX;				// enable the X-axis
	uint8_t enY;				// enable the Y-axis
	uint8_t enZ;				// enable the Z-axis
	uint8_t enHighRes;			// enable high resolution mode
	uint8_t enLowPower;			// enable low power mode
	uint8_t enHighPass;			// enable high pass filter

	uint8_t scale;				// gyro scale
	uint8_t sRate;				// sample rate
	uint8_t bWidth;				// standard bandwidth
	uint8_t bWidthHighRes;		// bandwidth of high resolution mode
	uint8_t cutOffHighPass;		// high pass cut-off

	uint8_t orientation;		// orientation setting
	uint8_t flipX;				// flip the X-axis
	uint8_t flipY;				// flip the Y-axis
	uint8_t flipZ;				// flip the Z-axis
} GyroSettings;

// magnetometer
typedef struct _MagSettings
{
	uint8_t addr;				// address of the magnetometer

	uint8_t en;					// enable the magnetometer
	uint8_t enLowPower;			// enable low power mode
	uint8_t enTempComp;			// enable temperature compensation

	uint8_t scale;				// magnetometer scale
	uint8_t sRate;				// sample rate
	uint8_t mode;				// operating mode
	uint8_t performanceXY;		// X-axis & Y-axis performance
	uint8_t performanceZ;		// Z-axis performance
} MagSettings;

// temperature settings
typedef struct _TempSettings
{
	uint8_t en;					// enable the temperature sensor
} TempSettings;

// IMU settings
typedef struct _IMUSettings
{
	AccelSettings accel;
	GyroSettings gyro;
	MagSettings mag;
	TempSettings temp;
} IMUSettings;


#endif // I2C_IMU_LSM9DS1_REG_H_