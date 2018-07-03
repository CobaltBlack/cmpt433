#include <stdio.h>

#include "fileutils.h"
#include "i2c.h"

#include "accelerometer.h"

#define I2C_DEVICE_ADDRESS 0x1C

#define CTRL_REG1 0x2A
#define CTRL_REG1_STANDBY 0x00
#define CTRL_REG1_ACTIVE 0x01

#define READINGS_SIZE 7
#define REG_XMSB 1
#define REG_XLSB 2
#define REG_YMSB 3
#define REG_YLSB 4
#define REG_ZMSB 5
#define REG_ZLSB 6

static const char* SLOTS_PATH = "/sys/devices/platform/bone_capemgr/slots";
static const char* I2C1_BUS = "BB-I2C1";

static int i2cFileDesc = 0;

void Accelerometer_init()
{
	// Enable hardware bus I2C1
	writeToFile(SLOTS_PATH, I2C1_BUS);

	// setup I2C
	i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

	// Set accelerometer register to ACTIVE
	writeI2cReg(i2cFileDesc, CTRL_REG1, CTRL_REG1_ACTIVE);
}

void Accelerometer_shutdown()
{
	// Deactivate accelerometer register
	writeI2cReg(i2cFileDesc, CTRL_REG1, CTRL_REG1_STANDBY);
}

AccelerometerOutput_t Accelerometer_getReadings()
{
	char buff[READINGS_SIZE];
	readI2cRegN(i2cFileDesc, 0x00, READINGS_SIZE, buff);

	short x = (buff[REG_XMSB] << 8) | (buff[REG_XLSB]);
	short y = (buff[REG_YMSB] << 8) | (buff[REG_YLSB]);
	short z = (buff[REG_ZMSB] << 8) | (buff[REG_ZLSB]);

	AccelerometerOutput_t res = { x, y, z };
	return res;
}
