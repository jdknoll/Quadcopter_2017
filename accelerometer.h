/*
 * accelerometer.h
 *
 *  Created on: Mar 16, 2017
 *      Author: jdknoll
 */

#define MPU9250_ADDRESS 0x68

#define COMPASS_ADDRESS 0x0C

#define WHO_AM_I 0x75
#define FIFO_EN 0x23

#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

typedef struct{
	int16_t x;
	int16_t y;
	int16_t z;
} accel_t;

extern accel_t accel;

void InitializeAccel();
void wait(int waitTime);
void PrintAccel();
void QueryAccel();
void PrintAccelConfigReg();
void PrintAccelWhoAmI();


#endif /* ACCELEROMETER_H_ */
