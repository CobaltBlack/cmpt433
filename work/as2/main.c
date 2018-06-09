#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fileutils.h"
#include "sorter.h"
#include "pot.h"
#include "seg14display.h"
#include "udplistener.h"

#include "main.h"

static pthread_mutex_t mainMutex = PTHREAD_MUTEX_INITIALIZER;

static void initializeModules()
{
	srand(time(NULL));

	//Pot_initialize();
	//Seg14_initialize();
	Sorter_startSorting();
	Listener_initialize();
}

static void shutdownModules()
{
	Listener_shutdown();
	Sorter_stopSorting();
	//Seg14_shutdown();
	//Pot_shutdown();
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
