/*
 * pwm_gpio.c
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
#include <math.h>

#include "inc/tm4c123gh6pm.h"

#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"

#include "motor_pwm.h"
#include "ultrasonic.h"
#include "uartterm/t_uart.h"
#include "altitude_pid.h"
#include "config.h"


void delaySEC(int sec)
{
    ROM_SysCtlDelay( (ROM_SysCtlClockGet()/1)*sec );
}

void pwm_gpio_configure()
{
    //Enabling the peripherals used by the program (F,C,D)
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); // The Tiva C Launchpad has two modules (0 and 1). Module 1 covers the LED pins

    // Configure the PF# pins as Pulse Width Modulation
    ROM_GPIOPinConfigure(GPIO_PB6_M0PWM0);
    ROM_GPIOPinConfigure(GPIO_PB4_M0PWM2);
    ROM_GPIOPinConfigure(GPIO_PF1_M1PWM5);
    ROM_GPIOPinConfigure(GPIO_PF2_M1PWM6);

    // Configures the pins as Pulse Width Modulation
    ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2);
    ROM_GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4|GPIO_PIN_6);

}

void pwm_configuration()
{

    //Enabling the peripherals used by the program (F,C,D)
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); //Enabled in Ultrasonic.c
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //Enabled in Ultrasonic.c
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); // The Tiva C Launchpad has two modules (0 and 1). Module 1 covers the LED pins
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

    //Setting the Pulse Width Modulation Clock
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_8);

    // Configure PWM Options
    // PWM_GEN_0 Covers M1PWM0 and M1PWM1
    // PWM_GEN_1 Covers M1PWM2 and M1PWM3
    // PWM_GEN_2 Covers M1PWM4 and M1PWM5
    // PWM_GEN_3 Covers M1PWM6 and M1PWM7 See page 417 DriverLib doc
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // Set the Period (expressed in clock ticks)
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, 5000);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_1, 5000);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 5000);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 5000);

    // Set PWM duty-50%
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,1950);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2,1950);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,1950);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,1950);

    // Enable the PWM generator
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_0);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_1);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    // Turn on the Output pins
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT | PWM_OUT_2_BIT | PWM_OUT_5_BIT | PWM_OUT_6_BIT, true);
}

void arm_the_motor()
{
    int flag = 1;

    if(flag == 1)
    {
        //TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,1800);
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2,1800);
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,1800);
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,1800);

        delaySEC(1);

        //TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        flag = 0;
    }
}

void TimerStart(/*int set_freq*/)
{
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  /*
    Configure the timer as periodic, by omission it's in count down mode.
    It counts from the load value to 0 and then resets back to the load value.
    REMEMBER: You need to configure the timer before setting the load and match
  */
  ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

  //We set the load value so the timer interrupts
  //freq =  ((ROM_SysCtlClockGet() / set_freq));

  //ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, freq);
  ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, pid.freq);

  /*
    Enable the timeout interrupt. In count down mode it's when the timer reaches
  0 and resets back to load. In count up mode it's when the timer reaches load
  and resets back to 0.
  */
  //IntMasterEnable();  //Interrupts are enabled in main
  ROM_IntEnable(INT_TIMER1A);
  ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  ROM_TimerEnable(TIMER1_BASE, TIMER_A);
  ROM_IntPrioritySet(INT_TIMER1A, INT_PRIORITY_LEVEL_2);    //set the timer priority to the top priority
}



// interrupt Handler for PWM speed
void pwm_interrupt()
{
    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	int error;

	error = pid.set_point - distance_cm;

	//call the PID loop to give updates on altitude
	double timescale = 1/(double)pid.freq;
	pid_update(error, timescale);

    // Sets the adjusted speed to the PWM pins
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, (int)pid.PWM_motor0);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, (int)pid.PWM_motor1);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, (int)pid.PWM_motor2);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, (int)pid.PWM_motor3);
		
	#ifdef _DEBUG_MODE
    UARTprintf("PID Control: %d\n", pid.control);
    UARTprintf("PWM_motor0 %d\n", (int)pid.PWM_motor0);
    UARTprintf("Distance cm: %d\n", (int)distance_cm);
    UARTprintf("Error: %d\n", (int)error);
	#endif
}
