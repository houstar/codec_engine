#include "audio_playback.h"
#include <sys/time.h>
#include <unistd.h>

namespace {
	const int kSampleRate  = 16000;
	const int kChunkSize   = 320;
}

void* TimeOutThread(void* param)
{
	audio_playback* ap = (audio_playback*)param;
	sleep(2);					// wait 3s
	ap->init("default");
	ap->start();
	return NULL;
}

audio_playback::audio_playback()
	: _initialed(false)
	, _started(false)
	, _audioBuffer(NULL)
	, _sampleSize(0)
	, _reopen_id(-1)
{
}

audio_playback::~audio_playback()
{
	deinit();
}

int audio_playback::init(char* device)
{
	if (!initplayback(device)) {
		printf("audio playback initplayback error\n");
		return false;
	}
	
	_initialed = true;
	return true;
}

int audio_playback::deinit()
{
	if (!_initialed)
		return false;
	_initialed = false;

	snd_pcm_wait(_playbackHandle, 1000);
	snd_pcm_close(_playbackHandle);
	printf("close handle: playback\n");
	
	if (NULL != _audioBuffer) {
		delete []_audioBuffer;
		_audioBuffer = NULL;
	}
	 
	return true;
}

int audio_playback::start()
{
	printf("audio playback start\n");
	if (!_initialed)
		return false;

	if (_audioBuffer == NULL) {
	
		_audioBuffer = new unsigned char[_sampleSize * 2];
		memset(_audioBuffer, 0, _sampleSize * 2);
	}

	_started = true;
	return true;
}

int audio_playback::stop()
{
	printf("audio playback stop\n");
	if (!_started)
		return false;
	
	_started = false;
	return true;
}

