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

#define ALTITUDE_MODE 0
#define LEVELING_X_MODE 1
#define LEVELING_Y_MODE 2


typedef struct {
    double windup_guard;
    double p_gain;
    double i_gain;
    double d_gain;
    double prev_error;
    double int_error;
    double set_point;
    int freq;
} t_PID;

void pid_update(double curr_error, double dt, t_PID * pid, int mode);

extern t_PID altitude_pid;
extern t_PID leveling_x_pid;
extern t_PID leveling_y_pid;

#endif /* ALTITUDE_PID_H_ */
