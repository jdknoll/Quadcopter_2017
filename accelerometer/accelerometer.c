#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../I2C.h"
#include "accelerometer.h"
#include "../motor_pwm.h"
#include "../uartterm/t_uart.h"

//Read and check WHO_AM_I
//

void AccelSelfTest(){
	uint8_t FS = 0;

	I2CWrite(MPU9250_ADDRESS, SMPLRT_DIV, 0x00);		//Set the sample rate to 1kHz
	I2CWrite(MPU9250_ADDRESS, CONFIG, 0x02);			// Set gyro sample rate to 1 kHz and DLPF to 92 Hz
	I2CWrite(MPU9250_ADDRESS, GYRO_CONFIG, 1<<FS);		// Set full scale range for the gyro to 250 dps
	I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG2, 0x02);		// Set accelerometer rate to 1 kHz and bandwidth to 92 Hz
	I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG, 1<<FS);	// Set full scale range for the accelerometer to 2 g
}

//Initializes the accelerometer with 4g or 2g range.
void InitializeAccel(){
    int enable_accel = 0b11100000; //enable self test
    //int accel_select2g = 0b00000000;
    int accel_select4g = 0b00001000; //+-2g (00), +-4g (01), +-8g (10), +-16g (11)
    I2CWrite(MPU9250_ADDRESS, 0x1C, (enable_accel+accel_select4g));
    wait(1000000);
    I2CWrite(MPU9250_ADDRESS, 0x1C, 0b00001000);	//Turn the self-test off, but keep 4g on
    I2CWrite(MPU9250_ADDRESS, 0x6C, 0b00000000);	//Turn everything on in PWR_MGMT_2



//	uint8_t c;
//	I2CWrite(MPU9250_ADDRESS, PWR_MGMT_1, 0x00);	//Reset the accelerometer
//	wait(100);
//    I2CWrite(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);	//Turn everything on in PWR_MGMT_1
//    wait(200);
//    I2CWrite(MPU9250_ADDRESS, PWR_MGMT_2, 0x00);
//
//    //Configure Gyro and Thermometer - Disable FSYNC and set thermometer and gyro bandwidth to 41
//    I2CWrite(MPU9250_ADDRESS, CONFIG, 0x03);
//
//    // use 200 Hz rate for sampling - tied to config register's value
//    I2CWrite(MPU9250_ADDRESS, SMPLRT_DIV, 0x04);
//
//    //Get current ACCEL_CONFIG register value
//    c = I2CReceive(MPU9250_ADDRESS, ACCEL_CONFIG);
//    c = c & ~0x18;									//clear AFS bits
//    I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG, c);
//
//    //Get the current ACCEL_CONFIG2 value
//    c = I2CReceive(MPU9250_ADDRESS, ACCEL_CONFIG2);
//    c = c & ~0x0F; 									// clear accel_fchoice_b and A_DLPFG bits
//    c = c | 0x01;									// set the sample rate to 4khz
//    I2CWrite(MPU9250_ADDRESS, ACCEL_CONFIG2, c);
//
//    //set SBYB (bit 0) to 1
//    c = I2CReceive(MPU9250_ADDRESS, USER_CTRL);
//    I2CWrite(MPU9250_ADDRESS, USER_CTRL, c | 0x01);

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
    UARTprintf("X Accelerometer output: %d\n", (I2CReceive(MPU9250_ADDRESS, 0x3B) << 8) + I2CReceive(MPU9250_ADDRESS, 0x3C));
    UARTprintf("Y Accelerometer output: %d\n", (I2CReceive(MPU9250_ADDRESS, 0x3D) << 8) + I2CReceive(MPU9250_ADDRESS, 0x3E));
    UARTprintf("Z Accelerometer output: %d\n", (I2CReceive(MPU9250_ADDRESS, 0x3F) << 8) + I2CReceive(MPU9250_ADDRESS, 0x40));
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
