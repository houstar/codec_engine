#include "audioEncode.h"
#include <alsa/asoundlib.h>
#include <time.h>
#include <sys/time.h>
#include "../utils/commonUtils.h"

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

static OMX_ERRORTYPE AENC_EventHandler(OMX_HANDLETYPE hComponent,
		OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1,
		OMX_U32 nData2, OMX_PTR pEventData) {
	AudioEncode *pSelf = (AudioEncode*) ptrAppData;
	OMX_S32 retval;
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_U32 postEvent;

	switch (eEvent) {
	case OMX_EventCmdComplete:
		if (nData1 == OMX_CommandStateSet) {
			SempUtils::semp_post(pSelf->m_state_sem);
		}
		if (nData1 == OMX_CommandPortEnable
				|| nData1 == OMX_CommandPortDisable) {
			SempUtils::semp_post(pSelf->m_port_sem);
		}
		break;
	case OMX_EventError:
		postEvent = AENC_ENCODER_ERROR_EVENT;
		retval = write(pSelf->m_Event_Pipe[1], &postEvent, sizeof(postEvent));
		if (retval != sizeof(postEvent)) {
			eError = OMX_ErrorNotReady;
			return eError;
		}
		eError = OMX_GetState(pSelf->m_pHandle, &pSelf->m_eState);
		/*For create errors: */
		if (pSelf->m_eState == OMX_StateLoaded) {
			SempUtils::semp_post(pSelf->m_state_sem);
		}
		break;
	case OMX_EventMark:

		break;
	case OMX_EventPortSettingsChanged:
		/* In case of change in output buffer sizes re-allocate the buffers */
		eError = pSelf->changePortSettings();

		break;
	case OMX_EventBufferFlag:
		postEvent = AENC_ENCODER_END_OF_STREAM;
		retval = write(pSelf->m_Event_Pipe[1], &postEvent, sizeof(postEvent));
		if (retval != sizeof(postEvent)) {
			eError = OMX_ErrorNotReady;
			return eError;
		}
		/* EOS here nData1-> port....  nData2->OMX_BUFFERFLAG_EOS */
		break;
	case OMX_EventResourcesAcquired:

		break;
	case OMX_EventComponentResumed:

		break;
	case OMX_EventDynamicResourcesAvailable:

		break;
	case OMX_EventPortFormatDetected:

		break;
	case OMX_EventMax:
		break;
	case OMX_EventVendorStartUnused:
		break;
	case OMX_EventKhronosExtensions:
		break;
	} /* end of switch */

	return eError;
}

static OMX_ERRORTYPE AENC_FillBufferDone(OMX_HANDLETYPE hComponent,
		OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE * pBuffer) {
	AudioEncode *pSelf = (AudioEncode*) ptrAppData;
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_S32 retval = 0;

	retval = write(pSelf->m_OpBuf_Pipe[1], &pBuffer, sizeof(pBuffer));

	if (retval != sizeof(pBuffer)) {
		eError = OMX_ErrorNotReady;
	}

	return eError;
}

static OMX_ERRORTYPE AENC_EmptyBufferDone(OMX_HANDLETYPE hComponent,
		OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE * pBuffer) {
	AudioEncode *pSelf = (AudioEncode*) ptrAppData;
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_S32 retval = 0;

	retval = write(pSelf->m_IpBuf_Pipe[1], &pBuffer, sizeof(pBuffer));

	if (retval != sizeof(pBuffer)) {
		printf("EBD error: %d / %d\n", (int) retval, sizeof(pBuffer));
		eError = OMX_ErrorNotReady;
		return eError;
	}

	return eError;
}

