/*
 * accelerometer2.h
 *
 *  Created on: Apr 15, 2017
 *      Author: josiahknoll
 */

#ifndef ACCELEROMETER_ACCELEROMETER2_H_
#define ACCELEROMETER_ACCELEROMETER2_H_

typedef struct{
	int xout;
	int yout;
	int zout;
} accelerometer_t;

extern accelerometer_t accel;
void initializeAccelADC();
void updateAccelerometer();
void update_x_Accelerometer();
void update_y_Accelerometer();
void update_z_Accelerometer();
void queryAccel2();

#endif /* ACCELEROMETER_ACCELEROMETER2_H_ */
