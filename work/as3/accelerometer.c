#include <stdio.h>
#include "fileutils.h"
#include "joystick.h"

//static const char* GPIO_EXPORT_PATH = "/sys/class/gpio/export";
//static const char* JOYSTICK_UP_PATH = "/sys/class/gpio/gpio26/";
//static const char* JOYSTICK_DOWN_PATH = "/sys/class/gpio/gpio46/";
//static const char* JOYSTICK_LEFT_PATH = "/sys/class/gpio/gpio65/";
//static const char* JOYSTICK_RIGHT_PATH = "/sys/class/gpio/gpio47/";
//static const char* JOYSTICK_PUSH_PATH = "/sys/class/gpio/gpio27/";
//
//static const char* DIRECTION_FILE = "direction";
//static const char* DIRECTION_IN = "IN";
//
//static const char* ACTIVE_LOW_FILE = "active_low";
//static const char* ACTIVE_LOW_0 = "0";
//
//static const char* VALUE_FILE = "value";
//static const char PIN_VALUE_0 = '0';
//static const int PIN_VALUE_LEN = 1;

void Accelerometer_init()
{
//	char directionPath[PATH_MAX];
//
//	// Export relevant pins
//	writeToFile(GPIO_EXPORT_PATH, "26");
//	writeToFile(GPIO_EXPORT_PATH, "46");
//	writeToFile(GPIO_EXPORT_PATH, "65");
//	writeToFile(GPIO_EXPORT_PATH, "47");
//	writeToFile(GPIO_EXPORT_PATH, "27");
//
//	// Make each joystick pin become an input pin
//	concatPath(JOYSTICK_UP_PATH, DIRECTION_FILE, directionPath);
//	writeToFile(directionPath, DIRECTION_IN);
//
//	concatPath(JOYSTICK_DOWN_PATH, DIRECTION_FILE, directionPath);
//	writeToFile(directionPath, DIRECTION_IN);
//
//	concatPath(JOYSTICK_LEFT_PATH, DIRECTION_FILE, directionPath);
//	writeToFile(directionPath, DIRECTION_IN);
//
//	concatPath(JOYSTICK_RIGHT_PATH, DIRECTION_FILE, directionPath);
//	writeToFile(directionPath, DIRECTION_IN);
//
//	concatPath(JOYSTICK_PUSH_PATH, DIRECTION_FILE, directionPath);
//	writeToFile(directionPath, DIRECTION_IN);
//
//	// Inverse value output so active => 1
//	concatPath(JOYSTICK_UP_PATH, ACTIVE_LOW_FILE, directionPath);
//	writeToFile(directionPath, ACTIVE_LOW_0);
//
//	concatPath(JOYSTICK_DOWN_PATH, ACTIVE_LOW_FILE, directionPath);
//	writeToFile(directionPath, ACTIVE_LOW_0);
//
//	concatPath(JOYSTICK_LEFT_PATH, ACTIVE_LOW_FILE, directionPath);
//	writeToFile(directionPath, ACTIVE_LOW_0);
//
//	concatPath(JOYSTICK_RIGHT_PATH, ACTIVE_LOW_FILE, directionPath);
//	writeToFile(directionPath, ACTIVE_LOW_0);
//
//	concatPath(JOYSTICK_PUSH_PATH, ACTIVE_LOW_FILE, directionPath);
//	writeToFile(directionPath, ACTIVE_LOW_0);
}

void Accelerometer_shutdown()
{
	return;
}
