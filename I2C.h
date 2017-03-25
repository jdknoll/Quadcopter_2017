/*
 * tivac_I2C.h
 *
 *  Created on: Mar 6, 2017
 *      Author: jdknoll
 */

#ifndef I2C_H_
#define I2C_H_

#include "driverlib/i2c.h"

void initI2C1(void);
uint32_t I2CReceive(uint32_t device_location, uint8_t reg_to_read);
void I2CWrite(uint32_t device_location, uint8_t reg_to_write, uint8_t info);



#endif /* I2C_H_ */
