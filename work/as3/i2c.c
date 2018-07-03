/*
 * i2c.c
 *
 *  Created on: May 30, 2018
 *      Author: eric
 *
 *  Code taken from guide I2CGuide.pdf
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c.h"

int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		return -1;
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("I2C: Unable to set I2C device to slave address.");
		return -1;
	}

	return i2cFileDesc;
}

// write to an I2C device's register
void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("I2C: Unable to write i2c register.");
		return;
	}
}

// read from an I2C device's register
unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
{
	// To read a register, must first write the address
	int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
	if (res != sizeof(regAddr)) {
		perror("I2C: Unable to write to i2c register.");
		return 0;
	}

	// Now read the value and return it
	char value = 0;
	res = read(i2cFileDesc, &value, sizeof(value));
	if (res != sizeof(value)) {
		perror("I2C: Unable to read from i2c register");
		return 0;
	}

	return value;
}

// read n bytes from an I2C device's register into a buffer
void readI2cRegN(int i2cFileDesc, unsigned char regAddr, size_t numBytes, char* outBuff)
{
	// To read a register, must first write the address
	int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
	if (res != sizeof(regAddr)) {
		perror("I2C: Unable to write to i2c register.");
		return;
	}

	// Now read the value and return it
	size_t nBytes = numBytes * sizeof(*outBuff);
	res = read(i2cFileDesc, outBuff, nBytes);
	if (res != nBytes) {
		perror("I2C: Unable to read from i2c register");
		return;
	}
}
