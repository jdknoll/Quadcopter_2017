/*
 * circularbuff.c
 *
 *  Created on: Mar 20, 2017
 *      Author: jdknoll
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/interrupt.h"
#include "driverlib/rom.h"

#include "circularbuff.h"

//initialize a new circular buffer
void circbuff_init(circbuff_t *buff){
	buff->count = 0;
	buff->max_size = CIRC_BUFF_SIZE;
	buff->rd_pos = 0;
	buff->wr_pos = 0;
}

//add a character to the buffer
int circbuff_addch(circbuff_t *buff, char ch){
	if (buff->count == buff->max_size) {
		return -1;
	}

	ROM_IntMasterDisable();

	buff->buff[buff->wr_pos++] = ch;
	buff->count++;
	buff->wr_pos %= buff->max_size;

	ROM_IntMasterEnable();

	return 0;
}

//get a character from the buffer
int circbuff_getch(circbuff_t *buff){
	if(buff -> count == 0){
		return -1;
	}

	ROM_IntMasterDisable();

	char ch = buff->buff[buff->rd_pos++];
	buff->count--;
	buff->rd_pos %= buff->max_size;

	ROM_IntMasterEnable();

	return ch;
}

//return true if the buffer is full
int circbuff_isfull(circbuff_t *buff){
	return (buff->count == buff->max_size);
}

//return true if the buffer is empty
int circbuff_isempty(circbuff_t *buff){
	return (buff->count == 0);
}

//return true if the buffer contains data
int circbuff_hasdata(circbuff_t *buff){
	return (buff->count > 0);
}

//return true if the buffer is almsot full
int circbuff_almostfull(circbuff_t *buff){
	return (buff->max_size - buff->count) < 10;
}
