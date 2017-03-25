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
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"

#include "motor_pwm.h"
#include "t_interrupt.h"
#include "ultrasonic.h"

//    int fade_Up = 1;
    unsigned long increment = 1;
    unsigned long pwmNow = 1950;

    int desired_height_cm;

void delaySEC(int sec)
{
    ROM_SysCtlDelay( (ROM_SysCtlClockGet()/1)*sec );
}

void pwm_gpio_configure()
{
    // Configure the PF# pins as Pulse Width Modulation
    ROM_GPIOPinConfigure(GPIO_PF0_M1PWM4);
    ROM_GPIOPinConfigure(GPIO_PF1_M1PWM5);
    ROM_GPIOPinConfigure(GPIO_PF2_M1PWM6);
    ROM_GPIOPinConfigure(GPIO_PF3_M1PWM7);

    // Configures the pins as Pulse Width Modulation
    ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

void pwm_configuration()
{

    //Setting the Pulse Width Modulation Clock
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_8);

    // Configure PWM Options
    // PWM_GEN_2 Covers M1PWM4 and M1PWM5
    // PWM_GEN_3 Covers M1PWM6 and M1PWM7 See page 207 4/11/13 DriverLib doc
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
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_4,1950);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,1950);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,1950);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,1950);

    // Enable the PWM generator
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_0);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_1);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    // Turn on the Output pins
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_4_BIT | PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
}

void arm_the_motor()
{
    int flag = 1;

    if(flag == 1)
    {
        //TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_4,1800);
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,1800);
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,1800);
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,1800);

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
  uint32_t freq =  ((ROM_SysCtlClockGet() / set_freq));

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

// interrupt Handler for PWM speed
void pwm_interrupt()
{
    // Set the starting value for desired height
    desired_height_cm = 5;

    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Read the input data and adjust motor speed
    // If calculated values are below set desired height, increase PWM duty cycle
    if(desired_height_cm > distance_cm)
      {
        if(pwmNow <= 2250)
        {
            pwmNow += increment;
            //distance_cm++;
        }
      }
    // If calculated values are above set desired height, decrease PWM duty cylce
    else if(desired_height_cm < distance_cm)
      {
        if(pwmNow >= 1950)
        {
            pwmNow -= increment;
            //distance_cm--;
        }
      }
    // If calculated values are equal to desired height, keep PWM duty cycle the same
    else
      {
        pwmNow = pwmNow;
      }

/**
 * Original PWM code
 *
//           // if read values are below set limits, set motor speed to increase
//           if (fade_Up)
//           {
//               pwmNow += increment;
//               if (pwmNow >= 2250)
//               {
//                   fade_Up = !fade_Up;
//               }
//           }
//        }
//           // If read values are above set limits, set motor speed to decreases
//           else if(!fade_Up)
//            {
//                pwmNow -= increment;
//                if (pwmNow <= 1950)
//                {
//                    fade_Up = !fade_Up;
//                }
//            }
**/
    // Sets the adjusted speed to the PWM pins
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_4,pwmNow);
//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,pwmNow);
//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,pwmNow);
//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,pwmNow);
}

void TimerStart2(int set_freq)
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
    // Set the starting value for desired height
    desired_height_cm = 5;

    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Read the input data and adjust motor speed
    // If calculated values are below set desired height, increase PWM duty cycle
    if(desired_height_cm > distance_cm)
      {
        if(pwmNow <= 2250)
        {
            pwmNow += increment;
            distance_cm++;
        }
      }
    // If calculated values are above set desired height, decrease PWM duty cylce
    else if(desired_height_cm < distance_cm)
      {
        if(pwmNow >= 1950)
        {
            pwmNow -= increment;
            distance_cm--;
        }
      }
    // If calculated values are equal to desired height, keep PWM duty cycle the same
    else
      {
        pwmNow = pwmNow;
      }

