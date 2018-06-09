/*
 * udplistener.c
 *
 *  Created on: May 30, 2018
 *      Author: eric
 */

#include <ctype.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()

#include "main.h"
#include "sorter.h"

#include "udplistener.h"

#define MSG_MAX_LEN 1500
#define PORT		12345

#define NUM_MAX_LEN 10
#define NUM_PER_NEWLINE 10

static const char* helpText =
	"Accepted command examples:\n"
	"count -- display number arrays sorted.\n"
	"get length -- display length of array currently being sorted.\n"
	"get array -- display the full array being sorted.\n"
	"get 10 -- display the tenth element of array currently being sorted.\n"
	"stop -- cause the server program to end.\n";

static pthread_t pListenerThread;
static bool isEnabled = false;

static struct sockaddr_in sin;
static int socketDescriptor;

//
// Prototype
//

static void* udpListen();

//
// Public functions
//

void Listener_initialize()
{
	// Start listener thread
	isEnabled = true;
	pthread_create(&pListenerThread, 0, &udpListen, 0);
}

void Listener_shutdown()
{
	isEnabled = false;
	pthread_join(pListenerThread, 0);
}

//
// Private functions
//

static void transmitReply(char* message)
{
	unsigned int sin_len = sizeof(sin);
	sendto( socketDescriptor,
		message, strlen(message),
		0,
		(struct sockaddr *) &sin, sin_len);
}

// Allocates an array of strings to carry the int arrays
// Each string has size of at most MSG_MAX_LEN
// Caller must free strings
static char* arrayToStrings(int* array, int len)
{
	char* outString = malloc(MSG_MAX_LEN * sizeof(*outString));
	outString[0] = '\0';

	for (int i = 0; i < len; i++)
	{
		char appendedStr[NUM_MAX_LEN];
		sprintf(appendedStr, "%d", array[i]);

		// Add comma for all except last element
		if (i != len - 1) {
			strcat(appendedStr, ", ");
		}

		// Newline per 10 numbers
		if ((i + 1) % NUM_PER_NEWLINE == 0) {
			strcat(appendedStr, "\n");
		}

		// Check outString size limit
		if ((strlen(outString) + strlen(appendedStr)) > MSG_MAX_LEN) {
			transmitReply(outString);
			outString[0] = '\0';
		}

		strcat(outString, appendedStr);
	}

	return outString;
}

static void handleMessage(char* msg)
{
	char* command = strtok(msg, " \n");
	printf("Msg received: %s\n", msg);
	if (strcmp(command, "help") == 0) {
		strcpy(msg, helpText);
	}
	else if (strcmp(command, "count") == 0) {
		sprintf(msg, "Number of array sorted = [%llu]\n", Sorter_getNumberArraysSorted());
	}
	else if (strcmp(command, "get") == 0) {
		char* subCommand = strtok(NULL, " \n");
		printf("Subcommand is %s\n", subCommand);

		if (!subCommand) {
			sprintf(msg, "Unrecognized command. Type \"help\"\n");
		}
		else if (strcmp(subCommand, "length") == 0) {
			sprintf(msg, "Current array length = [%d]\n", Sorter_getArrayLength());
		}
		else if (strcmp(subCommand, "array") == 0) {
			int arrayLength;
			int* arrayContents = Sorter_getArrayData(&arrayLength);

			// Array of strings to send back to client
			char* arrayStrings = arrayToStrings(arrayContents, arrayLength);
			free(arrayContents);

			// TODO: send only 1500 bytes of the array at a time
			sprintf(msg, "%s\n", arrayStrings);
			free(arrayStrings);
		}
		else {
			// If subCommand is not numeric, atoi defaults to value 0
			int index = atoi(subCommand);
			int arrayLength = Sorter_getArrayLength();
			if (index < 1 || arrayLength < index) {
				sprintf(msg, "Error: Acceptable range for \"get\" is [%d] to [%d]\n", 1, arrayLength);
			}
			else
			{
				// Subtract 1 because we out output to be 1-indexed
				sprintf(msg, "Value [%d] = [%d]\n", index, Sorter_getArrayElement(index-1));
			}
		}
	}
	else if (strcmp(command, "stop") == 0) {
		isEnabled = false;
		sprintf(msg, "Programming terminating\n");
		triggerShutdown();
	}
	else
	{
		sprintf(msg, "Unrecognized command. Type \"help\"\n");
	}
}

// Code referenced from demo_udpListen.c
static void* udpListen()
{
	// Buffer to hold packet data:
	char message[MSG_MAX_LEN];

	// Address
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network short

	// Create the socket for UDP
	socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port (PORT) that we specify
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

	while (isEnabled) {
		// Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		unsigned int sin_len = sizeof(sin);
		int bytesRx = recvfrom(socketDescriptor, message, MSG_MAX_LEN, 0,
				(struct sockaddr *) &sin, &sin_len);

		// Skip empty messages (has only new line character)
		if (bytesRx <= 1) {
			continue;
		}

		// Make it null terminated
		message[bytesRx] = 0;

		// Modifies message to contain the reply
		handleMessage(message);

		transmitReply(message);

		// Need to break before starting next loop and
		// entering the blocking recvfrom()
		if (!isEnabled) {
			break;
		}
	}

	// Close
	close(socketDescriptor);

	return 0;
}
