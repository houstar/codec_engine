#include "audioEncodeFAAC.h"
#include <time.h>
#include <sys/time.h>

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

static void* readPcmProc(void* pParam) {
	AudioEncodeFAAC *pSelf = (AudioEncodeFAAC*) pParam;
	if (NULL != pSelf) {
		pSelf->onReadPcm();
	}
	pthread_exit(NULL);
	return NULL;
}

void AudioEncodeFAAC::onReadPcm() {
    int err = 0;
	while (1) {
		if (m_bExitPcmReadThread) {
			printf("read pcm proc exit\n");
			SempUtils::semp_post(&m_exitReadSem);
			break;
		}

		int nRead = 0;
		size_t count = m_chunk_size;
		int nOffset = 0;
		while (count > 0) {
			nRead = snd_pcm_readi(m_recordHandle, m_audioBuf + nOffset, count);
			if (nRead == -EAGAIN) {
				snd_pcm_wait(m_recordHandle, 100);
				printf("pcm wait ********* %d\n", nRead);
			} else if (nRead == -EPIPE) { //EPIPE:32
				xrun();
				printf("overrun occurred\n");
			} else if (nRead == -ESTRPIPE) { //ESTRPIPE: 86 Streams pipe error
				suspend();
				printf("suspend\n");
			} else if (nRead < 0) {
				printf("pcm read error: value %d, meaning %s\n", (nRead), snd_strerror(nRead));
				if ((err = snd_pcm_prepare(m_recordHandle))<0) {
					printf(("xrun: prepare error: %s"), snd_strerror(err));
				}
			}
			if (nRead > 0) {
				count -= nRead;
				nOffset += nRead * 4;
			}

			if (m_bExitPcmReadThread) {
				printf("read pcm proc exit\n");
				SempUtils::semp_post(&m_exitReadSem);
				return;
			}
		}

		m_pcmMutex.lock();
		if (m_nPcmBufferSize + nRead * 4 > m_nPcmBufferMaxSize) {
			m_nPcmBufferMaxSize += nRead * 4;
			m_pPcmBuffer = (unsigned char*) realloc(m_pPcmBuffer, m_nPcmBufferMaxSize);
		}
		memcpy(m_pPcmBuffer + m_nPcmBufferSize, m_audioBuf, nRead * 4);
		m_nPcmBufferSize += nRead * 4;
		m_pcmMutex.unlock();
	}
}

static void * audioOutProc(void *pParam) {
	AudioEncodeFAAC *pSelf = (AudioEncodeFAAC *) pParam;
	if (NULL != pSelf) {
		pSelf->onAudioOut();
	}
	pthread_exit(NULL);
	return NULL;
}

void AudioEncodeFAAC::onAudioOut() {
	printf("audio encode process start\n");
	Int32  status;
	while (1) {
		if (m_bExit) {
			printf("audio out exit\n");
			SempUtils::semp_post(&m_eos_sem);
			break;
		}
		m_pcmMutex.lock();
		if (m_nPcmBufferSize < m_encStatus.bufInfo.minInBufSize[0]) {
			m_pcmMutex.unlock();
			usleep(300);
			continue;
		}
		memcpy(m_pInBuf, m_pPcmBuffer, m_encStatus.bufInfo.minInBufSize[0]);
		        /* Deal with cache issues, if necessary */
		#ifdef CACHE_ENABLED
		#if defined(xdc_target__isaCompatible_64P) || \
		    defined(xdc_target__isaCompatible_64T)
		        /*
		         *  fread() on this processor is implemented using CCS's stdio, which
		         *  is known to write into the cache, not physical memory.  To meet
		         *  XDAIS DMA Rule 7, we must writeback the cache into physical
		         *  memory.  Also, per DMA Rule 7, we must invalidate the buffer's
		         *  cache before providing it to any xDAIS algorithm.
		         */
		        Memory_cacheWbInv(inBuf, IFRAMESIZE);
		#else
		#error Unvalidated config - add appropriate fread-related cache maintenance
		#endif

		        /* Per DMA Rule 7, our output buffer cache lines must be cleaned */
		        Memory_cacheInv(encodedBuf, EFRAMESIZE);
		#endif

		    /* encode the frame */
		m_inBufSizes[0] = m_encStatus.bufInfo.minInBufSize[0];
		status = AUDENC_process(m_audioEncHandle, &m_inBufDesc, &m_encodedBufDesc, &m_encInArgs, &m_encOutArgs);

		if (m_encOutArgs.bytesGenerated == 0) {
			 m_pcmMutex.unlock();
		     usleep(300);
		     continue;
		}

		#ifdef CACHE_ENABLED
		        /* Writeback this outBuf from the previous call.  Also, as encodedBuf
		         * is an inBuf to the next process call, we must invalidate it also, to
		         * clean buffer lines.
		         */
		        Memory_cacheWbInv(encodedBuf, EFRAMESIZE);

		        /* Per DMA Rule 7, our output buffer cache lines must be cleaned */
		        Memory_cacheInv(outBuf, OFRAMESIZE);
		#endif

	 if (status != AUDENC_EOK) {
		        printf("Encoder frame processing FAILED, status = 0x%x, "
		             "extendedError = 0x%x", (IArg)status, (IArg)(m_encOutArgs.extendedError));
		        m_pcmMutex.unlock();
		        usleep(300);
		        continue;
		}
	   for (vector<IAudioEncodeListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
			(*it)->onAACEncoded((unsigned char*)m_pEncodedBuf, m_encOutArgs.bytesGenerated);
		}
		m_nPcmBufferSize -= m_encStatus.bufInfo.minInBufSize[0];

		m_pcmMutex.unlock();
	}
}