//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_4,pwmNow);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,pwmNow);
//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,pwmNow);
//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,pwmNow);
}

void TimerStart3(int set_freq)
{
//  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);

  /*
    Configure the timer as periodic, by omission it's in count down mode.
    It counts from the load value to 0 and then resets back to the load value.
  REMEMBER: You need to configure the timer before setting the load and match
  */
  ROM_TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);

  //We set the load value so the timer interrupts
  uint32_t freq =  ((ROM_SysCtlClockGet() / set_freq));

  ROM_TimerLoadSet(TIMER3_BASE, TIMER_A, freq);

  /*
    Enable the timeout interrupt. In count down mode it's when the timer reaches
  0 and resets back to load. In count up mode it's when the timer reaches load
  and resets back to 0.
  */
  //IntMasterEnable();  //Interrupts are enabled in main
  ROM_IntEnable(INT_TIMER3A);
  ROM_TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
  ROM_TimerEnable(TIMER3_BASE, TIMER_A);
  ROM_IntPrioritySet(INT_TIMER3A, INT_PRIORITY_LEVEL_0);    //set the timer priority to the top priority
}

void pwm_interrupt3()
{
    // Set the starting value for desired height
    desired_height_cm = 5;

    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Read the input data and adjust motor speed
    // If calculated values are below set desired height, increase PWM duty cycle
    if(desired_height_cm > distance_cm)
      {
        if(pwmNow <= 2250)
        {
            pwmNow += increment;
            distance_cm++;
        }
      }
    // If calculated values are above set desired height, decrease PWM duty cylce
    else if(desired_height_cm < distance_cm)
      {
        if(pwmNow >= 1950)
        {
            pwmNow -= increment;
            distance_cm--;
        }
      }
    // If calculated values are equal to desired height, keep PWM duty cycle the same
    else
      {
        pwmNow = pwmNow;
      }

//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_4,pwmNow);
//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,pwmNow);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,pwmNow);
//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,pwmNow);
}

void TimerStart4(int set_freq)
{
//  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);

  /*
    Configure the timer as periodic, by omission it's in count down mode.
    It counts from the load value to 0 and then resets back to the load value.
  REMEMBER: You need to configure the timer before setting the load and match
  */
  ROM_TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);

  //We set the load value so the timer interrupts
  uint32_t freq =  ((ROM_SysCtlClockGet() / set_freq));

  ROM_TimerLoadSet(TIMER3_BASE, TIMER_B, freq);

  /*
    Enable the timeout interrupt. In count down mode it's when the timer reaches
  0 and resets back to load. In count up mode it's when the timer reaches load
  and resets back to 0.
  */
  //IntMasterEnable();  //Interrupts are enabled in main
  ROM_IntEnable(INT_TIMER3B);
  ROM_TimerIntEnable(TIMER3_BASE, TIMER_TIMB_TIMEOUT);
  ROM_TimerEnable(TIMER3_BASE, TIMER_B);
  ROM_IntPrioritySet(INT_TIMER3B, INT_PRIORITY_LEVEL_0);    //set the timer priority to the top priority
}

void pwm_interrupt4()
{
    // Set the starting value for desired height
    desired_height_cm = 5;

    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Read the input data and adjust motor speed
    // If calculated values are below set desired height, increase PWM duty cycle
    if(desired_height_cm > distance_cm)
      {
        if(pwmNow <= 2250)
        {
            pwmNow += increment;
            distance_cm++;
        }
      }
    // If calculated values are above set desired height, decrease PWM duty cylce
    else if(desired_height_cm < distance_cm)
      {
        if(pwmNow >= 1950)
        {
            pwmNow -= increment;
            distance_cm--;
        }
      }
    // If calculated values are equal to desired height, keep PWM duty cycle the same
    else
      {
        pwmNow = pwmNow;
      }

//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_4,pwmNow);
//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,pwmNow);
//    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,pwmNow);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,pwmNow);
}