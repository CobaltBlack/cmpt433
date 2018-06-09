#include <stdio.h>
#include "fileutils.h"
#include "joystick.h"

static const char* GPIO_EXPORT_PATH = "/sys/class/gpio/export";
static const char* JOYSTICK_UP_PATH = "/sys/class/gpio/gpio26/";
static const char* JOYSTICK_DOWN_PATH = "/sys/class/gpio/gpio46/";
static const char* JOYSTICK_LEFT_PATH = "/sys/class/gpio/gpio65/";
static const char* JOYSTICK_RIGHT_PATH = "/sys/class/gpio/gpio47/";
static const char* JOYSTICK_PUSH_PATH = "/sys/class/gpio/gpio27/";

static const int PIN_VALUE_LEN = 1;

void Joystick_init()
{
	char directionPath[PATH_MAX];

	// Export relevant pins
	writeToFile(GPIO_EXPORT_PATH, "26");
	writeToFile(GPIO_EXPORT_PATH, "46");
	writeToFile(GPIO_EXPORT_PATH, "65");
	writeToFile(GPIO_EXPORT_PATH, "47");
	writeToFile(GPIO_EXPORT_PATH, "27");

	// Make each joystick pin become an input pin
	concatPath(JOYSTICK_UP_PATH, "direction", directionPath);
	writeToFile(directionPath, "in");

	concatPath(JOYSTICK_DOWN_PATH, "direction", directionPath);
	writeToFile(directionPath, "in");

	concatPath(JOYSTICK_LEFT_PATH, "direction", directionPath);
	writeToFile(directionPath, "in");

	concatPath(JOYSTICK_RIGHT_PATH, "direction", directionPath);
	writeToFile(directionPath, "in");

	concatPath(JOYSTICK_PUSH_PATH, "direction", directionPath);
	writeToFile(directionPath, "in");

	// Inverse value output so active => 1
	concatPath(JOYSTICK_UP_PATH, "active_low", directionPath);
	writeToFile(directionPath, "0");

	concatPath(JOYSTICK_DOWN_PATH, "active_low", directionPath);
	writeToFile(directionPath, "0");

	concatPath(JOYSTICK_LEFT_PATH, "active_low", directionPath);
	writeToFile(directionPath, "0");

	concatPath(JOYSTICK_RIGHT_PATH, "active_low", directionPath);
	writeToFile(directionPath, "0");

	concatPath(JOYSTICK_PUSH_PATH, "active_low", directionPath);
	writeToFile(directionPath, "0");
}

void Joystick_shutdown()
{
	return;
}

JoystickDirection Joystick_getInput()
{
	char pinValue[PIN_VALUE_LEN];
	char directionPath[PATH_MAX];

	concatPath(JOYSTICK_UP_PATH, "value", directionPath);
	if (readFromFile(directionPath, pinValue)) {
		if (pinValue[0] == '0') {
			return JOYSTICK_UP;
		}
	}

	concatPath(JOYSTICK_DOWN_PATH, "value", directionPath);
	if (readFromFile(directionPath, pinValue)) {
		if (pinValue[0] == '0') {
			return JOYSTICK_DOWN;
		}
	}

	concatPath(JOYSTICK_LEFT_PATH, "value", directionPath);
	if (readFromFile(directionPath, pinValue)) {
		if (pinValue[0] == '0') {
			return JOYSTICK_LEFT;
		}
	}

	concatPath(JOYSTICK_RIGHT_PATH, "value", directionPath);
	if (readFromFile(directionPath, pinValue)) {
		if (pinValue[0] == '0') {
			return JOYSTICK_RIGHT;
		}
	}

	concatPath(JOYSTICK_PUSH_PATH, "value", directionPath);
	if (readFromFile(directionPath, pinValue)) {
		if (pinValue[0] == '0') {
			return JOYSTICK_PUSH;
		}
	}

	return JOYSTICK_NOOPT;
}
