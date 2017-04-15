/*
 * accelerometer2.c
 *
 *  Created on: Apr 15, 2017
 *      Author: josiahknoll
 */

#include "accelerometer2.h"
#include "../uartterm/t_uart.h"

#define X_PIN 			GPIO_PIN_3
#define X_BASE 			GPIO_PORTD_BASE
#define X_PERIPHERAL		SYSCTL_PERIPH_GPIOD //TODO: change to ADC peripheral

#define Y_PIN 			GPIO_PIN_1
#define Y_BASE 			GPIO_PORTE_BASE
#define Y_PERIPHERAL		SYSCTL_PERIPH_GPIOE //TODO: change to ADC peripheral

#define Z_PIN 			GPIO_PIN_2
#define Z_BASE 			GPIO_PORTE_BASE
#define Z_PERIPHERAL		SYSCTL_PERIPH_GPIOE //TODO: change to ADC peripheral

accelerometer accel;

/*
 * Programming for setting up the ADC pin range, sensitivity, etc
 */
void initializeAccelADC(){

}

/*
 * Update the accelerometer global struct with data from the ADC
 */
void updateAccelerometer(){
	accel.xout = 0; //TODO: replace with read of ADC pin
	accel.yout = 0; //TODO: replace with read of ADC pin
	accel.zout = 0; //TODO: replace with read of ADC pin
}

void queryAccel2(){
	// xout = (ACCEL_XOUT_H << 8) + ACCEL_XOUT_L
    UARTprintf("X Accelerometer output: %d\n", accel.xout);
    UARTprintf("Y Accelerometer output: %d\n", accel.yout);
    UARTprintf("Z Accelerometer output: %d\n", accel.zout);
}
