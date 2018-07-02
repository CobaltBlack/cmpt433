#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "accelerometer.h"
#include "audioMixer.h"
#include "audioPlayer.h"
#include "fileutils.h"
#include "joystick.h"
#include "udplistener.h"
#include "zencontrol.h"

static pthread_mutex_t mainMutex = PTHREAD_MUTEX_INITIALIZER;

static void initializeModules()
{
	Joystick_init();
	Accelerometer_init();

	AudioMixer_init();
	AudioPlayer_init();

	ZenControl_init();
	UdpListener_init();
}

static void shutdownModules()
{
	UdpListener_shutdown();
	ZenControl_shutdown();

	AudioPlayer_shutdown();
	AudioMixer_cleanup();

	Accelerometer_shutdown();
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
