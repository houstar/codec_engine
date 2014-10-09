#include "audioEncodeOMX.h"
#include <alsa/asoundlib.h>
#include <time.h>
#include <sys/time.h>
#include "../utils/commonUtils.h"



static OMX_ERRORTYPE AENC_EventHandler(OMX_HANDLETYPE hComponent,
		OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1,
		OMX_U32 nData2, OMX_PTR pEventData) {
	AudioEncodeOMX *pSelf = (AudioEncodeOMX*) ptrAppData;
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
	AudioEncodeOMX *pSelf = (AudioEncodeOMX*) ptrAppData;
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
	AudioEncodeOMX *pSelf = (AudioEncodeOMX*) ptrAppData;
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

static void* audioInProc(void* pParam) {
	AudioEncodeOMX *pSelf = (AudioEncodeOMX*) pParam;
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;

	printf("audio in proc start\n");
	for (int i = 0; i < pSelf->m_pInPortDef->nBufferCountActual; i++) {
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

static void * audioOutProc(void *encode) {
	AudioEncodeOMX *pSelf = (AudioEncodeOMX *) encode;

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
		pBufferOut->nFilledLen = 0;
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


bool AudioEncodeOMX::onProviderPcm(const char* pBuffer, int len) {
	SempUtils::semp_pend(&m_pcmSem);
	if (m_nPcmBufferSize + len >m_nPcmBufferMaxSize) {
		m_nPcmBufferMaxSize +=  len;
		m_pPcmBuffer = (unsigned char*) realloc(m_pPcmBuffer, m_nPcmBufferMaxSize);
	}
	memcpy(m_pPcmBuffer + m_nPcmBufferSize, pBuffer, len);
	m_nPcmBufferSize += len;
	SempUtils::semp_post(&m_pcmSem);
	return true;
}

bool AudioEncodeOMX::start() {
	if (!initOMXHandle() ) {
		return false;
	}

	pthread_attr_init(&m_outConnectAttr);
	if (0 != pthread_create(&m_outThreadId, &m_outConnectAttr, audioOutProc, this)) {
		return false;
	}
	pthread_attr_init(&m_inConnectAttr);
	if (0 != pthread_create(&m_inThreadId, &m_inConnectAttr, audioInProc, this)) {
		return false;
	}

	m_bExit = false;
	m_bThreadStart = true;
	return true;
}

bool AudioEncodeOMX::stop() {
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

	pthread_join(m_outThreadId, NULL);
	pthread_join(m_inThreadId, NULL);


	printf("after audio pthread_join\n");
	uninit();
	return true;
}

AudioEncodeOMX::AudioEncodeOMX() : m_pHandle(NULL),
		m_bInited(false),
		m_pInPortDef(NULL), m_pOutPortDef(NULL) {
	m_pPcmBuffer = NULL;
	m_pCb = NULL;
}

AudioEncodeOMX::~AudioEncodeOMX() {
	uninit();
}

bool AudioEncodeOMX::init(int aAudioChannel, int aSampleRate,
		int aSampleBit, int aAACBitRate, bool bPCMNonBlock, string strDeviceName) {
	m_bInited = true;

	m_nChannels = aAudioChannel;
	m_nSampleBit = aSampleBit;
	m_nSampleRate = aSampleRate;
	m_aacBitRate = aAACBitRate;

	createComponents();

	m_eCompressionFormat = OMX_AUDIO_CodingAAC;
	m_outputformat = OMX_AUDIO_AACStreamFormatMP4ADTS;

	printf("audio before allocate Resources\n");
	if (!allocInOutParams()) {
		return false;
	}
	printf("audio after allocate Resources\n");

	m_eState = OMX_StateInvalid;
	m_pCb->EventHandler = AENC_EventHandler;
	m_pCb->EmptyBufferDone = AENC_EmptyBufferDone;
	m_pCb->FillBufferDone = AENC_FillBufferDone;

	m_bExit = false;

	m_pPcmBuffer = (unsigned char*)malloc(1024 * 1024);
	m_nPcmBufferSize = 0;
	m_nPcmBufferMaxSize = 1024 * 1024;

	SempUtils::semp_init(&m_pcmSem, 1);

	m_bThreadStart = false;
	return true;
}

bool AudioEncodeOMX::uninit() {
	if (m_bInited) {
		freeInOutParams();
		destroyComponents();

		if (NULL != m_pPcmBuffer) {
			free(m_pPcmBuffer);
			m_pPcmBuffer = NULL;
		}
		SempUtils::semp_deinit(&m_pcmSem);
	}
	m_bInited = false;

	return true;
}


void AudioEncodeOMX::createComponents() {
	m_port_sem = (semp_t *) malloc(sizeof(semp_t));
	SempUtils::semp_init(m_port_sem, 0);

	m_state_sem = (semp_t *) malloc(sizeof(semp_t));
	SempUtils::semp_init(m_state_sem, 0);

	m_eos_sem = (semp_t *) malloc(sizeof(semp_t));
	SempUtils::semp_init(m_eos_sem, 0);
}

void AudioEncodeOMX::destroyComponents() {
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

bool AudioEncodeOMX::allocInOutParams() {
	m_pCb = (OMX_CALLBACKTYPE*)malloc(sizeof(OMX_CALLBACKTYPE));
	if (NULL == m_pCb) {
		return false;
	}

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

void AudioEncodeOMX::freeInOutParams() {
	if (m_pInPortDef) {
		free(m_pInPortDef);
		m_pInPortDef = NULL;
	}

	if (m_pOutPortDef) {
		free(m_pOutPortDef);
		m_pOutPortDef = NULL;
	}
	if (NULL != m_pCb) {
		free(m_pCb);
		m_pCb = NULL;
	}

	close((int) m_IpBuf_Pipe[0]);
	close((int) m_IpBuf_Pipe[1]);

	close((int) m_OpBuf_Pipe[0]);
	close((int) m_OpBuf_Pipe[1]);

	close((int) m_Event_Pipe[0]);
	close((int) m_Event_Pipe[1]);
}

bool AudioEncodeOMX::initOMXHandle() {
	printf("init Audio OMX Handle\n");
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pHandle,
			(OMX_STRING) "OMX.TI.DSP.AUDENC", this, m_pCb);
	if ((eError != OMX_ErrorNone) || (m_pHandle == NULL)) {
		printf("Couldn't get a handle\n");
		return false;
	}

	printf("before set audio param port definition\n");
	eError = setParamPortDefinition();
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_SendCommand(m_pHandle, OMX_CommandPortEnable, OMX_AUDENC_INPUT_PORT, NULL);
	SempUtils::semp_pend(m_port_sem);

	OMX_SendCommand(m_pHandle, OMX_CommandPortEnable, OMX_AUDENC_OUTPUT_PORT, NULL);
	SempUtils::semp_pend(m_port_sem);

	eError = OMX_GetParameter(m_pHandle, OMX_IndexParamPortDefinition, m_pInPortDef);
	if (eError != OMX_ErrorNone) {
		return NULL;
	}

	eError = OMX_GetParameter(m_pHandle, OMX_IndexParamPortDefinition, m_pOutPortDef);
	if (eError != OMX_ErrorNone) {
		return NULL;
	}

	//  OMX_SendCommand expecting OMX_StateIdle
	eError = OMX_SendCommand(m_pHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);

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

    OMX_U32 pRetrievedEvents;
    struct timeval timeout;

    OMX_S32 nfds = 0;
    FD_ZERO(&m_wr);
    FD_ZERO(&m_er);

	 FD_ZERO(&m_rd);
	 FD_SET(m_Event_Pipe[0], &m_rd);
	 //nfds = MAX (nfds, m_Event_Pipe[0]);
	 timeout.tv_sec = 0;
	 timeout.tv_usec = 0;

	 select(m_Event_Pipe[0] + 1, &m_rd, &m_wr, &m_er, &timeout);

	 if (FD_ISSET(m_Event_Pipe[0], &m_rd)) {
	     read(m_Event_Pipe[0], &pRetrievedEvents, sizeof(pRetrievedEvents));
	     if (pRetrievedEvents == AENC_ENCODER_ERROR_EVENT) {
	          printf("Encoder returned an error while creating. Check your input parameters\n");
	          return false;
	     }
	}

	eError = OMX_SendCommand(m_pHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL );
	if (eError != OMX_ErrorNone) {
		return false;
	}
	SempUtils::semp_pend(m_state_sem);
	printf("Component transitioned from Idle to Execute\n");

	return true;
}

bool AudioEncodeOMX::initialInputResources(int aInputIndex) {
	int toRead = m_pInPortDef->nBufferSize;
	int  tFrames = toRead / (m_nChannels * 2);

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

	m_pInBuff[aInputIndex]->nFilledLen = toRead;
	m_pInBuff[aInputIndex]->nOffset = 0;

	OMX_ERRORTYPE eError = OMX_EmptyThisBuffer(m_pHandle, m_pInBuff[aInputIndex]);
	return (OMX_ErrorNone == eError);
}

OMX_U32 AudioEncodeOMX::fillData(OMX_BUFFERHEADERTYPE * pBuf) {
	OMX_U32 toRead = m_pInPortDef->nBufferSize;

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

	pBuf->nFilledLen = toRead;
	pBuf->nOffset = 0;

	return nRead;
}

OMX_ERRORTYPE AudioEncodeOMX::setParamPortDefinition() {
	OMX_ERRORTYPE eError = OMX_ErrorUndefined;
	OMX_PORT_PARAM_TYPE portInit;
	OMX_AUDIO_PARAM_AACPROFILETYPE tAACProfile;
	OMX_AUDIO_PARAM_PCMMODETYPE tPCMMode;

	if (!m_pHandle) {
		eError = OMX_ErrorBadParameter;
		return eError;
	}
	OMX_TEST_INIT_STRUCT_PTR(&portInit, OMX_PORT_PARAM_TYPE);

	portInit.nPorts = 2;
	portInit.nStartPortNumber = 0;
	eError = OMX_SetParameter(m_pHandle, OMX_IndexParamAudioInit, &portInit);
	if (eError != OMX_ErrorNone) {
		return eError;
	}
	printf("set parameter audio init success \n");

	/* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */

	OMX_TEST_INIT_STRUCT_PTR(m_pInPortDef, OMX_PARAM_PORTDEFINITIONTYPE);
	m_pInPortDef->nPortIndex = OMX_AUDENC_INPUT_PORT;

	OMX_GetParameter(m_pHandle, OMX_IndexParamPortDefinition, m_pInPortDef);

	m_pInPortDef->nBufferCountActual = NUM_OF_IN_BUFFERS;
	m_pInPortDef->format.audio.eEncoding = OMX_AUDIO_CodingPCM;

	eError =
			OMX_SetParameter(m_pHandle, OMX_IndexParamPortDefinition, m_pInPortDef);
	if (eError != OMX_ErrorNone) {
		return eError;
	}

	/* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (output) */
	OMX_TEST_INIT_STRUCT_PTR(m_pOutPortDef, OMX_PARAM_PORTDEFINITIONTYPE);
	m_pOutPortDef->nPortIndex = OMX_AUDENC_OUTPUT_PORT;
	OMX_GetParameter(m_pHandle, OMX_IndexParamPortDefinition, m_pOutPortDef);
	m_pOutPortDef->nBufferCountActual = NUM_OF_OUT_BUFFERS;
	m_pOutPortDef->format.audio.eEncoding = OMX_AUDIO_CodingAAC;

	eError =
			OMX_SetParameter(m_pHandle, OMX_IndexParamPortDefinition, m_pOutPortDef);
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

OMX_ERRORTYPE AudioEncodeOMX::changePortSettings() {
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

bool AudioEncodeOMX::freeAudioEncodeBuffers() {
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


bool AudioEncodeOMX::addListener(IAudioEncodeListener* pListener) {
	for (vector<IAudioEncodeListener*>::iterator it = m_vecListeners.begin();
			it != m_vecListeners.end(); ++it) {
		if (pListener == *(it)) {
			return true;
		}
	}
	m_vecListeners.push_back(pListener);
	return true;
}

bool AudioEncodeOMX::removeListener(IAudioEncodeListener* pListener) {
	for (vector<IAudioEncodeListener*>::iterator it = m_vecListeners.begin();
			it != m_vecListeners.end(); ++it) {
		if (pListener == *(it)) {
			m_vecListeners.erase(it);
			return true;
		}
	}
	return true;
}

