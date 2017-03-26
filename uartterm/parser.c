//Programmer: Josiah Knoll
//Course: CMPE 320-01 Operating Systems
//Instructor: Dr. Briggs
//Homework: UNIX Shell Project

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

#include "parser.h"
#include "internalprogs.h"

#include "t_uart.h"

#include "terminal.h"


int parser(char* input0) {

	char *input = (char*) malloc(sizeof(input0));
	strcpy(input, input0);

	char *token;
	token = strtok(input, " ");

    if(token == NULL) {
        return 0;
    }
    if(strcmp(token, "exit") == 0) {
        return -1;
    } else if(strcmp(token, "accelx") == 0) {
    	accelx();
    } else if(strcmp(token, "printreg") == 0) {
        printreg(strtok(NULL, " "));
    } else if(strcmp(token, "printwidereg") == 0) {
        printwidereg(strtok(NULL, " "));
    } else if(strcmp(token, "testterm") == 0) {
    		testterm();
    } else if(strcmp(token, "matlab_pwm") == 0) {
    	matlab_pwm(strtok(NULL, " "), strtok(NULL, " "), strtok(NULL, " "), strtok(NULL, " "));
    } else {
    	UARTprintf("incorrect input");
    	return -1;
    }
    free(input);
    return 0;
}
