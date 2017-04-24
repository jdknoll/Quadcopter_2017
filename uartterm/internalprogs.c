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
#include "../pid/pid.h"
#include "../ultrasonic.h"


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

	if ((pwm0_int >= MAX_MOTOR_SPEED) || (pwm1_int >= MAX_MOTOR_SPEED) || (pwm2_int >= MAX_MOTOR_SPEED) || (pwm3_int >= MAX_MOTOR_SPEED)){
		return;
	}

	pwm.motor0 = pwm0_int;
	pwm.motor1 = pwm1_int;
	pwm.motor2 = pwm2_int;
	pwm.motor3 = pwm3_int;

	//sets the motor PWM values to the values stored in the struct
	update_motors();
}

//set all PWM's to the argument given to the m command
void pwm_all(char *pwm_value){
	int pwm_int = atoi(pwm_value);

	if (pwm_int >= MAX_MOTOR_SPEED){
			return;
	}
	pwm.motor0 = pwm_int;
	pwm.motor1 = pwm_int;
	pwm.motor2 = pwm_int;
	pwm.motor3 = pwm_int;

	//sets the motor PWM values to the values stored in the struct
	update_motors();
}

void matlab_req(){
	UARTprintf("\n%s", distance_string);
}

void print_setup(){
	UARTprintf("\n");
	UARTprintf("set_point (mm):      %d\n", (int)leveling_x_pid.set_point);
	UARTprintf("proportional*10000:  %d\n", (int)(leveling_x_pid.p_gain*10000));
	UARTprintf("integral*10000:      %d\n", (int)(leveling_x_pid.i_gain*10000));
	UARTprintf("derivative*10000000: %d\n", (int)(leveling_x_pid.d_gain*10000000));
	UARTprintf("windup guard:        %d\n", (int)leveling_x_pid.windup_guard);
}

// set the height point
void set_point(char *set_point){
	leveling_x_pid.set_point = atoi(set_point)*10;
	print_setup();
}

// set the proportional
void set_p(char *set_p){
	leveling_x_pid.p_gain = (double)atoi(set_p)/10000;
	print_setup();
}

// set the integer value
void set_i(char *set_i){
	leveling_x_pid.i_gain = (double)atoi(set_i)/10000;
	print_setup();
}

void set_d(char *set_d){
	leveling_x_pid.d_gain = (double)atoi(set_d)/10000000;
	print_setup();
}

// go through the arming sequence again
void arm() {
	arm_the_motor();
	pwm.motor0 = MIN_MOTOR_SPEED;
	pwm.motor1 = MIN_MOTOR_SPEED;
	pwm.motor2 = MIN_MOTOR_SPEED;
	pwm.motor3 = MIN_MOTOR_SPEED;
}

// set the wind-up guard
void set_w(char *set_w){
	altitude_pid.windup_guard = (double)atoi(set_w);
	print_setup();
}