AudioEncodeFAAC::AudioEncodeFAAC()
: m_pInBuf(NULL)
, m_pEncodedBuf(NULL)
, m_pOutBuf(NULL)
, m_NSAMPLES(0)
, m_IFRAMESIZE(0)
, m_EFRAMESIZE(0)
, m_OFRAMESIZE(0)
, m_ce(NULL)
, m_audioEncHandle(NULL)
, m_bInited(false)
, m_bPCMNonBlock(false)
, m_recordHandle(NULL)
, m_audioBuf(NULL)
, m_pPcmBuffer(NULL) {

}


AudioEncodeFAAC::~AudioEncodeFAAC() {
	uninit();
}


bool AudioEncodeFAAC::init(int aAudioChannel, int aSampleRate,
			int aSampleBit, int aacBitRate, bool bPCMNonBlock, string strDeviceName) {

	printf("audioEncodeFAAC::init\n");
	m_nChannels = aAudioChannel;
	m_nSampleBit = aSampleBit;
	m_nSampleRate = aSampleRate;
	m_aacBitRate = aacBitRate;
	m_bPCMNonBlock = bPCMNonBlock;
	m_strDeviceName = strDeviceName;

	m_bExit = false;

	m_bThreadStart = false;
	if (!initCodecEngine()) {
		return false;
	}
	m_bInited = true;

	return true;
}

bool AudioEncodeFAAC::uninit() {
	if (m_bInited) {
		snd_pcm_close(m_recordHandle);

#ifdef	AUDIO_BUFFER_COPY
	if (NULL != m_pPcmBuffer) {
		free(m_pPcmBuffer);
		m_pPcmBuffer = NULL;
	}

	m_pcmMutex.uninit();
	SempUtils::semp_deinit(&m_exitReadSem);
	SempUtils::semp_deinit(&m_eos_sem);
	if (NULL != m_audioBuf) {
		delete []m_audioBuf;
		m_audioBuf = NULL;
	}
#endif
	}

	uninitCodecEngine();
	m_bInited = false;

	return true;
}

bool AudioEncodeFAAC::start() {
	if (!m_bInited) {
		printf("can not start\n");
		return false;
	}
	if (!setAlsaAudioParam()) {
		printf("Audio driver configuration failed \n");
		return false;
	}

#ifdef AUDIO_BUFFER_COPY
	m_pPcmBuffer = (unsigned char*)malloc(1024 * 1024);
	m_nPcmBufferSize = 0;
	m_nPcmBufferMaxSize = 1024 * 1024;
	m_bExitPcmReadThread = false;
	m_audioBuf = new unsigned char[m_chunk_size * 2 * m_nChannels];
#endif
	m_pcmMutex.init();
	SempUtils::semp_init(&m_exitReadSem, 1);
	SempUtils::semp_init(&m_eos_sem, 1);

	pthread_attr_init(&m_outThreadAttr);
	if (0 != pthread_create(&m_outThreadId, &m_outThreadAttr, audioOutProc, this)) {
		return false;
	}

#ifdef AUDIO_BUFFER_COPY
	pthread_attr_init(&m_readPcmAttr);
	if (0 != pthread_create(&m_readPcmThreadId, &m_readPcmAttr, readPcmProc, this)) {
		return false;
	}
#endif

	m_bExit = false;
	m_bThreadStart = true;
	printf("AudioEncodeFAAC::start\n");
	return true;
}

