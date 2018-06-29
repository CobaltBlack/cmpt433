#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "audioMixer.h"
#include "audioPlayer.h"
#include "fileutils.h"
#include "joystick.h"

static pthread_mutex_t mainMutex = PTHREAD_MUTEX_INITIALIZER;

static void initializeModules()
{
	Joystick_init();
	// Accelerometer
	AudioMixer_init();
	AudioPlayer_init();
	// udpInterface
}

static void shutdownModules()
{
	AudioPlayer_shutdown();
	AudioMixer_cleanup();
	Joystick_shutdown();
}

int main()
{
	initializeModules();

	printf("main() after initialize\n");

	// mutex block after initialize, wait for shutdown to unlock
	pthread_mutex_init(&mainMutex, NULL);
	pthread_mutex_lock(&mainMutex);
	pthread_mutex_lock(&mainMutex);

	printf("main() Beginning shutdown\n");
 	shutdownModules();

	return 0;
}

void triggerShutdown() {
	pthread_mutex_unlock(&mainMutex);
	pthread_mutex_unlock(&mainMutex);
}
