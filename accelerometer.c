#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "utils/uartstdio.h"

#include "inc/hw_gpio.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/i2c.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"

#include "I2C.h"
#include "accelerometer.h"
#include "motor_pwm.h"
#include "uartterm/t_uart.h"

//Initializes the accelerometer with 4g or 2g range.
void InitializeAccel(){
    int enable_accel = 0b11100000; //enable self test
    //int accel_select2g = 0b00000000;
    int accel_select4g = 0b00001000; //+-2g (00), +-4g (01), +-8g (10), +-16g (11)

    I2CWrite(MPU9250_ADDRESS, 0x1C, (enable_accel+accel_select4g));
    wait(1000000);
    I2CWrite(MPU9250_ADDRESS, 0x1C, 0b00001000);	//Turn the self-test off, but keep 4g on
    I2CWrite(MPU9250_ADDRESS, 0x6C, 0b00000000);	//Turn everything on in PWR_MGMT_2
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
	UARTprintf("CONFIG: %d\n", I2CReceive(MPU9250_ADDRESS, 0x1A));

	UARTprintf("ACCEL_CONFIG: %d\n", I2CReceive(MPU9250_ADDRESS, 0x1C));
	UARTprintf("ACCEL_CONFIG 2: %d\n", I2CReceive(MPU9250_ADDRESS, 0x1D));

	UARTprintf("PWR_MGMT_1: %d\n", I2CReceive(MPU9250_ADDRESS, 0x6B));
	UARTprintf("PWR_MGMT_2: %d\n", I2CReceive(MPU9250_ADDRESS, 0x6C));
}

void PrintAccelWhoAmI(){
	UARTprintf("%d\n", I2CReceive(MPU9250_ADDRESS, WHO_AM_I));
}
