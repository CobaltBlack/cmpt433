/*
 * audioPlayer.c
 *
 *  Created on: Jun 27, 2018
 *      Author: eric
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#include "audioMixer.h"
#include "fileutils.h"

#include "audioPlayer.h"

#define SEC_PER_MIN 60
#define MS_PER_SEC 1000
#define NS_PER_MS 1000000
#define HALF 1/2

static pthread_t pBeatThread;
static bool isEnabled = false;

static int currentBpm = 120;
static AudioPlayerBeatMode currentBeatMode = AUDIOPLAYER_MODE_ROCK1;

static const char* soundsDirectory = "beatbox-wav-files";
static const char* bassDrumFilename = "100051__menegass__gui-drum-bd-hard.wav";
static const char* hihatFilename = "100053__menegass__gui-drum-cc.wav";
static const char* snareFilename = "100059__menegass__gui-drum-snare-soft.wav";

static wavedata_t bassDrumSound, hihatSound, snareSound;

//
// Prototypes
//

static void* playBeatThread();

//
// Public functions
//

void AudioPlayer_init(void)
{
	// Load drum sounds
	char soundPath[PATH_MAX];

	concatPath(soundsDirectory, bassDrumFilename, soundPath);
	AudioMixer_readWaveFileIntoMemory(soundPath, &bassDrumSound);

	concatPath(soundsDirectory, hihatFilename, soundPath);
	AudioMixer_readWaveFileIntoMemory(soundPath, &hihatSound);

	concatPath(soundsDirectory, snareFilename, soundPath);
	AudioMixer_readWaveFileIntoMemory(soundPath, &snareSound);

	// Thread to play beat
	isEnabled = true;
	pthread_create(&pBeatThread, 0, &playBeatThread, 0);
}

void AudioPlayer_shutdown(void)
{
	isEnabled = false;
	pthread_join(pBeatThread, 0);

	// Unload drums sounds
	AudioMixer_freeWaveFileData(&bassDrumSound);
	AudioMixer_freeWaveFileData(&hihatSound);
	AudioMixer_freeWaveFileData(&snareSound);
}

AudioPlayerBeatMode AudioPlayer_getBeatMode()
{
	return currentBeatMode;
}

void AudioPlayer_setBeatMode(AudioPlayerBeatMode mode)
{
	currentBeatMode = mode;
}

void AudioPlayer_setNextBeatMode()
{
	AudioPlayerBeatMode currentMode = AudioPlayer_getBeatMode();
	if (currentMode == AUDIOPLAYER_MODE_NONE) {
		AudioPlayer_setBeatMode(AUDIOPLAYER_MODE_ROCK1);
	}
	else if (currentMode == AUDIOPLAYER_MODE_ROCK1) {
		AudioPlayer_setBeatMode(AUDIOPLAYER_MODE_ROCK2);
	}
	else if (currentMode == AUDIOPLAYER_MODE_ROCK2) {
		AudioPlayer_setBeatMode(AUDIOPLAYER_MODE_NONE);
	}
}

int AudioPlayer_getBpm()
{
	return currentBpm;
}

void AudioPlayer_setBpm(int bpm)
{
	if (bpm < MIN_BPM || MAX_BPM < bpm) {
		printf("ERROR: BPM must be between %d and %d.\n", MIN_BPM, MAX_BPM);
		return;
	}

	currentBpm = bpm;
}

void AudioPlayer_adjustBpm(int bpmDiff)
{
	int newBpm = bpmDiff + AudioPlayer_getBpm();
	if (newBpm > MAX_BPM) {
		newBpm = MAX_BPM;
	}
	else if (newBpm < MIN_BPM) {
		newBpm = MIN_BPM;
	}

	AudioPlayer_setBpm(newBpm);
}

static void sleepMs(int ms)
{
	struct timespec reqDelayOn = {0, ms * NS_PER_MS};
	nanosleep(&reqDelayOn, (struct timespec *) 0);
}

static float getBeatDurationMs()
{
	return (MS_PER_SEC * (float) SEC_PER_MIN) / (float) currentBpm;
}

static float getHalfBeatDurationMs()
{
	return getBeatDurationMs() * HALF;
}

static void playRock1Beat()
{
	AudioMixer_queueSound(&hihatSound);
	AudioMixer_queueSound(&bassDrumSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	AudioMixer_queueSound(&snareSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	AudioMixer_queueSound(&bassDrumSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	AudioMixer_queueSound(&snareSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	sleepMs(getHalfBeatDurationMs());
}

static void playRock2Beat()
{
	AudioMixer_queueSound(&hihatSound);
	AudioMixer_queueSound(&bassDrumSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	AudioMixer_queueSound(&snareSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	AudioMixer_queueSound(&bassDrumSound);
	sleepMs(getHalfBeatDurationMs() * HALF);
	AudioMixer_queueSound(&bassDrumSound);
	sleepMs(getHalfBeatDurationMs() * HALF);

	AudioMixer_queueSound(&hihatSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	AudioMixer_queueSound(&snareSound);
	sleepMs(getHalfBeatDurationMs());

	AudioMixer_queueSound(&hihatSound);
	sleepMs(getHalfBeatDurationMs());
}

static void* playBeatThread()
{
	while (isEnabled) {

		// Each loop plays 4 beats of chosen beat mode
		if (currentBeatMode == AUDIOPLAYER_MODE_ROCK1) {
			playRock1Beat();
		}
		else if (currentBeatMode == AUDIOPLAYER_MODE_ROCK2) {
			playRock2Beat();
		}
		else if (currentBeatMode == AUDIOPLAYER_MODE_NONE) {
			// Do nothing

		}
		else {
			// Unsupported mode
		}
	}

	return 0;
}
