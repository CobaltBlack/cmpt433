/*
 * led.c
 *
 *  Created on: Aug 1, 2018
 *      Author: eric
 */

#include <stdint.h>

#include "consoleUtils.h"
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"      // For HWREG(...) macro
#include "watchdog.h"

#include "led.h"

static volatile _Bool s_isTimerHit = false;
static enum Led_Mode s_mode = LED_MODE_BOUNCING;

#define MIN_SPEED '0'
#define MAX_SPEED '9'
#define SPEED_FACTOR_EXPONENT 9

static uint32_t s_elapsedCounter = 0;	// Used to count elapsed time between LED updates
static uint32_t s_speed = 5;	// 0-9 speed settings
static uint32_t s_speedFactor = 2;	// Number of timer ISR notifies needed before updating LEDs

static uint32_t s_current_n = 0;	// Used to track state of LED
static _Bool s_is_n_increasing = true;	// Used to track state of LED

/*****************************************************************************
 **                INTERNAL MACRO DEFINITIONS
 *****************************************************************************/
#define LED_GPIO_BASE           (SOC_GPIO_1_REGS)
#define LED0_PIN (21)
#define LED1_PIN (22)
#define LED2_PIN (23)
#define LED3_PIN (24)

#define LED_MASK ((1<<LED0_PIN) | (1<<LED1_PIN) | (1<<LED2_PIN) | (1<<LED3_PIN))

//#define DELAY_TIME 0x4000000		// Delay with MMU enabled
#define DELAY_TIME 0x40000		// Delay witouth MMU and cache


/*****************************************************************************
 **                LED CONTROL FUNCTIONS
 *****************************************************************************/
static void cycleN(void)
{
	switch (s_current_n) {
	case 0:
		s_current_n = 1;
		s_is_n_increasing = true;
		break;
	case 1:
		s_current_n = s_is_n_increasing ? 2 : 0;
		break;
	case 2:
		s_current_n = s_is_n_increasing ? 3 : 1;
		break;
	case 3:
		s_current_n = 2;
		s_is_n_increasing = false;
		break;
	default:
		// For whatever reason if N is not within expected range...
		s_current_n = 0;
	}
}

static void driveLedsBouncingMode(void)
{
	// Clear all the LEDs:
	HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = LED_MASK;
	uint32_t ledsToTurnOn = 0;

	switch(s_current_n) {
	case 0:
		ledsToTurnOn |= (1 << LED0_PIN);
		break;
	case 1:
		ledsToTurnOn |= (1 << LED1_PIN);
		break;
	case 2:
		ledsToTurnOn |= (1 << LED2_PIN);
		break;
	case 3:
		ledsToTurnOn |= (1 << LED3_PIN);
		break;
	}

	HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = ledsToTurnOn;
}

static void driveLedsBarMode(void)
{
	// Clear all the LEDs:
	HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = LED_MASK;
	uint32_t ledsToTurnOn = 0;

	switch(s_current_n) {
	case 0:
		ledsToTurnOn |= (1 << LED0_PIN);
	case 1:
		ledsToTurnOn |= (1 << LED1_PIN);
	case 2:
		ledsToTurnOn |= (1 << LED2_PIN);
	case 3:
		ledsToTurnOn |= (1 << LED3_PIN);
		break;
	}

	HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = ledsToTurnOn;
}




/*****************************************************************************
 **                PUBLIC FUNCTIONS
 *****************************************************************************/
void Led_init(void)
{
	/* Enabling functional clocks for GPIO1 instance. */
	GPIO1ModuleClkConfig();

	/* Selecting GPIO1[23] pin for use. */
	//GPIO1Pin23PinMuxSetup();

	/* Enabling the GPIO module. */
	GPIOModuleEnable(LED_GPIO_BASE);

	/* Resetting the GPIO module. */
	GPIOModuleReset(LED_GPIO_BASE);

	/* Setting the GPIO pin as an output pin. */
	GPIODirModeSet(LED_GPIO_BASE,
			LED0_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED1_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED2_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED3_PIN,
			GPIO_DIR_OUTPUT);
}


void Led_notifyTimerIsr(void)
{
	s_isTimerHit = true;
}

void Led_doBackgroundWork(void)
{
	// Timer should hit every 10ms
	if (s_isTimerHit) {
		s_isTimerHit = false;

		s_elapsedCounter++;
		if (s_elapsedCounter < s_speedFactor) {
			return;
		}

		s_elapsedCounter = 0;

		// TODO:
		if (s_mode == LED_MODE_BOUNCING) {
			driveLedsBouncingMode();
		}
		else if (s_mode == LED_MODE_BAR) {
			driveLedsBarMode();
		}

		cycleN();
	}
}

uint32_t Led_getSpeed(void)
{
	return s_speed;
}

void Led_setSpeed(char newSpeed)
{
	if (newSpeed == s_speed) {
		return;
	}

	if (newSpeed < MIN_SPEED || MAX_SPEED < newSpeed) {
		return;
	}

	uint32_t newSpeedInt = newSpeed - MIN_SPEED;	// Convert ASCII to int

	ConsoleUtilsPrintf("\nSetting LED speed to %d\n", newSpeedInt);

	s_speed = newSpeedInt;
	s_speedFactor = (1 << (SPEED_FACTOR_EXPONENT - newSpeedInt));	// equivalent to 2 ^ (9 - newSpeed)
}

void Led_setMode(enum Led_Mode newMode)
{
	s_mode = newMode;
	if (newMode == LED_MODE_BOUNCING) {
		ConsoleUtilsPrintf("\nChanging to bounce mode.\n");
	}
	else if (newMode == LED_MODE_BAR) {
		ConsoleUtilsPrintf("\nChanging to bar mode.\n");
	}
	else {
		ConsoleUtilsPrintf("\nChanging to unknown mode.\n");
	}
}

void Led_toggleMode(void)
{
	if (s_mode == LED_MODE_BOUNCING) {
		s_mode = LED_MODE_BAR;
		ConsoleUtilsPrintf("\nChanging to bounce mode.\n");
	}
	else if (s_mode == LED_MODE_BAR) {
		s_mode = LED_MODE_BOUNCING;
		ConsoleUtilsPrintf("\nChanging to bar mode.\n");
	}
}
