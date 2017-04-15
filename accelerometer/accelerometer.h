/*
 * accelerometer.h
 *
 *  Created on: Mar 16, 2017
 *      Author: jdknoll
 */

#define MPU9250_ADDRESS 0x68

#define COMPASS_ADDRESS 0x0C

#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E
#define SELF_TEST_Z_ACCEL 0x0F

#define ACCEL_CONFIG 	0x1C
#define ACCEL_CONFIG2	0x1D

#define PWR_MGMT_1		0x6B
#define PWR_MGMT_2		0x6C

#define CONFIG			0x1A
#define GYRO_CONFIG     0x1B
#define FIFO	        0x23
#define SMPLRT_DIV		0x19
#define USER_CTRL		0x6A

#define WHO_AM_I 0x75
#define FIFO_EN 0x23

#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

void InitializeAccel();
void wait(int waitTime);
void QueryAccel();
void PrintAccelConfigReg();
void PrintAccelWhoAmI();
void AccelSelfTest();

#endif /* ACCELEROMETER_H_ */
