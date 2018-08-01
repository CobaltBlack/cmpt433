/*
 * command.c
 *
 *  Created on: Aug 1, 2018
 *      Author: eric
 */

#include "consoleUtils.h"
#include <stdint.h>

#include "led.h"
#include "wdtimer.h"

#include "command.h"

void Command_printHelpMessage(void)
{
	ConsoleUtilsPrintf("\nAccepted serial commands:\n");
	ConsoleUtilsPrintf( "?   : Display this help message.\n"
						"0-9 : Set speed 0 (slow) to 9 (fast).\n"
						"a   : Select pattern A (bounce).\n"
						"b   : Select pattern B (bar).\n"
						"x   : Stop hitting the watchdog.\n");
}

static volatile uint8_t s_rxByte = 0;

void Command_serialRxIsrCallback(uint8_t rxByte)
{
	s_rxByte = rxByte;
}

void Command_doBackgroundSerialWork(void)
{
	if (s_rxByte != 0) {
		// Help
		if (s_rxByte == '?') {
			Command_printHelpMessage();
		}
		// Speed
		else if ('0' <= s_rxByte && s_rxByte <= '9') {
			Led_setSpeed(s_rxByte);
		}
		// Bounce pattern
		else if (s_rxByte == 'a') {
			Led_setMode(LED_MODE_BOUNCING);
		}
		// Bar pattern
		else if (s_rxByte == 'b') {
			Led_setMode(LED_MODE_BAR);
		}
		// Stop hitting watchdog
		else if (s_rxByte == 'x') {
			Watchdog_disable();
			ConsoleUtilsPrintf("\nNo longer hitting the watchdog.\n");
		}
		else {
			ConsoleUtilsPrintf("\nUnrecognized command!\n");
			Command_printHelpMessage();
		}

		s_rxByte = 0;
	}
}

