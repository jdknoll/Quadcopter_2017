/*
 * altitude_pid.h
 *
 *  Created on: Mar 26, 2017
 *      Author: jdknoll
 */

#ifndef ALTITUDE_PID_H_
#define ALTITUDE_PID_H_

void pid_interrupt();
void Timer1AIntHandler(void);
void pid_initialize();
void pid_update(double curr_error, double dt);

typedef struct {
    double windup_guard;
    double p_gain;
    double i_gain;
    double d_gain;
    double prev_error;
    double int_error;
    double set_point;
    double control;
    double PWM_motor0;
    double PWM_motor1;
    double PWM_motor2;
    double PWM_motor3;
    int freq;
} t_PID;


extern t_PID pid;

#endif /* ALTITUDE_PID_H_ */
