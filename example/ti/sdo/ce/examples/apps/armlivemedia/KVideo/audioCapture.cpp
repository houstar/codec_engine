#include "audioCapture.h"



#ifndef MAX
#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#endif

#define ALSA_BUFFER_SIZE_MAX 65536
#define FFMIN(a, b) ((a) > (b) ? (b) : (a))

#ifndef timermsub
#define	timermsub(a, b, result) \
do { \
	(result)->tv_sec = (a)->tv_sec - (b)->tv_sec; \
	(result)->tv_nsec = (a)->tv_nsec - (b)->tv_nsec; \
	if ((result)->tv_nsec < 0) { \
		--(result)->tv_sec; \
		(result)->tv_nsec += 1000000000L; \
	} \
} while (0)
#endif

void* readPcmThread(void* param)
{
	AudioCapture* pSelf = (AudioCapture*)param;
	printf("read pcm thread create\n");
	int err;
	while (1) {
		if (pSelf->m_bExitPcmReadThread) {
			printf("read pcm proc exit\n");
			SempUtils::semp_post(&pSelf->m_exitSem);
			break;
		}
		int nRead = 0;
		size_t count = pSelf->m_chunk_size;
		int nOffset = 0;
		while (count > 0) {
			nRead = snd_pcm_readi(pSelf->_caphandle, pSelf->m_audioBuf + nOffset, count);
			if (nRead == -EAGAIN) {
				snd_pcm_wait(pSelf->_caphandle, 100);
				printf("pcm wait ********* %d\n", nRead);
			} else if (nRead == -EPIPE) { //EPIPE:32
				pSelf->xrun();
				printf("overrun occurred\n");
			} else if (nRead == -ESTRPIPE) { //ESTRPIPE: 86 Streams pipe error
				pSelf->suspend();
				printf("suspend\n");
			} else if (nRead < 0) {
				printf("pcm read error: value %d, meaning %s\n", (nRead), snd_strerror(nRead));
				if ((err = snd_pcm_prepare(pSelf->_caphandle))<0) {
					printf(("xrun: prepare error: %s"), snd_strerror(err));
				}
			}
			if (nRead > 0) {
				count -= nRead;
				nOffset += nRead * 4;
			}

			for (vector<IAudioCaptureObserver*>::iterator it = pSelf->_pcm_listener.begin();
					it != pSelf->_pcm_listener.end();
					++it) {
				(*it)->onProviderPcm((const char*)pSelf->m_audioBuf, nOffset);
			}
			if (pSelf->m_bExitPcmReadThread) {
				printf("read pcm proc exit\n");
				SempUtils::semp_post(&pSelf->m_exitSem);
				return NULL;
			}
		}
	}
	return NULL;
}

AudioCapture::AudioCapture()
: _caphandle(NULL)
, m_audioBuf(NULL)
, m_nChannels(2)
, m_nSampleBit(16)
, m_bPCMNonBlock(false)
, m_nSampleRate(44100)
, m_bInited(false)
, _readpcmthrdid(0)
, m_bExitPcmReadThread(false)
, m_chunk_bytes(0)
, m_bits_per_frame(0)
, m_monotonic(0)
, m_chunk_size(0)
{
}

AudioCapture::~AudioCapture() {
	uninit();
}

bool AudioCapture::init(int aAudioChannel, int aSampleRate,
		int aSampleBit, bool bPCMNonBlock, string strDeviceName) {
	m_nChannels = aAudioChannel;
	m_nSampleBit = aSampleBit;
	m_nSampleRate = aSampleRate;
	m_bPCMNonBlock = bPCMNonBlock;
	m_strDeviceName = strDeviceName;

	if (!setAlsaAudioParam()) {
		return false;
	}

	SempUtils::semp_init(&m_exitSem, 1);
	m_bExitPcmReadThread = false;

	m_audioBuf = new unsigned char[m_chunk_size * 2 * m_nChannels];
	
	m_bInited = true;
	return true;
}

bool AudioCapture::uninit() {
	if (!m_bInited)
		return false;
	
	m_bInited = false;
  
	/*stop thread*/
	m_bExitPcmReadThread = true;
	SempUtils::semp_pend(&m_exitSem);
	pthread_join(_readpcmthrdid, NULL);
	pthread_attr_destroy(&_readpcmthrdattr);
	
	snd_pcm_close(_caphandle);
	SempUtils::semp_deinit(&m_exitSem);
	if (NULL != m_audioBuf) {
		delete []m_audioBuf;
		m_audioBuf = NULL;
	}
	return true;
}

bool AudioCapture::start() {
	if (!m_bInited) {
		return false;
	}

	pthread_attr_init(&_readpcmthrdattr);
	if (0 != pthread_create(&_readpcmthrdid, &_readpcmthrdattr, readPcmThread, this)) {
		return false;
	}
	return true;
}

