/*
 * pot.c
 *
 *  Created on: May 29, 2018
 *      Author: eric
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>	// for sleep()

#include "fileutils.h"
#include "sorter.h"

#include "pot.h"

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

static pthread_t pPotThread;
static bool isEnabled = false;

static void* readPotThread();

//
// Public functions
//

void Pot_initialize()
{
	// Enable the A2D Linux functionality
	writeToFile("/sys/devices/platform/bone_capemgr/slots", "BB-ADC");

	// Start thread
	isEnabled = true;
	pthread_create(&pPotThread, 0, &readPotThread, 0);
}

void Pot_shutdown()
{
	isEnabled = false;
	pthread_join(pPotThread, 0);
}

int getVoltage0Reading()
{
	// Open file
	FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
	if (!f) {
		printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
		printf("try: echo BB-ADC > /sys/devices/platform/bone_capemgr/slots\n");
		return -1;
	}

	// Get reading
	int a2dReading = 0;
	int itemsRead = fscanf(f, "%d", &a2dReading);

	// Close file
	fclose(f);

	if (itemsRead <= 0) {
		printf("ERROR: Unable to read values from voltage input file.\n");
		return -1;
	}

	return a2dReading;
}


//
// Private functions
//

#define POLL_INTERVAL_SEC 1

#define NUM_CONTROL_POINTS 10
static int pwl_a2dReadings[NUM_CONTROL_POINTS] = { 0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4100 };
static int pwl_arraySize[NUM_CONTROL_POINTS] =   { 1,  20,   60,  120,  250,  300,  500,  800, 1200, 2100 };

static int potToArraySizePWL(int voltage)
{
	if (voltage < pwl_a2dReadings[0]) {
		voltage = pwl_a2dReadings[0];
	}
	else if (voltage > pwl_a2dReadings[NUM_CONTROL_POINTS - 1]) {
		voltage = pwl_a2dReadings[NUM_CONTROL_POINTS - 1];
	}

	for (int i = 0; i < NUM_CONTROL_POINTS - 1; i++)
	{
		int reading_b = pwl_a2dReadings[i+1];
		if (reading_b >= voltage) {
			int reading_a = pwl_a2dReadings[i];

			int arraySize = (((float) (voltage - reading_a) / (float) (reading_b - reading_a)) *
					(pwl_arraySize[i+1] - pwl_arraySize[i])) + pwl_arraySize[i];

			return arraySize;
		}
	}

	// Should not get here
	return 0;
}

static void* readPotThread()
{
	while (isEnabled) {
		int voltage = getVoltage0Reading();
		int arraySize = potToArraySizePWL(voltage);
		Sorter_setArraySize(arraySize);

		// Output array size value
		printf("Array size: [%d]\n", arraySize);

		sleep(POLL_INTERVAL_SEC);
	}

	return 0;
}
