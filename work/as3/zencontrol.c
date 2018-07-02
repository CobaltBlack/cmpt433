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
#include "audioMixer.h"
#include "audioPlayer.h"
#include "joystick.h"

#include "zencontrol.h"

#define NS_PER_MS 1000000

#define POLL_INTERVAL_MS 10
#define DEBOUNCE_INTERVAL_MS 100

#define VOLUME_DECREMENT_AMOUNT -5
#define VOLUME_INCREMENT_AMOUNT 5
#define BPM_DECREMENT_AMOUNT -5
#define BPM_INCREMENT_AMOUNT 5


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

static void sleepMs(int ms)
{
	struct timespec reqDelayOn = {0, ms * NS_PER_MS};
	nanosleep(&reqDelayOn, (struct timespec *) 0);
}

static void* zenControl()
{
	while (isEnabled) {
		JoystickDirection joystickInput = Joystick_getInput();
		if (joystickInput != JOYSTICK_NOOPT) {
			// Up and down adjusts volume
			if (joystickInput == JOYSTICK_UP) {
				AudioMixer_adjustVolume(VOLUME_INCREMENT_AMOUNT);
			}
			else if (joystickInput == JOYSTICK_DOWN) {
				AudioMixer_adjustVolume(VOLUME_DECREMENT_AMOUNT);
			}
			// Left and right adjusts bpm
			else if (joystickInput == JOYSTICK_LEFT) {
				AudioPlayer_adjustBpm(BPM_DECREMENT_AMOUNT);
			}
			else if (joystickInput == JOYSTICK_RIGHT) {
				AudioPlayer_adjustBpm(BPM_INCREMENT_AMOUNT);
			}
			// Joystick push cycles the beatmode
			else if (joystickInput == JOYSTICK_PUSH) {
				AudioPlayer_setNextBeatMode();
			}
		}

		// TODO: Reset debounce timers for action


		// Poll inputs at an interval
		sleepMs(POLL_INTERVAL_MS);
	}

	return 0;
}