int audio_playback::initplayback(char* device)
{
	int ret = 1;
	/* Open PCM. The last parameter of this function is the mode. */
	if ((ret = snd_pcm_open(&_playbackHandle, device, SND_PCM_STREAM_PLAYBACK, 0))< 0) {
		printf("snd_pcm_open: %s\n", snd_strerror(ret));
		return false;
	}
	
	/* Allocate the snd_pcm_hw_params_t structure on the stack. */
	snd_pcm_hw_params_t* hw_params;
	if ((ret = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
		printf("snd_pcm_hw_params_malloc: %s\n", snd_strerror(ret));
		return false;
	}
	
	/* Init hwparams with full configuration space */
	if ((ret = snd_pcm_hw_params_any(_playbackHandle, hw_params)) < 0) {
		printf("cannot initialize hardware parameter structure (%s)\n", snd_strerror(ret));
		return false;
	}
	
	/* Set access type. */
	if ((ret = snd_pcm_hw_params_set_access(_playbackHandle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		printf("snd_pcm_hw_params_set_access: %s\n", snd_strerror(ret));
		return false;
	}

	/* Set sample rate. If the exact rate is not supported */
	/* by the hardware, use nearest possible rate.         */
	unsigned int exact_rate = kSampleRate;
	if ((ret = snd_pcm_hw_params_set_rate_near(_playbackHandle, hw_params, &exact_rate, 0)) < 0) {
		printf ("cannot set sample rate (%s)\n", snd_strerror(ret));
		return false;
	}
	printf("Playback using %d Hz.\n", kSampleRate, exact_rate);
	
	/* Set sample format */
	if ((ret = snd_pcm_hw_params_set_format(_playbackHandle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		printf("snd_pcm_hw_params_set_format: %s\n", snd_strerror(ret));
		return false;
	}
	
	/* Set number of channels */
	if ((ret = snd_pcm_hw_params_set_channels(_playbackHandle, hw_params, 1)) < 0) {
		printf("cannot set channel count (%s)\n", snd_strerror(ret));
		return false;
	}
		
	unsigned int buffer_time = 1000000;
	unsigned int period_time = 20000;
	
	if (period_time > 0)
		ret = snd_pcm_hw_params_set_period_time_near(_playbackHandle, hw_params, &period_time, 0);

	if (buffer_time > 0)
		ret = snd_pcm_hw_params_set_buffer_time_near(_playbackHandle, hw_params, &buffer_time, 0);

	snd_pcm_uframes_t buffer_size = 0;
	snd_pcm_uframes_t period_size = 0;
	/*
	  snd_pcm_hw_params_set_period_size_near(_playbackHandle, hw_params, &period_size, 0);
	  snd_pcm_hw_params_set_buffer_size_near(_playbackHandle, hw_params, &buffer_size);
	*/
  
	snd_pcm_hw_params_get_buffer_size(hw_params, &buffer_size);
	snd_pcm_hw_params_get_period_size(hw_params, &period_size, 0);
	
	_sampleSize = kChunkSize;//period_size / 2;
	
	printf("playback buffer size: %d period size: %d\n", buffer_size, period_size);
	
	unsigned int buffer_val, period_val;
	snd_pcm_hw_params_get_buffer_time(hw_params, &buffer_val, 0);
	snd_pcm_hw_params_get_period_time(hw_params, &period_val, 0);
	printf("playback buffer time: %d period time: %d\n", buffer_val, period_val);
	
	/* Apply parameter settings to PCM device and prepare device  */
	if ((ret = snd_pcm_hw_params(_playbackHandle, hw_params)) < 0) {
		printf("cannot set parameters (%s)\n", snd_strerror(ret));
		return false;
	}
	
	////////////////////////////////////////////////////////////
	snd_pcm_sw_params_t* sw_params;
	snd_pcm_sw_params_alloca(&sw_params);
	snd_pcm_sw_params_current(_playbackHandle, sw_params);
	
	snd_pcm_uframes_t start_threshold = kChunkSize * 2, stop_threshold = buffer_size;
	snd_pcm_sw_params_set_start_threshold(_playbackHandle, sw_params, start_threshold);
	//snd_pcm_sw_params_set_stop_threshold(_playbackHandle, sw_params, stop_threshold);

	snd_pcm_uframes_t boundary;
	snd_pcm_sw_params_get_boundary(sw_params, &boundary);
	printf("boundary: %d\n", boundary);
	snd_pcm_sw_params_set_stop_threshold(_playbackHandle, sw_params, boundary);

	snd_pcm_sw_params_set_silence_size(_playbackHandle, sw_params, boundary);
	
	
	snd_pcm_sw_params(_playbackHandle, sw_params);

	//snd_pcm_sw_params_free(sw_params);
  
	////////////////////////////////////////////////////////////
  
	snd_pcm_hw_params_free (hw_params);

	if ((ret = snd_pcm_prepare(_playbackHandle)) < 0) {
		printf("cannot prepare audio interface for use (%s)\n", snd_strerror(ret));
		return false;
	}
	
	return true;
}


int audio_playback::playback(char* sample, int len)
{
	if (!_started)
		return false;
	memcpy(_audioBuffer, sample, len);

	snd_pcm_sframes_t avail;
	avail = snd_pcm_avail_update(_playbackHandle);

	//printf("audio_playback:: len = %d\n", len);
	
	snd_pcm_sframes_t ret = snd_pcm_writei(_playbackHandle, _audioBuffer, len / 2);

	if (ret < 0) {
		printf("snd_pcm_writei failed: %s\n", snd_strerror(ret));
		ret = snd_pcm_prepare(_playbackHandle);
		if (ret < 0) {
			printf("audio_playback: strong error: %d\n", snd_strerror(ret));
		}
			
	} else {
		//printf("snd_pcm_writei length: %d\n", ret);
	}
	return true;
}

void audio_playback::reopen()
{
	printf("audio playback reopen\n");
	stop();
	deinit();
	pthread_create(&_reopen_id, NULL, TimeOutThread, this);
}
