/*
 * ultrasonic.h
 *
 *  Created on: Mar 19, 2017
 *      Author: jdknoll
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include <stdint.h>

void ultrasonicTriggerTimerHandler(void);
void ultrasonicEchoHandler(void);
void initialize_ultrasonic();
void distance_calculations(uint32_t clock_timer);

extern double distance_mm;		//current height of quadcopter
extern char distance_string[50];

#endif /* ULTRASONIC_H_ */
