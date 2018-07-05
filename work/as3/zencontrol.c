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
#include <time.h>

#include "accelerometer.h"
#include "audioMixer.h"
#include "audioPlayer.h"
#include "joystick.h"

#include "zencontrol.h"

#define NS_PER_MS 1000000

#define POLL_INTERVAL_MS 10
#define DEBOUNCE_INTERVAL_MS 200

#define VOLUME_DECREMENT_AMOUNT -5
#define VOLUME_INCREMENT_AMOUNT 5
#define BPM_DECREMENT_AMOUNT -5
#define BPM_INCREMENT_AMOUNT 5

#define ACCELEROMETER_THRESH_MS 5000
#define ACCELEROMETER_GRAVITY 15850

static pthread_t pZenControlThread;
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
	pthread_create(&pZenControlThread, 0, &zenControl, 0);
}

void ZenControl_shutdown()
{
	isEnabled = false;
	pthread_join(pZenControlThread, 0);
}

//
// Private functions
//

static void sleepMs(int ms)
{
	struct timespec reqDelayOn = {0, ms * NS_PER_MS};
	nanosleep(&reqDelayOn, (struct timespec *) 0);
}

// Code adapted from
// https://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
struct timespec diffTimespec(const struct timespec start, const struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec - start.tv_nsec) < 0) {
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	}
	else {
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}

	return temp;
}

// Returns true if different of currentTime and lastActiveTime exceeds a threshold
// If true, lastActiveTime is updated to the currentTime
static bool shouldDebounceAction(struct timespec *lastActiveTime)
{
	struct timespec currentTime;
	clock_gettime(CLOCK_MONOTONIC, &currentTime);

	struct timespec timeDiff = diffTimespec(*lastActiveTime, currentTime);
	if (timeDiff.tv_sec == 0 && timeDiff.tv_nsec < (DEBOUNCE_INTERVAL_MS * NS_PER_MS))
	{
		return false;
	}

	lastActiveTime->tv_sec = currentTime.tv_sec;
	lastActiveTime->tv_nsec = currentTime.tv_nsec;

	return true;
}

static void* zenControl()
{
	struct timespec joystickUpTimer, joystickDownTimer, joystickLeftTimer, joystickRightTimer, joystickPushTimer = {0, 0};
	struct timespec xDrumTimer, yDrumTimer, zDrumTimer = {0, 0};

	// Can only activate accelerometer "drumming" once these are reset back to true
	// Reset these to true only when x y or z returns to neutral position
	bool xReady = true;
	bool yReady = true;
	bool zReady = true;

	while (isEnabled) {
		// Process any joystick input
		JoystickDirection joystickInput = Joystick_getInput();
		if (joystickInput != JOYSTICK_NOOPT) {
			// Up and down adjusts volume
			if (joystickInput == JOYSTICK_UP) {
				if (shouldDebounceAction(&joystickUpTimer)) {
					AudioMixer_adjustVolume(VOLUME_INCREMENT_AMOUNT);
				}
			}
			else if (joystickInput == JOYSTICK_DOWN) {
				if (shouldDebounceAction(&joystickDownTimer)) {
					AudioMixer_adjustVolume(VOLUME_DECREMENT_AMOUNT);
				}
			}
			// Left and right adjusts bpm
			else if (joystickInput == JOYSTICK_LEFT) {
				if (shouldDebounceAction(&joystickLeftTimer)) {
					AudioPlayer_adjustBpm(BPM_DECREMENT_AMOUNT);
				}
			}
			else if (joystickInput == JOYSTICK_RIGHT) {
				if (shouldDebounceAction(&joystickRightTimer)) {
					AudioPlayer_adjustBpm(BPM_INCREMENT_AMOUNT);
				}
			}
			// Joystick push cycles the beatmode
			else if (joystickInput == JOYSTICK_PUSH) {
				if (shouldDebounceAction(&joystickPushTimer)) {
					AudioPlayer_setNextBeatMode();
				}
			}
		}

		// Process any accelerometer input
		AccelerometerOutput_t accelRes = Accelerometer_getReadings();
		accelRes.z = accelRes.z - ACCELEROMETER_GRAVITY;

		// Drum BBG to the left (physically) for snare
		if (xReady && accelRes.x < -ACCELEROMETER_THRESH_MS) {
			if (shouldDebounceAction(&xDrumTimer)) {
				printf("SNARE DRUM! \n");
				AudioMixer_queueSound(&snareSound);
				xReady = false;
			}
		}
		else if (accelRes.x > 0) {
			xReady = true;
		}

		// Drum BBG away for hi-hat
		if (yReady && accelRes.y > ACCELEROMETER_THRESH_MS) {
			if (shouldDebounceAction(&yDrumTimer)) {
				printf("HI-HAT! \n");
				AudioMixer_queueSound(&hihatSound);
				yReady = false;
			}
		}
		else if (accelRes.y < 0) {
			yReady = true;
		}

		// Drum BBG downwards for bass
		// Need to correct for gravity
		if (zReady && accelRes.z < -ACCELEROMETER_THRESH_MS) {
			if (shouldDebounceAction(&zDrumTimer)) {
				printf("BASS DRUM! \n");
				AudioMixer_queueSound(&bassDrumSound);
				zReady = false;
			}
		}
		else if (accelRes.z > 0) {
			zReady = true;
		}

		// Poll inputs at an interval
		sleepMs(POLL_INTERVAL_MS);
	}

	return 0;
}
