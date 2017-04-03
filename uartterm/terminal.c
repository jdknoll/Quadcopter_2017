//Programmer: Josiah Knoll
//Course: CMPE 320-01 Operating Systems
//Instructor: Dr. Briggs
//Homework: UNIX Shell Project

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "t_uart.h"
#include "internalprogs.h"

#include "terminal.h"
#include "../motor_pwm.h"

int parser(char* input0);

int terminal(){
    char input[MAXLINE];
    while(UARTgets(input, sizeof(input)) != NULL) { //get input

        parser(input);           //parse input
        UARTprintf("\n\r");
    }
	return 0;
}

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
    } else if(strcmp(token, "matlab_req") == 0) {
    	matlab_req();
    } else if(strcmp(token, "s") == 0) {		// set in cm
    	set_point(strtok(NULL, " "));
    } else if(strcmp(token, "p") == 0) {		// sets the proportial value
    	set_p(strtok(NULL, " "));
    } else if(strcmp(token, "i") == 0) {		// sets the integral value
    	set_i(strtok(NULL, " "));
    } else if(strcmp(token, "d") == 0) {		// sets the proportial value
    	set_d(strtok(NULL, " "));
    }  else if(strcmp(token, "w") == 0) {		// sets the windup value
    	set_w(strtok(NULL, " "));
    } else if(strcmp(token, "reset") == 0){		// goes through the initialization of the motor and sets motor back to 1950
    	arm();
    } else if(strcmp(token, "m") == 0){			// sets the motor's value - will be quickly overwritten by PID if it is on.
    	pwm_all(strtok(NULL, " "));
    } else {
    		UARTprintf("incorrect input");
    		free(input);
    		return -1;
    }
    free(input);
    return 0;
}
