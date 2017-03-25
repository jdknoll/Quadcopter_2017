/*
 * Programmer: Trey Perry
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

#define trigger GPIO_PIN_6
#define echo GPIO_PIN_7

#define timerA_freq 20000000
#define HIGH 1
#define LOW 0

int interrupt_status = 0;
int trigger_status = 0; //if high on trigger, 1 otherwise, 0

volatile uint32_t clock_timer;
int range_cm;
int distance_cm;

// Initialize function takes a variable to set frequency
void inter_initialize()
{
    // enable the timer2 peripheral
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    // configure timer2 peripheral
    ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);

    // set timer2A with the desired period
    ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, timerA_freq);

    // enable the interrupt
    ROM_IntEnable(INT_TIMER2A);
    ROM_TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    ROM_IntPrioritySet(INT_TIMER2A,   0x20);    // set the priority lower than the pwm
    ROM_IntMasterEnable();

    // enable timer2A
    ROM_TimerEnable(TIMER2_BASE, TIMER_A);

    // enable the GPIO driven interrupts using the sonic echo as a flag
    ROM_IntEnable(INT_GPIOD);
    GPIOIntEnable(GPIO_PORTD_BASE, echo);
    ROM_GPIOIntTypeSet(GPIO_PORTD_BASE, echo, GPIO_RISING_EDGE);

    ROM_IntMasterEnable();
}


void Timer2AIntHandler(void)
{
    // Clear the timer interrupt
    ROM_TimerIntClear(TIMER2_BASE, TIMER_A);

    // Send the 10 us trigger signal
    if(trigger_status){
        //if the trigger is high, set a timer for the low time and switch the signal
        ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, 800);
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, trigger, trigger);
        trigger_status = ~trigger_status;
    } else {
        //if the trigger is low, set a timer for the high time and switch the signal
        ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, 20000000);
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, trigger, 1);
        trigger_status = ~trigger_status;
    }
}


//this will output how long the echo signal was high
void PortDIntHandler(void)
{
    volatile uint32_t clock_timer = 0;
    GPIOIntClear(GPIO_PORTD_BASE, echo);  // Clear interrupt flag

    if(interrupt_status){               //if you're waiting for the echo to start
        ROM_GPIOIntTypeSet(GPIO_PORTD_BASE, echo, GPIO_RISING_EDGE);
        interrupt_status = ~interrupt_status;
        clock_timer = ROM_SysTickValueGet();
        ROM_UARTCharPut(UART0_BASE, '\n');
    } else {
        ROM_GPIOIntTypeSet(GPIO_PORTD_BASE, echo, GPIO_FALLING_EDGE);
        interrupt_status = ~interrupt_status;
        ROM_SysTickEnable();                //starts a 24 bit timer
    }
}

void distance_calculations(clock_timer)
{
//    distance_cm = 5;
    //formula given by the ultrasonic data sheet for centimeters
    distance_cm = clock_timer/58;
    //formula given by the ultrasonic data sheet for range
    //range_cm = (clock_timer * 340) / 2;
}

void initialize_gpio()
{

    // Enable the GPIOA peripheral
   ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    // Delay as advised in the datasheet
   ROM_SysCtlDelay(3);

   // Wait for the GPIOF module to be ready.
   while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));

   // Set pins PF1 to output trigger and PF2 to input the echo
   ROM_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, trigger);
   ROM_GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, echo);

   // Set the configuration so that the signals are pulled up to 15 mA
   //GPIOPadConfigSet(GPIO_PORTD_BASE, trigger, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
   //GPIOPadConfigSet(GPIO_PORTD_BASE, echo, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
}
