#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "audioMixer.h"
#include "fileutils.h"
#include "joystick.h"

static pthread_mutex_t mainMutex = PTHREAD_MUTEX_INITIALIZER;

static void initializeModules()
{
	srand(time(NULL));

	Joystick_init();
	AudioMixer_init();
}

static void shutdownModules()
{
	Joystick_shutdown();
	AudioMixer_cleanup();
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
