/*
 * tivac_I2C.c
 *
 *  Created on: Mar 6, 2017
 *      Author: jdknoll
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"

#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"

#include "I2C.h"
#include "accelerometer.h"
#include "uartterm/t_uart.h"

//Initializes I2C1 - the port that's on the output pins A6 for SCL and A7 for SDA
void initI2C1(void)
{
    //enable I2C module 0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

    //reset module
    ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);

    //enable GPIO peripheral that contains I2C 0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure the pin muxing for I2C1 functions on port B2 and B3.
    ROM_GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    ROM_GPIOPinConfigure(GPIO_PA7_I2C1SDA);

    // Select the I2C function for these pins.
    ROM_GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    ROM_GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

    // Enable and initialize the I2C1 master module.  Use the system clock for
    // the I2C1 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    ROM_I2CMasterInitExpClk(I2C1_BASE, ROM_SysCtlClockGet(), true);

    //clear I2C FIFOs
    HWREG(I2C1_BASE + I2C_O_FIFOCTL) = 80008000;
}

void I2CWrite(uint32_t device_location, uint8_t reg_to_write, uint8_t info)
{
    //specify that we are writing (a register address) to the
    //slave device
    //false if writing
	ROM_I2CMasterSlaveAddrSet(I2C1_BASE, device_location, false);

    //specify register to be write
    ROM_I2CMasterDataPut(I2C1_BASE, reg_to_write);

    //send control byte and register address byte to slave device
    //I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    ROM_I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    //wait for MCU to finish transaction
    while (ROM_I2CMasterBusy(I2C1_BASE));

    //specify that we are going to write to the slave device
    ROM_I2CMasterSlaveAddrSet(I2C1_BASE, device_location, false);

    //get the info ready to send to I2C1_BASE
    ROM_I2CMasterDataPut(I2C1_BASE, info);

    //send control byte and send information we want to send
    //specified
    ROM_I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

    //wait for MCU to finish transaction
    while (ROM_I2CMasterBusy(I2C1_BASE));
}


//read specified register on slave device
uint32_t I2CReceive(uint32_t device_location, uint8_t reg_to_read)
{
    //specify that we are writing (a register address) to the
    //slave device
    //false if writing
    ROM_I2CMasterSlaveAddrSet(I2C1_BASE, device_location, false);

    //specify register to be read
    ROM_I2CMasterDataPut(I2C1_BASE, reg_to_read);

    //send control byte and register address byte to slave device
    //I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    ROM_I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    //wait for MCU to finish transaction
    while (ROM_I2CMasterBusy(I2C1_BASE));

    //specify that we are going to read from slave device
    ROM_I2CMasterSlaveAddrSet(I2C1_BASE, device_location, true);

    //send control byte and read from the register we
    //specified
    ROM_I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

    //wait for MCU to finish transaction
    while(ROM_I2CMasterBusy(I2C1_BASE));

    //return data pulled from the specified register
    return ROM_I2CMasterDataGet(I2C1_BASE);
}
