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
#include "config.h"
#include "accelerometer.h"
#include "I2C.h"
#include "pid/pid.h"

#define DEBUG_PWM

#ifdef DEBUG_PWM
#define DBPRINTF(y,...) UARTprintf(y,__VA_ARGS__)
#else
#define DBPRINTF(...)
#endif

t_PWM pwm;

void delayMS(int ms)
{
    `( (ROM_SysCtlClockGet()/(3*1000))*ms );
}

void pwm_configuration()
{

    //Enabling the peripherals used by the program (F,C,D)
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); // The Tiva C Launchpad has two modules (0 and 1). Module 1 covers the LED pins
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

    // Configure the PF# pins as Pulse Width Modulation
    ROM_GPIOPinConfigure(GPIO_PB6_M0PWM0);
    ROM_GPIOPinConfigure(GPIO_PB4_M0PWM2);
    ROM_GPIOPinConfigure(GPIO_PF1_M1PWM5);
    ROM_GPIOPinConfigure(GPIO_PF2_M1PWM6);

    // Configures the pins as Pulse Width Modulation
    ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2);
    ROM_GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4|GPIO_PIN_6);

    //Setting the Pulse Width Modulation Clock
    ROM_SysCtlPWMClockSet(DIV_VALUE);

    // Configure PWM Options
    // PWM_GEN_0 Covers M1PWM0 and M1PWM1
    // PWM_GEN_1 Covers M1PWM2 and M1PWM3
    // PWM_GEN_2 Covers M1PWM4 and M1PWM5
    // PWM_GEN_3 Covers M1PWM6 and M1PWM7 See page 417 DriverLib doc
    ROM_PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    ROM_PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // Set the Period (expressed in clock ticks)
    ROM_PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, PWM_PERIOD);
    ROM_PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, PWM_PERIOD);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, PWM_PERIOD);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, PWM_PERIOD);

    // Set PWM duty-50%
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, MIN_MOTOR_SPEED);
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, MIN_MOTOR_SPEED);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, MIN_MOTOR_SPEED);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, MIN_MOTOR_SPEED);

    // Enable the PWM generator
    ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_1);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    // Turn on the Output pins
    ROM_PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_2_BIT, true);
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT, true);
}

void arm_the_motor()
{
	ROM_IntMasterDisable();
    int flag = 1;

    if(flag == 1)
    {
        //TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0,ARM_MOTOR_SPEED);
        ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2,ARM_MOTOR_SPEED);
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,ARM_MOTOR_SPEED);
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,ARM_MOTOR_SPEED);

        delayMS(1500);

        //TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        flag = 0;
    }
    ROM_IntMasterEnable();
}

void AltitudeTimerStart()
{
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  /*
    Configure the timer as periodic, by omission it's in count down mode.
    It counts from the load value to 0 and then resets back to the load value.
    REMEMBER: You need to configure the timer before setting the load and match
  */
  ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

  //We set the load value so the timer interrupts
  ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, altitude_pid.clock_ticks);

  /*
    Enable the timeout interrupt. In count down mode it's when the timer reaches
  0 and resets back to load. In count up mode it's when the timer reaches load
  and resets back to 0.
  */
  ROM_IntEnable(INT_TIMER1A);
  ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  ROM_TimerEnable(TIMER1_BASE, TIMER_A);
  ROM_IntPrioritySet(INT_TIMER1A, INT_PRIORITY_LEVEL_2);    //set the timer priority to the top priority
}

// updates the motors to the values stored in the PWM struct
void update_motors(){
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, pwm.motor0);
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, pwm.motor1);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, pwm.motor2);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, pwm.motor3);
}



// interrupt Handler for PWM speed
void altitude_PID_interrupt()
{
    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	int error;

	error = altitude_pid.set_point - distance_mm;

	//call the PID loop to give updates on altitude
	double timescale = 1/(double)(80000000/altitude_pid.clock_ticks);

	//Disable the other interrupt while we update the motor values
	//ROM_TimerIntDisable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
	pid_update(error, timescale, &altitude_pid, ALTITUDE_MODE);

    // Sets the adjusted speed to the PWM pins
	update_motors();
	//ROM_TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);

    DBPRINTF("PWM_motor0 %d\n", (int)pwm.motor0);
    DBPRINTF("Distance mm: %d\n", (int)distance_mm);
    DBPRINTF("Error: %d\n", (int)error);
}

#define LEVELING_TIMER_INT		INT_TIMER4A
#define LEVELING_TIMER_BASE 		TIMER4_BASE

void LevelingTimerStart()
{
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);

  /*
    Configure the timer as periodic, by omission it's in count down mode.
    It counts from the load value to 0 and then resets back to the load value.
    REMEMBER: You need to configure the timer before setting the load and match
  */
  ROM_TimerConfigure(LEVELING_TIMER_BASE, TIMER_CFG_PERIODIC);

  //We set the load value so the timer interrupts
  ROM_TimerLoadSet(LEVELING_TIMER_BASE, TIMER_A, leveling_x_pid.clock_ticks);

  /*
    Enable the timeout interrupt. In count down mode it's when the timer reaches
  0 and resets back to load. In count up mode it's when the timer reaches load
  and resets back to 0.
  */
  ROM_IntEnable(LEVELING_TIMER_INT);
  ROM_TimerIntEnable(LEVELING_TIMER_BASE, TIMER_TIMA_TIMEOUT);
  ROM_TimerEnable(LEVELING_TIMER_BASE, TIMER_A);
  ROM_IntPrioritySet(LEVELING_TIMER_INT, INT_PRIORITY_LEVEL_2);    //set the timer priority to the top priority
}

void leveling_PID_interrupt()
{
    // Clear the timer interrupt
    TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);

    QueryAccel();

    int x_error = leveling_x_pid.set_point - accel.x;
    //int y_error = leveling_y_pid.set_point - accel_y;

	double timescale = 1/(double)(80000000/leveling_x_pid.clock_ticks);

	//Disable the other interrupt while we update the motor values
	//ROM_TimerIntDisable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    pid_update(x_error, timescale, &leveling_x_pid, LEVELING_X_MODE);
    //pid_update(y_error, timescale, &leveling_y_pid, LEVELING_Y_MODE);

    // Sets the adjusted speed to the PWM pins
	//update_motors();
    //ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}
