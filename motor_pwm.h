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
void arm_the_motor();
void pwm_configuration();
void pwm_gpio_configure();
void TimerStart(int set_freq);
void TimerStart2(int set_freq);

extern int desired_height_cm;	// height for the copter


extern int PWM_motor0;
extern int PWM_motor1;
extern int PWM_motor2;
extern int PWM_motor3;


#endif /* MOTOR_PWM_H_ */
