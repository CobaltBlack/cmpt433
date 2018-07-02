/*
 * udplistener.c
 *
 *  Created on: May 30, 2018
 *      Author: eric
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "accelerometer.h"
#include "joystick.h"

#include "zencontrol.h"

#define POLL_INTERVAL_MS 10
#define DEBOUNCE_INTERVAL_MS 100

static pthread_t pListenerThread;
static bool isEnabled = false;


//
// Prototype
//

static void* zenControl();

//
// Public functions
//

void ZenControl_init()
{
	// Start listener thread
	isEnabled = true;
	pthread_create(&pListenerThread, 0, &zenControl, 0);
}

void ZenControl_shutdown()
{
	isEnabled = false;
	pthread_join(pListenerThread, 0);
}

//
// Private functions
//

static void* zenControl()
{
	while (isEnabled) {
		// Poll inputs at an interval

		// Reset debounce timers for action
	}

	return 0;
}
