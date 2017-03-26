/*
 * ultrasonic.h
 *
 *  Created on: Mar 19, 2017
 *      Author: jdknoll
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

void ultrasonicTriggerTimerHandler(void);
void ultrasonicEchoHandler(void);
void initialize_ultrasonic();

extern double distance_cm;		//current height of quadcopter
extern char distance_string[50];

#endif /* ULTRASONIC_H_ */