bool AudioEncodeFAAC::stop() {
	if (!m_bThreadStart) {
		return false;
	}
	m_bExit = true;
	SempUtils::semp_pend(&m_eos_sem);
	printf("audio stop\n");
#ifdef AUDIO_BUFFER_COPY
	m_bExitPcmReadThread = true;
	SempUtils::semp_pend(&m_exitReadSem);
#endif

	pthread_join(m_outThreadId, NULL);

#ifdef AUDIO_BUFFER_COPY
	pthread_join(m_readPcmThreadId, NULL);
#endif

	printf("after audio pthread_join\n");
	uninit();
	return true;
}

bool AudioEncodeFAAC::addListener(IAudioEncodeListener* pListener) {
	for (vector<IAudioEncodeListener*>::iterator it = m_vecListeners.begin();
			it != m_vecListeners.end(); ++it) {
		if (pListener == *(it)) {
			return true;
		}
	}
	m_vecListeners.push_back(pListener);
	return true;
}

bool AudioEncodeFAAC::removeListener(IAudioEncodeListener* pListener) {
	for (vector<IAudioEncodeListener*>::iterator it = m_vecListeners.begin();
			it != m_vecListeners.end(); ++it) {
		if (pListener == *(it)) {
			m_vecListeners.erase(it);
			return true;
		}
	}
	return true;
}


