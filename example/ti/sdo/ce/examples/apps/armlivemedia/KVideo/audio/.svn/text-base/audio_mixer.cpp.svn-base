#include "audio_mixer.h"

#include <string.h>
#include <exception>
#include <sys/time.h>
#include <stdio.h>

#include "audio_mix_linear2ulaw_table.h"

namespace {
	const int kSamplesPerFrame = 320;
	const int kSampleRate = 16000;
	
	const int kFifoBufferCount = 500; // 20ms per frame
	const int kFifoEnableSize  = 20;

	const int kDeleteFrameCount = 100; // 5s
	
	stAudioMixLevel otherMixLevel;
	stAudioMixLevel allMixLevel;
	
	short sAllMixSamplesData[kSamplesPerFrame] = {0}; // mix all channels including key channel
	short sMixSamplesData[kSamplesPerFrame] = {0};	  // mix channels except key channel
	int   iMixSamplesData[kSamplesPerFrame] = {0};
	
	
	/* These are settings to control the adaptivity of changes in the
	 * signal level of the ports, so that sudden change in signal level
	 * in the port does not cause misaligned signal (which causes noise).
	 */
#define AUDIO_NORMAL_LEVEL			128
	
#define AUDIO_ATTACK_A    (kSampleRate / kSamplesPerFrame)
#define AUDIO_ATTACK_B    1
#define AUDIO_DECAY_A	  0
#define AUDIO_DECAY_B	  1
	
#define AUDIO_SIMPLE_AGC(last, target)									\
    if (target >= last)													\
		target = (AUDIO_ATTACK_A * (last + 1) + AUDIO_ATTACK_B * target) / (AUDIO_ATTACK_A + AUDIO_ATTACK_B); \
    else																\
		target = (AUDIO_DECAY_A * last + AUDIO_DECAY_B * target) / (AUDIO_DECAY_A + AUDIO_DECAY_B)
	
#define AUDIO_MAX_LEVEL   (32767)
#define AUDIO_MIN_LEVEL   (-32768)
	
#define AUDIO_IS_OVERFLOW(s) ((s > AUDIO_MAX_LEVEL) || (s < AUDIO_MIN_LEVEL))
#define audio_mix_linear2ulaw(pcm_value)  audio_mix_linear2ulaw_table[(((short)pcm_value) >> 2) & 0x3fff]

}

audio_mixer* audio_mixer::_audio_mixer = NULL;

static void updateframe(struct timeval* start,struct timeval* now, unsigned int frame_count)
{
	int start_us = (start->tv_sec % 86400) * 1000 + start->tv_usec / 1000.0;
	
	int ideal_us = 20 * frame_count;
	int now_us = (now->tv_sec % 86400) * 1000 + now->tv_usec / 1000.0;
	
	int sl = ideal_us - (now_us - start_us);  // 
	if (sl > 0) {
		//printf("wait time = %d\n", sl);
		usleep(sl * 1000);
	}
}

audio_mixer::audio_mixer()
	: _initialed(false)
	, _index(0)
	, _key_channel(NULL)
	, _start_delete(kDeleteFrameCount)
{
}

audio_mixer::~audio_mixer()
{
	uninit();
}

audio_mixer* audio_mixer::getInstance()
{
	if (_audio_mixer == NULL)
		_audio_mixer = new audio_mixer();
	return _audio_mixer;
}

int audio_mixer::init()
{
	_initialed = true;
	_channels.clear();

	allMixLevel.level          = 0;
	allMixLevel.mix_adj        = AUDIO_NORMAL_LEVEL;
	allMixLevel.last_mix_adj   = AUDIO_NORMAL_LEVEL;
	allMixLevel.tx_level       = 0;
	allMixLevel.tx_adj_level   = AUDIO_NORMAL_LEVEL;

	otherMixLevel.level        = 0;
	otherMixLevel.mix_adj      = AUDIO_NORMAL_LEVEL;
	otherMixLevel.last_mix_adj = AUDIO_NORMAL_LEVEL;
	otherMixLevel.tx_level     = 0;
	otherMixLevel.tx_adj_level = AUDIO_NORMAL_LEVEL;

	
	
	return true;
}

