/*
 * system_clock_set.c
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

#include "driverlib/sysctl.h"
#include "driverlib/rom.h"

#include "motor_pwm.h"

void clock_config()
{
       //Setting the Clock
	   ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_XTAL_16MHZ);
}
