//Programmer: Josiah Knoll
//Course: CMPE 320-01 Operating Systems
//Instructor: Dr. Briggs
//Homework: UNIX Shell Project

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "../accelerometer.h"
#include "../I2C.h"

#include "terminal.h"


#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44

#define GYROX 0x43

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C

/*
 * output the value in the x accelerometer
 */
void accelx () {
    UARTprintf("X Accelerometer output: %d\n", (I2CReceive(MPU9250_ADDRESS, ACCEL_XOUT_H) << 8) + I2CReceive(MPU9250_ADDRESS, ACCEL_XOUT_L));
}

void printreg (char* reg) {
	int regint = atoi(reg);
    UARTprintf("%d\n", I2CReceive(MPU9250_ADDRESS, regint));
}

/*
 * input the register of the high part of the number
 * it will print out a value for the register
 */
void printwidereg (char* reg) {
	int regint = atoi(reg);
    UARTprintf("%d\n", (I2CReceive(MPU9250_ADDRESS, regint) << 8) + I2CReceive(MPU9250_ADDRESS, (regint+1)));
}

void testterm () {
	UARTprintf("test_successful");
}

