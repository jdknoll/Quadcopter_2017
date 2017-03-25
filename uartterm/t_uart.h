/*
 * uart.h
 *
 *  Created on: Mar 6, 2017
 *      Author: jdknoll
 */

#include <stdbool.h>
#include <stdint.h>

#include "../circularbuff.h"
#include "driverlib/uart.h"

#ifndef T_UART_H_
#define T_UART_H_

#define TXBUFFSZ CIRC_BUFF_SIZE		//TX circular buffer
#define RZBUFFSZ CIRC_BUFF_SIZE		//RX circular buffer
#define CLK 80000000				//UART clock
#define BAUDRATE 115200

extern circbuff_t txbuff, rxbuff;	// the tx and rx buffers will be defined in t_uart.c


void uart_fast_puts(const char *str);	//print a string to the output without interrupts
void uart_puts(char *s);				//print a string to the output UART
void uart_putc(char c);					//print a character to the output UART
int uart_haschar();						//return true if the uart buffer contains data
int uart_getc(void);					//get the char from the buffer
void uart_echo(int enable);				//echo the tx


// enable CR to LF mapping
extern int eolfix;

// buffering of STDIN/STDOUT or not
extern int buffering;

extern void UART0_Config(void);

void UARTIntHandler(void);
void initUART(void);

#endif /* T_UART_H_ */