#ifdef AUDIO_BUFFER_COPY
void* readPcmProc(void* pParam) {
	AudioEncode *pSelf = (AudioEncode*) pParam;
	bool bWriteIn = true;
	int nSampleRate = pSelf->m_nSampleRate;
    int err = 0;
	while (1) {
		if (pSelf->m_bExitPcmReadThread) {
			printf("read pcm proc exit\n");
			if (NULL != pSelf->m_fIn) {
				fclose(pSelf->m_fIn);
			}
			SempUtils::semp_post(&pSelf->m_exitSem);
			break;
		}

		int nRead = 0;
		if (pSelf->m_fIn != NULL) {
			SempUtils::semp_pend(&pSelf->m_pcmSem);
			if (pSelf->m_nPcmBufferSize + pSelf->m_pInPortDef->nBufferSize > pSelf->m_nPcmBufferMaxSize) {
				pSelf->m_nPcmBufferMaxSize += pSelf->m_pInPortDef->nBufferSize;
				pSelf->m_pPcmBuffer = (unsigned char*) realloc(pSelf->m_pPcmBuffer, pSelf->m_nPcmBufferMaxSize);
			}

		    nRead = fread(pSelf->m_pPcmBuffer + pSelf->m_nPcmBufferSize, sizeof(char),
		    			pSelf->m_pInPortDef->nBufferSize, pSelf->m_fIn);
		    if (nRead > 0) {
		    	pSelf->m_nPcmBufferSize += nRead;
		    }
		    SempUtils::semp_post(&pSelf->m_pcmSem);
		    usleep(3000);
		} else {
			size_t count = pSelf->m_chunk_size;
			int nOffset = 0;
			while (count > 0) {
				nRead = snd_pcm_readi(pSelf->m_recordHandle,
						pSelf->m_audioBuf + nOffset, count);
				if (nRead == -EAGAIN) {
					snd_pcm_wait(pSelf->m_recordHandle, 100);
					printf("pcm wait ********* %d\n", nRead);
				} else if (nRead == -EPIPE) { //EPIPE:32
					pSelf->xrun();
					printf("overrun occurred\n");
				} else if (nRead == -ESTRPIPE) { //ESTRPIPE: 86 Streams pipe error
					pSelf->suspend();
					printf("suspend\n");
				} else if (nRead < 0) {
					printf("pcm read error: value %d, meaning %s\n", (nRead), snd_strerror(nRead));
					if ((err = snd_pcm_prepare(pSelf->m_recordHandle))<0) {
						printf(("xrun: prepare error: %s"), snd_strerror(err));
					}
				}
				if (nRead > 0) {
					count -= nRead;
					nOffset += nRead * 4;
				}

				if (pSelf->m_bExitPcmReadThread) {
					printf("read pcm proc exit\n");
					if (NULL != pSelf->m_fIn) {
						fclose(pSelf->m_fIn);
					}
					SempUtils::semp_post(&pSelf->m_exitSem);
					return NULL;
				}
			}

			SempUtils::semp_pend(&pSelf->m_pcmSem);
			if (pSelf->m_nPcmBufferSize + nRead * 4 > pSelf->m_nPcmBufferMaxSize) {
				pSelf->m_nPcmBufferMaxSize += nRead * 4;
				pSelf->m_pPcmBuffer = (unsigned char*) realloc(
						pSelf->m_pPcmBuffer, pSelf->m_nPcmBufferMaxSize);
			}
			memcpy(pSelf->m_pPcmBuffer + pSelf->m_nPcmBufferSize,
					pSelf->m_audioBuf, nRead * 4);
			pSelf->m_nPcmBufferSize += nRead * 4;
			SempUtils::semp_post(&pSelf->m_pcmSem);
		}
	}
	return NULL;
}
#endif

void* audioInProc(void* pParam) {
	AudioEncode *pSelf = (AudioEncode*) pParam;
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;

	printf("audio in proc start\n");
	for (OMX_U32 i = 0; i < pSelf->m_pInPortDef->nBufferCountActual; i++) {
		if (!pSelf->initialInputResources(i) ) {
			i--;
			usleep(30);
		}
		if (pSelf->m_bExit) {
			printf("audio in proc exit\n");
			SempUtils::semp_post(pSelf->m_eos_sem);
			pSelf->m_pInBuff[i]->nFlags = OMX_BUFFERFLAG_EOS;
			OMX_EmptyThisBuffer(pSelf->m_pHandle, pSelf->m_pInBuff[i]);
			pthread_exit(NULL);
			return NULL;
		}
	}
	printf("after initial audio Input resource\n");

	while (1) {
		read(pSelf->m_IpBuf_Pipe[0], &pBufferIn, sizeof(pBufferIn));
		if (pSelf->m_bExit) {
			printf("audio in proc exit\n");
			SempUtils::semp_post(pSelf->m_eos_sem);
			pBufferIn->nFlags = OMX_BUFFERFLAG_EOS;
			OMX_EmptyThisBuffer(pSelf->m_pHandle, pBufferIn);
			pthread_exit(NULL);
			break;
		}
		int nReadSize = pSelf->fillData(pBufferIn);
		while (nReadSize == 0 && !pSelf->m_bExit) {
			nReadSize = pSelf->fillData(pBufferIn);
			usleep(30);
		}
		if (pSelf->m_bExit) {
			printf("audio in proc exit\n");
			SempUtils::semp_post(pSelf->m_eos_sem);
			pBufferIn->nFlags = OMX_BUFFERFLAG_EOS;
			OMX_EmptyThisBuffer(pSelf->m_pHandle, pBufferIn);
			pthread_exit(NULL);
			break;
		}
		OMX_EmptyThisBuffer(pSelf->m_pHandle, pBufferIn);
	}
	return NULL;
}

