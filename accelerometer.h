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

void InitializeAccel();
void wait(int waitTime);
void QueryAccel();
void PrintAccelConfigReg();
void PrintAccelWhoAmI();


#endif /* ACCELEROMETER_H_ */
