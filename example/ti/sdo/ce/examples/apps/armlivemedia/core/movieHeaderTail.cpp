#include <signal.h>
#include "movieHeaderTail.h"
#include "OMXComponentUtils.h"
#include "../utils/colorConvert.h"
#include "mp4Output.h"


OMX_ERRORTYPE MovieHeaderTail::procCallbackEventHandler(OMX_HANDLETYPE hComponent,
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

OMX_ERRORTYPE MovieHeaderTail::procCallbackEmptyBufferDone(OMX_HANDLETYPE hComponent,
		OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer) {
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

OMX_ERRORTYPE MovieHeaderTail::procCallbackFillBufferDone(OMX_HANDLETYPE hComponent,
		OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer) {
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


void* MovieHeaderTail::OutputBitStreamWriteTask(void *threadsArg) {
	MovieHeaderTail* pSelf = (MovieHeaderTail*) threadsArg;
	pSelf->onWriteTask();
	return NULL;
}

void* MovieHeaderTail::InputBitStreamReadTask(void *threadsArg) {
	MovieHeaderTail* pSelf = (MovieHeaderTail*) threadsArg;
	pSelf->onReadTask();
	return NULL;
}

void* MovieHeaderTail::ConnInConnOutTask(void *threadsArg) {
	stConnInConnOutThreadArgs *pInOutParam =
			(stConnInConnOutThreadArgs*) threadsArg;
	ComponentWrapper *thisComp = pInOutParam->pComponent;
	MovieHeaderTail* pSelf = (MovieHeaderTail*) pInOutParam->pParent;
	pSelf->onConnectTask(thisComp);
	return NULL;
}

int MovieHeaderTail::start() {
	if (!initOMXHandle()) {
		return 0;
	}

	pthread_attr_init(&m_encComponent->ThreadAttr);
	OMXComponentUtils::initialOutputResources(m_encComponent);
	if (0 != pthread_create(&m_encComponent->outDataStrmThrdId,
					&m_encComponent->ThreadAttr,
					OutputBitStreamWriteTask,
					this)) {
		printf("Create_Task failed !");
		return 0;
	}
	printf(" file write thread created \n ");

	pthread_attr_init(&m_encComponent->ThreadAttr);
	m_encParams.pParent = this;
	m_encParams.pComponent = m_encComponent;
	if (0 != pthread_create(&m_encComponent->connDataStrmThrdId,
					&m_encComponent->ThreadAttr,
					ConnInConnOutTask,
					&m_encParams)) {
		printf("Create_Task failed !");
		return 0;
	}
	printf(" encode connect thread created \n ");

	pthread_attr_init(&m_deiComponent->ThreadAttr);
	if (0 != pthread_create(&m_deiComponent->inDataStrmThrdId,
					&m_deiComponent->ThreadAttr,
					InputBitStreamReadTask,
					this)) {
		printf("Create_Task failed !");
		return 0;
	}
	printf(" file read thread created \n ");

	pthread_attr_init(&m_deiComponent->ThreadAttr);
	OMXComponentUtils::initialOutputResources(m_deiComponent);
	m_deiParams.pParent = this;
	m_deiParams.pComponent = m_deiComponent;
	if (0 != pthread_create(&m_deiComponent->connDataStrmThrdId,
					&m_deiComponent->ThreadAttr,
					ConnInConnOutTask,
					&m_deiParams)) {
		printf("Create_Task failed !");
		return 0;
	}



	pthread_attr_init(&m_displayComponent->ThreadAttr);
	OMXComponentUtils::initialOutputResources(m_displayComponent);
	m_displayParams.pParent = this;
	m_displayParams.pComponent = m_displayComponent;
	if (0 != pthread_create(&m_displayComponent->connDataStrmThrdId,
					&m_displayComponent->ThreadAttr,
					ConnInConnOutTask,
					&m_displayParams)) {
		printf("Create_Task failed !");
		return 0;
	}

	return 0;
}

BOOL MovieHeaderTail::stop() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	PipeMsg pipeMsg;
	OMX_S32 ret_value;

	m_bExit = true;
	pipeMsg.cmd = ePipeCmdExit;

	write(m_deiComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
	write(m_displayComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
	write(m_encComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));

	eError = OMX_SendCommand(m_pDeiHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	OMXComponentUtils::waitStateSem(m_deiComponent);

	eError = OMX_SendCommand (m_pEncHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	OMXComponentUtils::waitStateSem(m_encComponent);

	eError = OMX_SendCommand (m_pDisplayHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	OMXComponentUtils::waitStateSem(m_displayComponent);
	printf(" display state idle \n ");

	eError = OMX_SendCommand (m_pctrlHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	OMXComponentUtils::waitStateSem(m_displayComponent);
	printf(" display control state idle \n ");


	/* change the state to loded */
	eError = OMX_SendCommand (m_pDeiHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	if (!freeDeiBuffers() ) {
		return 0;
	}
	printf(" dei state loaded \n ");

	eError = OMX_SendCommand (m_pEncHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	if (!freeEncBuffers() ) {
		return 0;
	}

	eError = OMX_SendCommand (m_pDisplayHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	if (!freeDisplayBuffers() ) {
		return 0;
	}
	printf(" display state loaded \n ");

	eError = OMX_SendCommand (m_pctrlHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	OMXComponentUtils::waitStateSem(m_displayComponent);
	printf(" ctrl-dc state loaded \n ");

	eError = OMX_FreeHandle(m_pDeiHandle);
	if (OMX_ErrorNone != eError) {
		return 0;
	}

	eError = OMX_FreeHandle(m_pEncHandle);
	if (eError != OMX_ErrorNone) {
		return 0;
	}

	/* free handle for all component */
	eError = OMX_FreeHandle(m_pDisplayHandle);
	if (eError != OMX_ErrorNone) {
		return 0;
	}

	eError = OMX_FreeHandle(m_pctrlHandle);
	if (eError != OMX_ErrorNone) {
		return 0;
	}

	printf(" display free handle \n");

	pthread_join(m_deiComponent->inDataStrmThrdId, (void **) &ret_value);
	printf("end thread 1\n");
	pthread_join(m_deiComponent->connDataStrmThrdId, (void **) &ret_value);
	printf("end thread 2\n");
	pthread_join(m_encComponent->connDataStrmThrdId, (void **) &ret_value);
	printf("end thread 3\n");
	pthread_join(m_encComponent->outDataStrmThrdId, (void **) &ret_value);
	printf("end thread 4\n");
	pthread_join(m_displayComponent->connDataStrmThrdId, (void **) &ret_value);

	printf("before uninit\n");

	uninit();
	printf("after uninit\n");
	return (0);
}

BOOL MovieHeaderTail::pause() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	eError = OMX_SendCommand(m_pDeiHandle, OMX_CommandStateSet,
			OMX_StatePause, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_deiComponent);

	eError = OMX_SendCommand (m_pEncHandle, OMX_CommandStateSet,
			OMX_StatePause, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_encComponent);

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

BOOL MovieHeaderTail::resume() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	eError = OMX_SendCommand(m_pDeiHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_deiComponent);

	eError = OMX_SendCommand (m_pEncHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_encComponent);

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

OMX_U32 MovieHeaderTail::fillData(OMX_BUFFERHEADERTYPE *pBuf) {
	int frameSize = m_inWidth * m_inHeight * 3 /2;
	memcpy(pBuf->pBuffer, m_pInImage, frameSize);
	pBuf->nFilledLen = frameSize;
	pBuf->nOffset = 0;
	pBuf->nAllocLen = frameSize;
	pBuf->nInputPortIndex = 0;

	/* Update the buffer header with buffer filled length and alloc length */
	pBuf->nFilledLen = frameSize;

	return frameSize;
}

bool MovieHeaderTail::initialInputResources(int aInputIndex) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	int frameSize = m_inWidth * m_inHeight * 3 /2;
	memcpy(m_deiComponent->inPortParams->pInBuff[aInputIndex]->pBuffer, m_pInImage, frameSize);
	m_deiComponent->inPortParams->pInBuff[aInputIndex]->nFilledLen = frameSize;
	m_deiComponent->inPortParams->pInBuff[aInputIndex]->nOffset = 0;
	m_deiComponent->inPortParams->pInBuff[aInputIndex]->nAllocLen = frameSize;
	m_deiComponent->inPortParams->pInBuff[aInputIndex]->nInputPortIndex = 0;


	err = OMX_EmptyThisBuffer (m_pDeiHandle,
			m_deiComponent->inPortParams->pInBuff[aInputIndex]);
	return true;
}

MovieHeaderTail::MovieHeaderTail() :
			m_pDisplayHandle(NULL), m_pctrlHandle(NULL),
			m_displayComponent(NULL), m_nEncBitRate(2000000),
			m_bExit(false), m_deiComponent(NULL), m_pDeiHandle(NULL),
			m_pEncHandle(NULL), m_encComponent(NULL), m_pListener(NULL),
			m_pInImage(NULL), m_inWidth(1024), m_inHeight(768){

}

MovieHeaderTail::~MovieHeaderTail() {
	uninit();
}

bool MovieHeaderTail::init(int inWidth, int inHeight,
		int frameRate, int videoBitrate,
		int outWidth, int outHeight,
		int displayId) {

	OMX_Init ();
	m_inWidth = inWidth;
	m_inHeight = inHeight;

	m_outWidth = outWidth;
	m_outHeight = outHeight;
	m_nFrameRate = frameRate;
	m_displayId = displayId;
	m_nEncBitRate = videoBitrate;

	if (m_nEncBitRate >= 1024000) {
		m_nEncBitRate = 1024000;
	}

	createComponents();

	allocInOutParams();

	m_pCb.EventHandler = procCallbackEventHandler;
	m_pCb.EmptyBufferDone = procCallbackEmptyBufferDone;
	m_pCb.FillBufferDone = procCallbackFillBufferDone;
	return true;
}

bool MovieHeaderTail::uninit() {
	if (!m_bExit) {
		return false;
	}
	freeInOutParams();

	destroyComponents();
	if (NULL != m_pInImage) {
		delete []m_pInImage;
		m_pInImage = NULL;
	}
	m_bExit = false;
	return true;
}

void MovieHeaderTail::setListener(IMovieHeaderTailStreamListener* pListener) {
	m_pListener = pListener;

}

BOOL MovieHeaderTail::putRGB32Buf(char* pRGB32Buf) {
	if (NULL != m_pInImage) {
		delete [] m_pInImage;
		m_pInImage = NULL;
	}
	m_pInImage = (unsigned char*)malloc(m_inWidth * m_inHeight * 3 /2);
	unsigned char* pYUV420 = (unsigned char*)malloc(m_inWidth * m_inHeight * 3 /2);
	ColorConvertUtils::rgb32_to_yuv420(pYUV420, (unsigned char*)pRGB32Buf,
			m_inWidth, m_inHeight);
	ColorConvertUtils::yuv420_to_nv12(m_pInImage, pYUV420, m_inWidth, m_inHeight);
	free(pYUV420);
	return TRUE;
}

void MovieHeaderTail::createComponents() {
	OMXComponentUtils::initComponent(m_displayComponent);

	OMXComponentUtils::initComponent(m_deiComponent);
	OMXComponentUtils::initComponent(m_encComponent);

	m_displayComponent->numInport = 1;

	m_displayComponent->numOutport = 0;
	m_displayComponent->startOutportIndex = 0;

	m_deiComponent->numInport = 1;
	m_deiComponent->numOutport = 2;
	m_deiComponent->startOutportIndex = OMX_VFPC_NUM_INPUT_PORTS;

	m_encComponent->numInport = 1;
	m_encComponent->numOutport = 1;
	m_encComponent->startOutportIndex = 1;
}

void MovieHeaderTail::destroyComponents() {
	OMXComponentUtils::uninitComponent(m_displayComponent);

	OMXComponentUtils::uninitComponent(m_deiComponent);
	OMXComponentUtils::uninitComponent(m_encComponent);

}

void MovieHeaderTail::allocInOutParams() {
	InportParams *inPortParamsPtr;
	OutportParams *outPortParamsPtr;

	OMXComponentUtils::allocInOutParams(m_displayComponent);
	OMXComponentUtils::allocInOutParams(m_deiComponent);
	OMXComponentUtils::allocInOutParams(m_encComponent);

	for (OMX_U32 i = 0; i < m_deiComponent->numInport; ++i) {
		inPortParamsPtr = m_deiComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual = K720PBufferCountActual;
		inPortParamsPtr->nBufferSize =
				(UTIL_ALIGN(m_inWidth, 32) * UTIL_ALIGN(m_inHeight, 32) * 3) >>1;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (OMX_U32 i = 0; i < m_deiComponent->numOutport; i++) {
		outPortParamsPtr = m_deiComponent->outPortParams + i;
		outPortParamsPtr->nBufferCountActual = K720PBufferCountActual;
		if (!(i % 2)) {
			outPortParamsPtr->nBufferSize = (UTIL_ALIGN(m_outWidth, 32))
					* (UTIL_ALIGN(m_outHeight, 32)) * 2;
			outPortParamsPtr->nBufferCountActual = K720PBufferCountActual;

		} else {
			outPortParamsPtr->nBufferSize =
				((UTIL_ALIGN(m_outWidth, 32)) * (UTIL_ALIGN(m_outHeight, 32))  * 3) >> 1;
			outPortParamsPtr->nBufferCountActual = K720PBufferCountActual;
		}

		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int *) m_deiComponent->localPipe);

	for (OMX_U32 i = 0; i < m_encComponent->numInport; ++i) {
		inPortParamsPtr = m_encComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual = K720PBufferCountActual;
		inPortParamsPtr->nBufferSize =
			((UTIL_ALIGN(m_outWidth, 32)) * (UTIL_ALIGN(m_outHeight, 32))  * 3) >> 1;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (OMX_U32 i = 0; i < m_encComponent->numOutport; ++i) {
		outPortParamsPtr = m_encComponent->outPortParams + i;
		outPortParamsPtr->nBufferCountActual = K720PBufferCountActual;
		outPortParamsPtr->nBufferSize =
			((UTIL_ALIGN(m_outWidth, 32)) * (UTIL_ALIGN(m_outHeight, 32))  * 3) >> 1;
		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int*) m_encComponent->localPipe);

	for (OMX_U32 i = 0; i < m_displayComponent->numInport; i++) {
		inPortParamsPtr = m_displayComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual = K720PBufferCountActual;
		inPortParamsPtr->nBufferSize = ((UTIL_ALIGN(m_outHeight, 32))
				* (UTIL_ALIGN(m_outWidth, 32))  * 2);

		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	pipe((int *) m_displayComponent->localPipe);
}

void MovieHeaderTail::freeInOutParams() {
	OMXComponentUtils::freeInOutParams(m_displayComponent);
	OMXComponentUtils::freeInOutParams(m_encComponent);
	OMXComponentUtils::freeInOutParams(m_deiComponent);
}

BOOL MovieHeaderTail::initOMXHandle() {
	OMX_U32 i;
	OMX_ERRORTYPE eError;

	if (!initDeiOMXHandle()) {
		printf("init dei OMX handle failed\n");
		return FALSE;
	}
	printf("init dei OMX handle OK\n");

	if (!initEncodeOMXHandle()) {
		printf("init encode OMX handle failed\n");
		return FALSE;
	}
	printf("init encode OMX handle OK\n");

	if (!initDisplayOMXHandle()) {
		return FALSE;
	}

	establishConnections();


	eError = OMX_SendCommand (m_pDeiHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	for (i = 0; i < m_deiComponent->inPortParams->nBufferCountActual; i++) {
		OMX_AllocateBuffer (m_pDeiHandle,
						&m_deiComponent->inPortParams->pInBuff[i],
						OMX_VFPC_INPUT_PORT_START_INDEX, this,
						m_deiComponent->inPortParams->nBufferSize);
		if (eError != OMX_ErrorNone) {
			return FALSE;
		}
	}
	printf(" Dei input port use buffer done \n ");

	/* DEI is dual o/p port OMX component; allocate buffers on both ports */
	for (OMX_U32 j = 0; j < m_deiComponent->numOutport; j++) {
		OutportParams *outPortParamsPtr =
				m_deiComponent->outPortParams + j;
		/* buffer alloaction for output port */
		for (i = 0; i < outPortParamsPtr->nBufferCountActual; i++) {
			eError = OMX_AllocateBuffer (m_pDeiHandle,
					&outPortParamsPtr->pOutBuff[i],
					OMX_VFPC_OUTPUT_PORT_START_INDEX + j,
					this, outPortParamsPtr->nBufferSize);
			if (eError != OMX_ErrorNone) {
				return FALSE;
			}
		}
	}
	printf(" DEI outport buffers allocated \n ");

	OMXComponentUtils::waitStateSem(m_deiComponent);

	printf(" DEI is in IDLE state \n");

	eError = OMX_SendCommand (m_pEncHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	for (i = 0; i < m_encComponent->inPortParams->nBufferCountActual; ++i) {
		OutportParams* outPortParamsPtr =
				m_deiComponent->outPortParams + 1;
		eError = OMX_UseBuffer (m_pEncHandle,
				&m_encComponent->inPortParams->pInBuff[i],
				OMX_VIDENC_INPUT_PORT,
				this,
				outPortParamsPtr->nBufferSize,
				outPortParamsPtr->pOutBuff[i]->pBuffer);

		if (eError != OMX_ErrorNone) {
			return FALSE;
		}
	}
	printf(" encoder input port use buffer done \n ");

	for (i = 0; i < m_encComponent->outPortParams->nBufferCountActual; ++i) {
		eError = OMX_AllocateBuffer (m_pEncHandle,
				&m_encComponent->outPortParams->pOutBuff[i],
				OMX_VIDENC_OUTPUT_PORT, this,
				m_encComponent->outPortParams->nBufferSize);
		if (eError != OMX_ErrorNone) {
			return FALSE;
		}
	}
	printf(" encoder outport buffers allocated \n ");
	OMXComponentUtils::waitStateSem(m_encComponent);

	printf(" Encoder state IDLE \n ");

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
				m_deiComponent->outPortParams->nBufferSize,
				m_deiComponent->outPortParams->pOutBuff[i]->pBuffer);
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

	eError = OMX_SendCommand (m_pEncHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	OMXComponentUtils::waitStateSem(m_encComponent);
	printf(" encoder state execute \n ");

	eError = OMX_SendCommand (m_pDeiHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	OMXComponentUtils::waitStateSem(m_deiComponent);

	return TRUE;
}


BOOL MovieHeaderTail::establishConnections() {
	OMXComponentUtils::connectComponents(m_deiComponent,
			OMX_VFPC_OUTPUT_PORT_START_INDEX + 1, m_encComponent,
			OMX_VIDENC_INPUT_PORT);

	OMXComponentUtils::connectComponents(m_deiComponent,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, m_displayComponent,
			OMX_VFDC_INPUT_PORT_START_INDEX);

	return TRUE;
}



void MovieHeaderTail::onWriteTask() {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	while (1) {
		read(m_encComponent->outPortParams->opBufPipe[0], &pBufferOut,
				sizeof(pBufferOut));

		if (NULL != m_pListener) {
			m_pListener->onNotifyOutStream(pBufferOut->pBuffer,
					pBufferOut->nFilledLen);
		}
	    if (m_bExit)
	    {
	    	SempUtils::semp_post(m_encComponent->eos_sem);
	    	pthread_exit(m_encComponent);
	    	break;
	    }

		err = OMX_FillThisBuffer (m_encComponent->handle, pBufferOut);

		if (OMX_ErrorNone != err) {
			write(m_encComponent->outPortParams->opBufPipe[1],
					&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client \n");
			pthread_exit(m_encComponent);

		}
	}
}

void MovieHeaderTail::onReadTask() {
	unsigned int dataRead = 0;
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;

	printf("before IL_ClientInputBitStreamReadTask \n");
	for (OMX_U32 i = 0; i < m_deiComponent->inPortParams->nBufferCountActual; i++) {
		printf("the index %d\n", i);
		if (!initialInputResources(i) ) {
			i--;
			usleep(30000);
		}
	}
	printf("IL_ClientInputBitStreamReadTask \n");
	int nUsecondsPerFrame = 1000000 / m_nFrameRate;
	gettimeofday(&m_tv, NULL);
	m_beftime = (m_tv.tv_sec * 1000000 + m_tv.tv_usec);
	while (1) {
		/* Read empty buffer pointer from the pipe */
		read(m_deiComponent->inPortParams->ipBufPipe[0], &pBufferIn,
				sizeof(pBufferIn));

		gettimeofday(&m_tv, NULL);
		m_afttime = (m_tv.tv_sec * 1000000 + m_tv.tv_usec);
		long long diff = m_afttime - m_beftime;
		if (diff < 0) {
			diff += 60000000; //59 sec to 0 sec
		}
		if (diff < nUsecondsPerFrame) {
			usleep(nUsecondsPerFrame - diff);
			gettimeofday(&m_tv, NULL);
			m_beftime = (m_tv.tv_sec * 1000000 + m_tv.tv_usec);
		} else {
			m_beftime = m_afttime;
		}
		/* Fill the data in the empty buffer */
		dataRead = fillData(pBufferIn);

		if (m_bExit) {
			pthread_exit(m_deiComponent);
			break;
		}

		if (dataRead <= 0 || m_bExit) {
			printf("No data available for Read\n");
			pBufferIn->nFlags |= OMX_BUFFERFLAG_EOS;
			err = OMX_EmptyThisBuffer (m_deiComponent->handle, pBufferIn);
			pthread_exit(m_deiComponent);
			/* can be handled as EOS .. decILComp->inPortParams->flagInputEos =
			 OMX_TRUE; */
			break;
		}

		/* Pass the input buffer to the component */
		err = OMX_EmptyThisBuffer (m_deiComponent->handle, pBufferIn);

		if (OMX_ErrorNone != err) {
			/* put back the frame in pipe and wait for state change */
			write(m_deiComponent->inPortParams->ipBufPipe[1], &pBufferIn,
					sizeof(pBufferIn));
			printf(" waiting for action from IL Cleint \n");

			/* since in this example we are changing states in other thread it will
			 return error for giving ETB/FTB calls in non-execute state. Since
			 example is shutting down, we exit the thread */

			pthread_exit(m_deiComponent);

		}
	}
}

void MovieHeaderTail::onConnectTask(ComponentWrapper *aComponent) {
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
			err = OMXComponentUtils::procPipeCmdEmptyThisBuffer(aComponent, &pipeMsg);
			if (OMX_ErrorNone != err) {
				write(aComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
				pthread_exit(aComponent);
			}

			break;
		case ePipeCmdFillThisBuffer:
			err = OMXComponentUtils::procPipeCmdFillThisBuffer(aComponent, &pipeMsg);
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


BOOL MovieHeaderTail::initDeiOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pDeiHandle,
			(OMX_STRING) "OMX.TI.VPSSM3.VFPC.DEIMDUALOUT", m_deiComponent,
			&m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pDeiHandle == NULL)) {
		return FALSE;
	}
	m_deiComponent->handle = m_pDeiHandle;
	printf(" dei component is created \n");
	if (!setDeiParams() ) {
		return FALSE;
	}

	printf("enable dei input port \n");
	OMX_SendCommand(m_pDeiHandle, OMX_CommandPortEnable,
			OMX_VFPC_INPUT_PORT_START_INDEX, NULL);

	OMXComponentUtils::waitPortSem(m_deiComponent);

	printf("enable dei output port 0 \n");
	OMX_SendCommand(m_pDeiHandle, OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);
	OMXComponentUtils::waitPortSem(m_deiComponent);

	printf("enable dei output port 1 \n");
	OMX_SendCommand(m_pDeiHandle, OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX + 1, NULL);
	OMXComponentUtils::waitPortSem(m_deiComponent);
	return TRUE;
}

BOOL MovieHeaderTail::initEncodeOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pEncHandle,
			(OMX_STRING) "OMX.TI.DUCATI.VIDENC", m_encComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pEncHandle == NULL)) {
		return FALSE;
	}

	m_encComponent->handle = m_pEncHandle;
	printf("before set encode params \n");
	return setEncodeParams();
}

BOOL MovieHeaderTail::initDisplayOMXHandle() {
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
	if (!setDisplayParams() ) {
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

bool MovieHeaderTail::setDeiParams() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_PARAM_BUFFER_MEMORYTYPE memTypeCfg;
	OMX_PARAM_PORTDEFINITIONTYPE paramPort;
	OMX_PARAM_VFPC_NUMCHANNELPERHANDLE sNumChPerHandle;
	OMX_CONFIG_ALG_ENABLE algEnable;
	OMX_CONFIG_VIDCHANNEL_RESOLUTION chResolution;
	OMX_PARAM_COMPPORT_NOTIFYTYPE pNotifyType;

	OMX_CONFIG_SUBSAMPLING_FACTOR sSubSamplinginfo = { 0 };

	OMX_INIT_PARAM(&sSubSamplinginfo);

	sSubSamplinginfo.nSubSamplingFactor = 1;
	eError = OMX_SetConfig (m_pDeiHandle, ( OMX_INDEXTYPE )
			( OMX_TI_IndexConfigSubSamplingFactor ),
			&sSubSamplinginfo );

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError =
			OMX_SetParameter (m_pDeiHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
					&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at input port\n");
		return false;
	}

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError =
			OMX_SetParameter (m_pDeiHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
					&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at output port\n");
		return false;
	}

	OMX_INIT_PARAM(&pNotifyType);
	pNotifyType.eNotifyType = OMX_NOTIFY_TYPE_NONE;
	pNotifyType.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	eError = OMX_SetParameter (m_pDeiHandle,
			(OMX_INDEXTYPE)OMX_TI_IndexParamCompPortNotifyType,
			&pNotifyType);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	pNotifyType.eNotifyType = OMX_NOTIFY_TYPE_NONE;
	pNotifyType.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	eError = OMX_SetParameter (m_pDeiHandle,
			(OMX_INDEXTYPE)OMX_TI_IndexParamCompPortNotifyType,
			&pNotifyType);
	if (eError != OMX_ErrorNone) {
		printf("output port OMX_SetParameter failed\n");
		return false;
	}

	pNotifyType.eNotifyType = OMX_NOTIFY_TYPE_NONE;
	pNotifyType.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1;
	eError = OMX_SetParameter (m_pDeiHandle,
			(OMX_INDEXTYPE)OMX_TI_IndexParamCompPortNotifyType,
			&pNotifyType);
	if (eError != OMX_ErrorNone) {
		printf("output port OMX_SetParameter failed\n");
		return false;
	}

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError =
			OMX_SetParameter (m_pDeiHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
					&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at output port\n");
		return false;
	}

	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

	OMX_GetParameter(m_pDeiHandle, OMX_IndexParamPortDefinition, &paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_inWidth, 32);
	paramPort.format.video.nFrameHeight = m_inHeight;

	paramPort.format.video.nStride = UTIL_ALIGN(m_inWidth, 32);
	paramPort.format.video.xFramerate = (m_nFrameRate) << 16;
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	paramPort.nBufferSize = (paramPort.format.video.nFrameWidth *
			paramPort.format.video.nFrameHeight * 3) /2;

	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = K720PBufferCountActual;
	OMX_SetParameter(m_pDeiHandle, OMX_IndexParamPortDefinition, &paramPort);

	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	OMX_GetParameter(m_pDeiHandle, OMX_IndexParamPortDefinition, &paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_outWidth, 32);
	paramPort.format.video.nFrameHeight = m_outHeight;
	paramPort.format.video.xFramerate = (m_nFrameRate) << 16;
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
	paramPort.nBufferAlignment = 0;
	paramPort.nBufferCountActual = K720PBufferCountActual;

	paramPort.format.video.nStride = UTIL_ALIGN(m_outWidth * 2, 32);
	paramPort.nBufferSize = paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight;

	OMX_SetParameter(m_pDeiHandle, OMX_IndexParamPortDefinition, &paramPort);

	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1;
	OMX_GetParameter(m_pDeiHandle, OMX_IndexParamPortDefinition, &paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_outWidth, 32);
	paramPort.format.video.nFrameHeight = m_outHeight;
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.nBufferAlignment = 0;
	paramPort.nBufferCountActual = K720PBufferCountActual;
	paramPort.format.video.nStride = UTIL_ALIGN(m_outWidth, 32);

	paramPort.nBufferSize = (paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight * 3) >> 1;

	OMX_SetParameter(m_pDeiHandle, OMX_IndexParamPortDefinition, &paramPort);

	printf("set number of channels");

	OMX_INIT_PARAM(&sNumChPerHandle);
	sNumChPerHandle.nNumChannelsPerHandle = 1;
	eError = OMX_SetParameter (m_pDeiHandle,
			(OMX_INDEXTYPE)
			OMX_TI_IndexParamVFPCNumChPerHandle,
			&sNumChPerHandle);
	if (eError != OMX_ErrorNone) {
		printf("failed to set num of channels\n");
		return false;
	}

	printf("set input resolution");

	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_inWidth, 32);
	chResolution.Frm0Height = m_inHeight;
	chResolution.Frm0Pitch =  UTIL_ALIGN(m_inWidth, 32);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = m_inWidth;
	chResolution.FrmCropHeight = m_inHeight;

	chResolution.eDir = OMX_DirInput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pDeiHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("failed to set input channel resolution\n");
		return false;
	}

	printf("set output resolution");
	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_outWidth, 32);
	chResolution.Frm0Height = m_outHeight;
	chResolution.Frm0Pitch = UTIL_ALIGN(m_outWidth * 2, 32);

	chResolution.Frm1Width = UTIL_ALIGN(m_outWidth, 32);
	chResolution.Frm1Height = m_outHeight;
	chResolution.Frm1Pitch = UTIL_ALIGN(m_outWidth, 32);
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = 0;
	chResolution.FrmCropHeight = 0;
	chResolution.eDir = OMX_DirOutput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pDeiHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("failed to set output channel resolution\n");
		return false;
	}

	OMX_INIT_PARAM(&algEnable);
	algEnable.nPortIndex = 0;
	algEnable.nChId = 0;
	algEnable.bAlgBypass = OMX_TRUE;
	/*if (strcmp((char *) pAppData->mode, "1080i") == 0) {
	 algEnable.bAlgBypass = 0;
	 }*/

	eError = OMX_SetConfig (m_pDeiHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable,
			&algEnable);
	if (eError != OMX_ErrorNone) {
		printf("failed to disable algo by pass mode\n");
		return false;
	}

	return true;
}

bool MovieHeaderTail::setEncodeParams() {
	if (NULL == m_pEncHandle) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorUndefined;
	OMX_VIDEO_PARAM_PROFILELEVELTYPE tProfileLevel;
	OMX_VIDEO_PARAM_ENCODER_PRESETTYPE tEncoderPreset;
	OMX_VIDEO_PARAM_BITRATETYPE tVidEncBitRate;
	OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
	OMX_VIDEO_CONFIG_DYNAMICPARAMS tDynParams;
	OMX_VIDEO_CONFIG_STATICPARAMS tStaticParam;
	//OMX_VIDEO_PARAM_AVCNALUCONTROLTYPE tNaluMasks;
	//unsigned short int SPSnPPS = 0x0180; // SPS is B7 and PPS is B8 ...

	OMX_INIT_PARAM(&tPortDef);
	tPortDef.nPortIndex = OMX_VIDENC_INPUT_PORT;
	eError =
			OMX_GetParameter(m_pEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* set the actual number of buffers required */
	tPortDef.nBufferCountActual = K720PBufferCountActual;
	/* set the video format settings */

	tPortDef.format.video.nFrameWidth = m_outWidth;
	tPortDef.format.video.nFrameHeight = m_outHeight;
	tPortDef.format.video.nStride = m_outWidth;
	tPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	/* settings for OMX_IndexParamVideoPortFormat */
	tPortDef.nBufferSize = (tPortDef.format.video.nStride
			* tPortDef.format.video.nFrameHeight * 3) >> 1;

	eError =
			OMX_SetParameter(m_pEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf("failed to set Encode OMX_IndexParamPortDefinition for input \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for input \n");

	OMX_INIT_PARAM(&tPortDef);

	tPortDef.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError =
			OMX_GetParameter(m_pEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* settings for OMX_IndexParamPortDefinition */
	/* set the actual number of buffers required */

	tPortDef.nBufferCountActual = K720PBufferCountActual;
	tPortDef.format.video.cMIMEType = "H264";
	tPortDef.format.video.nFrameWidth = m_outWidth;
	tPortDef.format.video.nFrameHeight = m_outHeight;
	tPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	tPortDef.format.video.nBitrate = m_nEncBitRate;
	/* settings for OMX_IndexParamVideoPortFormat */

	eError =
			OMX_SetParameter(m_pEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf("failed to set Encode OMX_IndexParamPortDefinition for output \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for output \n");

	/* For changing bit rate following index can be used */
	OMX_INIT_PARAM(&tVidEncBitRate);

	tVidEncBitRate.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pEncHandle, OMX_IndexParamVideoBitrate,
			&tVidEncBitRate);

	tVidEncBitRate.eControlRate = OMX_Video_ControlRateVariable;
	tVidEncBitRate.nTargetBitrate = m_nEncBitRate;
	eError = OMX_SetParameter(m_pEncHandle, OMX_IndexParamVideoBitrate,
			&tVidEncBitRate);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Set the profile and level for H264 */
	OMX_INIT_PARAM(&tProfileLevel);
	tProfileLevel.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError =
			OMX_GetParameter(m_pEncHandle, OMX_IndexParamVideoProfileLevelCurrent,
					&tProfileLevel);

	tProfileLevel.eProfile = OMX_VIDEO_AVCProfileMain; //OMX_VIDEO_AVCProfileHigh
	tProfileLevel.eLevel = OMX_VIDEO_AVCLevel42;

	eError =
			OMX_SetParameter(m_pEncHandle, OMX_IndexParamVideoProfileLevelCurrent,
					&tProfileLevel);
	if (eError != OMX_ErrorNone)
		return false;

	OMX_INIT_PARAM(&tEncoderPreset);
	tEncoderPreset.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError =
			OMX_GetParameter(m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset,
					&tEncoderPreset);
	tEncoderPreset.eEncodingModePreset = OMX_Video_Enc_High_Speed_Med_Quality;

	tEncoderPreset.eRateControlPreset = OMX_Video_RC_Low_Delay;
	eError =
			OMX_SetParameter(m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset,
					&tEncoderPreset);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_INIT_PARAM(&tDynParams);

	tDynParams.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval =
			25;

	eError = OMX_SetParameter (m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	OMX_INIT_PARAM (&tStaticParam);

	tStaticParam.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter (m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	tStaticParam.videoStaticParams.h264EncStaticParams.videnc2Params.encodingPreset = XDM_USER_DEFINED;

	tStaticParam.videoStaticParams.h264EncStaticParams.numTemporalLayer = IH264_TEMPORAL_LAYERS_1;


	tStaticParam.videoStaticParams.h264EncStaticParams.transformBlockSize = IH264_TRANSFORM_8x8;
	tStaticParam.videoStaticParams.h264EncStaticParams.entropyCodingMode = IH264_ENTROPYCODING_CAVLC;
	tStaticParam.videoStaticParams.h264EncStaticParams.intraCodingParams.chromaIntra8x8Enable |= 1;


	 tStaticParam.videoStaticParams.h264EncStaticParams.nalUnitControlParams.naluControlPreset = IH264_NALU_CONTROL_USERDEFINED;
	 tStaticParam.videoStaticParams.h264EncStaticParams.nalUnitControlParams.naluPresentMaskStartOfSequence |= 0x2180;
	 tStaticParam.videoStaticParams.h264EncStaticParams.nalUnitControlParams.naluPresentMaskIDRPicture |= 0x2180;
	 tStaticParam.videoStaticParams.h264EncStaticParams.nalUnitControlParams.naluPresentMaskIntraPicture |= 0x2180;
	 tStaticParam.videoStaticParams.h264EncStaticParams.nalUnitControlParams.naluPresentMaskNonIntraPicture |= 0;
	 tStaticParam.videoStaticParams.h264EncStaticParams.nalUnitControlParams.naluPresentMaskEndOfSequence |= 0x2180;

	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.vuiCodingPreset = IH264_VUICODING_USERDEFINED;
	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.aspectRatioInfoPresentFlag = 0;
	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.aspectRatioIdc = 0;
	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.videoSignalTypePresentFlag = 0;
	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.videoFormat = IH264ENC_VIDEOFORMAT_NTSC;
	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.videoFullRangeFlag = 0;
	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.timingInfoPresentFlag = 1;
	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.hrdParamsPresentFlag = 0;
	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.numUnitsInTicks = 1000;

	eError = OMX_SetParameter (m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	return (OMX_ErrorNone == eError);
}

bool MovieHeaderTail::setDisplayParams() {
	if (NULL == m_pDisplayHandle) {
		return false;
	}
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
	paramPort.format.video.nFrameWidth = m_outWidth;
	paramPort.format.video.nFrameHeight = m_outHeight;
	paramPort.format.video.nStride = m_outWidth * 2; /**< should be twice of width*/
	paramPort.format.video.xFramerate = (m_nFrameRate << 16);
	paramPort.nBufferCountActual = K720PBufferCountActual;
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;

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

	mosaicLayout.sMosaicWinFmt[0].winStartX = (HD_WIDTH - m_outWidth) / 2;
	mosaicLayout.sMosaicWinFmt[0].winStartY = (HD_HEIGHT - m_outHeight) / 2;
	mosaicLayout.sMosaicWinFmt[0].winWidth = m_outWidth;
	mosaicLayout.sMosaicWinFmt[0].winHeight = m_outHeight;
	mosaicLayout.sMosaicWinFmt[0].pitch[VFDC_YUV_INT_ADDR_IDX] = m_outWidth * 2;
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


bool MovieHeaderTail::freeDeiBuffers() {
	if (NULL == m_deiComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_deiComponent->inPortParams->nBufferCountActual; i++) {
			eError = OMX_FreeBuffer (m_pDeiHandle,
					OMX_VFPC_INPUT_PORT_START_INDEX,
					m_deiComponent->inPortParams->pInBuff[i]);
			if (eError != OMX_ErrorNone) {
				return false;
			}
		}

	for (OMX_U32 j = 0; j < m_deiComponent->numOutport; ++j) {
		OutportParams *outPortParamsPtr = m_deiComponent->outPortParams + j;
		for (OMX_U32 i = 0; i < outPortParamsPtr->nBufferCountActual; i++) {
			eError = OMX_FreeBuffer (m_pDeiHandle,
					OMX_VFPC_OUTPUT_PORT_START_INDEX + j,
					outPortParamsPtr->pOutBuff[i]);
			if (eError != OMX_ErrorNone) {
				return false;
			}
		}
	}

	OMXComponentUtils::waitStateSem(m_deiComponent);
	return true;
}

bool MovieHeaderTail::freeEncBuffers() {
	if (NULL == m_encComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_encComponent->inPortParams->nBufferCountActual; i++) {
			eError = OMX_FreeBuffer (m_pEncHandle,
					OMX_VIDENC_INPUT_PORT,
					m_encComponent->inPortParams->pInBuff[i]);
			if (eError != OMX_ErrorNone) {
				return false;
			}
		}

	for (OMX_U32 i = 0; i < m_encComponent->outPortParams->nBufferCountActual; i++) {
		eError = OMX_FreeBuffer (m_pEncHandle,
				OMX_VIDENC_OUTPUT_PORT,
				m_encComponent->outPortParams->pOutBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	OMXComponentUtils::waitStateSem(m_encComponent);
	return true;
}

bool MovieHeaderTail::freeDisplayBuffers() {
	if (NULL == m_displayComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (OMX_U32 i = 0;
			i < m_displayComponent->inPortParams->nBufferCountActual; i++) {
		eError = OMX_FreeBuffer(m_pDisplayHandle, OMX_VFDC_INPUT_PORT_START_INDEX,
						m_displayComponent->inPortParams->pInBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	OMXComponentUtils::waitStateSem(m_displayComponent);
	return true;
}