void * audioOutProc(void *encode) {
	AudioEncode *pSelf = (AudioEncode *) encode;

	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	OMX_U32 nRead = 0;

	printf("audio encode process start\n");


	for (OMX_U32 i = 0; i < pSelf->m_pOutPortDef->nBufferCountActual; i++) {
		eError = OMX_FillThisBuffer( pSelf->m_pHandle, pSelf->m_pOutBuff[i]);
		if (eError != OMX_ErrorNone) {
			printf("audio out exit\n");
			SempUtils::semp_post(pSelf->m_eos_sem);
			pthread_exit(NULL);
			return NULL;
		}
	}

	printf("after fill this buffer out port\n");

	while (1) {
		if (pSelf->m_bExit) {
			printf("audio out exit\n");
			SempUtils::semp_post(pSelf->m_eos_sem);
			pthread_exit(NULL);
			break;
		}
		read(pSelf->m_OpBuf_Pipe[0], &pBufferOut, sizeof(pBufferOut));
        if ((pBufferOut->nFlags & OMX_BUFFERFLAG_EOS)) {
			printf("audio out exit buffer flag eos\n");
			SempUtils::semp_post(pSelf->m_eos_sem);
			pthread_exit(NULL);
            break;
        }

		if (pBufferOut != NULL) {
			for (vector<IAudioEncodeListener*>::iterator it = pSelf->m_vecListeners.begin();
					it != pSelf->m_vecListeners.end(); ++it) {
				(*it)->onAACEncoded(pBufferOut->pBuffer, pBufferOut->nFilledLen);
			}
		}
		eError = OMX_FillThisBuffer(pSelf->m_pHandle, pBufferOut);
		if (eError != OMX_ErrorNone) {
			printf("audio out exit\n");
			SempUtils::semp_post(pSelf->m_eos_sem);
			pthread_exit(NULL);
			break;
		}

	}
	return NULL;
}




bool AudioEncode::start() {
	if (!initOMXHandle() ) {
		return false;
	}
	if (NULL == m_fIn) {
		bool bError = configureAudioDriver();
		if (!bError) {
			printf("Audio driver configuration failed \n");
			return false;
		}
	}

#ifdef AUDIO_BUFFER_COPY
	m_audioBuf = new unsigned char[m_chunk_size * 2 * m_nChannels];
#endif
	pthread_attr_init(&m_outConnectAttr);
	if (0 != pthread_create(&m_outThreadId, &m_outConnectAttr, audioOutProc, this)) {
		return false;
	}
	pthread_attr_init(&m_inConnectAttr);
	if (0 != pthread_create(&m_inThreadId, &m_inConnectAttr, audioInProc, this)) {
		return false;
	}

#ifdef AUDIO_BUFFER_COPY
	pthread_attr_init(&m_readPcmAttr);
	if (0 != pthread_create(&m_readPcmThreadId, &m_readPcmAttr, readPcmProc, this)) {
		return false;
	}
#endif

	m_bExit = false;

	//system("/aic3106_config/aic3106_setting_16M");
	//system("/aic3106_config/line1_vol_adc");
	//system("/aic3106_config/lineout_vol");
	m_bThreadStart = true;
	return true;
}

