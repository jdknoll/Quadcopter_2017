
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "altitude_pid.h"
#include "../ultrasonic.h"
#include "../uartterm/t_uart.h"

#define alt_pid_freq 20000000

t_PID pid;


void pid_initialize()
{
    // set previous and integrated error to zero
    pid.prev_error = 0;
    pid.int_error = 0;

    pid.windup_guard = 200;
    pid.p_gain = .023;			// p is more aggresive - scalar value
    pid.i_gain = .004;			// keep i less than p - integral value
    pid.d_gain = .0000002;
    pid.set_point = 0;//100;
    pid.altitude_freq = 1894000; 		//20000000
    pid.leveling_freq = 200000;
    pid.PWM_motor0 = 1950;
    pid.PWM_motor1 = 1950;
    pid.PWM_motor2 = 1950;
    pid.PWM_motor3 = 1950;
}

double pwm_saturate_add(double a, double b){
	double output = a + b;
	if(output > 2250){
		return 2250;
	} else  if (output < 1950){
		return 1950;
	} else {
		return output;
	}
}

void pid_update(double curr_error, double dt)
{
    double diff;
    double p_term;
    double i_term;
    double d_term;

    // integration with windup guarding
    pid.int_error += (curr_error * dt);

    if (pid.int_error < -(pid.windup_guard)) {
    	pid.int_error = -(pid.windup_guard);
    	UARTprintf("Windup low\n");
    } else if (pid.int_error > pid.windup_guard){ // integral windup control
    	pid.int_error = pid.windup_guard;
    	UARTprintf("Windup high\n");
    }

    // differentiation
    diff = ((curr_error - pid.prev_error) / dt);

    // scaling
    p_term = (pid.p_gain * curr_error);
    i_term = (pid.i_gain * pid.int_error);
    d_term = (pid.d_gain * diff);

    // summation of terms
    pid.control = p_term + i_term + d_term;

    // calculate the double value for PWM_motors
	pid.PWM_motor0 = pwm_saturate_add(pid.PWM_motor0, pid.control);
	pid.PWM_motor1 = pwm_saturate_add(pid.PWM_motor1, pid.control);
	pid.PWM_motor2 = pwm_saturate_add(pid.PWM_motor2, pid.control);
	pid.PWM_motor3 = pwm_saturate_add(pid.PWM_motor3, pid.control);

	// save current error as previous error for next iteration
	pid.prev_error = curr_error;


}
