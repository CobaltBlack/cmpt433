// Lightbouncer BareMetal program

#include "consoleUtils.h"
#include "hw_types.h"      // For HWREG(...) macro
#include <stdint.h>

// My hardware abstraction modules
#include "joystick.h"
#include "led.h"
#include "serial.h"
#include "timer.h"
#include "wdtimer.h"

// My application's modules
#include "command.h"


/******************************************************************************
 **              TIMER
 ******************************************************************************/

// Sets all the required flags when the timer is hit
static void notifyTimerIsr()
{
	Led_notifyTimerIsr();
	Joystick_notifyTimerIsr();
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
	Joystick_init();
	Serial_init();
	Timer_init();
	Watchdog_init();

	// Setup callbacks from hardware abstraction modules to application:
	Serial_setRxIsrCallback(Command_serialRxIsrCallback);
	Timer_setTimerIsrCallback(notifyTimerIsr);

	// Display startup messages to console:
	ConsoleUtilsPrintf("\nWelcome to LightBouncer! This BareMetal program is brought to you by:\n");
	ConsoleUtilsPrintf("Feng Liu - 301218282 - liufengl@sfu.ca\n");
	ConsoleUtilsPrintf("Created for: CMPT 433 Assignment 5\n\n");

	printClearResetSource();

	Command_printHelpMessage();

	// Main loop:
	while(1) {
		// Handle background processing
		Command_doBackgroundSerialWork();
		Led_doBackgroundWork();
		Joystick_doBackgroundWork();

		// Timer ISR signals intermittent background activity.
		if(Timer_isIsrFlagSet()) {
			Timer_clearIsrFlag();
			Watchdog_hit();
		}
	}
}
