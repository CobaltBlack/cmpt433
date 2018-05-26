#include <stdbool.h>
#include <time.h>
#include "fileutils.h"
#include "led.h"

static const char* BBG_LED0_PATH = "/sys/class/leds/beaglebone:green:usr0/";
static const char* BBG_LED1_PATH = "/sys/class/leds/beaglebone:green:usr1/";
static const char* BBG_LED2_PATH = "/sys/class/leds/beaglebone:green:usr2/";
static const char* BBG_LED3_PATH = "/sys/class/leds/beaglebone:green:usr3/";

void Led_init()
{
	char directionPath[PATH_MAX];

	// Set to no trigger, because we will be controlling them instead
	concatPath(BBG_LED0_PATH, "trigger", directionPath);
	writeToFile(directionPath, "none");

	concatPath(BBG_LED1_PATH, "trigger", directionPath);
	writeToFile(directionPath, "none");

	concatPath(BBG_LED2_PATH, "trigger", directionPath);
	writeToFile(directionPath, "none");

	concatPath(BBG_LED3_PATH, "trigger", directionPath);
	writeToFile(directionPath, "none");

	Led_setAll(false);
}

void Led_shutdown()
{
	Led_setAll(false);
}

void Led_set(int ledId, bool isOn)
{
	char directionPath[PATH_MAX];
	char* brightness = "0";
	if (isOn) {
		brightness = "1";
	}

	switch (ledId)
	{
	case BBG_LED0:
		concatPath(BBG_LED0_PATH, "brightness", directionPath);
		break;

	case BBG_LED1:
		concatPath(BBG_LED1_PATH, "brightness", directionPath);
		break;

	case BBG_LED2:
		concatPath(BBG_LED2_PATH, "brightness", directionPath);
		break;

	case BBG_LED3:
		concatPath(BBG_LED3_PATH, "brightness", directionPath);
		break;

	default:
		return;
	}

	writeToFile(directionPath, brightness);
}

void Led_setAll(bool isOn)
{
	char directionPath[PATH_MAX];
	char* brightness = "0";
	if (isOn) {
		brightness = "1";
	}

	concatPath(BBG_LED0_PATH, "brightness", directionPath);
	writeToFile(directionPath, brightness);

	concatPath(BBG_LED1_PATH, "brightness", directionPath);
	writeToFile(directionPath, brightness);

	concatPath(BBG_LED2_PATH, "brightness", directionPath);
	writeToFile(directionPath, brightness);

	concatPath(BBG_LED3_PATH, "brightness", directionPath);
	writeToFile(directionPath, brightness);
}

void Led_flashAll(long onDuration_ns, long offDuration_ns)
{
	Led_setAll(LED_ON);
	struct timespec reqDelayOn = {0, onDuration_ns};
	nanosleep(&reqDelayOn, (struct timespec *) 0);

	Led_setAll(LED_OFF);
	struct timespec reqDelayOff = {0, offDuration_ns};
	nanosleep(&reqDelayOff, (struct timespec *) 0);
}
