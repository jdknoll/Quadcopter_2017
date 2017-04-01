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
#include "t_uart.h"
#include "../motor_pwm.h"
#include "../ultrasonic.h"
#include "../altitude_pid.h"


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

// set all PWM's to the arguments given to the matlab_pwm command
void matlab_pwm(char *pwm0, char *pwm1, char *pwm2, char *pwm3) {
	int pwm0_int = atoi(pwm0);
	int pwm1_int = atoi(pwm1);
	int pwm2_int = atoi(pwm2);
	int pwm3_int = atoi(pwm3);

	if ((pwm0_int >= 5000) || (pwm1_int >= 5000) || (pwm2_int >= 5000) || (pwm3_int >= 5000)){
		return;
	}

    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,pwm0_int);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2,pwm1_int);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,pwm2_int);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,pwm3_int);
}

//set all PWM's to the argument given to the m command
void pwm_all(char *pwm){
	int pwm_int = atoi(pwm);

	if (pwm_int >= 5000){
			return;
	}

    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,pwm_int);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2,pwm_int);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,pwm_int);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,pwm_int);
}

void matlab_req(){
	UARTprintf("\n%s", distance_string);
}

void set_point(char *set_point){
	pid.set_point = atoi(set_point);
}

void set_i(char *set_i){
	pid.i_gain = (double)atoi(set_i)/100;
}

void set_p(char *set_p){
	pid.p_gain = (double)atoi(set_p)/100;
}
