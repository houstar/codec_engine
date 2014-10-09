#include "audio_capture.h"
#include <sys/time.h>

namespace {
	const int kSampleRate     = 16000;
	const int kAlsaBufSizeMax = 65536;
	const int kChunkSize      = 320;

}

void* readpcmthread(void* param)
{
	audio_capture* pthis = (audio_capture*)param;
	printf("read pcm thread create\n");
	
	int read_count = 0;

	while (true == pthis->_initialed) {		
		while (true == pthis->_started) {
			read_count = snd_pcm_readi(pthis->_caphandle, pthis->_audiobuffer, pthis->_samplesize);
			
			if (read_count < 0) {
				
				memset(pthis->_audiobuffer, 0, pthis->_samplesize);
				
				for (listenerList::iterator it = pthis->_pcm_listener.begin(); it != pthis->_pcm_listener.end(); ++ it)
					(*it)->OnPCM((char*)pthis->_audiobuffer, read_count * 2);
				
				snd_pcm_prepare(pthis->_caphandle);
				printf("\n\nsnd_pcm_readi: %s\n", snd_strerror(read_count));
				continue;
				
			}
			
			for (listenerList::iterator it = pthis->_pcm_listener.begin(); it != pthis->_pcm_listener.end(); ++ it)
				(*it)->OnPCM((char*)pthis->_audiobuffer, read_count * 2);						
		}
		usleep(10000);
	}
	
}

audio_capture::audio_capture()
	: _initialed(false)
	, _started(false)
	, _audiobuffer(NULL)
	, _samplesize(0)
{
}

audio_capture::~audio_capture()
{
	deinit();
}

int audio_capture::init(char* device)
{
	if (false == initcapture(device))
		return false;
	
	return true;
}

int audio_capture::deinit()
{
	if (_initialed != true)
		return false;
	
	_initialed = false;
  
	/*stop thread*/
	int ret = 0;
	pthread_join(_readpcmthrdid, (void**)&ret);
	pthread_attr_destroy(&_readpcmthrdattr);
	
	snd_pcm_wait(_caphandle, 1000);
	/* close capture */
	snd_pcm_close(_caphandle);
	
	printf("close handle: capture\n");
	
	if (NULL != _audiobuffer) {
		delete []_audiobuffer;
		_audiobuffer = NULL;
	}
	return true;
}

int audio_capture::start()
{
	if (false == _initialed)
		return false;

	if (_audiobuffer == NULL) {
		_audiobuffer = new unsigned char[_samplesize * 2]; // channels = 2 16bits
		memset(_audiobuffer, 0, _samplesize * 2);
	}
	_started = true;
	return true;
}

int audio_capture::stop()
{
	if (_started != true)
		return false;

	_started = false;
	return true;
}

void audio_capture::addListener(iaudio* _listener)
{
	_pcm_listener.push_back(_listener);
}

void audio_capture::removeListener(iaudio* _listener)
{
	listenerList::iterator it = _pcm_listener.begin();

	for (; it != _pcm_listener.end(); ++ it) {
		if ((*it) == _listener) {
			_pcm_listener.erase(it);
			break;
		}
	}
}
	
int audio_capture::initcapture(char* device)
{
	int ret = 1;
  
	ret = snd_pcm_open(&_caphandle, device, SND_PCM_STREAM_CAPTURE, 0);
	if (ret < 0) {
		printf("snd_pcm_open: %s\n", snd_strerror(ret));
		return false;
	}

	snd_pcm_hw_params_t* hw_params;
	ret = snd_pcm_hw_params_malloc(&hw_params);
	if (ret < 0) {
		printf("snd_pcm_hw_params_malloc: %s\n", snd_strerror(ret));
		return false;
	}
	
	ret = snd_pcm_hw_params_any(_caphandle, hw_params);
	if (ret < 0) {
		printf("snd_pcm_hw_params_any: %s\n", snd_strerror(ret));
		return false;
	}
  
	ret = snd_pcm_hw_params_set_access(_caphandle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (ret < 0) {
		printf("snd_pcm_hw_params_set_access: %s\n", snd_strerror(ret));
		return false;
	}
	
	ret = snd_pcm_hw_params_set_format(_caphandle, hw_params, SND_PCM_FORMAT_S16_LE);
	if (ret < 0) {
		printf("snd_pcm_hw_params_set_format: %s\n", snd_strerror(ret));
		return false;
	}
	
	ret = snd_pcm_hw_params_set_channels(_caphandle, hw_params, 1);
	if (ret < 0) {
		printf("snd_pcm_hw_params_set_channel: %s\n", snd_strerror(ret));
		return false;
	}
	
	unsigned int rate = kSampleRate;
	ret = snd_pcm_hw_params_set_rate_near(_caphandle, hw_params, &rate, 0);
	if (ret < 0) {
		printf("snd_pcm_hw_params_set_rate_near: %s\n", snd_strerror(ret));
		return false;
	}
	printf("Capture using %d Hz.\n", rate);

	unsigned int buffer_time = 1000000;
	unsigned int period_time = 20000;
	
	if (period_time > 0)
		ret = snd_pcm_hw_params_set_period_time_near(_caphandle, hw_params, &period_time, 0);

	if (buffer_time > 0)
		ret = snd_pcm_hw_params_set_buffer_time_near(_caphandle, hw_params, &buffer_time, 0);
  
	snd_pcm_uframes_t buffer_size, period_size;
	snd_pcm_hw_params_get_buffer_size(hw_params, &buffer_size);
	snd_pcm_hw_params_get_period_size(hw_params, &period_size, 0);	
  
	printf("capture buffer_size: %d period_size: %d\n", buffer_size, period_size);

	
	_samplesize = kChunkSize;//period_size / 2;
	
	unsigned int val1, val2;
	snd_pcm_hw_params_get_period_time(hw_params, &val2, 0);
	snd_pcm_hw_params_get_buffer_time(hw_params, &val1, 0);
	printf("capture buffer_time: %d period_time: %d\n", val1, val2);
	
	ret = snd_pcm_hw_params(_caphandle, hw_params);
	if (ret < 0) {
		printf("snd_pcm_hw_params: %s\n", snd_strerror(ret));
		return false;
	}
  
	snd_pcm_hw_params_free(hw_params);


	////////////////////////////////////////////////////////////
	snd_pcm_sw_params_t* sw_params;
	snd_pcm_sw_params_alloca(&sw_params);
	snd_pcm_sw_params_current(_caphandle, sw_params);
	int n = _samplesize;
	ret = snd_pcm_sw_params_set_avail_min(_caphandle, sw_params, n);
	if (ret < 0)
		return false;
	
	snd_pcm_uframes_t start_threshold = 0;
	snd_pcm_sw_params_set_start_threshold(_caphandle, sw_params, start_threshold);
	
	snd_pcm_sw_params(_caphandle, sw_params);
	
	//snd_pcm_sw_params_free(sw_params);
	
	////////////////////////////////////////////////////////////
	ret = snd_pcm_prepare(_caphandle);
	if (ret < 0) {
		printf("snd_pcm_prepare: %s\n", snd_strerror(ret));
		return false;
	}
	
	_initialed = true;
	
	pthread_attr_init(&_readpcmthrdattr);
	if (0 != pthread_create(&_readpcmthrdid, &_readpcmthrdattr,
							readpcmthread, this)) {
		perror("audio capture pthread_create");
		return false;
	}
	
	return true;
}
