/*
 * Programmer: Trey Perry, Josiah Knoll, Scott Keller
 * Project: Altitude_Control
 * File: ultrasonic.c
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/i2c.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "driverlib/rom.h"

#include "ultrasonic.h"
#include "uartterm/t_uart.h"
#include "t_interrupt.h"
#include "inc/hw_timer.h"
#include "ftoa.h"

#define TRIGGER_PIN 			GPIO_PIN_6
#define TRIGGER_BASE 			GPIO_PORTD_BASE
#define TRIGGER_PERIPHERAL		SYSCTL_PERIPH_GPIOD

#define ECHO_PIN 				GPIO_INT_PIN_7
#define ECHO_BASE 				GPIO_PORTC_BASE
#define ECHO_PERIPHERAL			SYSCTL_PERIPH_GPIOC

#define ECHO_TIMER_BASE			TIMER5_BASE
#define ECHO_TIMER_PERIPHERAL	SYSCTL_PERIPH_TIMER5
#define ECHO_TIMER				TIMER_A


#define TIMERA_FREQ 20000000
#define HIGH 1
#define LOW 0

int interrupt_status = 0;
int trigger_status = 0; //if high on trigger, 1 otherwise, 0

volatile uint32_t clock_timer;
int range_cm;
double distance_cm;

char distance_string[50];

void ultrasonicTriggerTimerHandler(void)
{
    // Clear the timer interrupt
    ROM_TimerIntClear(TIMER2_BASE, TIMER_A);

    // Send the 10 us trigger signal
    if(trigger_status){
        //if the trigger is high, set a timer for the low time and switch the signal
        ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, 800);
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, TRIGGER_PIN, TRIGGER_PIN);
        trigger_status = ~trigger_status;
    } else {
        //if the trigger is low, set a timer for the high time and switch the signal
        ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, 1892800); // calculated to guarantee that trigger shouldn't run while echo is high
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, TRIGGER_PIN, 1);
        trigger_status = ~trigger_status;
    }
}


void distance_calculations(uint32_t clock_timer)
{
//    distance_cm = 5;
    //formula given by the ultrasonic data sheet for centimeters
    //distance_cm = ((double)clock_timer/80)/58;
    distance_cm = ((double)clock_timer/80)/58;

    //formula given by the ultrasonic data sheet for range
    //range_cm = (clock_timer * 340) / 2;
    ftoa(distance_cm, distance_string, 3);
}


//this will output how long the echo signal was high
//pull clock reading information from a TI QA: https://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/t/256323
void ultrasonicEchoHandler(void)
{
	int clock_timer;

    GPIOIntClear(ECHO_BASE, ECHO_PIN);  // Clear interrupt flag
    bool value = GPIOPinRead(ECHO_BASE, ECHO_PIN);

    if(value){               				// this trigger occurs when echo is high going low - measure here
    	//ROM_SysTickEnable();                //starts a 24 bit timer
    	ROM_TimerDisable(ECHO_TIMER_BASE, ECHO_TIMER);
    	HWREG(ECHO_TIMER_BASE + TIMER_O_TAV) = 0;			// GPTM Timer A Value set to 0
    	ROM_TimerLoadSet(ECHO_TIMER_BASE, ECHO_TIMER,  0xFFFFFFFF);
    	ROM_TimerEnable(ECHO_TIMER_BASE, ECHO_TIMER);
    } else {								// this trigger occurs when the echo is low going high
    	//ROM_IntDisable(INT_WTIMER1A);
    	//ROM_TimerIntDisable(ECHO_TIMER_BASE, TIMER_TIMA_TIMEOUT);
    	ROM_TimerDisable(ECHO_TIMER_BASE, ECHO_TIMER);

    	clock_timer = ROM_TimerValueGet(ECHO_TIMER_BASE, ECHO_TIMER);

    	//clock_timer = ROM_SysTickValueGet();
        distance_calculations(clock_timer);

        UARTprintf("%d\n", distance_string);
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
    ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC); 	// configure timer2 peripheral for the trigger
    ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, TIMERA_FREQ);   	// set timer2A with the desired period

    // enable the interrupt
    ROM_IntEnable(INT_TIMER2A);
    ROM_TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    ROM_IntPrioritySet(INT_TIMER2A, INT_PRIORITY_LEVEL_1);	// set the priority lower than the pwm
    ROM_TimerEnable(TIMER2_BASE, TIMER_A);					// enable timer2A


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
