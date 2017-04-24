/*
 * pwm_setting_config.h
 *
 *  Created on: Mar 15, 2017
 *      Author: Scott Keller
 */
/*################################################
#
# Hardware Pulse Width Modulation with
# Interrupts changing speed, using the
# Tiva C LaunchPad
#
#################################################*/


#ifndef MOTOR_PWM_H_
#define MOTOR_PWM_H_

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"

void delayMS(int ms);
void delaySEC(int sec);
void clock_config();
void altitude_PID_interrupt();
void leveling_PID_interrupt();
void arm_the_motor();
void pwm_configuration();
void TimerStart();
void TimerStart2();
void update_motors();

typedef struct {
    double motor0;
    double motor1;
    double motor2;
    double motor3;
} t_PWM;

extern t_PWM pwm;

//Values for the Turnigy Plush 30A
#define PWM_PERIOD 		5735			//5735
#define MAX_MOTOR_SPEED 	4000			//4290
#define MIN_MOTOR_SPEED 	3000
#define ARM_MOTOR_SPEED 	2800
#define DIV_VALUE			SYSCTL_PWMDIV_32

//Values for the Afro ESC
//#define PWM_PERIOD 		5000
//#define MAX_MOTOR_SPEED 	3900
//#define MIN_MOTOR_SPEED 	1950
//#define ARM_MOTOR_SPEED 	1800
//#define DIV_VALUE		SYSCTL_PWMDIV_8

#endif /* MOTOR_PWM_H_ */
