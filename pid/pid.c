
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../motor_pwm.h"
#include "../ultrasonic.h"
#include "../uartterm/t_uart.h"
#include "pid.h"

#define WINDUP 200
#define PGAIN  .001
#define IGAIN  .001
#define DGAIN  0
#define CTICKS 700000	//2000000

t_PID altitude_pid;
t_PID leveling_x_pid;
t_PID leveling_y_pid;


void pid_initialize()
{
    // set previous and integrated error to zero
    altitude_pid.prev_error = 0;
    altitude_pid.int_error = 0;
    altitude_pid.windup_guard = 200;
    altitude_pid.p_gain = .023;			// p is more aggresive - scalar value
    altitude_pid.i_gain = .004;			// keep i less than p - integral value
    altitude_pid.d_gain = .0000002;
    altitude_pid.set_point = 300;//100;
    altitude_pid.clock_ticks = 1894000; 		//20000000

    leveling_x_pid.prev_error = 0;
    leveling_x_pid.int_error = 0;
    leveling_x_pid.windup_guard = WINDUP;
    leveling_x_pid.p_gain = PGAIN;			// p is more aggresive - scalar value
    leveling_x_pid.i_gain = IGAIN;			// keep i less than p - integral value
    leveling_x_pid.d_gain = DGAIN;
    leveling_x_pid.set_point = 0;
    leveling_x_pid.clock_ticks = CTICKS; 		//20000000

    leveling_y_pid.prev_error = 0;
    leveling_y_pid.int_error = 0;
    leveling_y_pid.windup_guard = WINDUP;
    leveling_y_pid.p_gain = PGAIN;			// p is more aggresive - scalar value
    leveling_y_pid.i_gain = IGAIN;			// keep i less than p - integral value
    leveling_y_pid.d_gain = DGAIN;
    leveling_y_pid.set_point = 0;
    leveling_y_pid.clock_ticks = CTICKS; 		//20000000

    pwm.motor0 = MIN_MOTOR_SPEED + 400;
    pwm.motor1 = MIN_MOTOR_SPEED + 400;
    pwm.motor2 = MIN_MOTOR_SPEED + 400;
    pwm.motor3 = MIN_MOTOR_SPEED + 400;
}

double pwm_saturate_add(double a, double b){
	double output = a + b;
	if(output > MAX_MOTOR_SPEED){
		return MAX_MOTOR_SPEED;
	} else  if (output < MIN_MOTOR_SPEED){
		return MIN_MOTOR_SPEED;
	} else {
		return output;
	}
}

void pid_update(double curr_error, double dt, t_PID * pid, int mode)
{
    double diff;
    double p_term;
    double i_term;
    double d_term;

    // integration with windup guarding
    pid->int_error += (curr_error * dt);

    if (pid->int_error < -(pid->windup_guard)) {
    	pid->int_error = -(pid->windup_guard);
    	//UARTprintf("Windup low\n");
    } else if (pid->int_error > pid->windup_guard){ // integral windup control
    	pid->int_error = pid->windup_guard;
    	//UARTprintf("Windup high\n");
    }

    // differentiation
    diff = ((curr_error - pid->prev_error) / dt);

    // scaling
    p_term = (pid->p_gain * curr_error);
    i_term = (pid->i_gain * pid->int_error);
    d_term = (pid->d_gain * diff);

    // summation of terms
    int control = p_term + i_term + d_term;

    if(mode == ALTITUDE_MODE){
        // calculate the double value for PWM_motors
    		pwm.motor0 = pwm_saturate_add(pwm.motor0, control);
    		pwm.motor1 = pwm_saturate_add(pwm.motor1, control);
    		pwm.motor2 = pwm_saturate_add(pwm.motor2, control);
    		pwm.motor3 = pwm_saturate_add(pwm.motor3, control);
    }if(mode == LEVELING_X_MODE){
    		pwm.motor0 = pwm_saturate_add(pwm.motor0, control);
    		pwm.motor1 = pwm_saturate_add(pwm.motor1, -control);
    		pwm.motor2 = pwm_saturate_add(pwm.motor2, -control);
    		pwm.motor3 = pwm_saturate_add(pwm.motor3, control);
    }if(mode == LEVELING_Y_MODE){
		pwm.motor0 = pwm_saturate_add(pwm.motor0, -control);
		pwm.motor1 = pwm_saturate_add(pwm.motor1, control);
		pwm.motor2 = pwm_saturate_add(pwm.motor2, control);
		pwm.motor3 = pwm_saturate_add(pwm.motor3, -control);
    }

	// save current error as previous error for next iteration
	pid->prev_error = curr_error;
}