int audio_mixer::uninit()
{
	_mix_listener.clear();
	
	if (!_initialed)
		return false;
	
	_initialed = false;

	/*
	  int ret = 0;
	  pthread_attr_destroy(&_mix_thread_attr);
	  pthread_join(_mix_thread_id, (void**)&ret);
	*/
	for (channelList::iterator it = _channels.begin(); it != _channels.end(); ++ it) {
		this->destroy((*it));
	}
	destroy(_key_channel);

	return true;	
}

void* audio_mixer::createChannel(bool key)
{
	if (!_initialed)
		init();
	return mix_init(key);
}

void audio_mixer::destroy(void* handle)
{
	mix_deinit((stChannelInfo*)handle);
}

void audio_mixer::inputData(void* handle, char* frame_buffer, int frame_length)
{
	if (frame_length != (kSamplesPerFrame << 1)) {
		printf("mixer::inputData() framelength = %d\n", frame_length);
		return;
	}
	stChannelInfo* channel = (stChannelInfo *)handle;

	if (channel == _key_channel) {		
		mixprocess(frame_buffer, frame_length);
		return;
	}

	if (_start_delete > 0) {
		_start_delete --;
		return;
	}
	
	if (kFalse == channel->buf->push(frame_buffer, frame_length, 0))
		printf("mixer push error %d.\n\n", frame_length);
	
}

void audio_mixer::addListener(iaudio* _listener)
{
	_mix_listener.push_back(_listener);
}

void audio_mixer::removeListener(iaudio* _listener)
{
	listenerList::iterator it = _mix_listener.begin();

	for (; it != _mix_listener.end(); ++ it) {
		if ((*it) == _listener) {
			_mix_listener.erase(it);
			break;
		}
	}
}
	
void audio_mixer::mixprocess(char* frame, int len)
{

	int length = 0;

	memset(iMixSamplesData, 0, kSamplesPerFrame * sizeof(int));
	
	for (channelList::iterator it = _channels.begin(); it != _channels.end(); ++ it) {
		
		// get the data going to mix in current loop
		if (kTrue != (*it)->buf->pop((char*)(*it)->frame, &length, 0)) {			
			// insert frame
			memset((*it)->frame, 0, kSamplesPerFrame * 2);
			// delete next frame
			(*it)->buf->disablepacket();			
		} 

		// get level
		get_level((*it)->level, (*it)->frame, kSamplesPerFrame);
		
		if ((*it)->level->level > 0) {
			mix_frame(&otherMixLevel, (*it)->frame, iMixSamplesData);
			adjust_out_level(&otherMixLevel, iMixSamplesData, sMixSamplesData);
		}
	}

	// mix key channel
	get_level(_key_channel->level, (short* )frame, kSamplesPerFrame);
	//printf("capture level: %d\n", _key_channel->level->level);
	if (_key_channel->level->level > 0) {
		mix_frame(&allMixLevel, (short* )frame, iMixSamplesData);	
		adjust_out_level(&allMixLevel, iMixSamplesData, sAllMixSamplesData);
	}

	if (!_mix_listener.empty()) {
		for (listenerList::iterator l = _mix_listener.begin(); l != _mix_listener.end(); ++ l) {
			(*l)->OnMixOut((char*)sMixSamplesData, (kSamplesPerFrame << 1), (char*)sAllMixSamplesData);
		}
	}
}

int  audio_mixer::get_level(stAudioMixLevel* plevel, short* frame_buffer, int samples_per_frame)
{
	int level = 0, j = 0;
	for (; j < samples_per_frame; ++ j)
		level += ((frame_buffer[j] > 0) ? frame_buffer[j] : -frame_buffer[j]);
	level /= samples_per_frame; 
	// convert level to 8bit complement ulaw
	plevel->level   = audio_mix_linear2ulaw(level) ^ 0xff;
	plevel->mix_adj = AUDIO_NORMAL_LEVEL;


	return level;
}