bool AudioCapture::stop() {
	return true;
}

bool AudioCapture::addListener(IAudioCaptureObserver* _listener)
{
	for (vector<IAudioCaptureObserver*>::iterator it = _pcm_listener.begin();
			it != _pcm_listener.end(); ++ it) {
		if ((*it) == _listener) {
			return false;
		}
	}
	_pcm_listener.push_back(_listener);
	return true;
}

bool AudioCapture::removeListener(IAudioCaptureObserver* _listener) {
	vector<IAudioCaptureObserver*>::iterator it = _pcm_listener.begin();

	for (; it != _pcm_listener.end(); ++ it) {
		if ((*it) == _listener) {
			_pcm_listener.erase(it);
			break;
		}
	}
}
	

bool AudioCapture::setAlsaAudioParam() {
	snd_pcm_stream_t stream = SND_PCM_STREAM_CAPTURE;

	/*
	 This structure contains information about the hardware and can be
	 used to specify the configuration to be used for the PCM stream.
	 */
	snd_pcm_hw_params_t *params;
	snd_pcm_sw_params_t *swparams;

	// snd_pcm_uframes_t xfer_align;
	 snd_pcm_uframes_t start_threshold, stop_threshold;//设置软参数主要这两个参数要设置
	 snd_pcm_hw_params_alloca(&params);
	 snd_pcm_sw_params_alloca(&swparams);
	  unsigned int period_time = 0;
	  unsigned int buffer_time = 0;
	  snd_pcm_uframes_t period_frames = 0;
	  snd_pcm_uframes_t buffer_frames = 0;
	//  int period_size; //周期长度(桢数)
	//  int chunk_byte; //周期长度(字节数)
	//  chunk_byte = period_size * bit_per_sample * hw_params.channels / 8; //计算周期长度（字节数(bytes) = 每周期的桢数 * 样本长度(bit) * 通道数 / 8 ）
	  int avail_min = -1;
	  int start_delay = 1;
	  int stop_delay = 0;
	// name of the device

	int err = 0;
	unsigned int rate;
	  int dir;

	//unsigned int sleep_min = 0;

	/*Open PCM. The last parameter of this function is the mode. */
	if (snd_pcm_open(&_caphandle, m_strDeviceName.c_str(), stream, 0) < 0) {
		printf("Could not open audio device\n");
		return false;
	}

	if (m_bPCMNonBlock) {
		err = snd_pcm_nonblock(_caphandle, SND_PCM_NONBLOCK);
	}


	/* Init hwparams with full configuration space */
	if ( (err = snd_pcm_hw_params_any(_caphandle, params)) < 0) {
		fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
				snd_strerror(err));
		return false;
	}

	/* Set access type. */
	if ((err = snd_pcm_hw_params_set_access(_caphandle, params,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
		return false;
	}

	/* Set sample format */
	if ( (err = snd_pcm_hw_params_set_format(_caphandle, params,
			SND_PCM_FORMAT_S16_LE) < 0)) {
		fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
		return false;
	}

	if ((err = snd_pcm_hw_params_set_channels(_caphandle, params,
			m_nChannels)) < 0) {
		fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror(err));
		return false;
	}

	/* Set sample rate. If the exact rate is not supported by the
	 hardware, use nearest possible rate. */
	rate = m_nSampleRate;
	if ((err = snd_pcm_hw_params_set_rate_near(_caphandle, params,
			(unsigned int *) &m_nSampleRate, 0)) < 0) {

		fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
		return false;
	}

	rate = m_nSampleRate;
	printf("pcm rate is %d\n", rate);

	if (buffer_time == 0 && buffer_frames == 0) {
		err = snd_pcm_hw_params_get_buffer_time_max(params,
							    &buffer_time, 0);
		assert(err >= 0);
		if (buffer_time > 500000)
			buffer_time = 500000;
	}
	if (period_time == 0 && period_frames == 0) {
		if (buffer_time > 0)
			period_time = buffer_time / 4;
		else
			period_frames = buffer_frames / 4;
	}
	if (period_time > 0)
		err = snd_pcm_hw_params_set_period_time_near(_caphandle, params,
							     &period_time, 0);
	else
		err = snd_pcm_hw_params_set_period_size_near(_caphandle, params,
							     &period_frames, 0);
	assert(err >= 0);
	if (buffer_time > 0) {
		err = snd_pcm_hw_params_set_buffer_time_near(_caphandle, params,
							     &buffer_time, 0);
	} else {
		err = snd_pcm_hw_params_set_buffer_size_near(_caphandle, params,
							     &buffer_frames);
	}
	unsigned long int buffer_size = 0;
	//snd_pcm_hw_params_get_buffer_size_max(params, &buffer_size);

	//err = snd_pcm_hw_params_set_buffer_size_near(m_recordHandle, params, &buffer_size);

	assert(err >= 0);

	m_monotonic = snd_pcm_hw_params_is_monotonic(params);
	err = snd_pcm_hw_params(_caphandle, params);
	if (err < 0) {
		return false;
	}
	snd_pcm_hw_params_get_period_size(params, &m_chunk_size, 0);

	snd_pcm_hw_params_get_buffer_size(params, &buffer_size);
	if (m_chunk_size == buffer_size) {
		printf("Can't use period equal to buffer size (%lu == %lu)",
		      m_chunk_size, buffer_size);
		return false;
	}

	unsigned  int val;

	snd_pcm_hw_params_get_period_time(params, &m_periodTime, 0);
	printf("period time %d us\n", m_periodTime);

	printf("period size %d frames\n", (int)m_chunk_size);

	snd_pcm_hw_params_get_buffer_time(params, &val, 0);
	printf("get buffer time %d us\n", (int)val);

	printf("buffer size %d frames\n", (int)buffer_size);

	snd_pcm_hw_params_get_periods(params, &val, 0);
	printf("get periods buffer per frame %d\n", val);
	m_periodCount = val;

	snd_pcm_sw_params_current(_caphandle, swparams);
	/*err = snd_pcm_sw_params_get_xfer_align(swparams, &xfer_align);
	if (err < 0) {
		return false;
	}
	if (sleep_min)
		xfer_align = 1;
	err = snd_pcm_sw_params_set_sleep_min(m_recordHandle, swparams, sleep_min);
	assert(err >= 0);*/

	size_t n = 0;
	if (avail_min < 0)
		n = m_chunk_size;
	else
		n = (double) rate * avail_min / 1000000;
	err = snd_pcm_sw_params_set_avail_min(_caphandle, swparams, n);

	/* round up to closest transfer boundary */
	//n = (buffer_size / xfer_align) * xfer_align;
	n = buffer_size;
	if (start_delay <= 0) {
		start_threshold = n + (double) rate * start_delay / 1000000;
	} else
		start_threshold = (double) rate * start_delay / 1000000;
	if (start_threshold < 1)
		start_threshold = 1;
	if (start_threshold > n)
		start_threshold = n;
	err = snd_pcm_sw_params_set_start_threshold(_caphandle, swparams, start_threshold);
	assert(err >= 0);
	if (stop_delay <= 0)
		stop_threshold = buffer_size + (double) rate * stop_delay / 1000000;
	else
		stop_threshold = (double) rate * stop_delay / 1000000;
	err = snd_pcm_sw_params_set_stop_threshold(_caphandle, swparams, stop_threshold);
	assert(err >= 0);
	//err = snd_pcm_sw_params_set_xfer_align(m_recordHandle, swparams, xfer_align);
	//assert(err >= 0);
	if (snd_pcm_sw_params(_caphandle, swparams) < 0) {
		return false;
	}
	return true;
}

