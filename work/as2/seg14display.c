/*
 * seg14display.c
 *
 *  Created on: May 30, 2018
 *      Author: eric
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "fileutils.h"
#include "i2c.h"

#include "seg14display.h"

static const char* EXPORT_PATH = "/sys/class/gpio/export";
static const char* LEFT_PIN = "61";
static const char* RIGHT_PIN = "44";

static pthread_t pDisplayThread;
static bool isEnabled = false;

#define I2C_DEVICE_ADDRESS 0x20

// TODO: Map digits to segment bitcodes
#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

static int i2cFileDesc = 0;

static unsigned char leftRegA = 0x00;
static unsigned char leftRegB = 0x00;
static unsigned char rightRegA = 0x00;
static unsigned char rightRegB = 0x00;

static void setPinProperty(const char* pinNum, const char* property, const char* value);
static void* runDisplayThread();

void Seg14_initialize()
{

	writeToFile(EXPORT_PATH, LEFT_PIN);
	setPinProperty(LEFT_PIN, "direction", "out");
	setPinProperty(LEFT_PIN, "value", "1");

	writeToFile(EXPORT_PATH, RIGHT_PIN);
	setPinProperty(RIGHT_PIN, "direction", "out");
	setPinProperty(RIGHT_PIN, "value", "1");

	// Enable hardware bus I2C1
	writeToFile("/sys/devices/platform/bone_capemgr/slots", "BB-I2C1");

	// setup I2C
	i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
	writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
	writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);

	// Start thread to run display
	isEnabled = true;
	pthread_create(&pDisplayThread, 0, &runDisplayThread, 0);
}

void Seg14_shutdown()
{
	setPinProperty(LEFT_PIN, "value", "0");
	setPinProperty(RIGHT_PIN, "value", "0");

	isEnabled = false;
	pthread_join(pDisplayThread, 0);
}

// Set one of the characters to a specific digit
void Seg14_setDisplay(int digitId, char digit)
{
	unsigned char regA = 0x00;
	unsigned char regB = 0x00;
	switch (digit) {
	case '0':
		regA = 0xA1;
		regB = 0x86;
		break;
	case '1':
		regA = 0x80;
		regB = 0x12;
		break;
	case '2':
		regA = 0x31;
		regB = 0x0E;
		break;
	case '3':
		regA = 0xB0;
		regB = 0x06;
		break;
	case '4':
		regA = 0x90;
		regB = 0x8A;
		break;
	case '5':
		regA = 0xB0;
		regB = 0x8C;
		break;
	case '6':
		regA = 0xB1;
		regB = 0x8C;
		break;
	case '7':
		regA = 0x04;
		regB = 0x14;
		break;
	case '8':
		regA = 0xB1;
		regB = 0x8E;
		break;
	case '9':
		regA = 0xB0;
		regB = 0x8E;
		break;
	default:
		regA = 0x00;
		regB = 0x00;
	}

	switch (digitId) {
	case SEG14_DIGIT_0:
		leftRegA = regA;
		leftRegB = regB;
		break;
	case SEG14_DIGIT_1:
		rightRegA = regA;
		rightRegB = regB;
		break;
	}
}

// Turn one display on or off
void Seg14_setEnabled(int digitId, bool isEnabled)
{
	if (digitId == SEG14_DIGIT_0) {
		setPinProperty(LEFT_PIN, "value", isEnabled ? "1" : "0");
	}
	else if (digitId == SEG14_DIGIT_1) {
		setPinProperty(RIGHT_PIN, "value", isEnabled ? "1" : "0");
	}
}

#define DISPLAY_DURATION_NS 5000000

static void* runDisplayThread()
{
	struct timespec displayDuration = {0, DISPLAY_DURATION_NS};

	// Alternate between driving the left and right digits
	while(isEnabled) {
		// Drive the left digit, then turn on the display for a short time
		writeI2cReg(i2cFileDesc, REG_OUTA, leftRegA);
		writeI2cReg(i2cFileDesc, REG_OUTB, leftRegB);

		setPinProperty(LEFT_PIN, "value", "1");
		nanosleep(&displayDuration, (struct timespec *) 0);
		setPinProperty(LEFT_PIN, "value", "0");

		// Drive the right digit, then turn on the display for a short time
		writeI2cReg(i2cFileDesc, REG_OUTA, rightRegA);
		writeI2cReg(i2cFileDesc, REG_OUTB, rightRegB);

		setPinProperty(RIGHT_PIN, "value", "1");
		nanosleep(&displayDuration, (struct timespec *) 0);
		setPinProperty(RIGHT_PIN, "value", "0");
	}

	return 0;
}

static void setPinProperty(const char* pinNum, const char* property, const char* value)
{
	char pinPath[PATH_MAX];
	sprintf(pinPath, "/sys/class/gpio/gpio%s/%s", pinNum, property);
	writeToFile(pinPath, value);
}
