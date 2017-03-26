/*
 * main.c
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "inc/hw_gpio.h"
//#include "inc/tm4c123gh6pm.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"

#include "utils/uartstdio.h"

#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"

#include "I2C.h"
#include "accelerometer.h"
#include "motor_pwm.h"
#include "uartterm/t_uart.h"
#include "ultrasonic.h"
#include "uartterm/terminal.h"

void wait(int waitTime){
    int i = 0;
    while (i < waitTime){
        i++;
    }
    i = 0;
}

int main(void)
 {
	// Enable all interrupts
	ROM_IntMasterEnable();

    // Initialize clock settings
    clock_config();

    // initialize pwm configuration
    pwm_configuration();

    // initialize gpio configuration
    pwm_gpio_configure();

    // arms the motor for start
    arm_the_motor();

    // initialize pwm timer
    TimerStart(100);

    // initialize the UART
    initUART();

    // Initialize the I2C1 interface
    initI2C1();

    // Initialize the Accelerometer
    InitializeAccel();

    // Print out the WHO_AM_I register
    PrintAccelWhoAmI();

    // Print the accelerometer's configuration bits
    PrintAccelConfigReg();

    // Initialize the GPIO for the sonic distance
    initialize_ultrasonic();


    // Testing Loop
    while(1)
    {
    	//terminal();
    	//QueryAccel();	//prints out accelerometer data

        //wait(100000);
        //UARTCharPut(UART0_BASE, '\n');


    }
}



