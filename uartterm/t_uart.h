/*
 * uart.h
 *
 *  Created on: Mar 6, 2017
 *      Author: jdknoll
 */

#include <stdbool.h>
#include <stdint.h>

#include "driverlib/uart.h"

#ifndef T_UART_H_
#define T_UART_H_

#define TXBUFFSZ CIRC_BUFF_SIZE		//TX circular buffer
#define RZBUFFSZ CIRC_BUFF_SIZE		//RX circular buffer
#define CLK 80000000				//UART clock
#define BAUDRATE 115200

void uart_echo(int enable);				//echo the tx

extern void UART0_Config(void);

void UARTIntHandler(void);
void initUART(void);

#endif /* T_UART_H_ */
