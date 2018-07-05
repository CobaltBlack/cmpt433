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

#include "audioMixer.h"
#include "audioPlayer.h"
#include "fileutils.h"

#include "udplistener.h"

#define MSG_MAX_LEN 1500
#define PORT		12345

#define NUM_MAX_LEN 10
#define NUM_PER_NEWLINE 10

static const char* systemUptimePath = "/proc/uptime";
#define UPTIME_INFO_LEN 100

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

void UdpListener_init()
{
	// Start listener thread
	isEnabled = true;
	pthread_create(&pListenerThread, 0, &udpListen, 0);
}

void UdpListener_shutdown()
{
	isEnabled = false;
	pthread_join(pListenerThread, 0);
}

//
// Private functions
//

// Returns system uptime in seconds
static int getSystemUptime()
{
	char contents[UPTIME_INFO_LEN];
	readFromFile(systemUptimePath, contents);

	char* uptimeStr = strtok(contents, " .");
	return atoi(uptimeStr);
}

static void transmitReply(char* message)
{
	unsigned int sin_len = sizeof(sin);
	sendto( socketDescriptor,
		message, strlen(message),
		0,
		(struct sockaddr *) &sin, sin_len);
}

static void handleMessage(char* msg)
{
	char* command = strtok(msg, " \n");
	if (strcmp(command, "mode") == 0) {
		char* subCommand = strtok(NULL, " \n");
		if (!subCommand) {
			sprintf(msg, "error Unrecognized beat mode.");
		}
		else if (strcmp(subCommand, AudioPlayer_modeId_None) == 0) {
			AudioPlayer_setBeatMode(AUDIOPLAYER_MODE_NONE);
		}
		else if (strcmp(subCommand, AudioPlayer_modeId_Rock1) == 0) {
			AudioPlayer_setBeatMode(AUDIOPLAYER_MODE_ROCK1);
		}
		else if (strcmp(subCommand, AudioPlayer_modeId_Rock2) == 0) {
			AudioPlayer_setBeatMode(AUDIOPLAYER_MODE_ROCK2);
		}
		else {
			sprintf(msg, "error Unrecognized beat mode.");
		}

		char modeId[10];
		AudioPlayer_getBeatModeId(modeId);
		sprintf(msg, "mode %s", modeId);
	}

	else if (strcmp(command, "play") == 0) {
		char* subCommand = strtok(NULL, " \n");

		if (!subCommand) {
			sprintf(msg, "error Unrecognized sound.");
		}
		else if (strcmp(subCommand, "hihat") == 0) {
			AudioMixer_queueSound(&hihatSound);
		}
		else if (strcmp(subCommand, "snare") == 0) {
			AudioMixer_queueSound(&snareSound);
		}
		else if (strcmp(subCommand, "bass") == 0) {
			AudioMixer_queueSound(&bassDrumSound);
		}
		else {
			sprintf(msg, "error Unrecognized sound.");
		}

		// No reply needed when playing a sound
		msg[0] = '\0';
	}

	else if (strcmp(command, "volume") == 0) {
		char* subCommand = strtok(NULL, " \n");
		AudioMixer_setVolume(atoi(subCommand));

		sprintf(msg, "volume %d", AudioMixer_getVolume());
	}

	else if (strcmp(command, "bpm") == 0) {
		char* subCommand = strtok(NULL, " \n");
		AudioPlayer_setBpm(atoi(subCommand));

		sprintf(msg, "bpm %d", AudioPlayer_getBpm());
	}

	else if (strcmp(command, "update") == 0) {

		char modeId[10];
		AudioPlayer_getBeatModeId(modeId);

		sprintf(msg, "bpm %d;volume %d;mode %s;uptime %d",
				AudioPlayer_getBpm(),
				AudioMixer_getVolume(),
				modeId,
				getSystemUptime());
	}

	else
	{
		sprintf(msg, "error Unrecognized command.");
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

		if (strlen(message) > 1) {
			transmitReply(message);
		}

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
