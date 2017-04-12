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
void pwm_interrupt();
void pwm_interrupt2();
void arm_the_motor();
void pwm_configuration();
void TimerStart();
void update_motors();

extern int desired_height_cm;	// height for the copter


#endif /* MOTOR_PWM_H_ */