bool AudioEncodeFAAC::setAlsaAudioParam() {
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
	if (snd_pcm_open(&m_recordHandle, m_strDeviceName.c_str(), stream, 0) < 0) {
		printf("Could not open audio device\n");
		return false;
	}

	if (m_bPCMNonBlock) {
		err = snd_pcm_nonblock(m_recordHandle, SND_PCM_NONBLOCK);
	}


	/* Init hwparams with full configuration space */
	if ( (err = snd_pcm_hw_params_any(m_recordHandle, params)) < 0) {
		fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
				snd_strerror(err));
		return false;
	}

	/* Set access type. */
	if ((err = snd_pcm_hw_params_set_access(m_recordHandle, params,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
		return false;
	}

	/* Set sample format */
	if ( (err = snd_pcm_hw_params_set_format(m_recordHandle, params,
			SND_PCM_FORMAT_S16_LE) < 0)) {
		fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
		return false;
	}

	if ((err = snd_pcm_hw_params_set_channels(m_recordHandle, params,
			m_nChannels)) < 0) {
		fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror(err));
		return false;
	}

	/* Set sample rate. If the exact rate is not supported by the
	 hardware, use nearest possible rate. */
	rate = m_nSampleRate;
	if ((err = snd_pcm_hw_params_set_rate_near(m_recordHandle, params,
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
		err = snd_pcm_hw_params_set_period_time_near(m_recordHandle, params,
							     &period_time, 0);
	else
		err = snd_pcm_hw_params_set_period_size_near(m_recordHandle, params,
							     &period_frames, 0);
	assert(err >= 0);
	if (buffer_time > 0) {
		err = snd_pcm_hw_params_set_buffer_time_near(m_recordHandle, params,
							     &buffer_time, 0);
	} else {
		err = snd_pcm_hw_params_set_buffer_size_near(m_recordHandle, params,
							     &buffer_frames);
	}
	unsigned long int buffer_size = 0;
	//snd_pcm_hw_params_get_buffer_size_max(params, &buffer_size);

	//err = snd_pcm_hw_params_set_buffer_size_near(m_recordHandle, params, &buffer_size);

	assert(err >= 0);

	m_monotonic = snd_pcm_hw_params_is_monotonic(params);
	err = snd_pcm_hw_params(m_recordHandle, params);
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

	snd_pcm_sw_params_current(m_recordHandle, swparams);
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
	err = snd_pcm_sw_params_set_avail_min(m_recordHandle, swparams, n);

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
	err = snd_pcm_sw_params_set_start_threshold(m_recordHandle, swparams, start_threshold);
	assert(err >= 0);
	if (stop_delay <= 0)
		stop_threshold = buffer_size + (double) rate * stop_delay / 1000000;
	else
		stop_threshold = (double) rate * stop_delay / 1000000;
	err = snd_pcm_sw_params_set_stop_threshold(m_recordHandle, swparams, stop_threshold);
	assert(err >= 0);
	//err = snd_pcm_sw_params_set_xfer_align(m_recordHandle, swparams, xfer_align);
	//assert(err >= 0);
	if (snd_pcm_sw_params(m_recordHandle, swparams) < 0) {
		return false;
	}
	return true;
}

void AudioEncodeFAAC::xrun()
{
	snd_pcm_status_t *status;
	int res;

	snd_pcm_status_alloca(&status);
	if ((res = snd_pcm_status(m_recordHandle, status))<0) {
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

		if ((res = snd_pcm_prepare(m_recordHandle))<0) {
			printf(("xrun: prepare error: %s"), snd_strerror(res));
			return;
		}
		return;
	} if (snd_pcm_status_get_state(status) == SND_PCM_STATE_DRAINING) {
		if ((res = snd_pcm_prepare(m_recordHandle))<0) {
			return;
		}
	}
	return;
}

/* I/O suspend handler */
void AudioEncodeFAAC::suspend(void)
{
	int res;

	while ((res = snd_pcm_resume(m_recordHandle)) == -EAGAIN)
		sleep(1);
	if (res < 0) {
		if ((res = snd_pcm_prepare(m_recordHandle)) < 0) {
			return;
		}
	}

}


bool AudioEncodeFAAC::initCodecEngine() {
	printf("AudioEncodeFAAC::initCodecEngine begin\n");
	//m_NSAMPLES  =  (1024);  /* must be multiple of 128 for cache/DMA reasons */
	//m_IFRAMESIZE = (m_NSAMPLES * 8);  /* raw frame (input) */
	//m_EFRAMESIZE = (m_NSAMPLES * 8);  /* encoded frame */
	//m_OFRAMESIZE = (m_NSAMPLES * 8);  /* decoded frame (output) */

	m_NSAMPLES  =  (384);  /* must be multiple of 128 for cache/DMA reasons */
	m_IFRAMESIZE = (m_NSAMPLES * 2);  /* raw frame (input) */
	m_EFRAMESIZE = (m_NSAMPLES * 2);  /* encoded frame */
	m_OFRAMESIZE = (m_NSAMPLES * 2);  /* decoded frame (output) */

	/* allocate buffers */
    m_allocParams.type = Memory_CONTIGPOOL;
	m_allocParams.flags = Memory_NONCACHED;
	m_allocParams.align = BUFALIGN;
	m_allocParams.seg = 0;

	m_pInBuf = (XDAS_Int8 *)ti_sdo_ce_osal_Memory_alloc(m_IFRAMESIZE, &m_allocParams);
	m_pEncodedBuf = (XDAS_Int8 *)ti_sdo_ce_osal_Memory_alloc(m_EFRAMESIZE, &m_allocParams);
	m_pOutBuf = (XDAS_Int8 *)ti_sdo_ce_osal_Memory_alloc(m_OFRAMESIZE, &m_allocParams);

	if ((m_pInBuf == NULL) || (m_pEncodedBuf == NULL) || (m_pOutBuf == NULL)) {
	   return false;
	}

	/* Initialize attrs fields to default values, and set the procId */
	Engine_initAttrs(&m_EngineAttrs);
	m_EngineAttrs.procId = "DSP";

	/* Open the Engine */
	if ((m_ce = Engine_open("remote_copy_DSP", &m_EngineAttrs, NULL)) == NULL) {
	        fprintf(stderr, "error: can't open engine faacenc\n");
	        return false;
	}

	/* allocate and initialize audio encoder on the engine */
	m_audioEncHandle = AUDENC_create(m_ce, "speexenc", NULL);
	if (m_audioEncHandle == NULL) {
		fprintf(stderr, "error: can't open codec faacenc\n");
	    return false;
	}

	/* clear and initialize the buffer descriptors */
	memset(m_src,     0, sizeof(m_src[0])     * XDM_MAX_IO_BUFFERS);
	memset(m_encoded, 0, sizeof(m_encoded[0]) * XDM_MAX_IO_BUFFERS);
	memset(m_dst,     0, sizeof(m_dst[0])     * XDM_MAX_IO_BUFFERS);
	m_src[0]     = m_pInBuf;
	m_encoded[0] = m_pEncodedBuf;
	m_dst[0]     = m_pOutBuf;

	m_inBufDesc.numBufs = m_encodedBufDesc.numBufs = m_outBufDesc.numBufs = 1;

	m_inBufDesc.bufSizes      = m_inBufSizes;
	m_encodedBufDesc.bufSizes = m_encBufSizes;
	m_outBufDesc.bufSizes     = m_outBufSizes;

	m_inBufSizes[0] = m_IFRAMESIZE;
	m_encBufSizes[0] = m_EFRAMESIZE;
	m_outBufSizes[0] = m_OFRAMESIZE;

	m_inBufDesc.bufs      = m_src;
	m_encodedBufDesc.bufs = m_encoded;
	m_outBufDesc.bufs     = m_dst;

	/* initialize all "sized" fields */
	m_encInArgs.size    = sizeof(m_encInArgs);
	m_encOutArgs.size   = sizeof(m_encOutArgs);
	m_encDynParams.size = sizeof(m_encDynParams);
	m_encStatus.size    = sizeof(m_encStatus);

	/*
	 * Query the encoder and decoder.
	 * This app expects the encoder to accept 1 buf in and get 1 buf out,
	 * and the buf sizes of the in and out buffer must be able to handle
	 * NSAMPLES bytes of data.
	 */
	Int32 status = AUDENC_control(m_audioEncHandle, XDM_GETSTATUS, &m_encDynParams, &m_encStatus);
	if (status != AUDENC_EOK) {
	      /* failure, report error and exit */
	    Log_print1(Diags_USER7, "[+7] encode control status = %ld", (IArg)status);
	    return false;
	}
	printf("encStatus.bufInfo.minInBufSize[0] = %d :%d\n", m_encStatus.bufInfo.minInBufSize[0], m_IFRAMESIZE);
	printf("encStatus.bufInfo.minOutBufSize[0] = %d :%d\n", m_encStatus.bufInfo.minOutBufSize[0], m_EFRAMESIZE);

	/* Validate this encoder codec will meet our buffer requirements */
	if ((m_inBufDesc.numBufs < m_encStatus.bufInfo.minNumInBufs) ||
	        (m_IFRAMESIZE < m_encStatus.bufInfo.minInBufSize[0]) ||
	        (m_encodedBufDesc.numBufs < m_encStatus.bufInfo.minNumOutBufs) ||
	        (m_EFRAMESIZE < m_encStatus.bufInfo.minOutBufSize[0])) {

	        /* failure, report error and exit */
	        Log_print0(Diags_USER7, "[+7] Error:  encoder codec feature conflict");
	        return false;
	}
	return true;
}

bool AudioEncodeFAAC::uninitCodecEngine() {
	 /* teardown the codecs */
	if (NULL != m_audioEncHandle) {
		AUDENC_delete(m_audioEncHandle);
		m_audioEncHandle = NULL;
	}

	/* close the engine */
	if (NULL != m_ce) {
		Engine_close(m_ce);
		m_ce = NULL;
	}

	/* free buffers */
	if (NULL != m_pInBuf) {
		ti_sdo_ce_osal_Memory_free(m_pInBuf, m_IFRAMESIZE, &m_allocParams);
		m_pInBuf = NULL;
	}
	if (NULL != m_pEncodedBuf) {
		ti_sdo_ce_osal_Memory_free(m_pEncodedBuf, m_EFRAMESIZE, &m_allocParams);
		m_pEncodedBuf = NULL;
	}
	if (NULL != m_pOutBuf) {
		ti_sdo_ce_osal_Memory_free(m_pOutBuf, m_OFRAMESIZE, &m_allocParams);
		m_pOutBuf = NULL;
	}

	Log_print0(Diags_USER1, "[+1] codec engine manager uninit.");
	return true;
}
