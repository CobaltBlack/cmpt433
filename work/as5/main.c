// Fake Typing bare metal sample application
// On the serial port, fakes

#include "consoleUtils.h"
#include <stdint.h>

// My hardware abstraction modules
#include "serial.h"
#include "timer.h"
#include "wdtimer.h"

// My application's modules
#include "fakeTyper.h"


/******************************************************************************
 **              SERIAL PORT HANDLING
 ******************************************************************************/
static volatile uint8_t s_rxByte = 0;
static void serialRxIsrCallback(uint8_t rxByte) {
	s_rxByte = rxByte;
}

static void doBackgroundSerialWork(void)
{
	// TODO?
}




/******************************************************************************
 **              RESET SOURCES
 ******************************************************************************/
#define RESET_SOURCE_REG 0x44E00F00


static void PrintClearResetSource(void) {
	HWREG()
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
	Timer_setTimerIsrCallback(FakeTyper_notifyOnTimeIsr);

	// Display startup messages to console:
	ConsoleUtilsPrintf("\nWelcome! This BareMetal program is brought to you by:\n");
	ConsoleUtilsPrintf("Feng Liu - 301218282 - liufengl@sfu.ca\n");
	ConsoleUtilsPrintf("Created for: CMPT 433 Assignment 5\n");

	PrintClearResetSource();

	ConsoleUtilsPrintf("\nAccepted serial commands:\n");
	ConsoleUtilsPrintf("\n");

	// Main loop:
	while(1) {
		// Handle background processing
		doBackgroundSerialWork();
		FakeTyper_doBackgroundWork();

		// Timer ISR signals intermittent background activity.
		if(Timer_isIsrFlagSet()) {
			Timer_clearIsrFlag();
			Watchdog_hit();
		}
	}
}