bool AudioEncode::stop() {
	if (!m_bThreadStart) {
		return false;
	}
	m_bExit = true;
	SempUtils::semp_pend(m_eos_sem);
	SempUtils::semp_pend(m_eos_sem);
	printf("audio stop\n");
	OMX_ERRORTYPE eError = OMX_SendCommand(m_pHandle, OMX_CommandStateSet, OMX_StateIdle, NULL );
	if (eError != OMX_ErrorNone) {
		return false;
	}
	SempUtils::semp_pend(m_state_sem);

	printf("Going to Loaded\n");
	eError = OMX_SendCommand(m_pHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (OMX_ErrorNone != eError) {
		return false;
	}

	printf("Freeing buffers\n");
	if (!freeAudioEncodeBuffers() ) {
		return false;
	}

	printf("Component transitioned from Idle to Loaded\n");
	eError = OMX_FreeHandle(m_pHandle);
	if ((eError != OMX_ErrorNone)) {
		return false;
	}

#ifdef AUDIO_BUFFER_COPY
	m_bExitPcmReadThread = true;
	SempUtils::semp_pend(&m_exitSem);
#endif

	pthread_join(m_outThreadId, NULL);
	pthread_join(m_inThreadId, NULL);

#ifdef AUDIO_BUFFER_COPY
	pthread_join(m_readPcmThreadId, NULL);
#endif


	printf("after audio pthread_join\n");
	uninit();
	return true;
}

AudioEncode::AudioEncode() : m_pHandle(NULL),
		m_bInited(false), m_fIn(NULL),
		m_pInPortDef(NULL), m_pOutPortDef(NULL), m_bPCMNonBlock(false) {
	m_recordHandle = NULL;
#ifdef AUDIO_BUFFER_COPY
	m_audioBuf = NULL;
	m_pPcmBuffer = NULL;
#endif
}

AudioEncode::~AudioEncode() {
	uninit();
}

bool AudioEncode::init(int aAudioChannel, int aSampleRate,
		int aSampleBit, int aAACBitRate, bool bPCMNonBlock, string strDeviceName, char* pInFileName) {
	m_bInited = true;

	m_nChannels = aAudioChannel;
	m_nSampleBit = aSampleBit;
	m_nSampleRate = aSampleRate;
	m_aacBitRate = aAACBitRate;
	m_bPCMNonBlock = bPCMNonBlock;
	m_strDeviceName = strDeviceName;

	createComponents();

	m_eCompressionFormat = OMX_AUDIO_CodingAAC;
	m_outputformat = OMX_AUDIO_AACStreamFormatMP4ADTS;

	printf("audio before allocate Resources\n");
	if (!allocInOutParams()) {
		return false;
	}
	printf("audio after allocate Resources\n");

	m_eState = OMX_StateInvalid;
	m_pCb.EventHandler = AENC_EventHandler;
	m_pCb.EmptyBufferDone = AENC_EmptyBufferDone;
	m_pCb.FillBufferDone = AENC_FillBufferDone;

	m_bExit = false;

	if (NULL != pInFileName) {
	    m_fIn = fopen(pInFileName, "rb");
		if (NULL == m_fIn) {
			return false;
		} else {
			fseek(m_fIn, 0, SEEK_END);
		    m_InFileSize = ftell(m_fIn);
		    fseek(m_fIn, 0, SEEK_SET);
		}
	}

#ifdef AUDIO_BUFFER_COPY
	m_pPcmBuffer = (unsigned char*)malloc(1024 * 1024);
	m_nPcmBufferSize = 0;
	m_nPcmBufferMaxSize = 1024 * 1024;
	SempUtils::semp_init(&m_pcmSem, 1);
	SempUtils::semp_init(&m_exitSem, 1);
	m_bExitPcmReadThread = false;
#endif

	//system("/aic3106_config/aic3106_setting_16M");
	//system("/aic3106_config/line1_vol_adc");
	//system("/aic3106_config/lineout_vol");

	m_bThreadStart = false;
	return true;
}

bool AudioEncode::uninit() {
	if (m_bInited) {
		freeInOutParams();
		destroyComponents();

		snd_pcm_close(m_recordHandle);

#ifdef	AUDIO_BUFFER_COPY
		if (NULL != m_pPcmBuffer) {
			free(m_pPcmBuffer);
			m_pPcmBuffer = NULL;
		}

		SempUtils::semp_deinit(&m_pcmSem);
		SempUtils::semp_deinit(&m_exitSem);
		if (NULL != m_audioBuf) {
			delete []m_audioBuf;
			m_audioBuf = NULL;
		}
#endif
	}
	m_bInited = false;

	return true;
}


void AudioEncode::createComponents() {
	m_port_sem = (semp_t *) malloc(sizeof(semp_t));
	SempUtils::semp_init(m_port_sem, 0);

	m_state_sem = (semp_t *) malloc(sizeof(semp_t));
	SempUtils::semp_init(m_state_sem, 0);

	m_eos_sem = (semp_t *) malloc(sizeof(semp_t));
	SempUtils::semp_init(m_eos_sem, 0);
}

void AudioEncode::destroyComponents() {
	SempUtils::semp_deinit(m_state_sem);
	free(m_state_sem);
	m_state_sem = NULL;

	SempUtils::semp_deinit(m_port_sem);
	free(m_port_sem);
	m_port_sem = NULL;

	SempUtils::semp_deinit(m_eos_sem);
	free(m_eos_sem);
	m_eos_sem = NULL;
}

bool AudioEncode::allocInOutParams() {
	m_pInPortDef = (OMX_PARAM_PORTDEFINITIONTYPE *) malloc(
			sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
	if (!m_pInPortDef) {
		return false;
	}

	m_pOutPortDef = (OMX_PARAM_PORTDEFINITIONTYPE *) malloc(
			sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
	if (!m_pOutPortDef) {
		return false;
	}

	/* Create a pipes for Input and Output Buffers.. used to queue data from the callback. */
	OMX_U32 retval = pipe((int *) m_IpBuf_Pipe);
	if (retval == -1) {
		return false;
	}
	retval = pipe((int *) m_OpBuf_Pipe);
	if (retval == -1) {
		return false;
	}
	retval = pipe((int *) m_Event_Pipe);
	if (retval == -1) {
		return false;
	}

	return true;
}

void AudioEncode::freeInOutParams() {
	if (m_pInPortDef)
		free(m_pInPortDef);

	if (m_pOutPortDef)
		free(m_pOutPortDef);

	close((int) m_IpBuf_Pipe);

	close((int) m_OpBuf_Pipe);

	close((int) m_Event_Pipe);
}

bool AudioEncode::initOMXHandle() {
	printf("init Audio OMX Handle\n");
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pHandle,
			(OMX_STRING) "OMX.TI.DSP.AUDENC", this, &m_pCb);
	if ((eError != OMX_ErrorNone) || (m_pHandle == NULL)) {
		printf("Couldn't get a handle\n");
		return false;
	}

	printf("before set audio param port definition\n");
	eError = setParamPortDefinition();
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_SendCommand(m_pHandle, OMX_CommandPortEnable,
			OMX_AUDENC_INPUT_PORT, NULL);
	SempUtils::semp_pend(m_port_sem);

	OMX_SendCommand(m_pHandle, OMX_CommandPortEnable,
			OMX_AUDENC_OUTPUT_PORT, NULL);
	SempUtils::semp_pend(m_port_sem);

	eError = OMX_GetParameter(m_pHandle, OMX_IndexParamPortDefinition,
			m_pInPortDef);
	if (eError != OMX_ErrorNone) {
		return NULL;
	}

	eError = OMX_GetParameter(m_pHandle, OMX_IndexParamPortDefinition,
			m_pOutPortDef);
	if (eError != OMX_ErrorNone) {
		return NULL;
	}

	//  OMX_SendCommand expecting OMX_StateIdle
	eError = OMX_SendCommand(m_pHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);

	if (eError != OMX_ErrorNone) {
		return NULL;
	}

	for (OMX_U32 i = 0; i < m_pInPortDef->nBufferCountActual;
			i++) {
		eError = OMX_AllocateBuffer(m_pHandle,
				&m_pInBuff[i],
				m_pInPortDef->nPortIndex, this,
				m_pInPortDef->nBufferSize );
	}

	for (OMX_U32 i = 0; i < m_pOutPortDef->nBufferCountActual;
			i++) {
		eError = OMX_AllocateBuffer(m_pHandle,
				&m_pOutBuff[i],
				m_pOutPortDef->nPortIndex,
				this,
				m_pOutPortDef->nBufferSize );
	}
	SempUtils::semp_pend(m_state_sem);

	eError = OMX_SendCommand(m_pHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL );
	if (eError != OMX_ErrorNone) {
		return false;
	}
	SempUtils::semp_pend(m_state_sem);
	printf("Component transitioned from Idle to Execute\n");

	return true;
}

bool AudioEncode::initialInputResources(int aInputIndex) {
	int toRead = m_pInPortDef->nBufferSize;
	int  tFrames = toRead / (m_nChannels * 2);

#ifdef AUDIO_BUFFER_COPY
	SempUtils::semp_pend(&m_pcmSem);
	if (m_nPcmBufferSize < toRead) {
		SempUtils::semp_post(&m_pcmSem);
		return false;
	}
	memcpy(m_pInBuff[aInputIndex]->pBuffer, m_pPcmBuffer, toRead);
	m_nPcmBufferSize -= toRead;
	if (m_nPcmBufferSize > 0) {
		memmove(m_pPcmBuffer, m_pPcmBuffer + toRead, m_nPcmBufferSize);
	}
	SempUtils::semp_post(&m_pcmSem);

#else
	OMX_U32 nRead = 0;
	nRead = snd_pcm_readi(m_recordHandle, m_pInBuff[aInputIndex]->pBuffer, tFrames);
	if (nRead == -EPIPE) {
	     /* EPIPE means overrun */
	     fprintf(stderr, "overrun occurred\n");
	     snd_pcm_prepare(m_recordHandle);
	} else if (nRead < 0) {
	     fprintf(stderr, "error from read: %s\n", snd_strerror(nRead));
	} else if (nRead != tFrames) {
	     fprintf(stderr, "short read, read %d frames\n", (int) nRead);
	}
#endif


	m_pInBuff[aInputIndex]->nFilledLen = toRead;
	m_pInBuff[aInputIndex]->nOffset = 0;

	OMX_ERRORTYPE eError = OMX_EmptyThisBuffer(m_pHandle, m_pInBuff[aInputIndex]);
	return (OMX_ErrorNone == eError);
}

OMX_U32 AudioEncode::fillData(OMX_BUFFERHEADERTYPE * pBuf) {
	OMX_U32 toRead = m_pInPortDef->nBufferSize;

#ifdef AUDIO_BUFFER_COPY
	SempUtils::semp_pend(&m_pcmSem);
	if (m_nPcmBufferSize < toRead) {
		SempUtils::semp_post(&m_pcmSem);
		return 0;
	}
	memcpy(pBuf->pBuffer, m_pPcmBuffer, toRead);
	m_nPcmBufferSize -= toRead;
	if (m_nPcmBufferSize > 0) {
		memmove(m_pPcmBuffer, m_pPcmBuffer + toRead, m_nPcmBufferSize);
	}
	SempUtils::semp_post(&m_pcmSem);
	OMX_U32 nRead = toRead;
#else
	int  tFrames = toRead / (m_nChannels * 2);

		  OMX_U32 nRead = 0;
		  nRead = snd_pcm_readi(m_recordHandle, pBuf->pBuffer, tFrames);
	      if (nRead == -EPIPE) {
	          /* EPIPE means overrun */
	          fprintf(stderr, "overrun occurred\n");
	          snd_pcm_prepare(m_recordHandle);
	      } else if (nRead < 0) {
	          fprintf(stderr, "error from read: %s\n", snd_strerror(nRead));
	      } else if (nRead != tFrames) {
	          fprintf(stderr, "short read, read %d frames\n", (int) nRead);
	      }
#endif
	pBuf->nFilledLen = toRead;
	pBuf->nOffset = 0;

	return nRead;
}

OMX_ERRORTYPE AudioEncode::setParamPortDefinition() {
	OMX_ERRORTYPE eError = OMX_ErrorUndefined;
	OMX_PORT_PARAM_TYPE portInit;
	OMX_PARAM_PORTDEFINITIONTYPE *pInPortDef, *pOutPortDef;
	OMX_AUDIO_PARAM_AACPROFILETYPE tAACProfile;
	OMX_AUDIO_PARAM_PCMMODETYPE tPCMMode;

	if (!m_pHandle) {
		eError = OMX_ErrorBadParameter;
		return eError;
	}
	pInPortDef = m_pInPortDef;
	pOutPortDef = m_pOutPortDef;
	OMX_TEST_INIT_STRUCT_PTR(&portInit, OMX_PORT_PARAM_TYPE);

	portInit.nPorts = 2;
	portInit.nStartPortNumber = 0;
	eError = OMX_SetParameter(m_pHandle, OMX_IndexParamAudioInit, &portInit);
	if (eError != OMX_ErrorNone) {
		return eError;
	}
	printf("set parameter audio init success \n");

	/* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */

	OMX_TEST_INIT_STRUCT_PTR(pInPortDef, OMX_PARAM_PORTDEFINITIONTYPE);
	pInPortDef->nPortIndex = OMX_AUDENC_INPUT_PORT;

	OMX_GetParameter(m_pHandle, OMX_IndexParamPortDefinition, pInPortDef);

	pInPortDef->nBufferCountActual = NUM_OF_IN_BUFFERS;
	pInPortDef->format.audio.eEncoding = OMX_AUDIO_CodingPCM;

	eError =
			OMX_SetParameter(m_pHandle, OMX_IndexParamPortDefinition, pInPortDef);
	if (eError != OMX_ErrorNone) {
		return eError;
	}

	/* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (output) */
	OMX_TEST_INIT_STRUCT_PTR(pOutPortDef, OMX_PARAM_PORTDEFINITIONTYPE);
	pOutPortDef->nPortIndex = OMX_AUDENC_OUTPUT_PORT;
	OMX_GetParameter(m_pHandle, OMX_IndexParamPortDefinition, pOutPortDef);
	pOutPortDef->nBufferCountActual = NUM_OF_OUT_BUFFERS;
	pOutPortDef->format.audio.eEncoding = OMX_AUDIO_CodingAAC;

	eError =
			OMX_SetParameter(m_pHandle, OMX_IndexParamPortDefinition, pOutPortDef);
	if (eError != OMX_ErrorNone) {
		return eError;
	}
	OMX_TEST_INIT_STRUCT_PTR(&tPCMMode, OMX_AUDIO_PARAM_PCMMODETYPE);
	tPCMMode.nPortIndex = OMX_AUDENC_INPUT_PORT;

	OMX_GetParameter(m_pHandle, OMX_IndexParamAudioPcm, &tPCMMode);
	printf("get parameter audio pcm success \n");

	tPCMMode.nChannels = m_nChannels;
	tPCMMode.nSamplingRate = m_nSampleRate;
	eError = OMX_SetParameter(m_pHandle, OMX_IndexParamAudioPcm, &tPCMMode);
	if (eError != OMX_ErrorNone) {
		return eError;
	}
	printf("set parameter audio pcm success\n");

	OMX_TEST_INIT_STRUCT_PTR(&tAACProfile, OMX_AUDIO_PARAM_AACPROFILETYPE);
	tAACProfile.nPortIndex = OMX_AUDENC_OUTPUT_PORT;

	OMX_GetParameter(m_pHandle, OMX_IndexParamAudioAac, &tAACProfile);
	tAACProfile.nBitRate = m_aacBitRate;
	tAACProfile.eAACStreamFormat = m_outputformat;
	eError = OMX_SetParameter(m_pHandle, OMX_IndexParamAudioAac, &tAACProfile);
	if (eError == OMX_ErrorNone) {
		printf("set parameter audio aac success \n");
	}
	return eError;
}

OMX_ERRORTYPE AudioEncode::changePortSettings() {
	OMX_ERRORTYPE eError = OMX_SendCommand(m_pHandle, OMX_CommandPortDisable,
			m_pOutPortDef->nPortIndex, NULL);
	if (eError != OMX_ErrorNone) {
		return eError;
	}

	for (OMX_U32 i = 0; i < m_pOutPortDef->nBufferCountActual; i++) {
		eError = OMX_FreeBuffer(m_pHandle, m_pOutPortDef->nPortIndex,
				m_pOutBuff[i]);
		if (eError != OMX_ErrorNone) {
			return eError;
		}
	}

	eError = OMX_GetParameter(m_pHandle, OMX_IndexParamPortDefinition,
			m_pOutPortDef);
	if (eError != OMX_ErrorNone) {
		return eError;
	}

	eError = OMX_SendCommand(m_pHandle, OMX_CommandPortEnable,
			m_pOutPortDef->nPortIndex, NULL);
	if (eError != OMX_ErrorNone) {
		return eError;
	}

	for (OMX_U32 i = 0; i < m_pOutPortDef->nBufferCountActual; i++) {
		eError = OMX_AllocateBuffer(m_pHandle, &m_pOutBuff[i],
				m_pOutPortDef->nPortIndex, this,
				m_pOutPortDef->nBufferSize);
		if (eError != OMX_ErrorNone) {
			return eError;
		}
	}

	return eError;
}

bool AudioEncode::freeAudioEncodeBuffers() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (OMX_U32 i = 0; i < m_pInPortDef->nBufferCountActual; i++) {
		eError = OMX_FreeBuffer(m_pHandle, m_pInPortDef->nPortIndex, m_pInBuff[i] );
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	for (OMX_U32 i = 0; i < m_pOutPortDef->nBufferCountActual; i++) {
		eError = OMX_FreeBuffer( m_pHandle, m_pOutPortDef->nPortIndex,
					m_pOutBuff[i] );
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	SempUtils::semp_pend(m_state_sem);
	return true;
}

bool AudioEncode::setAlsaAudioParam() {
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

	printf("period size %d frames\n", m_chunk_size);

	snd_pcm_hw_params_get_buffer_time(params, &val, 0);
	printf("get buffer time %d us\n", val);

	printf("buffer size %d frames\n", buffer_size);

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

bool AudioEncode::configureAudioDriver() {
#ifdef AUDIO_BUFFER_COPY
	return setAlsaAudioParam();
#else

	int period_size =  (m_pInPortDef->nBufferSize) / (m_nChannels * 2);
	snd_pcm_stream_t stream = SND_PCM_STREAM_CAPTURE;

	/*
	 This structure contains information about the hardware and can be
	 used to specify the configuration to be used for the PCM stream.
	 */
    snd_pcm_hw_params_t *hw_params;

    int err, exact_rate;
    int dir, exact_period_size;
    exact_period_size = period_size;
    if ((err = snd_pcm_open(&m_recordHandle, m_strDeviceName.c_str(), stream, 0)) < 0) {
          printf("Could not open audio device\n");
          return false;
     }

      /* Allocate the snd_pcm_hw_params_t structure on the stack. */
     if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
          fprintf(stderr, "cannot allocate hardware parameters (%s)\n",
                  snd_strerror(err));
          return false;
     }

      /* Init hwparams with full configuration space */
      if ((err = snd_pcm_hw_params_any(m_recordHandle, hw_params)) < 0) {
          fprintf(stderr,
                  "cannot initialize hardware parameter structure (%s)\n",
                  snd_strerror(err));
          return false;
      }

      /* Set access type. */
      if ((err =
           snd_pcm_hw_params_set_access(m_recordHandle, hw_params,
                                        SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
          fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
          return false;
      }

      /* Set sample format */
      if ((err =
           snd_pcm_hw_params_set_format(m_recordHandle, hw_params,
                                        SND_PCM_FORMAT_S16_LE)) < 0) {
          fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
          return false;
      }

      /* Set sample rate. If the exact rate is not supported by the
         hardware, use nearest possible rate. */
      exact_rate = m_nSampleRate;
      if ((err =
           snd_pcm_hw_params_set_rate_near(m_recordHandle, hw_params,
                                           (unsigned int *) &m_nSampleRate, 0)) < 0)
      {

          fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
          return false;
      }


      if (m_nSampleRate != exact_rate) {
          fprintf(stderr, "The rate %d Hz is not supported by the hardware. \
  			Using %d Hz instead.\n", m_nSampleRate, exact_rate);
      }

      /* Set number of channels */
      if ((err =
           snd_pcm_hw_params_set_channels(m_recordHandle, hw_params,
                                          m_nChannels)) < 0) {
          fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror(err));
          return false;
      }

      /* Set period size */
      if ((err =
           snd_pcm_hw_params_set_period_size_near(m_recordHandle, hw_params,
                                                  (snd_pcm_uframes_t *) &period_size,
                                                  &dir)) < 0)
      {
          fprintf(stderr, "cannot set periodsize (%s)\n", snd_strerror(err));
          return false;
      }
      if (period_size != exact_period_size) {
          fprintf(stderr, "using periodsize %d instead of %d \n", period_size,
                  exact_period_size);
      }



      /* Apply HW parameter settings to PCM device and prepare device. */
      if ((err = snd_pcm_hw_params(m_recordHandle, hw_params)) < 0) {
          fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
          return false;
      }

      snd_pcm_hw_params_free(hw_params);

      if ((err = snd_pcm_prepare(m_recordHandle)) < 0) {
          fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                  snd_strerror(err));
          return false;
      }
	return true;
#endif
}

bool AudioEncode::addListener(IAudioEncodeListener* pListener) {
	for (vector<IAudioEncodeListener*>::iterator it = m_vecListeners.begin();
			it != m_vecListeners.end(); ++it) {
		if (pListener == *(it)) {
			return true;
		}
	}
	m_vecListeners.push_back(pListener);
	return true;
}

bool AudioEncode::removeListener(IAudioEncodeListener* pListener) {
	for (vector<IAudioEncodeListener*>::iterator it = m_vecListeners.begin();
			it != m_vecListeners.end(); ++it) {
		if (pListener == *(it)) {
			m_vecListeners.erase(it);
			return true;
		}
	}
	return true;
}

void AudioEncode::xrun()
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
void AudioEncode::suspend(void)
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
