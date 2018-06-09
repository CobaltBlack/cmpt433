/*
 * i2c.h
 *
 *  Created on: May 30, 2018
 *      Author: eric
 *
 *  Initializes and provides interface to i2c device
 */

#ifndef I2C_H_
#define I2C_H_

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

int initI2cBus(char* bus, int address);

// write to an I2C device's register
void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);

// read from an I2C device's register
unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr);

#endif /* I2C_H_ */
