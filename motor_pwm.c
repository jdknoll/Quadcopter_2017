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

#include "inc/tm4c123gh6pm.h"

#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#include "motor_pwm.h"
#include "ultrasonic.h"
#include "uartterm/t_uart.h"
#include "altitude_pid.h"
#include "config.h"

	uint32_t freq;

//    int fade_Up = 1;
    unsigned long increment = 1;
    int PWM_motor0 = 2802;
    int PWM_motor1 = 2802;
    int PWM_motor2 = 2802;
    int PWM_motor3 = 2802;


    int desired_height_cm;

void delaySEC(int sec)
{
    ROM_SysCtlDelay( (ROM_SysCtlClockGet()/1)*sec );
}

void pwm_gpio_configure()
{
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
//    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); //Enabled in Ultrasonic.c
//    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //Enabled in Ultrasonic.c
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); // The Tiva C Launchpad has two modules (0 and 1). Module 1 covers the LED pins
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

    //Setting the Pulse Width Modulation Clock
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_32);

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
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, 5721);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_1, 5721);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 5721);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 5721);

    // Set PWM duty-50%
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,2802);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2,2802);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,2802);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,2802);

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

        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,2802);
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2,2802);
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,2802);
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,2802);

        delaySEC(1);

        //TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        flag = 0;
    }
}

void TimerStart(int set_freq)
{
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
//  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);

  /*
    Configure the timer as periodic, by omission it's in count down mode.
    It counts from the load value to 0 and then resets back to the load value.
    REMEMBER: You need to configure the timer before setting the load and match
  */
  ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

  //We set the load value so the timer interrupts
  freq =  ((ROM_SysCtlClockGet() / set_freq));

  ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, freq);

  /*
    Enable the timeout interrupt. In count down mode it's when the timer reaches
  0 and resets back to load. In count up mode it's when the timer reaches load
  and resets back to 0.
  */
  //IntMasterEnable();  //Interrupts are enabled in main
  ROM_IntEnable(INT_TIMER1A);
  ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  ROM_TimerEnable(TIMER1_BASE, TIMER_A);
  ROM_IntPrioritySet(INT_TIMER1A, INT_PRIORITY_LEVEL_0);    //set the timer priority to the top priority
}

int pwm_saturate_add(int a, int b){
	int output = a + b;
	if(output > 4291){
		return 4291;
	} else  if (output < 2802){
		return 2802;
	} else {
		return output;
	}
}

// interrupt Handler for PWM speed
void pwm_interrupt()
{
    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	double error;

	error = pid.set_point - distance_cm;

	//call the PID loop to give updates on altitude
	pid_update(error, 1/freq);

	PWM_motor0 = pwm_saturate_add(PWM_motor0, pid.control);
	PWM_motor1 = pwm_saturate_add(PWM_motor1, pid.control);
	PWM_motor2 = pwm_saturate_add(PWM_motor2, pid.control);
	PWM_motor3 = pwm_saturate_add(PWM_motor3, pid.control);


    // Sets the adjusted speed to the PWM pins
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, PWM_motor0);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, PWM_motor1);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, PWM_motor2);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, PWM_motor3);
		
	#ifdef _DEBUG_MODE
    UARTprintf("PID Control: %d", pid.control);
    UARTprintf("PWM_motor0 %d\n", PWM_motor0);
    int test_cm = (int) distance_cm;
    UARTprintf("Distance cm: %d\n", test_cm);
    int test_error = (int) error;
    UARTprintf("Error: %d\n", test_error);
	#endif
}

void TimerStart2(int set_freq)
{
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

  /*
    Configure the timer as periodic, by omission it's in count down mode.
    It counts from the load value to 0 and then resets back to the load value.
    REMEMBER: You need to configure the timer before setting the load and match
  */
  ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

  //We set the load value so the timer interrupts
  uint32_t freq =  ((ROM_SysCtlClockGet() / set_freq));

  ROM_TimerLoadSet(TIMER1_BASE, TIMER_B, freq);

  /*
    Enable the timeout interrupt. In count down mode it's when the timer reaches
  0 and resets back to load. In count up mode it's when the timer reaches load
  and resets back to 0.
  */
  //IntMasterEnable();  //Interrupts are enabled in main
  ROM_IntEnable(INT_TIMER1B);
  ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMB_TIMEOUT);
  ROM_TimerEnable(TIMER1_BASE, TIMER_B);
  ROM_IntPrioritySet(INT_TIMER1B, INT_PRIORITY_LEVEL_0);    //set the timer priority to the top priority
}

void pwm_interrupt2()
{
    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    UARTprintf("%d", distance_cm);
}




