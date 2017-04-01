/*
 * Programmer: Trey Perry, Josiah Knoll, Scott Keller
 * Project: Altitude_Control
 * File: ultrasonic.c
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"

#include "ultrasonic.h"
#include "uartterm/t_uart.h"
#include "inc/hw_timer.h"
#include "ftoa.h"
#include "config.h"

#define TRIGGER_TIMER_FREQ 20000000

#define TRIGGER_PIN 			GPIO_PIN_6
#define TRIGGER_BASE 			GPIO_PORTD_BASE
#define TRIGGER_PERIPHERAL		SYSCTL_PERIPH_GPIOD

#define TRIGGER_TIMER_BASE		TIMER2_BASE
#define TRIGGER_TIMER			TIMER_A

#define ECHO_PIN 				GPIO_INT_PIN_7
#define ECHO_BASE 				GPIO_PORTC_BASE
#define ECHO_PERIPHERAL			SYSCTL_PERIPH_GPIOC

#define ECHO_TIMER_BASE			TIMER5_BASE
#define ECHO_TIMER_PERIPHERAL	SYSCTL_PERIPH_TIMER5
#define ECHO_TIMER				TIMER_A


#define HIGH 1
#define LOW 0

#define TRIGGER_HIGH_TIME 800
#define TRIGGER_LOW_TIME 1999200

int range_cm;
double distance_cm;

char distance_string[50];

void ultrasonicTriggerTimerHandler(void)
{
	// Declare the trigger_status static integer
	static int trigger_status = 0;

    // Clear the timer interrupt
    ROM_TimerIntClear(TRIGGER_TIMER_BASE, TRIGGER_TIMER);


    if(trigger_status){
    	// If the trigger is high, flip it low and set the low time
        ROM_TimerLoadSet(TRIGGER_TIMER_BASE, TRIGGER_TIMER, TRIGGER_HIGH_TIME);
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, TRIGGER_PIN, TRIGGER_PIN);
        trigger_status = ~trigger_status;
    } else {
        // If the trigger is low, flip it high and set the high time
        ROM_TimerLoadSet(TRIGGER_TIMER_BASE, TRIGGER_TIMER, TRIGGER_LOW_TIME); // calculated to guarantee that trigger shouldn't run while echo is high: 1892800
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, TRIGGER_PIN, 1);
        trigger_status = ~trigger_status;
    }
}


void distance_calculations(uint32_t clock_timer)
{
    //formula given by the ultrasonic data sheet for centimeters
    distance_cm = ((double)clock_timer/80)/58;

    //formula given by the ultrasonic data sheet for range
    //range_cm = (clock_timer * 340) / 2;
    if (distance_cm < 0){
    		distance_string[0] = '-';
    		distance_string[1] = '1';
    		distance_string[2] = '\0';
    } else {
    		ftoa(distance_cm, distance_string, 3);
    }
	#ifdef _MATLAB_OUT
    UARTprintf("%s\n", distance_string);						// This line will need to be un-commented for MatLab
	#endif
}


//this will output how long the echo signal was high
//pull clock reading information from a TI QA: https://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/t/256323
void ultrasonicEchoHandler(void)
{
	volatile uint32_t clock_timer;
	GPIOIntClear(ECHO_BASE, ECHO_PIN);  // Clear interrupt flag
    bool value = GPIOPinRead(ECHO_BASE, ECHO_PIN);

    if(value){               				// this trigger occurs when echo is high going low - measure here
    		ROM_TimerDisable(ECHO_TIMER_BASE, ECHO_TIMER);
    		HWREG(ECHO_TIMER_BASE + TIMER_O_TAV) = 0;			// GPTM Timer A Value set to 0
    		ROM_TimerLoadSet(ECHO_TIMER_BASE, ECHO_TIMER,  0xFFFFFFFF);
    		ROM_TimerEnable(ECHO_TIMER_BASE, ECHO_TIMER);
    } else {								// this occurs on the negedge of echo
    		ROM_TimerDisable(ECHO_TIMER_BASE, ECHO_TIMER);
    		clock_timer = ROM_TimerValueGet(ECHO_TIMER_BASE, ECHO_TIMER);
            // Do the pwm calculations on the negedge of echo
            distance_calculations(clock_timer);
    }
}


void initialize_ultrasonic()
{
	// start by initializing the Trigger GPIO
	ROM_SysCtlPeripheralEnable(TRIGGER_PERIPHERAL);			// enable to GPIO peripheral
    ROM_SysCtlDelay(3);										// Delay as advised in the datasheet
    while(!ROM_SysCtlPeripheralReady(TRIGGER_PERIPHERAL));	// Wait for the GPIOF module to be ready.
    ROM_GPIOPinTypeGPIOOutputOD(TRIGGER_BASE, TRIGGER_PIN);	// Set pins PF1 to output trigger

    // Set up the trigger timer interrupt
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);		// enable the timer2 peripheral for the trigger
    ROM_TimerConfigure(TRIGGER_TIMER_BASE, TIMER_CFG_PERIODIC); 	// configure timer2 peripheral for the trigger
    ROM_TimerLoadSet(TRIGGER_TIMER_BASE, TRIGGER_TIMER, TRIGGER_TIMER_FREQ);   	// set timer2A with the desired period

    // enable the interrupt
    ROM_IntEnable(INT_TIMER2A);
    ROM_TimerIntEnable(TRIGGER_TIMER_BASE, TIMER_TIMA_TIMEOUT);
    ROM_IntPrioritySet(INT_TIMER2A, INT_PRIORITY_LEVEL_1);	// set the priority lower than the pwm
    ROM_TimerEnable(TRIGGER_TIMER_BASE, TRIGGER_TIMER);					// enable timer2A


    // initialize the echo timer
    ROM_SysCtlPeripheralEnable(ECHO_TIMER_PERIPHERAL);
    ROM_SysCtlPeripheralReset(ECHO_TIMER_PERIPHERAL);
    ROM_TimerConfigure(ECHO_TIMER_BASE, TIMER_CFG_ONE_SHOT_UP);
    ROM_TimerControlStall(ECHO_TIMER_BASE, ECHO_TIMER, true);

    // then initialize the echo GPIO
    ROM_SysCtlPeripheralEnable(ECHO_PERIPHERAL);
    ROM_SysCtlDelay(3);										// Delay as advised in the datasheet
    while(!ROM_SysCtlPeripheralReady(ECHO_PERIPHERAL));		// Wait for the GPIOF module to be ready.
    ROM_GPIOPinTypeGPIOInput(ECHO_BASE, ECHO_PIN);

    // enable the GPIO driven interrupts using the sonic echo as a flag
    ROM_GPIOIntTypeSet(ECHO_BASE, ECHO_PIN, GPIO_BOTH_EDGES);
    GPIOIntEnable(ECHO_BASE, ECHO_PIN);
    GPIOIntClear(ECHO_BASE, ECHO_PIN);  // Clear interrupt flag

    ROM_IntEnable(INT_GPIOC);
    ROM_IntPrioritySet(INT_GPIOC, INT_PRIORITY_LEVEL_1);	// set the priority lower than the pwm
}
