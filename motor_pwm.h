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

void delayMS(int ms);
void delaySEC(int sec);
void clock_config();
void pwm_interrupt();
void arm_the_motor();
void pwm_configuration();
void pwm_gpio_configure();
void TimerStart(int set_freq);


#endif /* MOTOR_PWM_H_ */
