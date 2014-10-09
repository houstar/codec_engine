#include "Mp4Player.h"
#include "OMXComponentUtils.h"

void* Mp4FilePlayer::InputBitStreamReadTask(void *threadsArg) {
	stInputThreadArgs *pDecArgs = (stInputThreadArgs*) threadsArg;
	Mp4FilePlayer* pSelf = (Mp4FilePlayer*) pDecArgs->ptrAppData;
	pSelf->onReadTask();
	return NULL;
}

void* Mp4FilePlayer::ConnInConnOutTask(void *threadsArg) {
	stConnInConnOutThreadArgs *pInOutParam =
			(stConnInConnOutThreadArgs*) threadsArg;
	ComponentWrapper *thisComp = pInOutParam->pComponent;
	Mp4FilePlayer* pSelf = (Mp4FilePlayer*) pInOutParam->pParent;
	pSelf->onConnectTask(thisComp);
	return NULL;
}

OMX_ERRORTYPE Mp4FilePlayer::procCallbackEventHandler(OMX_HANDLETYPE hComponent,
		OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1,
		OMX_U32 nData2, OMX_PTR pEventData) {
	ComponentWrapper *comp = (ComponentWrapper *) ptrAppData;

	printf("got event %d\n", eEvent);
	if (eEvent == OMX_EventCmdComplete) {
		if (nData1 == OMX_CommandStateSet) {
			printf("State changed to: ");
			switch ((int) nData2) {
			case OMX_StateInvalid:
				printf("OMX_StateInvalid \n");
				break;
			case OMX_StateLoaded:
				printf("OMX_StateLoaded \n");
				break;
			case OMX_StateIdle:
				printf("OMX_StateIdle \n");
				break;
			case OMX_StateExecuting:
				printf("OMX_StateExecuting \n");
				break;
			case OMX_StatePause:
				printf("OMX_StatePause\n");
				break;
			case OMX_StateWaitForResources:
				printf("OMX_StateWaitForResources\n");
				break;
			}
			OMXComponentUtils::postStateSem(comp);
		} else if (OMX_CommandFlush == nData1) {
			OMXComponentUtils::postStateSem(comp);
		} else if (OMX_CommandPortEnable || OMX_CommandPortDisable) {
			printf("Enable/Disable Event \n");
			OMXComponentUtils::postPortSem(comp);
		}
	} else if (eEvent == OMX_EventBufferFlag) {
		printf("OMX_EventBufferFlag \n");
		if ((int) nData2 == OMX_BUFFERFLAG_EOS) {

		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE Mp4FilePlayer::procCallbackEmptyBufferDone(
		OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData,
		OMX_BUFFERHEADERTYPE *pBuffer) {
	ComponentWrapper *thisComp = (ComponentWrapper *) ptrAppData;
	InportParams *inPortParamsPtr;
	PipeMsg localPipeMsg;

	OMX_ERRORTYPE eError = OMX_ErrorNone;
	int retVal = 0;

	inPortParamsPtr = thisComp->inPortParams + pBuffer->nInputPortIndex;

	/* if the buffer is from file i/o, write the free buffer header into ipbuf
	 pipe, else keep it in its local pipe. From local pipe It would be given to
	 remote component as "consumed buffer " */

	if (!inPortParamsPtr->connInfo.remotePipe[0]) {
		/* write the empty buffer pointer to input pipe */
		retVal = write(inPortParamsPtr->ipBufPipe[1], &pBuffer,
				sizeof(pBuffer));

		if (sizeof(pBuffer) != retVal) {
			printf("Error writing into Input buffer i/p Pipe!\n");
			eError = OMX_ErrorNotReady;
			return eError;
		}
	} else {
		/* Create a message that EBD is done and this buffer is ready to be
		 recycled. This message will be read in buffer processing thread and
		 remote component will be indicated about its status */
		localPipeMsg.cmd = ePipeCmdEmptyBufferDone;
		localPipeMsg.pbufHeader = pBuffer;
		retVal = write(thisComp->localPipe[1], &localPipeMsg, sizeof(PipeMsg));
		if (sizeof(PipeMsg) != retVal) {
			printf("Error writing into local Pipe!\n");
			eError = OMX_ErrorNotReady;
			return eError;
		}
	}

	return eError;
}

OMX_ERRORTYPE Mp4FilePlayer::procCallbackFillBufferDone(
		OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData,
		OMX_BUFFERHEADERTYPE *pBuffer) {
	ComponentWrapper *thisComp = (ComponentWrapper *) ptrAppData;
	OutportParams *outPortParamsPtr;
	PipeMsg localPipeMsg;

	OMX_ERRORTYPE eError = OMX_ErrorNone;
	int retVal = 0;

	/* get the pipe corrsponding to this port, portIndex is part of bufferheader
	 structure */
	outPortParamsPtr = thisComp->outPortParams
			+ (pBuffer->nOutputPortIndex - thisComp->startOutportIndex);

	/* if the buffer is from file i/o, write the free buffer header into outbuf
	 pipe, else keep it in its local pipe. From local pipe It would be given to
	 remote component as "filled buffer " */
	if (!outPortParamsPtr->connInfo.remotePipe[0]) {
		/* write the empty buffer pointer to input pipe */
		retVal = write(outPortParamsPtr->opBufPipe[1], &pBuffer,
				sizeof(pBuffer));

		if (sizeof(pBuffer) != retVal) {
			printf("Error writing to Input buffer i/p Pipe!\n");
			eError = OMX_ErrorNotReady;
			return eError;
		}
	} else {
		/* Create a message that FBD is done and this buffer is ready to be used by
		 other component. This message will be read in buffer processing thread
		 and remote component will be indicated about its status */
		localPipeMsg.cmd = ePipeCmdFillBufferDone;
		localPipeMsg.pbufHeader = pBuffer;
		retVal = write(thisComp->localPipe[1], &localPipeMsg, sizeof(PipeMsg));

		if (sizeof(PipeMsg) != retVal) {
			printf("Error writing to local Pipe!\n");
			eError = OMX_ErrorNotReady;
			return eError;
		}
	}
	return eError;
}

Mp4FilePlayer::Mp4FilePlayer() :
		m_pDecHandle(NULL), m_pScaleHandle(NULL), m_pDisplayHandle(NULL), m_pctrlHandle(
		NULL), m_decComponent(NULL), m_scaleComponent(NULL), m_displayComponent(
		NULL), m_mp4Reader(NULL), m_bExit(false), m_pInBuffer(NULL), m_nInBufferLen(
				0), m_nInBufferMaxLen(0) {

}

Mp4FilePlayer::~Mp4FilePlayer() {
	uninit();
}

bool Mp4FilePlayer::init(const char* pFileName) {
	if (NULL == pFileName) {
		return false;
	}
	if (NULL == m_mp4Reader) {
		m_mp4Reader = new CMP4FileReader();
	}
	BOOL ret = m_mp4Reader->Read(pFileName);
	if (!ret) {
		return false;
	}

	m_mp4Reader->GetVideoResolution((int&) m_inWidth, (int&) m_inHeight);
	m_nFrameRate = m_mp4Reader->getVideoFrameRate();
	m_displayId = 0;
	DecodeUtils::Decode_ParserInit(&m_pc);

	createComponents();

	allocInOutParams();

	m_pInBuffer = (unsigned char *) malloc(READSIZE);
	m_nInBufferLen = 0;
	m_nInBufferMaxLen = READSIZE;

	m_bGetHeader = false;
	m_videoSampleId = 1;
	m_videoSampleCount = m_mp4Reader->GetVideoSampleCount();

	m_pCb.EventHandler = procCallbackEventHandler;
	m_pCb.EmptyBufferDone = procCallbackEmptyBufferDone;
	m_pCb.FillBufferDone = procCallbackFillBufferDone;
}

bool Mp4FilePlayer::uninit() {
	if (!m_bExit) {
		return false;
	}
	freeInOutParams();
	printf("base mode before destroy components \n");
	destroyComponents();
	printf("base mode after destroy components \n");

	if (m_pc.readBuf) {
		free(m_pc.readBuf);
		m_pc.readBuf = NULL;
	}
	if (m_pInBuffer) {
		free(m_pInBuffer);
		m_pInBuffer = NULL;
	}
	m_bExit = false;
	if (NULL == m_mp4Reader) {
		delete m_mp4Reader;
		m_mp4Reader = NULL;
	}
	return true;
}

bool Mp4FilePlayer::play() {
	if (m_bExit) {
		return false;
	}
	if (!initOMXHandle()) {
		return false;
	}
	return startOMXThread();
}

bool Mp4FilePlayer::stop() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	PipeMsg pipeMsg;
	OMX_S32 ret_value;
	m_bExit = true;

	pipeMsg.cmd = ePipeCmdExit;

	write(m_decComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
	write(m_scaleComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
	write(m_displayComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));

	/* change state to idle so that buffers processing would stop */
	eError = OMX_SendCommand (m_pDecHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMXComponentUtils::waitStateSem(m_decComponent);
	printf(" decoder state idle \n ");

	eError = OMX_SendCommand(m_pScaleHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (OMX_ErrorNone != eError) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_scaleComponent);

	/* change state to execute so that buffers processing can stop */
	eError = OMX_SendCommand (m_pDisplayHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_displayComponent);
	printf(" display state idle \n ");

	eError = OMX_SendCommand (m_pctrlHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_displayComponent);
	printf(" display control state idle \n ");

	/* change the state to loded */
	eError = OMX_SendCommand (m_pDecHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	if (!freeDecBuffers()) {
		return false;
	}

	eError = OMX_SendCommand(m_pScaleHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (OMX_ErrorNone != eError) {
		return false;
	}
	if (!freeScaleBuffers()) {
		return false;
	}

	eError = OMX_SendCommand (m_pDisplayHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	if (!freeDisplayBuffers()) {
		return false;
	}
	eError = OMX_SendCommand (m_pctrlHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_displayComponent);
	printf(" ctrl-dc state loaded \n ");

	eError = OMX_FreeHandle(m_pDecHandle);
	if ((eError != OMX_ErrorNone)) {
		return false;
	}
	printf(" decoder free handle \n");

	eError = OMX_FreeHandle(m_pScaleHandle);
	if (OMX_ErrorNone != eError) {
		return false;
	}

	eError = OMX_FreeHandle(m_pDisplayHandle);
	if ((eError != OMX_ErrorNone)) {
		return false;
	}

	printf(" display free handle \n");

	eError = OMX_FreeHandle(m_pctrlHandle);
	if ((eError != OMX_ErrorNone)) {
		return false;
	}

	pthread_join(m_decComponent->inDataStrmThrdId, (void **) &ret_value);
	pthread_join(m_decComponent->connDataStrmThrdId, (void **) &ret_value);
	pthread_join(m_scaleComponent->connDataStrmThrdId, (void**) &ret_value);
	pthread_join(m_displayComponent->connDataStrmThrdId, (void **) &ret_value);

	printf("before m_capture vga .stop\n");

	uninit();
	m_bExit = false;
	return true;
}

bool Mp4FilePlayer::pause() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	eError = OMX_SendCommand (m_pDecHandle, OMX_CommandStateSet,
			OMX_StatePause, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_decComponent);

	eError = OMX_SendCommand(m_pScaleHandle, OMX_CommandStateSet,
			OMX_StatePause, NULL);
	if (OMX_ErrorNone != eError) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_scaleComponent);

	eError = OMX_SendCommand (m_pDisplayHandle, OMX_CommandStateSet,
			OMX_StatePause, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_displayComponent);

	eError = OMX_SendCommand (m_pctrlHandle, OMX_CommandStateSet,
			OMX_StatePause, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_displayComponent);
	return TRUE;
}

bool Mp4FilePlayer::resume() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	eError = OMX_SendCommand (m_pDecHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_decComponent);

	eError = OMX_SendCommand(m_pScaleHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (OMX_ErrorNone != eError) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_scaleComponent);

	eError = OMX_SendCommand (m_pDisplayHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_displayComponent);

	eError = OMX_SendCommand (m_pctrlHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_displayComponent);
	return TRUE;
}

void Mp4FilePlayer::createComponents() {
	OMXComponentUtils::initComponent(m_decComponent);

	OMXComponentUtils::initComponent(m_displayComponent);

	OMXComponentUtils::initComponent(m_scaleComponent);

	m_decComponent->numInport = 1;
	m_decComponent->numOutport = 1;
	m_decComponent->startOutportIndex = 1;

	m_displayComponent->numInport = 1;
	m_displayComponent->numOutport = 0;
	m_displayComponent->startOutportIndex = 0;

	m_scaleComponent->numInport = 1;
	m_scaleComponent->numOutport = 2;
	m_scaleComponent->startOutportIndex = OMX_VFPC_NUM_INPUT_PORTS;
}

void Mp4FilePlayer::destroyComponents() {
	OMXComponentUtils::uninitComponent(m_displayComponent);
	OMXComponentUtils::uninitComponent(m_scaleComponent);
	OMXComponentUtils::uninitComponent(m_decComponent);
}

void Mp4FilePlayer::allocInOutParams() {
	InportParams *inPortParamsPtr;
	OutportParams *outPortParamsPtr;

	OMXComponentUtils::allocInOutParams(m_decComponent);
	OMXComponentUtils::allocInOutParams(m_displayComponent);
	OMXComponentUtils::allocInOutParams(m_scaleComponent);

	for (OMX_U32 i = 0; i < m_decComponent->numInport; i++) {
		inPortParamsPtr = m_decComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual = K720PBufferCountActual;
		inPortParamsPtr->nBufferSize = m_inHeight * m_inWidth;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (OMX_U32 i = 0; i < m_decComponent->numOutport; i++) {
		outPortParamsPtr = m_decComponent->outPortParams + i;
		outPortParamsPtr->nBufferCountActual = K720PBufferCountActual;
		/* H264 decoder uses padding on both sides, as well requires 128 byte
		 alignment so this value is calcualtes as follows, decoder o/p is always
		 YUV420 packet semi planner so * 1.5 */
		outPortParamsPtr->nBufferSize =
				(UTIL_ALIGN ((m_inWidth + (2 * PADX_H264)), 128)
						* ((((m_inHeight + 15) & 0xfffffff0) + (4 * PADY_H264)))
						* 3) >> 1;
		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int *) m_decComponent->localPipe);

	for (OMX_U32 i = 0; i < m_scaleComponent->numInport; ++i) {
		inPortParamsPtr = m_scaleComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual = K720PBufferCountActual;
		inPortParamsPtr->nBufferSize =
				(UTIL_ALIGN ((m_inWidth + (2 * PADX_H264)), 128)
						* ((((m_inHeight + 15) & 0xfffffff0) + (4 * PADY_H264)))
						* 3) >> 1;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (OMX_U32 i = 0; i < m_scaleComponent->numOutport; i++) {
		outPortParamsPtr = m_scaleComponent->outPortParams + i;
		outPortParamsPtr->nBufferCountActual =
				K720PBufferCountActual;
		if (!(i % 2)) {
			outPortParamsPtr->nBufferSize = (UTIL_ALIGN(m_inHeight, 32))
					* (UTIL_ALIGN(m_inWidth, 32)) * 2;
			outPortParamsPtr->nBufferCountActual =
					K720PBufferCountActual;

			if (1 == m_displayId) {
				outPortParamsPtr->nBufferSize = DISPLAY_HEIGHT * DISPLAY_WIDTH
						* 2;
			}

		} else {
			outPortParamsPtr->nBufferSize = ((UTIL_ALIGN(m_inHeight, 32))
					* (UTIL_ALIGN(m_inWidth, 32)) * 3) >> 1;
			outPortParamsPtr->nBufferCountActual =
					K720PBufferCountActual;
		}

		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int *) m_scaleComponent->localPipe);

	for (OMX_U32 i = 0; i < m_displayComponent->numInport; i++) {
		inPortParamsPtr = m_displayComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual =
				K720PBufferCountActual;
		inPortParamsPtr->nBufferSize = ((UTIL_ALIGN(m_inHeight, 32))
				* (UTIL_ALIGN(m_inWidth, 32)) * 2);

		/* configuring for custom display parameters */
		if (1 == m_displayId) {
			inPortParamsPtr->nBufferSize = DISPLAY_HEIGHT * DISPLAY_WIDTH * 2;
		}
		/* For SD display 420 output */
		if (2 == m_displayId) {
			inPortParamsPtr->nBufferSize = (SD_DISPLAY_HEIGHT * SD_DISPLAY_WIDTH
					* 3) >> 1;
		}
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	pipe((int *) m_displayComponent->localPipe);
}

void Mp4FilePlayer::freeInOutParams() {
	OMXComponentUtils::freeInOutParams(m_displayComponent);
	OMXComponentUtils::freeInOutParams(m_scaleComponent);
	OMXComponentUtils::freeInOutParams(m_decComponent);
}

BOOL Mp4FilePlayer::initOMXHandle() {
	OMX_U32 i;
	OMX_ERRORTYPE eError;
	printf("init omx handle\n");
	if (!initDecOMXHandle()) {
		printf("init dec OMX handle failed\n");
		return FALSE;
	}
	printf("init dec OMX handle OK\n");

	if (!initScaleOMXHandle()) {
		printf("init scale OMX handle failed\n");
		return FALSE;
	}

	if (!initDisplayOMXHandle()) {
		return FALSE;
	}

	establishConnections();

	/* OMX_SendCommand expecting OMX_StateIdle, after this command component
	 would create codec, and will wait for all buffers to be allocated as per
	 omx buffers are created during loaded to Idle transition ( if ports are
	 enabled ) */
	eError = OMX_SendCommand (m_pDecHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	/* Allocate I/O Buffers; component would allocate buffers and would return
	 the buffer header containing the pointer to buffer */
	for (i = 0; i < m_decComponent->inPortParams->nBufferCountActual; i++) {
		eError = OMX_AllocateBuffer (m_pDecHandle,
				&m_decComponent->inPortParams->pInBuff[i],
				OMX_VIDDEC_INPUT_PORT, this,
				m_decComponent->inPortParams->nBufferSize);
		if (eError != OMX_ErrorNone) {
			return FALSE;
		}
	}
	printf(" decoder inport buffers allocated \n ");

	for (i = 0; i < m_decComponent->outPortParams->nBufferCountActual; i++) {
		eError = OMX_AllocateBuffer (m_pDecHandle,
				&m_decComponent->outPortParams->pOutBuff[i],
				OMX_VIDDEC_OUTPUT_PORT, this,
				m_decComponent->outPortParams->nBufferSize);
		if (eError != OMX_ErrorNone) {
			return FALSE;
		}
	}

	printf(" decoder outport buffers allocated \n ");

	/* Wait for initialization to complete.. Wait for Idle stete of component
	 after all buffers are alloacted componet would chnage to idle */
	OMXComponentUtils::waitStateSem(m_decComponent);

	printf(" decoder state IDLE \n ");

	eError = OMX_SendCommand (m_pScaleHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	for (i = 0; i < m_scaleComponent->inPortParams->nBufferCountActual; i++) {

		eError = OMX_UseBuffer (m_pScaleHandle,
				&m_scaleComponent->inPortParams->pInBuff[i],
				OMX_VFPC_INPUT_PORT_START_INDEX, this,
				m_decComponent->outPortParams->nBufferSize,
				m_decComponent->outPortParams->pOutBuff[i]->pBuffer);

		if (eError != OMX_ErrorNone) {
			return FALSE;
		}
	}
	printf(" scale input port use buffer done \n ");

	for (OMX_U32 j = 0; j < m_scaleComponent->numOutport; j++) {
		OutportParams *outPortParamsPtr = m_scaleComponent->outPortParams + j;
		/* buffer alloaction for output port */
		for (i = 0; i < outPortParamsPtr->nBufferCountActual; i++) {
			eError = OMX_AllocateBuffer (m_pScaleHandle,
					&outPortParamsPtr->pOutBuff[i],
					OMX_VFPC_OUTPUT_PORT_START_INDEX + j,
					this, outPortParamsPtr->nBufferSize);
			if (eError != OMX_ErrorNone) {
				return FALSE;
			}
		}
	}
	printf(" scale outport buffers allocated \n ");

	OMXComponentUtils::waitStateSem(m_scaleComponent);

	printf(" scale is in IDLE state \n");

	eError = OMX_SendCommand (m_pctrlHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	OMXComponentUtils::waitStateSem(m_displayComponent);

	printf(" ctrl-dc state IDLE \n ");

	eError = OMX_SendCommand (m_pDisplayHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	for (i = 0; i < m_displayComponent->inPortParams->nBufferCountActual; i++) {
		eError = OMX_UseBuffer (m_pDisplayHandle,
				&m_displayComponent->inPortParams->pInBuff[i],
				OMX_VFDC_INPUT_PORT_START_INDEX,
				this,
				m_scaleComponent->outPortParams->nBufferSize,
				m_scaleComponent->outPortParams->pOutBuff[i]->pBuffer);
	}
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	OMXComponentUtils::waitStateSem(m_displayComponent);

	printf(" display state IDLE \n ");

	eError = OMX_SendCommand (m_pctrlHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	OMXComponentUtils::waitStateSem(m_displayComponent);

	printf(" display control state execute \n ");

	/* change state to execute so that buffers processing can start */
	eError = OMX_SendCommand (m_pDisplayHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	OMXComponentUtils::waitStateSem(m_displayComponent);

	printf(" display state execute \n ");

	eError = OMX_SendCommand (m_pScaleHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	OMXComponentUtils::waitStateSem(m_scaleComponent);

	/* change state to execute so that buffers processing can start */
	eError = OMX_SendCommand (m_pDecHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	OMXComponentUtils::waitStateSem(m_decComponent);

	printf(" decoder state execute \n ");

	return TRUE;
}

BOOL Mp4FilePlayer::establishConnections() {
	OMXComponentUtils::connectComponents(m_decComponent, OMX_VIDDEC_OUTPUT_PORT,
			m_scaleComponent, OMX_VFPC_INPUT_PORT_START_INDEX);

	OMXComponentUtils::connectComponents(m_scaleComponent,
	OMX_VFPC_OUTPUT_PORT_START_INDEX, m_displayComponent,
	OMX_VFDC_INPUT_PORT_START_INDEX);

	return TRUE;
}

bool Mp4FilePlayer::startOMXThread() {
	pthread_attr_init(&m_scaleComponent->ThreadAttr);
	OMXComponentUtils::initialOutputResources(m_scaleComponent);
	m_scaleParams.pParent = this;
	m_scaleParams.pComponent = m_scaleComponent;
	if (0
			!= pthread_create(&m_scaleComponent->connDataStrmThrdId,
					&m_scaleComponent->ThreadAttr, ConnInConnOutTask,
					&m_scaleParams)) {
		printf("Create_Task failed !");
		return false;
	}

	pthread_attr_init(&m_decComponent->ThreadAttr);
	m_inputThreadArgs.ptrAppData = this;
	m_inputThreadArgs.instId = 0;
	if (0
			!= pthread_create(&m_decComponent->inDataStrmThrdId,
					&m_decComponent->ThreadAttr, InputBitStreamReadTask,
					&m_inputThreadArgs)) {
		printf("Create_Task failed !");
		return false;
	}
	printf(" file read thread created \n ");

	pthread_attr_init(&m_decComponent->ThreadAttr);
	OMXComponentUtils::initialOutputResources(m_decComponent);
	m_decParams.pParent = this;
	m_decParams.pComponent = m_decComponent;
	if (0
			!= pthread_create(&m_decComponent->connDataStrmThrdId,
					&m_decComponent->ThreadAttr, ConnInConnOutTask,
					&m_decParams)) {
		printf("Create_Task failed !");
		return false;
	}

	printf(" decoder connect thread created \n ");

	pthread_attr_init(&m_displayComponent->ThreadAttr);
	OMXComponentUtils::initialOutputResources(m_displayComponent);
	m_displayParams.pParent = this;
	m_displayParams.pComponent = m_displayComponent;
	if (0
			!= pthread_create(&m_displayComponent->connDataStrmThrdId,
					&m_displayComponent->ThreadAttr, ConnInConnOutTask,
					&m_displayParams)) {
		printf("Create_Task failed !");
		return false;
	}
	return true;
}

BOOL Mp4FilePlayer::initDecOMXHandle() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	eError = OMX_GetHandle(&m_pDecHandle, (OMX_STRING) "OMX.TI.DUCATI.VIDDEC",
			m_decComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pDecHandle == NULL)) {
		printf("dec handle is null %x\n", eError);
		return FALSE;
	}

	m_decComponent->handle = m_pDecHandle;
	if (!setDecodeParams()) {
		return FALSE;
	}
	return TRUE;
}

BOOL Mp4FilePlayer::initScaleOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pScaleHandle,
			(OMX_STRING) "OMX.TI.VPSSM3.VFPC.INDTXSCWB", m_scaleComponent,
			&m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pScaleHandle == NULL)) {
		return FALSE;
	}
	m_scaleComponent->handle = m_pScaleHandle;

	if (!setScalarParams()) {
		return FALSE;
	}
	OMX_SendCommand(m_pScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_INPUT_PORT_START_INDEX, NULL);

	OMXComponentUtils::waitPortSem(m_scaleComponent);

	printf("enable scalar output port \n");
	OMX_SendCommand(m_pScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);
	OMXComponentUtils::waitPortSem(m_scaleComponent);

	return TRUE;
}

BOOL Mp4FilePlayer::initDisplayOMXHandle() {
	/*****************************************************************************/
	/* Noise filter is used to convert 422 o/p to 420 for SD display only */
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	/* Create the display component */
	/* getting display component handle */
	eError = OMX_GetHandle(&m_pDisplayHandle, "OMX.TI.VPSSM3.VFDC",
			m_displayComponent, &m_pCb);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	m_displayComponent->handle = m_pDisplayHandle;
	/* getting display controller component handle, Display contrller is
	 implemented as an OMX component, however it does not have any input or
	 output ports. It is used only for controling display hw */
	eError = OMX_GetHandle(&m_pctrlHandle, "OMX.TI.VPSSM3.CTRL.DC",
			m_displayComponent, &m_pCb);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	printf("found handle %p for component %s\n", m_pctrlHandle,
			"OMX.TI.VPSSM3.CTRL.DC");
	/* omx calls are made in this function for setting the parameters for display
	 component, For clarity purpose it is written as separate function */
	if (!setDisplayParams()) {
		return FALSE;
	}
	/* as per openmax specs all the ports should be enabled by default but EZSDK
	 OMX component does not enable it hence we manually need to enable it */
	printf("enable input port \n");

	OMX_SendCommand(m_pDisplayHandle, OMX_CommandPortEnable,
			OMX_VFDC_INPUT_PORT_START_INDEX, NULL);

	/* wait for port to get enabled, event handler would be notified from the
	 component after enabling the port, which inturn would post this semaphore */
	OMXComponentUtils::waitPortSem(m_displayComponent);

	return TRUE;
}

bool Mp4FilePlayer::setDecodeParams() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_PORT_PARAM_TYPE portInit;
	OMX_PARAM_PORTDEFINITIONTYPE pInPortDef, pOutPortDef;
	OMX_VIDEO_PARAM_STATICPARAMS tStaticParam;
	OMX_PARAM_COMPPORT_NOTIFYTYPE pNotifyType;

	OMX_INIT_PARAM(&portInit);

	portInit.nPorts = 2;
	portInit.nStartPortNumber = 0;
	eError =
	OMX_SetParameter (m_pDecHandle, OMX_IndexParamVideoInit, &portInit);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */
	OMX_INIT_PARAM(&pInPortDef);

	/* populate the input port definataion structure, It is Standard OpenMax
	 structure */
	/* set the port index */
	pInPortDef.nPortIndex = OMX_VIDDEC_INPUT_PORT;
	/* It is input port so direction is set as Input, Empty buffers call would be
	 accepted based on this */
	pInPortDef.eDir = OMX_DirInput;
	/* number of buffers are set here */
	pInPortDef.nBufferCountActual = K720PBufferCountActual;
	/* buffer size by deafult is assumed as width * height for input bitstream
	 which would suffice most of the cases */
	pInPortDef.nBufferSize = m_inWidth * m_inHeight;

	pInPortDef.bEnabled = OMX_TRUE;
	pInPortDef.bPopulated = OMX_FALSE;

	/* OMX_VIDEO_PORTDEFINITION values for input port */
	/* set the width and height, used for buffer size calculation */
	pInPortDef.format.video.cMIMEType = "H264";
	pInPortDef.format.video.nFrameWidth = m_inWidth;
	pInPortDef.format.video.nFrameHeight = m_inHeight;
	/* for bitstream buffer stride is not a valid parameter */
	pInPortDef.format.video.nStride = -1;
	/* component supports only frame based processing */

	/* bitrate does not matter for decoder */
	//pInPortDef.format.video.nBitrate = 104857600;
	/* as per openmax frame rate is in Q16 format */
	pInPortDef.format.video.xFramerate = (m_nFrameRate) << 16;
	/* input port would receive encoded stream */
	pInPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	/* this is codec setting, OMX component does not support it */
	/* color format is irrelavant */
	pInPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420Planar;

	eError =
	OMX_SetParameter (m_pDecHandle, OMX_IndexParamPortDefinition, &pInPortDef);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (output) */
	OMX_INIT_PARAM(&pOutPortDef);

	/* setting the port index for output port, properties are set based on this
	 index */
	pOutPortDef.nPortIndex = OMX_VIDDEC_OUTPUT_PORT;
	pOutPortDef.eDir = OMX_DirOutput;
	/* componet would expect these numbers of buffers to be allocated */
	pOutPortDef.nBufferCountActual = K720PBufferCountActual;

	/* codec requires padded height and width and width needs to be aligned at
	 128 byte boundary */

	pOutPortDef.nBufferSize = ((((m_inWidth + (2 * PADX_H264) + 127)
			& 0xFFFFFF80)
			* ((((m_inHeight + 15) & 0xfffffff0) + (4 * PADY_H264))) * 3) >> 1);
	pOutPortDef.bEnabled = OMX_TRUE;
	pOutPortDef.bPopulated = OMX_FALSE;
	/* currently component alloactes contigous buffers with 128 alignment, these
	 values are do't care */

	/* OMX_VIDEO_PORTDEFINITION values for output port */
	pOutPortDef.format.video.cMIMEType = "H264";
	pOutPortDef.format.video.nFrameWidth = m_inWidth;
	pOutPortDef.format.video.nFrameHeight = ((m_inHeight + 15) & 0xfffffff0);
	/* stride is set as buffer width */
	pOutPortDef.format.video.nStride =
	UTIL_ALIGN ((m_inWidth + (2 * PADX_H264)), 128);

	/* bitrate does not matter for decoder */
	//pOutPortDef.format.video.nBitrate = 25000000;
	/* as per openmax frame rate is in Q16 format */
	pOutPortDef.format.video.xFramerate = (m_nFrameRate) << 16;
	/* output is raw YUV 420 SP format, It support only this */
	pOutPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	pOutPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;

	eError =
	OMX_SetParameter (m_pDecHandle, OMX_IndexParamPortDefinition, &pOutPortDef);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	eError =
	OMX_GetParameter (m_pDecHandle, OMX_IndexParamPortDefinition, &pOutPortDef);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	m_nDecStride = pOutPortDef.format.video.nStride;

	/* Make VDEC execute periodically based on fps */
	OMX_INIT_PARAM(&pNotifyType);
	pNotifyType.eNotifyType = OMX_NOTIFY_TYPE_NONE;
	pNotifyType.nPortIndex = OMX_VIDDEC_INPUT_PORT;
	eError =
			OMX_SetParameter (m_pDecHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamCompPortNotifyType,
					&pNotifyType);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	pNotifyType.eNotifyType = OMX_NOTIFY_TYPE_NONE;
	pNotifyType.nPortIndex = OMX_VIDDEC_OUTPUT_PORT;
	eError =
			OMX_SetParameter (m_pDecHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamCompPortNotifyType,
					&pNotifyType);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_INIT_PARAM(&tStaticParam);

	tStaticParam.nPortIndex = OMX_VIDDEC_OUTPUT_PORT;

	eError =
			OMX_GetParameter (m_pDecHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
					&tStaticParam);

	tStaticParam.videoStaticParams.h264DecStaticParams.presetLevelIdc =
			IH264VDEC_LEVEL42;

	eError =
			OMX_SetParameter (m_pDecHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
					&tStaticParam);
	return true;
}

bool Mp4FilePlayer::setScalarParams() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_PARAM_BUFFER_MEMORYTYPE memTypeCfg;
	OMX_PARAM_PORTDEFINITIONTYPE paramPort;
	OMX_PARAM_VFPC_NUMCHANNELPERHANDLE sNumChPerHandle;
	OMX_CONFIG_ALG_ENABLE algEnable;
	OMX_CONFIG_VIDCHANNEL_RESOLUTION chResolution;

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError =
			OMX_SetParameter (m_pScaleHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
					&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Setting Memory type at output port to Raw Memory */
	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError =
			OMX_SetParameter (m_pScaleHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
					&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* set input height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

	OMX_GetParameter(m_pScaleHandle, OMX_IndexParamPortDefinition, &paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = m_inWidth;
	paramPort.format.video.nFrameHeight = m_inHeight;

	/* Scalar is connceted to H264 decoder, whose stride is different than width*/
	paramPort.format.video.nStride = m_nDecStride;
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	paramPort.nBufferSize = (paramPort.format.video.nStride * m_inHeight * 3)
			>> 1;

	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = K720PBufferCountActual;
	eError =
	OMX_SetParameter (m_pScaleHandle, OMX_IndexParamPortDefinition,
			&paramPort);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* set output height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	OMX_GetParameter(m_pScaleHandle, OMX_IndexParamPortDefinition, &paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = m_inWidth;
	paramPort.format.video.nFrameHeight = m_inHeight;

	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = K720PBufferCountActual;
	/* scalar buffer pitch should be multiple of 16 */
	paramPort.format.video.nStride =
	UTIL_ALIGN(m_inWidth * 2, 16);

	if (1 == m_displayId) {
		/*For the case of On-chip HDMI as display device*/
		paramPort.format.video.nFrameWidth = DISPLAY_WIDTH / 2;
		paramPort.format.video.nFrameHeight = DISPLAY_HEIGHT / 2;
		paramPort.format.video.nStride = (DISPLAY_WIDTH / 2) * 2;
	}

	if (2 == m_displayId) {
		/*For the case of On-chip HDMI as display device*/
		paramPort.format.video.nFrameWidth = SD_DISPLAY_WIDTH;
		paramPort.format.video.nFrameHeight = SD_DISPLAY_HEIGHT;
		paramPort.format.video.nStride = SD_DISPLAY_WIDTH * 2;
	}

	paramPort.nBufferSize = paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight;

	eError =
	OMX_SetParameter (m_pScaleHandle, OMX_IndexParamPortDefinition,
			&paramPort);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* set number of channles */
	OMX_INIT_PARAM(&sNumChPerHandle);
	sNumChPerHandle.nNumChannelsPerHandle = 1;
	eError = OMX_SetParameter (m_pScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexParamVFPCNumChPerHandle,
			&sNumChPerHandle);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* set VFPC input and output resolution information */
	printf("set input resolution \n");

	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = m_inWidth;
	chResolution.Frm0Height = m_inHeight;
	chResolution.Frm0Pitch = m_nDecStride;
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = m_inWidth;
	chResolution.FrmCropHeight = m_inHeight;
	chResolution.eDir = OMX_DirInput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	printf("set output resolution \n");
	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = m_inWidth;
	chResolution.Frm0Height = m_inHeight;

	chResolution.Frm0Pitch = UTIL_ALIGN(m_inWidth * 2, 16);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = 0;
	chResolution.FrmCropHeight = 0;
	chResolution.eDir = OMX_DirOutput;
	chResolution.nChId = 0;

	if (1 == m_displayId) {
		/* on secondary display, it is scaled to display size */
		chResolution.Frm0Width = DISPLAY_WIDTH / 2;
		chResolution.Frm0Height = DISPLAY_HEIGHT / 2;
		chResolution.Frm0Pitch = (DISPLAY_WIDTH / 2) * 2;
	}

	if (2 == m_displayId) {
		/* on secondary display, it is scaled to display size */
		chResolution.Frm0Width = SD_DISPLAY_WIDTH;
		chResolution.Frm0Height = SD_DISPLAY_HEIGHT;
		chResolution.Frm0Pitch = SD_DISPLAY_WIDTH * 2;
	}

	eError = OMX_SetConfig (m_pScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* disable algo bypass mode */
	OMX_INIT_PARAM(&algEnable);
	algEnable.nPortIndex = 0;
	algEnable.nChId = 0;
	algEnable.bAlgBypass = OMX_TRUE;

	eError = OMX_SetConfig (m_pScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable, &algEnable);
	return true;
}

bool Mp4FilePlayer::setDisplayParams() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_PARAM_BUFFER_MEMORYTYPE memTypeCfg;
	OMX_PARAM_PORTDEFINITIONTYPE paramPort;
	OMX_PARAM_VFDC_DRIVERINSTID driverId;
	OMX_PARAM_VFDC_CREATEMOSAICLAYOUT mosaicLayout;
	OMX_CONFIG_VFDC_MOSAICLAYOUT_PORT2WINMAP port2Winmap;

	OMX_INIT_PARAM(&paramPort);

	/* set input height/width and color format */
	paramPort.nPortIndex = OMX_VFDC_INPUT_PORT_START_INDEX;
	OMX_GetParameter(m_pDisplayHandle, OMX_IndexParamPortDefinition,
			&paramPort);
	paramPort.nPortIndex = OMX_VFDC_INPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = m_inWidth;
	paramPort.format.video.nFrameHeight = m_inHeight;
	paramPort.format.video.nStride = m_inWidth * 2; /**< should be twice of width*/
	paramPort.nBufferCountActual = K720PBufferCountActual;
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;

	if (1 == m_displayId) {
		/* on secondary display, it is scaled to display size */
		paramPort.format.video.nFrameWidth = DISPLAY_WIDTH;
		paramPort.format.video.nFrameHeight = DISPLAY_HEIGHT;
		paramPort.format.video.nStride = DISPLAY_WIDTH * 2;
	}

	if (2 == m_displayId) {
		/* on secondary display, it is scaled to display size */
		paramPort.format.video.nFrameWidth = SD_WIDTH;
		paramPort.format.video.nFrameHeight = SD_DISPLAY_HEIGHT;
		paramPort.format.video.nStride = SD_DISPLAY_WIDTH;
		paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	}

	paramPort.nBufferSize = paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight;

	eError = OMX_SetParameter (m_pDisplayHandle, OMX_IndexParamPortDefinition,
			&paramPort);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	/* --------------------------------------------------------------------------*
	 Supported display IDs by VFDC and DC are below The names will be renamed in
	 future releases as some of the driver names & interfaces will be changed in
	 future @ param OMX_VIDEO_DISPLAY_ID_HD0: 422P On-chip HDMI @ param
	 OMX_VIDEO_DISPLAY_ID_HD1: 422P HDDAC component output @ param
	 OMX_VIDEO_DISPLAY_ID_SD0: 420T/422T SD display (NTSC): Not supported yet.
	 ------------------------------------------------------------------------ */

	/* set the parameter to the disaply component to 1080P @60 mode */
	OMX_INIT_PARAM(&driverId);
	/* Configured to use on-chip HDMI */
	driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD0;
	driverId.eDispVencMode = OMX_DC_MODE_1080P_60;

	if (1 == m_displayId) {
		driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD1;
		/* for custom display, it will be set differently */
		driverId.eDispVencMode = DISPLAY_VENC_MODE;
	}

	if (2 == m_displayId) {
		driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_SD0;
		/* for custom display, it will be set differently */
		driverId.eDispVencMode = OMX_DC_MODE_NTSC;
	}

	eError = OMX_SetParameter (m_pDisplayHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexParamVFDCDriverInstId,
			&driverId);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* set the parameter to the disaply controller component to 1080P @60 mode */
	OMX_INIT_PARAM(&driverId);
	/* Configured to use on-chip HDMI */
	driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD0;
	driverId.eDispVencMode = OMX_DC_MODE_1080P_60;

	if (1 == m_displayId) {
		driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD1;
		/* for custom display, it will be set differently */
		driverId.eDispVencMode = DISPLAY_VENC_MODE;
	}

	if (2 == m_displayId) {
		driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_SD0;
		/* for custom display, it will be set differently */
		driverId.eDispVencMode = OMX_DC_MODE_NTSC;
	}

	eError = OMX_SetParameter (m_pctrlHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexParamVFDCDriverInstId,
			&driverId);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	if (1 == m_displayId) {

	}

	OMX_INIT_PARAM(&mosaicLayout);
	/* Configuring the first (and only) window */
	/* position of window can be changed by following cordinates, keeping in
	 center by default */

	mosaicLayout.sMosaicWinFmt[0].winStartX = (HD_WIDTH - m_inWidth) / 2;
	mosaicLayout.sMosaicWinFmt[0].winStartY = (HD_HEIGHT - m_inHeight) / 2;
	mosaicLayout.sMosaicWinFmt[0].winWidth = m_inWidth;
	mosaicLayout.sMosaicWinFmt[0].winHeight = m_inHeight;
	mosaicLayout.sMosaicWinFmt[0].pitch[VFDC_YUV_INT_ADDR_IDX] = m_inWidth * 2;
	mosaicLayout.sMosaicWinFmt[0].dataFormat = VFDC_DF_YUV422I_YVYU;
	mosaicLayout.sMosaicWinFmt[0].bpp = VFDC_BPP_BITS16;
	mosaicLayout.sMosaicWinFmt[0].priority = 0;
	mosaicLayout.nDisChannelNum = 0;
	/* Only one window in this layout, hence setting it to 1 */
	mosaicLayout.nNumWindows = 1;

	if (1 == m_displayId) {
		/* For secondary Display, start the window at (0,0), since it is
		 scaled to display device size */
		mosaicLayout.sMosaicWinFmt[0].winStartX = 0;
		mosaicLayout.sMosaicWinFmt[0].winStartY = 0;

		/*If LCD is chosen, fir the mosaic window to the size of the LCD display*/
		mosaicLayout.sMosaicWinFmt[0].winWidth = DISPLAY_WIDTH;
		mosaicLayout.sMosaicWinFmt[0].winHeight = DISPLAY_HEIGHT;
		mosaicLayout.sMosaicWinFmt[0].pitch[VFDC_YUV_INT_ADDR_IDX] =
		DISPLAY_WIDTH * 2;
	}

	eError = OMX_SetParameter (m_pDisplayHandle, (OMX_INDEXTYPE)
			OMX_TI_IndexParamVFDCCreateMosaicLayout,
			&mosaicLayout);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* map OMX port to window */
	OMX_INIT_PARAM(&port2Winmap);
	/* signifies the layout id this port2win mapping refers to */
	port2Winmap.nLayoutId = 0;
	/* Just one window in this layout, hence setting the value to 1 */
	port2Winmap.numWindows = 1;
	/* Only 1st input port used here */
	port2Winmap.omxPortList[0] = OMX_VFDC_INPUT_PORT_START_INDEX + 0;
	eError = OMX_SetConfig (m_pDisplayHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVFDCMosaicPort2WinMap,
			&port2Winmap);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Setting Memory type at input port to Raw Memory */
	printf("setting input and output memory type to default\n");
	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError =
			OMX_SetParameter (m_pDisplayHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
					&memTypeCfg);

	return (OMX_ErrorNone == eError);
}

bool Mp4FilePlayer::freeDecBuffers() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (OMX_U32 j = 0; j < m_decComponent->inPortParams->nBufferCountActual;
			j++) {
		eError = OMX_FreeBuffer(m_pDecHandle, OMX_VIDDEC_INPUT_PORT,
				m_decComponent->inPortParams->pInBuff[j]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	for (OMX_U32 j = 0; j < m_decComponent->outPortParams->nBufferCountActual;
			j++) {
		eError = OMX_FreeBuffer(m_pDecHandle, OMX_VIDDEC_OUTPUT_PORT,
				m_decComponent->outPortParams->pOutBuff[j]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	OMXComponentUtils::waitStateSem(m_decComponent);
	printf(" decoder state loaded \n ");
	return true;
}

bool Mp4FilePlayer::freeScaleBuffers() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (OMX_U32 j = 0; j < m_scaleComponent->inPortParams->nBufferCountActual;
			j++) {
		eError =
		OMX_FreeBuffer(m_pScaleHandle, OMX_VFPC_INPUT_PORT_START_INDEX,
				m_scaleComponent->inPortParams->pInBuff[j]);
		if (eError != OMX_ErrorNone) {
			return 0;
		}
	}

	for (OMX_U32 j = 0; j < m_scaleComponent->outPortParams->nBufferCountActual;
			j++) {
		eError =
		OMX_FreeBuffer(m_pScaleHandle, OMX_VFPC_OUTPUT_PORT_START_INDEX,
				m_scaleComponent->outPortParams->pOutBuff[j]);
		if (eError != OMX_ErrorNone) {
			return 0;
		}
	}

	OMXComponentUtils::waitStateSem(m_scaleComponent);
	return true;
}

bool Mp4FilePlayer::freeDisplayBuffers() {
	if (NULL == m_displayComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (OMX_U32 i = 0;
			i < m_displayComponent->inPortParams->nBufferCountActual; i++) {
		eError =
		OMX_FreeBuffer(m_pDisplayHandle, OMX_VFDC_INPUT_PORT_START_INDEX,
				m_displayComponent->inPortParams->pInBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	OMXComponentUtils::waitStateSem(m_displayComponent);
	return true;
}

OMX_U32 Mp4FilePlayer::getFrameSize() {
	/*int frameSize = 0;
	while (0 == frameSize) {
		if (m_pc.bytes <= m_pc.tmp) {
			if (m_nInBufferLen == 0) {
				return 0;
			}

			m_pc.bytes = (m_nInBufferLen > READSIZE) ? READSIZE : m_nInBufferLen;

			memcpy(m_pc.readBuf, m_pInBuffer,
					m_pc.bytes);
			m_nInBufferLen -= m_pc.bytes;
			memmove(m_pInBuffer,
					m_pInBuffer + m_pc.bytes,
					m_nInBufferLen);

			if (!m_pc.bytes) {
				return 0;
			}

			m_pc.inBuf.ptr = m_pc.readBuf;
			m_pc.tmp = 0;
		}
		frameSize = DecodeUtils::Decode_GetNextFrameSize(&m_pc);
	}
	return frameSize;*/
}

bool Mp4FilePlayer::initialInputResources(int aInputIndex) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	int frameSize = 0;

	/*m_pc.outBuf.ptr =
			m_decComponent->inPortParams->pInBuff[aInputIndex]->pBuffer;
	m_pc.outBuf.bufsize = m_decComponent->inPortParams->nBufferSize;
	m_pc.outBuf.bufused = 0;*/

	int size = 0;
	if (!m_bGetHeader) {
		m_mp4Reader->GetSeqHeaderBuf(m_pInBuffer, size);
		frameSize += size;
		m_mp4Reader->GetPicHeaderBuf(m_pInBuffer + frameSize, size);
		frameSize += size;
		m_bGetHeader = true;
	}
	if (m_videoSampleId >= m_videoSampleCount) {
		return false;
	}
	m_mp4Reader->GetVideoBuffer(m_videoSampleId, (char*)m_pInBuffer + frameSize, size);
	frameSize += size;
	m_videoSampleId++;
	memcpy(m_decComponent->inPortParams->pInBuff[aInputIndex]->pBuffer, m_pInBuffer, frameSize);

	m_decComponent->inPortParams->pInBuff[aInputIndex]->nFilledLen = frameSize;
	m_decComponent->inPortParams->pInBuff[aInputIndex]->nOffset = 0;
	m_decComponent->inPortParams->pInBuff[aInputIndex]->nAllocLen = frameSize;
	m_decComponent->inPortParams->pInBuff[aInputIndex]->nInputPortIndex = 0;

	err = OMX_EmptyThisBuffer (m_pDecHandle,
			m_decComponent->inPortParams->pInBuff[aInputIndex]);
	return OMX_ErrorNone == err;
}

OMX_U32 Mp4FilePlayer::fillData(OMX_BUFFERHEADERTYPE *pBuf) {
	unsigned int dataRead = 0;
	int frameSize = 0;

	/*m_pc.outBuf.ptr = pBuf->pBuffer;
	m_pc.outBuf.bufsize = m_decComponent->inPortParams->nBufferSize;
	m_pc.outBuf.bufused = 0;
	frameSize = getFrameSize();
	if (frameSize <= 0) {
		return 0;
	}
	 */
	if (m_videoSampleId >= m_videoSampleCount) {
		return 0;
	}
	m_mp4Reader->GetVideoBuffer(m_videoSampleId, (char*)m_pInBuffer, frameSize);
	m_videoSampleId++;
	memcpy(pBuf->pBuffer, m_pInBuffer, frameSize);

	dataRead = frameSize;

	pBuf->nFilledLen = frameSize;

	return frameSize;
}

void Mp4FilePlayer::onReadTask() {
	unsigned int dataRead = 0;
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;
	ComponentWrapper* decILComp = m_decComponent;

	for (OMX_U32 i = 0; i < decILComp->inPortParams->nBufferCountActual; i++) {
		if (!initialInputResources(i)) {
			i--;
			usleep(5000);
		}
	}

	while (1) {
		/* Read empty buffer pointer from the pipe */
		read(decILComp->inPortParams->ipBufPipe[0], &pBufferIn,
				sizeof(pBufferIn));
		do {
			dataRead = fillData(pBufferIn);
			usleep(3000);
		} while (dataRead <= 0 && !m_bExit);

		if (dataRead <= 0 || m_bExit) {
			printf("No data available for Read\n");
			pBufferIn->nFlags |= OMX_BUFFERFLAG_EOS;
			err = OMX_EmptyThisBuffer (decILComp->handle, pBufferIn);
			pthread_exit(decILComp);
			break;
		}

		/* Pass the input buffer to the component */
		err = OMX_EmptyThisBuffer (decILComp->handle, pBufferIn);

		if (OMX_ErrorNone != err) {
			/* put back the frame in pipe and wait for state change */
			write(decILComp->inPortParams->ipBufPipe[1], &pBufferIn,
					sizeof(pBufferIn));

			pthread_exit(decILComp);
		}
	}
}

void Mp4FilePlayer::onConnectTask(ComponentWrapper *aComponent) {
	PipeMsg pipeMsg;
	OMX_ERRORTYPE err = OMX_ErrorNone;

	for (;;) {
		/* Read from its own local Pipe */
		read(aComponent->localPipe[0], &pipeMsg, sizeof(PipeMsg));

		switch (pipeMsg.cmd) {
		case ePipeCmdExit:
			printf("exiting thread\n ");
			pthread_exit(aComponent);
			break;
		case ePipeCmdEmptyThisBuffer:
			err = OMXComponentUtils::procPipeCmdEmptyThisBuffer(aComponent,
					&pipeMsg);
			if (OMX_ErrorNone != err) {
				write(aComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
				pthread_exit(aComponent);
			}

			break;
		case ePipeCmdFillThisBuffer:
			err = OMXComponentUtils::procPipeCmdFillThisBuffer(aComponent,
					&pipeMsg);
			if (OMX_ErrorNone != err) {
				write(aComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
				pthread_exit(aComponent);
			}
			break;

		case ePipeCmdEmptyBufferDone:
			OMXComponentUtils::procPipeCmdEmptyBufferDone(aComponent, &pipeMsg);

			break;

		case ePipeCmdFillBufferDone:
			OMXComponentUtils::procPipeCmdFillBufferDone(aComponent, &pipeMsg);
			break;

		default:
			break;
		} /* switch () */
	} /* for (;;) */
}
