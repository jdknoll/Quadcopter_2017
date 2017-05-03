#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "I2C.h"
#include "accelerometer.h"
#include "motor_pwm.h"
#include "uartterm/t_uart.h"


#define DEBUG_ACCEL

#ifdef DEBUG_ACCEL
#define DBPRINTF(y,...) UARTprintf(y,__VA_ARGS__)
#else
#define DBPRINTF(...)
#endif



#define ACCEL_CONFIG 	0x1C
#define ACCEL_CONFIG2	0x1D

#define PWR_MGMT_1		0x6B
#define PWR_MGMT_2		0x6C

#define CONFIG			0x1A
#define FIFO				0x23
#define SMPLRT_DIV		0x19
#define USER_CTRL		0x6A

accel_t accel;

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
    c = c | 0x00 << 2; // Set full scale range for the accelerometer
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

//Initializes the accelerometer with 4g or 2g range.
void InitializeAccel2(){
	I2CWrite(MPU9250_ADDRESS, PWR_MGMT_1, 0x00);	//Reset the accelerometer
	wait(100);
    I2CWrite(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);	//Turn everything on in PWR_MGMT_1
    wait(200);

    //I2CWrite(MPU9250_ADDRESS, PWR_MGMT_2, 0x00);

    //Configure Gyro and Thermometer - Disable FSYNC and set thermometer and gyro bandwidth to 41
    I2CWrite(MPU9250_ADDRESS, CONFIG, 0x03);

    // use 200 Hz rate for sampling - tied to config register's value
    I2CWrite(MPU9250_ADDRESS, SMPLRT_DIV, 0x04);

	uint8_t c = I2CReceive(MPU9250_ADDRESS, GYRO_CONFIG);
	// c = c & ~0xE0; // Clear self-test bits [7:5]
	c = c & ~0x02; // Clear Fchoice bits [1:0]
	c = c & ~0x18; // Clear AFS bits [4:3]
	c = c | 0 << 3; // Set full scale range for the gyro
	// Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of
	// GYRO_CONFIG
	// c =| 0x00;
	// Write new GYRO_CONFIG value to register
	I2CWrite(MPU9250_ADDRESS, GYRO_CONFIG, c );

    //Get current ACCEL_CONFIG register value
    c = I2CReceive(MPU9250_ADDRESS, ACCEL_CONFIG);
    c = c & ~0x18;									//clear AFS bits
	c = c | 0x00 << 3; // Set full scale range for the accelerometer
    I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG, c);

    //Get the current ACCEL_CONFIG2 value
    c = I2CReceive(MPU9250_ADDRESS, ACCEL_CONFIG2);
    c = c & ~0x0F; 									// clear accel_fchoice_b and A_DLPFG bits
    //c = c | 0x04;									// turn on the accel_fchoice_b bit
    c = c | 0x01;									// set the sample rate to 1khz
    I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG2, c);

    // //set SBYB (bit 0) to 1
    // c = I2CReceive(MPU9250_ADDRESS, USER_CTRL);
    // I2CWrite(MPU9250_ADDRESS, USER_CTRL, c | 0x01);
	//
	//
    // I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG, 0xE0);	// 0b1110_0000 for enabeling the self test for the accelerometer
    // wait(1000000);
    // I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG, 0x00);	//Turn the self-test off
    // I2CWrite(MPU9250_ADDRESS, FIFO, 0x08);		// enable the fifo for the Accelerometer
    // I2CWrite(MPU9250_ADDRESS, PWR_MGMT_2, 0x01);	//Turn everything on in PWR_MGMT_2
    // wait(1000000);
}

void PrintAccel(){
	//UARTprintf("X Accelerometer output: %d\n", accel.x);
	UARTprintf("%20d                ", accel.x);
	UARTprintf("%20d\n", accel.y);
}

void QueryAccel(){
//	int32_t xval = 0;
//	int i;
//	for (i = 0; i < 10; i++){
//		xval += (int16_t)((I2CReceive(MPU9250_ADDRESS, 0x3B) << 8) + I2CReceive(MPU9250_ADDRESS, 0x3C));
//	}
//	//accel.x = (I2CReceive(MPU9250_ADDRESS, 0x3B) << 8) + I2CReceive(MPU9250_ADDRESS, 0x3C);
//	accel.x = (accel.x + xval/10)/2;



	accel.x = (I2CReceive(MPU9250_ADDRESS, 0x3B) << 8) + I2CReceive(MPU9250_ADDRESS, 0x3C);
	DBPRINTF("%d\n", accel.x);
    //accel.y = (I2CReceive(MPU9250_ADDRESS, 0x3D) << 8) + I2CReceive(MPU9250_ADDRESS, 0x3E);
    //DBPRINTF("%20d\n", accel.y);
    //accel.z = (I2CReceive(MPU9250_ADDRESS, 0x3F) << 8) + I2CReceive(MPU9250_ADDRESS, 0x40);
	PrintAccel();
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
