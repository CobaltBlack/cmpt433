/*
 * audioPlayer.h
 *
 *  Created on: Jun 27, 2018
 *      Author: eric
 *
 *      Plays a continuous beat
 *      Configure BPM
 */

#ifndef AUDIOPLAYER_H_
#define AUDIOPLAYER_H_

typedef enum
{
	AUDIOPLAYER_MODE_NONE,
	AUDIOPLAYER_MODE_ROCK1,
	AUDIOPLAYER_MODE_ROCK2,
} AudioPlayerBeatMode;

void AudioPlayer_init(void);
void AudioPlayer_shutdown(void);

AudioPlayerBeatMode AudioPlayer_getBeatMode();
void AudioPlayer_setBeatMode(AudioPlayerBeatMode mode);

int AudioPlayer_getBpm();
void AudioPlayer_setBpm(int bpm);

#endif /* AUDIOPLAYER_H_ */
