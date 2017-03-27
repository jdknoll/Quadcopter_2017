
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "altitude_pid.h"
#include "ultrasonic.h"

#define alt_pid_freq 20000000

t_PID pid;

//// Initialize function takes a variable to set frequency
//void pid_interrupt()
//{
//    // enable the timer1 peripheral
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
//    // configure timer1 peripheral
//    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
//
//    // set timer1A with the desired period
//    TimerLoadSet(TIMER1_BASE, TIMER_A, alt_pid_freq);
//
//    // enable the interrupt
//    IntEnable(INT_TIMER1A);
//    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
//    IntMasterEnable();
//
//    // enable timer1A
//    TimerEnable(TIMER2_BASE, TIMER_A);
//
//    IntMasterEnable();
//
//    // initialize the integral and current error for the PID loop
//    pid_initialize(pid);
//}



void pid_initialize()
{
    // set previous and integrated error to zero
    pid.prev_error = 0;
    pid.int_error = 0;

    pid.windup_guard_low = -200;
    pid.windup_guard_high = 200;
    pid.p_gain = .03;
    pid.i_gain = .03;
    pid.set_point = 15;
}


void pid_update(double curr_error, double dt)
{
    //double diff;
    double p_term;
    double i_term;
    //double d_term;

    // integration with windup guarding
    pid.int_error += (curr_error * dt);

    if (pid.int_error < -(pid.windup_guard_low))
        pid.int_error = -(pid.windup_guard_low);
    else if (pid.int_error > pid.windup_guard_high)		// integral windup control
        pid.int_error = pid.windup_guard_high;


    // differentiation
    //diff = ((curr_error - pid->prev_error) / dt);

    // scaling
    p_term = (pid.p_gain * curr_error);
    i_term = (pid.i_gain * pid.int_error);
    //d_term = (pid->d_gain * diff);

    // summation of terms
    pid.control = p_term + i_term; // + d_term;

    // save current error as previous error for next iteration
    pid.prev_error = curr_error;
}
