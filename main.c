/*
 * main.c
 *
 *  Created on: Mar 15, 2017
 *      Author: Scott Keller, Josiah Knoll, Trey Perry, and Kyle Shoop
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


#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#include "I2C.h"
#include "accelerometer.h"
#include "motor_pwm.h"
#include "pid/pid.h"
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
	// Set the clock to 80MHz
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_XTAL_16MHZ);

	// Enable all interrupts
	ROM_IntMasterEnable();

    // initialize pwm configuration
    pwm_configuration();

    // arms the motor for start
    arm_the_motor();

    // initialize the UART
    initUART();

    // Initialize the I2C1 interface
    initI2C1();

    // Initialize the Accelerometer
    InitializeAccel();

    // Print out the WHO_AM_I register
    PrintAccelWhoAmI();

    // Print the accelerometer's configuration bits
    //PrintAccelConfigReg();

    // Initialize the GPIO for the sonic distance
    initialize_ultrasonic();
    wait(100000);

    // Initialize the PID and PWM struct values
    pid_initialize();

    // initialize pwm timer
    AltitudeTimerStart();		// timer for the ultrasonic altitude loop
    //LevelingTimerStart();		// timer for the accelerometer leveling loop
    update_motors();

    // Testing Loop
    while(1)
    {
    	//UARTprintf("hello!");
    	//terminal();
    //PrintAccel();	//prints out accelerometer data
    //	PrintAccelWhoAmI();


    wait(18000);
        //UARTCharPut(UART0_BASE, '\n');
    }
}
