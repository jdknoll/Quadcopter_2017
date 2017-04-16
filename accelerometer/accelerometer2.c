/*
 * accelerometer2.c
 *
 *  Created on: Apr 15, 2017
 *      Author: josiahknoll
 */

#include "accelerometer2.h"
#include "../uartterm/t_uart.h"

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

//AIN4
#define X_PIN 					GPIO_PIN_3
#define X_BASE 					GPIO_PORTD_BASE
#define X_ADC_BASE			ADC0_BASE
#define X_PERIPHERAL		SYSCTL_PERIPH_GPIOD //TODO: change to ADC peripheral

//AIN2
#define Y_PIN 					GPIO_PIN_1
#define Y_BASE 					GPIO_PORTE_BASE
#define Y_ADC_BASE			ADC1_BASE
#define Y_PERIPHERAL		SYSCTL_PERIPH_GPIOE //TODO: change to ADC peripheral

//AIN1
#define Z_PIN 					GPIO_PIN_2
#define Z_BASE 					GPIO_PORTE_BASE
#define Z_ADC_BASE			ADC0_BASE
#define Z_PERIPHERAL		SYSCTL_PERIPH_GPIOE //TODO: change to ADC peripheral

accelerometer_t accel;

/*
 * Programming for setting up the ADC pin range, sensitivity, etc
 */


void initializeAccelADC(){

	// Enable the peripheral for using ADC
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
	SysCtlDelay(3);

	 GPIOPinTypeGPIOInput(X_BASE, X_PIN);
	 GPIOPadConfigSet(X_BASE, X_PIN, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_ANALOG);


	 GPIOPinTypeGPIOInput(Y_BASE, Y_PIN);
	 GPIOPadConfigSet(Y_BASE, Y_PIN, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_ANALOG);

	 GPIOPinTypeGPIOInput(Z_BASE, Z_PIN);
	 GPIOPadConfigSet(Z_BASE, Z_PIN, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_ANALOG);

	// Configure Ports
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlDelay(3);

	// Configure the GPIO for ADC
	GPIOPinTypeADC(X_BASE, X_PIN);
	GPIOPinTypeADC(Y_BASE, Y_PIN);
	GPIOPinTypeADC(Z_BASE, Z_PIN);


	// Configure the sample sequence of 1 with a processor signal trigger
	ADCSequenceDisable(ADC0_BASE, 3);
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);

	ADCSequenceEnable(ADC0_BASE, 3);

	// Configure the sample sequence of 1 with a processor signal trigger
	ADCSequenceDisable(ADC1_BASE, 3);
	ADCSequenceConfigure(ADC1_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

	ADCSequenceStepConfigure(ADC1_BASE, 3, 0, ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);

	ADCSequenceEnable(ADC1_BASE, 3);


}

/*
 * Update the accelerometer global struct with data from the ADC
 */
void update_x_Accelerometer(){

	uint32_t ADC0_x_value[1];
	volatile unsigned long accel_x;

	ADCIntClear(X_ADC_BASE, 3);
	ADCProcessorTrigger(X_ADC_BASE, 3);

	while(!ADCIntStatus(X_ADC_BASE, 3, false));

	ADCSequenceDataGet(X_ADC_BASE, 3, ADC0_x_value);
	accel_x = ADC0_x_value[0];

	accel.xout = accel_x;

}

void update_y_Accelerometer(){


	uint32_t ADC0_y_value[1];
	volatile unsigned long accel_y;

	ADCIntClear(Y_ADC_BASE, 3);
	ADCProcessorTrigger(Y_ADC_BASE, 3);

	while(!ADCIntStatus(Y_ADC_BASE, 3, false));

	ADCSequenceDataGet(Y_ADC_BASE, 3, ADC0_y_value);
	accel_y = ADC0_y_value[0];

	accel.yout = accel_y;
}

void update_z_Accelerometer(){

	uint32_t ADC0_z_value[1];
	volatile unsigned long accel_z;

	ADCIntClear(Z_ADC_BASE, 3);
	ADCProcessorTrigger(Z_ADC_BASE, 3);

	while(!ADCIntStatus(Z_ADC_BASE, 3, false));

	ADCSequenceDataGet(Z_ADC_BASE, 3, ADC0_z_value);
	accel_z = ADC0_z_value[0];

	accel.zout = accel_z;
}

void updateAccelerometer(){
	update_x_Accelerometer();
	update_y_Accelerometer();
	//update_z_Accelerometer();
}

void queryAccel2(){
	update_x_Accelerometer();
	update_y_Accelerometer();
	//update_z_Accelerometer();
	// xout = (ACCEL_XOUT_H << 8) + ACCEL_XOUT_L
	UARTprintf("X Accelerometer output: %d\n", accel.xout);
    UARTprintf("Y Accelerometer output: %d\n", accel.yout);
    //UARTprintf("Z Accelerometer output: %d\n", accel.zout);
}


