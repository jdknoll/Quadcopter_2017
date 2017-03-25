//Programmer: Josiah Knoll
//Course: CMPE 320-01 Operating Systems
//Instructor: Dr. Briggs
//Homework: UNIX Shell Project

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "utils/uartstdio.h"

#include "../I2C.h"
#include "t_uart.h"

#include "tokenizer.h"
#include "parser.h"
#include "terminal.h"


int terminal(){
    char input[MAXLINE];
    while(UARTgets(input, sizeof(input)) != NULL) { //get input

        int out = parser(input);           //parse input
        if(out == -1) {
            UARTprintf("Goodbye\n");
        }
        if(out == 0) {
            UARTprintf("\n");
        }
    }
    if(feof(stdin)){
        UARTprintf("Goodbye\n");
        return 0;
    }
//    else if(ferror(stdin)) {
//        return 1;
//    }
	return 0;
}

