/*
 * sorter.c
 *
 *  Created on: May 29, 2018
 *      Author: eric
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sorter.h"

static pthread_t pSorterThread;
static bool isEnabled = false;

static int arraySize = 100;
static pthread_mutex_t arraySizeMutex = PTHREAD_MUTEX_INITIALIZER;

static int* currentArray;
static pthread_mutex_t arrayMutex = PTHREAD_MUTEX_INITIALIZER;

static long long numArraysSorted = 0;

//
// Prototypes
//

static void* sortArrays();

//
// Public functions
//

// Begin/end the background thread which sorts random permutations.
void Sorter_startSorting()
{
	pthread_mutex_init(&arrayMutex, NULL);
	pthread_mutex_init(&arraySizeMutex, NULL);

	isEnabled = true;
	pthread_create(&pSorterThread, 0, &sortArrays, 0);
}

void Sorter_stopSorting()
{
	isEnabled = false;
	pthread_join(pSorterThread, 0);
}

// Set the size the next array to sort (don’t change current array)
void Sorter_setArraySize(int newSize)
{
	if (arraySize != newSize) {
		pthread_mutex_lock(&arraySizeMutex);
		arraySize = newSize;
		pthread_mutex_unlock(&arraySizeMutex);
	}
}

// Get the size of the array currently being sorted.
int Sorter_getArrayLength()
{
	return arraySize;
}

// Get a copy of the current (potentially partially sorted) array.
// Returns a newly allocated array and sets 'length' to be the
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
int* Sorter_getArrayData(int *length)
{
	*length = Sorter_getArrayLength();
	int *outArray = malloc(*length * sizeof(*outArray));

	pthread_mutex_lock(&arrayMutex);
	memcpy(outArray, currentArray, *length * sizeof(*outArray));
	pthread_mutex_unlock(&arrayMutex);

	return outArray;
}

int Sorter_getArrayElement(int index)
{
	return currentArray[index];
}

// Get the number of arrays which have finished being sorted.
long long Sorter_getNumberArraysSorted()
{
	return numArraysSorted;
}

//
// Private functions
//

static void randomizeArray(int* intArray, int len)
{
	pthread_mutex_lock(&arrayMutex);

	for (int i = 0; i < len; i++) {
		int randomIndex = rand() % len;
		int temp = intArray[i];
		intArray[i] = intArray[randomIndex];
		intArray[randomIndex] = temp;
	}

	pthread_mutex_unlock(&arrayMutex);
}

static void bubbleSort(int* intArray, int len)
{
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < len - i - 1; j++) {
			if (intArray[j] > intArray[j+1]) {
				// Lock array before editing
				pthread_mutex_lock(&arrayMutex);

				int temp = intArray[j];
				intArray[j] = intArray[j+1];
				intArray[j+1] = temp;

				pthread_mutex_unlock(&arrayMutex);
			}
		}
	}
}

static void* sortArrays()
{
	while (isEnabled) {

		int currentArraySize = arraySize;

		// malloc and initialize array with random permutation
		pthread_mutex_lock(&arrayMutex);
		currentArray = malloc(currentArraySize * sizeof(*currentArray));
		for (int i = 0; i < currentArraySize; i++) {
			currentArray[i] = i + 1;
		}
		pthread_mutex_unlock(&arrayMutex);

		randomizeArray(currentArray, currentArraySize);

		// Bubble sort array?!?!
		bubbleSort(currentArray, currentArraySize);

		// free array memory
		free(currentArray);

		numArraysSorted++;
	}

	return 0;
}

