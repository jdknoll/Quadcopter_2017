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
#include "../motor_pwm.h"


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

void matlab_pwm(char *pwm0, char *pwm1, char *pwm2, char *pwm3) {
	int pwm0_int = atoi(pwm0);
	int pwm1_int = atoi(pwm1);
	int pwm2_int = atoi(pwm2);
	int pwm3_int = atoi(pwm3);

    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_4,PWM_motor0);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,PWM_motor1);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,PWM_motor2);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,PWM_motor3);
}

void matlab_req(){

	UARTprintf("");
}

