#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "I2C.h"
#include "accelerometer.h"
#include "motor_pwm.h"
#include "uartterm/t_uart.h"

#define ACCEL_CONFIG 	0x1C
#define ACCEL_CONFIG2	0x1D

#define PWR_MGMT_1		0x6B
#define PWR_MGMT_2		0x6C

#define CONFIG			0x1A
#define FIFO				0x23
#define SMPLRT_DIV		0x19
#define USER_CTRL		0x6A

//Initializes the accelerometer with 4g or 2g range.
void InitializeAccel(){
	uint8_t c;
	I2CWrite(MPU9250_ADDRESS, PWR_MGMT_1, 0x00);	//Reset the accelerometer
	wait(100);
    I2CWrite(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);	//Turn everything on in PWR_MGMT_1
    wait(200);
    I2CWrite(MPU9250_ADDRESS, PWR_MGMT_2, 0x00);

    //Configure Gyro and Thermometer - Disable FSYNC and set thermometer and gyro bandwidth to 41
    I2CWrite(MPU9250_ADDRESS, CONFIG, 0x03);

    // use 200 Hz rate for sampling - tied to config register's value
    I2CWrite(MPU9250_ADDRESS, SMPLRT_DIV, 0x04);

    //Get current ACCEL_CONFIG register value
    c = I2CReceive(MPU9250_ADDRESS, ACCEL_CONFIG);
    c = c & ~0x18;									//clear AFS bits
    I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG, c);

    //Get the current ACCEL_CONFIG2 value
    c = I2CReceive(MPU9250_ADDRESS, ACCEL_CONFIG2);
    c = c & ~0x0F; 									// clear accel_fchoice_b and A_DLPFG bits
    c = c | 0x01;									// set the sample rate to 4khz
    I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG2, c);

    //set SBYB (bit 0) to 1
    c = I2CReceive(MPU9250_ADDRESS, USER_CTRL);
    I2CWrite(MPU9250_ADDRESS, USER_CTRL, c | 0x01);

//
//    I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG, 0xE0);	// 0b1110_0000 for enabeling the self test for the accelerometer
//    wait(1000000);
//    I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG, 0x00);	//Turn the self-test off
//    I2CWrite(MPU9250_ADDRESS, FIFO, 0x08);		// enable the fifo for the Accelerometer
//    I2CWrite(MPU9250_ADDRESS, PWR_MGMT_2, 0x01);	//Turn everything on in PWR_MGMT_2
//    wait(1000000);
}

void QueryAccel(){
	// xout = (ACCEL_XOUT_H << 8) + ACCEL_XOUT_L
    //UARTprintf("X Accelerometer output: %d\n", (I2CReceive(MPU9250_ADDRESS, 0x3B) << 8) + I2CReceive(MPU9250_ADDRESS, 0x3C));
    UARTprintf("X_HIGH: %d\n", I2CReceive(MPU9250_ADDRESS, 0x3B));
    UARTprintf("X_LOW:  %d\n", I2CReceive(MPU9250_ADDRESS, 0x3C));
    UARTprintf("Y Accelerometer output: %d\n", (I2CReceive(MPU9250_ADDRESS, 0x3D) << 8) + I2CReceive(MPU9250_ADDRESS, 0x3E));
    //UARTprintf("Z Accelerometer output: %d\n", (I2CReceive(MPU9250_ADDRESS, 0x3F) << 8) + I2CReceive(MPU9250_ADDRESS, 0x40));
}

void PrintAccelConfigReg(){
    // Print the configuration of some of the registers post-initialization
	UARTprintf("FIFO_EN: %d\n", I2CReceive(MPU9250_ADDRESS, FIFO_EN));	//FIFO_EN register
	UARTprintf("CONFIG: %d\n", I2CReceive(MPU9250_ADDRESS, CONFIG));

	UARTprintf("ACCEL_CONFIG: %d\n", I2CReceive(MPU9250_ADDRESS, ACCEL_CONFIG));
	UARTprintf("ACCEL_CONFIG 2: %d\n", I2CReceive(MPU9250_ADDRESS, ACCEL_CONFIG2));

	UARTprintf("PWR_MGMT_1: %d\n", I2CReceive(MPU9250_ADDRESS, PWR_MGMT_1));
	UARTprintf("PWR_MGMT_2: %d\n", I2CReceive(MPU9250_ADDRESS, PWR_MGMT_2));
}

void PrintAccelWhoAmI(){
	UARTprintf("%d\n", I2CReceive(MPU9250_ADDRESS, WHO_AM_I));
}
