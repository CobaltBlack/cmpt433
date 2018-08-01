// Fake Typing bare metal sample application
// On the serial port, fakes

#include "consoleUtils.h"
#include "hw_types.h"      // For HWREG(...) macro
#include <stdint.h>

// My hardware abstraction modules
#include "led.h"
#include "serial.h"
#include "timer.h"
#include "wdtimer.h"

// My application's modules
#include "fakeTyper.h"




/******************************************************************************
 **              COMMANDS
 ******************************************************************************/

void printHelpMessage(void)
{
	ConsoleUtilsPrintf("\nAccepted serial commands:\n");
	ConsoleUtilsPrintf( "?   : Display this help message.\n"
						"0-9 : Set speed 0 (slow) to 9 (fast).\n"
						"a   : Select pattern A (bounce).\n"
						"b   : Select pattern B (bar).\n"
						"x   : Stop hitting the watchdog.\n");
}

/******************************************************************************
 **              SERIAL PORT HANDLING
 ******************************************************************************/
static volatile uint8_t s_rxByte = 0;
static void serialRxIsrCallback(uint8_t rxByte)
{
	s_rxByte = rxByte;
}

static void doBackgroundSerialWork(void)
{
	// TODO?
	if (s_rxByte != 0) {
		// Help
		if (s_rxByte == '?') {
			printHelpMessage();
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
		}
		else {
			ConsoleUtilsPrintf("\nUnrecognized command!\n");
			printHelpMessage();
		}

		s_rxByte = 0;
	}
}




/******************************************************************************
 **              TIMER
 ******************************************************************************/

// Sets all the required flags when the timer is hit
static void notifyTimerIsr()
{
	Led_notifyTimerIsr();
}

/******************************************************************************
 **              RESET SOURCES
 ******************************************************************************/
#define PRM_RSTST_BASE 	0x44E00F00
#define PRM_RSTST 		PRM_RSTST_BASE + 0x8

#define PRM_RSTST_GLOBAL_COLD_RST 		0
#define PRM_RSTST_GLOBAL_WARM_SW_RST	1
#define PRM_RSTST_WDT1_RST				4
#define PRM_RSTST_EXTERNAL_WARM_RST 	5
#define PRM_RSTST_ICEPICK_RST 			9

static void printClearResetSource(void)
{
	uint32_t resetReg = HWREG(PRM_RSTST);

	if ((resetReg & (1 << PRM_RSTST_GLOBAL_COLD_RST)) != 0) {
		ConsoleUtilsPrintf("Reset source (0x0) = Cold reset (Unplugged power)\n");
	}

	if ((resetReg & (1 << PRM_RSTST_GLOBAL_WARM_SW_RST)) != 0) {
		ConsoleUtilsPrintf("Reset source (0x1) = Warm software reset\n");
	}

	if ((resetReg & (1 << PRM_RSTST_WDT1_RST)) != 0) {
		ConsoleUtilsPrintf("Reset source (0x4) = Watchdog reset\n");
	}

	if ((resetReg & (1 << PRM_RSTST_EXTERNAL_WARM_RST)) != 0) {
		ConsoleUtilsPrintf("Reset source (0x5) = External warm reset (Reset button)\n");
	}

	if ((resetReg & (1 << PRM_RSTST_ICEPICK_RST)) != 0) {
		ConsoleUtilsPrintf("Reset source (0x9) = Icepick\n");
	}

	HWREG(PRM_RSTST) |= (1 << PRM_RSTST_GLOBAL_COLD_RST)
						| (1 << PRM_RSTST_GLOBAL_WARM_SW_RST)
						| (1 << PRM_RSTST_WDT1_RST)
						| (1 << PRM_RSTST_EXTERNAL_WARM_RST)
						| (1 << PRM_RSTST_ICEPICK_RST);

}

/******************************************************************************
 **              MAIN
 ******************************************************************************/
int main(void)
{
	// Initialization
	Serial_init(serialRxIsrCallback);
	Timer_init();
	Watchdog_init();
	FakeTyper_init();

	// Setup callbacks from hardware abstraction modules to application:
	Serial_setRxIsrCallback(serialRxIsrCallback);
	Timer_setTimerIsrCallback(notifyTimerIsr);

	// Display startup messages to console:
	ConsoleUtilsPrintf("\nWelcome! This BareMetal program is brought to you by:\n");
	ConsoleUtilsPrintf("Feng Liu - 301218282 - liufengl@sfu.ca\n");
	ConsoleUtilsPrintf("Created for: CMPT 433 Assignment 5\n");

	printClearResetSource();

	printHelpMessage();

	// Main loop:
	while(1) {
		// Handle background processing
		doBackgroundSerialWork();
		Led_doBackgroundWork();


		FakeTyper_doBackgroundWork();

		// Timer ISR signals intermittent background activity.
		if(Timer_isIsrFlagSet()) {
			Timer_clearIsrFlag();
			Watchdog_hit();
		}
	}
}
