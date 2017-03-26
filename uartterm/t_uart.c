/*
 * uart.c
 *
 *  Created on: Mar 6, 2017
 *      Author: jdknoll
 */

#include "../uartterm/t_uart.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"

#include "t_uart.h"

int echo = 1; //by default, echo the input

/*
 * Handles the interrupt from a UART incoming communication
 */
void UARTIntHandler(void)
{
	char ch;

    //get interrupt status
    uint32_t ui32Status;
	ui32Status = ROM_UARTIntStatus(UART0_BASE, true);

    while(ROM_UARTCharsAvail(UART0_BASE)) //loop while there are chars
    {
    	ch = ROM_UARTCharGetNonBlocking(UART0_BASE);
    	if(echo){
    		ROM_UARTCharPutNonBlocking(UART0_BASE, ch); //echo character
    	}
    }

    //clear the asserted interrupt
    ROM_UARTIntClear(UART0_BASE, ui32Status);
}


/*
 * Configures UART0 on the Tiva C board
 * pulled information from
 * https://sites.google.com/site/luiselectronicprojects/tutorials/tiva-tutorials/tiva-uart/basic-use-of-uart-stdio
 */
void initUART(void){

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlDelay(3);

    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlDelay(3);

    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //UART0 at 115200 baud running off of an 80MHz clock
	//makes sure that UARTprint references this UART
    UARTStdioConfig(0, BAUDRATE, CLK);

    ROM_UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), BAUDRATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //Allow interrupt-based UART interface
    //IntMasterEnable();	//Interrupts are enabled in main
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    ROM_IntPrioritySet(INT_UART0,   0x20);	// set the priority lower than the pwm
}

/*
 * toggle echo on input
 */
void uart_echo(int enable){
	echo = enable;
}

