#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H

#include <alsa/asoundlib.h>
#include <pthread.h>

class audio_playback {	
public:
	audio_playback(void);
	~audio_playback(void);

	friend void* TimeOutThread(void* param);
public:
	int  init(char* device);
	int  deinit(void);
	int  start(void);
	int  stop(void);
	
	int  playback(char* sample, int len);
	void reopen(void);
private:
	int  initplayback(char* device);
	
private:
	snd_pcm_t* _playbackHandle;
	int _initialed;
	int _started;
	unsigned char* _audioBuffer;
	unsigned long int _sampleSize;

	pthread_t _reopen_id;	
};

#endif	// AUDIO_PLAYBACK_H
