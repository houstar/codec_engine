#ifndef AUDIO_CAP_ENCODE_H
#define AUDIO_CAP_ENCODE_H
#include <alsa/asoundlib.h>
#include <pthread.h>
#include "iaudio.h"


class audio_capture : public iaudio {
public:
	audio_capture(void);
	~audio_capture(void);
	
	friend void* readpcmthread(void* param);
	
public:
	int  init(char* device);
	int  deinit(void);
	int  start(void);
	int  stop(void);

	void  addListener(iaudio* _listener);
	void  removeListener(iaudio* _listener);
private:
	int  initcapture(char* device);
	
private:
	snd_pcm_t* _caphandle;
	pthread_attr_t _readpcmthrdattr;
	pthread_t _readpcmthrdid;
	int  _initialed;
	int  _started;
	unsigned char* _audiobuffer;
	unsigned long int   _samplesize;

	listenerList _pcm_listener;
};

#endif	// AUDIO_CAP_ENCODE_H
