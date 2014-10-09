#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

#include <pthread.h>
#include "fifo.h"
#include "iaudio.h"

typedef struct {
	int level;
	int mix_adj;				// adjustment level for mixbuf
	int last_mix_adj;			// last adjustment level
	int tx_level;
	int tx_adj_level;
}stAudioMixLevel;

typedef struct {
	int index;
	stAudioMixLevel* level;
	fifo* buf;
	short* frame;
}stChannelInfo;

typedef std::list<stChannelInfo*>    channelList;

class audio_mixer {
public:
	virtual ~audio_mixer();

	static audio_mixer* getInstance(void);
	
	int   init(void);
	int   uninit(void);
	
	void* createChannel(bool key = false);
	void  destroy(void* handle);
	void  inputData(void* handle, char* frame_buffer, int length);

	void  addListener(iaudio* _listener);
	void  removeListener(iaudio* _listener);

	void process(void);
	
	// called by local thread
	void  mixprocess(char* frame, int len);

	void printfifostatus(void);
private:
	audio_mixer();

	int  get_level(stAudioMixLevel* plevel, short* frame_buffer, int samples_per_frame);
	void mix_frame(stAudioMixLevel* plevel, short* mix_in_buffer, int* mix_out_buffer);
	int  adjust_out_level(stAudioMixLevel* plevel, int* mix_in_buffer, short* mix_out_buffer);
	stChannelInfo* mix_init(bool key);
	void mix_deinit(stChannelInfo* plevel);
	
private:
	static audio_mixer* _audio_mixer;
	
	channelList _channels;
	int _initialed;
	int _index;
	listenerList _mix_listener;
	stChannelInfo* _key_channel;

	pthread_t _mix_thread_id;
	pthread_attr_t _mix_thread_attr;

	int _start_delete;
	
};



#endif	// AUDIO_MIXER_H
