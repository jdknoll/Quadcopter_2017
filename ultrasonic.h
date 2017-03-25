/*
 * ultrasonic.h
 *
 *  Created on: Mar 19, 2017
 *      Author: jdknoll
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

void inter_initialize();
void Timer2AIntHandler(void);
void PortDIntHandler(void);
void initialize_gpio();

extern int distance_cm;		//current height of quadcopter

#endif /* ULTRASONIC_H_ */