void AudioCapture::xrun() {
	snd_pcm_status_t *status;
	int res;

	snd_pcm_status_alloca(&status);
	if ((res = snd_pcm_status(_caphandle, status))<0) {
		printf(("status error: %s"), snd_strerror(res));
		return;
	}
	if (snd_pcm_status_get_state(status) == SND_PCM_STATE_XRUN) {
		if (m_monotonic) {

			struct timespec now, diff, tstamp;
			clock_gettime(CLOCK_MONOTONIC, &now);
			snd_pcm_status_get_trigger_htstamp(status, &tstamp);
			timermsub(&now, &tstamp, &diff);
			fprintf(stderr, ("overrun!!! (at least %.3f ms long)\n"),
				diff.tv_sec * 1000 + diff.tv_nsec / 10000000.0);
		} else {
			struct timeval now, diff, tstamp;
			gettimeofday(&now, 0);
			snd_pcm_status_get_trigger_tstamp(status, &tstamp);
			timersub(&now, &tstamp, &diff);
			printf("overrun\n");
		}

		if ((res = snd_pcm_prepare(_caphandle))<0) {
			printf(("xrun: prepare error: %s"), snd_strerror(res));
			return;
		}
		return;
	} if (snd_pcm_status_get_state(status) == SND_PCM_STATE_DRAINING) {
		if ((res = snd_pcm_prepare(_caphandle))<0) {
			return;
		}
	}
	return;
}

/* I/O suspend handler */
void AudioCapture::suspend(void)
{
	int res;

	while ((res = snd_pcm_resume(_caphandle)) == -EAGAIN)
		sleep(1);
	if (res < 0) {
		if ((res = snd_pcm_prepare(_caphandle)) < 0) {
			return;
		}
	}

}