void audio_mixer::mix_frame(stAudioMixLevel* plevel, short* mix_in_buffer, int* mix_out_buffer)
{

	int k = 0, tmp_adj = 0;
	for (; k < kSamplesPerFrame; ++ k) {
		mix_out_buffer[k] += mix_in_buffer[k];

		if (AUDIO_IS_OVERFLOW(mix_out_buffer[k])) {
			tmp_adj = (AUDIO_MAX_LEVEL << 7) / mix_out_buffer[k];
			if (tmp_adj < 0)
				tmp_adj = -tmp_adj;
			if (tmp_adj < plevel->mix_adj)
				plevel->mix_adj = tmp_adj;
		} // if any overflow in the mixed signals
	}	  // looping mixing singals

}

 /* If there are sources in the mix buffer, convert the mixed samples
  * from 32bit to 16bit in the mixed samples itself. This is possible 
  * because mixed sample is 32bit.
  *
  * In addition to this process, if we need to change the level of
  * TX signal, we adjust is here too.
  */

/* Calculate signal level and adjust the signal when needed. 
 * Two adjustments performed at once: 
 * 1. user setting adjustment (tx_adj_level). 
 * 2. automatic adjustment of overflowed mixed buffer (mix_adj).
 */

/* Apply simple AGC to the mix_adj, the automatic adjust, to avoid 
 * dramatic change in the level thus causing noise because the signal 
 * is now not aligned with the signal from the previous frame.
 */
int  audio_mixer::adjust_out_level(stAudioMixLevel* plevel, int* mix_in_buffer, short* mix_out_buffer)
{
	int adj_level = 0, tx_level = 0, m = 0;
	int temp = 0;
	
	AUDIO_SIMPLE_AGC(plevel->last_mix_adj, plevel->mix_adj);
	plevel->last_mix_adj = plevel->mix_adj;
	adj_level = plevel->tx_adj_level * plevel->mix_adj;
	adj_level >>= 7;

	for (; m < kSamplesPerFrame; ++ m) {
		temp = mix_in_buffer[m];

		// adjust the level
		if (adj_level != AUDIO_NORMAL_LEVEL)
			temp = (temp * adj_level) >> 7;

		// clip the singal if it is too loud
		if (temp > AUDIO_MAX_LEVEL)
			temp = AUDIO_MAX_LEVEL;
		else if (temp < AUDIO_MIN_LEVEL)
			temp = AUDIO_MIN_LEVEL;

		// put back in the buffer
		mix_out_buffer[m] = (short)temp;

		tx_level += ((mix_out_buffer[m] >= 0) ? mix_out_buffer[m] : -mix_out_buffer[m]);		
	}

	tx_level /= kSamplesPerFrame;

	/* Convert level to 8bit complement ulaw */
	tx_level = audio_mix_linear2ulaw(tx_level) ^ 0xff;

	plevel->tx_level = tx_level;


	return plevel->tx_level;
}

stChannelInfo* audio_mixer::mix_init(bool key)
{
	stChannelInfo *channel = new stChannelInfo;

	channel->index = _index;
	_index ++;
	
	channel->level = new stAudioMixLevel;

	channel->level->level        = 0;
	channel->level->mix_adj      = AUDIO_NORMAL_LEVEL;
	channel->level->last_mix_adj = AUDIO_NORMAL_LEVEL;
	channel->level->tx_level     = 0;
	channel->level->tx_adj_level = AUDIO_NORMAL_LEVEL;

	channel->buf = new fifo;
	channel->buf->init(kFifoBufferCount);
	channel->buf->enablesize(kFifoEnableSize);

	channel->frame = new short[kSamplesPerFrame];
	memset(channel->frame, 0, kSamplesPerFrame * 2);

	if (key) {
		_key_channel = channel;
		return _key_channel;
	}

	_channels.push_back(channel);

	return channel;
}

void audio_mixer::mix_deinit(stChannelInfo* channel)
{
	for (channelList::iterator it = _channels.begin(); it != _channels.end(); ++ it) {
		if (NULL != (*it) && channel->index == (*it)->index) {
			_channels.erase(it);
			break;
		}
	}

	if (channel) {
		delete channel->level;
		channel->level = NULL;

		delete channel->buf;
		channel->buf = NULL;

		delete channel->frame;
		channel->frame = NULL;

		delete channel;
		channel = NULL;
	}
}

void audio_mixer::printfifostatus()
{
	int i = 0;
	for (channelList::iterator it = _channels.begin(); it != _channels.end(); ++ it) {
		if (NULL != (*it)) {
			printf("amixer [%d] fifo length: %d\n", ++ i, (*it)->buf->getcurlength());			
		}
	}
}
