#include "vgaProviderSystem.h"
#include "../core/OMXComponentUtils.h"
#include "../utils/commonUtils.h"
#include "../kernel.h"

extern unsigned char *buffer_addr[MAX_BUFFER];
extern "C" Ptr DomxCore_mapUsrVirtualAddr2phy (UInt32 pUsrVirtAddr);

void* VGAProviderSystem::scaleOutWriteTask(void* pParam) {
	VGAProviderSystem* pSelf = (VGAProviderSystem*) pParam;
	if (NULL != pSelf) {
		pSelf->onVGAScaleWriteTask();
	}
	return NULL;
}

void* VGAProviderSystem::VGAEncodeOutWriteTask(void* pParam) {
	VGAProviderSystem* pSelf = (VGAProviderSystem*) pParam;
	if (NULL != pSelf) {
		pSelf->onVGAEncodeWriteTask();
	}
	return NULL;
}

OMX_ERRORTYPE VGAProviderSystem::procCallbackEventHandler(OMX_HANDLETYPE hComponent,
			OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1,
			OMX_U32 nData2, OMX_PTR pEventData) {
	ComponentWrapper *comp = (ComponentWrapper *) ptrAppData;
	VGAProviderSystem* pSelf = NULL;
	if (NULL != comp->pParent) {
		pSelf = (VGAProviderSystem*)comp->pParent;
	}

	printf("got event %s\n", comp->name);
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
			SempUtils::semp_post(comp->eos_sem);
		}
	} else if (eEvent == OMX_EventError) {
		printf("get OMX error %x\n", nData1);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE VGAProviderSystem::procCallbackEmptyBufferDone(OMX_HANDLETYPE hComponent,
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
			CommonUtils::omx_printf("Error writing into Input buffer i/p Pipe!\n");
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
			CommonUtils::omx_printf("Error writing into local Pipe!\n");
			eError = OMX_ErrorNotReady;
			return eError;
		}
	}

	return eError;
}

OMX_ERRORTYPE VGAProviderSystem::procCallbackFillBufferDone(OMX_HANDLETYPE hComponent,
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
			CommonUtils::omx_printf("Error writing to Input buffer i/p Pipe!\n");
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
			CommonUtils::omx_printf("Error writing to local Pipe!\n");
			eError = OMX_ErrorNotReady;
			return eError;
		}
	}
	return eError;
}

VGAProviderSystem::VGAProviderSystem()
: m_pKernel(NULL)
, m_pScaleHandle(NULL)
, m_scaleComponent(NULL)
, m_pEncHandle(NULL)
, m_encComponent(NULL)
, m_bExitThread(false)
, m_bExitVGAEncodeThread(false)
, m_pCaptureVGAHandle(NULL)
, m_inVGAWidth(1024)
, m_inVGAHeight(768)
, m_captureVGATimerValid(false)
, m_bgetVGABuffer(false)
, m_bInited(false)
, m_nVGAScaleInputIndex(0)
, m_nVGAEncodeInputIndex(0)
, m_bVGAUnder768(0)
, m_bStartCaptureVGASucess(false)
, m_captureChangeTime(0)
, m_bFreezeVGA(false) {

}

VGAProviderSystem::~VGAProviderSystem() {
	uninit();
}

bool VGAProviderSystem::init(Kernel* pKernel) {
	m_pKernel = pKernel;

	m_inVGAWidth = 1024;
	m_inVGAHeight = 768;

	m_pCaptureVGAHandle = new CaptureVGA();
	m_pCaptureVGAHandle->addListener(this);

	m_vgaMutex.init();

	m_pCb.EventHandler = procCallbackEventHandler;
	m_pCb.EmptyBufferDone = procCallbackEmptyBufferDone;
	m_pCb.FillBufferDone = procCallbackFillBufferDone;

	m_heap = m_pKernel->getHeapObjectInstance();

	createComponents();
	allocInOutParams();

	m_bgetVGABuffer = false;


	if (!initOMXHandle() ) {
		printf("inited vga provider system failed\n");
		return false;
	}
	m_bInited = true;
	printf("inited vga provider system ok\n");
	return true;
}

bool VGAProviderSystem::uninit() {
	if (!m_bInited) {
		return false;
	}
	freeInOutParams();
	printf("before VGAProviderSystem destroy components \n");
	destroyComponents();
	printf("after VGAProviderSystem destroy components \n");

	if (NULL != m_pCaptureVGAHandle) {
		delete m_pCaptureVGAHandle;
		m_pCaptureVGAHandle = NULL;
	}
	printf("after delete capture vga\n");

	m_vgaMutex.uninit();
	m_bInited = false;

	printf("VGAProviderSystem uninited ok\n");
	return true;
}


bool VGAProviderSystem::start() {
	if (NULL == m_pCaptureVGAHandle) {
		return false;
	}
	m_pCaptureVGAHandle->addListener(this);
	bool bInited = m_pCaptureVGAHandle->init();
	if (bInited) {
		m_bStartCaptureVGASucess = m_pCaptureVGAHandle->start();
	}

	m_captureVGATimer.init();
	m_captureVGATimer.start(this);
	m_captureVGATimer.setInterval(2000);
	m_captureVGATimerValid = true;


	m_captureChangeTime = CommonUtils::GetTickCount();
	startOMXThread();
	return true;
}

bool VGAProviderSystem::stop() {
	if (NULL != m_pCaptureVGAHandle) {
		m_pCaptureVGAHandle->stop();

	}
	if (m_captureVGATimerValid) {
		m_captureVGATimer.stop();
		m_captureVGATimer.uninit();
		m_captureVGATimerValid = false;
	}

	stopOMXThread();

	return false;
}


bool VGAProviderSystem::addListener(IVGAProviderObserver* pObserver) {
	for (vector<IVGAProviderObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
		if ( *it == pObserver) {
			return false;
		}
	}
	m_listeners.push_back(pObserver);
	return true;
}

bool VGAProviderSystem::removeListener(IVGAProviderObserver* pObserver) {
	for (vector<IVGAProviderObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
		if (*it == pObserver) {
			m_listeners.erase(it);
			return true;
		}
	}
	return true;
}

bool VGAProviderSystem::vgaStatusChanged(int reason) {
	m_vgaMutex.lock();
	printf("vga status changed %d\n", reason);
	unsigned long nTemp = CommonUtils::GetTickCount();
	if (nTemp - m_captureChangeTime >= 160) {
		m_captureChangeTime = nTemp;
	} else {
		m_vgaMutex.unlock();
		return false;
	}

	if (NULL != m_pCaptureVGAHandle) {
		printf("vgaproviderSystem vga status changed before stop\n");
		m_pCaptureVGAHandle->stop();
		delete m_pCaptureVGAHandle;
		m_pCaptureVGAHandle = NULL;
	}
	m_bgetVGABuffer = false;
	m_bStartCaptureVGASucess = false;

	int size = m_inVGAWidth * m_inVGAHeight * 3/2;

	m_pCaptureVGAHandle = new CaptureVGA();
	m_pCaptureVGAHandle->addListener(this);
	if (m_pCaptureVGAHandle->init() ) {
		m_bStartCaptureVGASucess = m_pCaptureVGAHandle->start();
	} else {
		for (vector<IVGAProviderObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
			(*it)->onVgaStatus(eVGAStatusChange);
		}
	}
	m_vgaMutex.unlock();

	return true;
}

bool VGAProviderSystem::createComponents() {
	OMXComponentUtils::initComponent(m_scaleComponent);
	strcpy(m_scaleComponent->name, "vgascale");
	m_scaleComponent->pParent = this;

	OMXComponentUtils::initComponent(m_encComponent);
	strcpy(m_encComponent->name, "vgaEncode");
	m_encComponent->pParent = this;

	m_scaleComponent->numInport = 1;
	m_scaleComponent->numOutport = 1;

	m_scaleComponent->startOutportIndex = OMX_VFPC_NUM_OUTPUT_PORTS;

	m_encComponent->numInport = 1;
	m_encComponent->numOutport = 1;
	m_encComponent->startOutportIndex = 1;
	return true;
}

bool VGAProviderSystem::destroyComponents() {
	OMXComponentUtils::uninitComponent(m_scaleComponent);
	OMXComponentUtils::uninitComponent(m_encComponent);
	return true;
}

bool VGAProviderSystem::allocInOutParams() {
	InportParams *inPortParamsPtr;
	OutportParams *outPortParamsPtr;
	OMXComponentUtils::allocInOutParams(m_scaleComponent);
	OMXComponentUtils::allocInOutParams(m_encComponent);

	for (OMX_U32 i = 0; i < m_scaleComponent->numInport; ++i) {
		inPortParamsPtr = m_scaleComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		inPortParamsPtr->nBufferSize = (UTIL_ALIGN (m_inVGAWidth, 32)) * (UTIL_ALIGN(m_inVGAHeight, 32)) * 3/2;

		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (OMX_U32 i = 0; i < m_scaleComponent->numOutport; ++i) {
		outPortParamsPtr = m_scaleComponent->outPortParams + i;
		outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		outPortParamsPtr->nBufferSize = (UTIL_ALIGN(m_inVGAWidth, 32)) * (UTIL_ALIGN(m_inVGAHeight, 32)) * 2;
		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int *) m_scaleComponent->localPipe);

	for (OMX_U32 i = 0; i < m_encComponent->numInport; ++i) {
		inPortParamsPtr = m_encComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		inPortParamsPtr->nBufferSize = (UTIL_ALIGN(1280, 32))
				* (UTIL_ALIGN(1024, 32)) * 3 / 2;
		pipe((int*) inPortParamsPtr->ipBufPipe);
	}

	for (OMX_U32 i = 0; i < m_encComponent->numOutport; ++i) {
		outPortParamsPtr = m_encComponent->outPortParams + i;
		outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		outPortParamsPtr->nBufferSize = ((UTIL_ALIGN(1280, 32))
				* ((UTIL_ALIGN(1024, 32))) * 3) >> 1;
		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int*) m_encComponent->localPipe);

	return true;
}

bool VGAProviderSystem::freeInOutParams() {
	OMXComponentUtils::freeInOutParams(m_scaleComponent);
	OMXComponentUtils::freeInOutParams(m_encComponent);
	return true;
}

bool VGAProviderSystem::initOMXHandle() {
	if (!initScaleOMXHandle()) {
		return false;
	}

	if (!initEncodeOMXHandle()) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_SendCommand (m_scaleComponent->handle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	for (OMX_U32 j = 0; j < m_scaleComponent->inPortParams->nBufferCountActual; j++) {
		eError = OMX_AllocateBuffer (m_pScaleHandle,
				&m_scaleComponent->inPortParams->pInBuff[j],
				OMX_VFPC_INPUT_PORT_START_INDEX,
				this,
				m_scaleComponent->inPortParams->nBufferSize);

		if (eError != OMX_ErrorNone) {
			printf("can not use buffer index %d ,error %x\n", j, eError);
			return false;
		}
	}

	for (OMX_U32 j = 0; j < m_scaleComponent->outPortParams->nBufferCountActual; j++) {
		m_pHeapBuffer = Memory_alloc (m_heap, m_scaleComponent->outPortParams->nBufferSize, 128, NULL);
		if (m_pHeapBuffer == NULL) {
			printf ("Buffer Allocation Failed \n");
			return false;
		}
		eError = OMX_UseBuffer (m_pScaleHandle,
	        &m_scaleComponent->outPortParams->pOutBuff[j],
	        OMX_VFPC_OUTPUT_PORT_START_INDEX, this,
	        m_scaleComponent->outPortParams->nBufferSize,
	        (OMX_U8*)m_pHeapBuffer);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	OMXComponentUtils::waitStateSem(m_scaleComponent);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	eError = OMX_SendCommand(m_pScaleHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_scaleComponent);


	printf("init vga enc omx handle ok\n");
	if (!OMXComponentUtils::changeComponentState(m_encComponent, OMX_StateIdle)) {
		return false;
	}

	for (OMX_U32 j = 0; j < m_encComponent->inPortParams->nBufferCountActual; ++j) {
		eError = OMX_AllocateBuffer (m_pEncHandle,
				&m_encComponent->inPortParams->pInBuff[j],
				OMX_VIDENC_INPUT_PORT,
				this,
				m_encComponent->inPortParams->nBufferSize);

		if (eError != OMX_ErrorNone) {
			printf("can not use buffer index %d ,error %x\n", j, eError);
			return false;
		}
	}

	for (OMX_U32 j = 0; j < m_encComponent->outPortParams->nBufferCountActual; ++j) {
		eError = OMX_AllocateBuffer (m_pEncHandle,
					&m_encComponent->outPortParams->pOutBuff[j],
					OMX_VIDENC_OUTPUT_PORT, this,
					m_encComponent->outPortParams->nBufferSize);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	OMXComponentUtils::waitStateSem(m_encComponent);
	printf("after allocate vga enc buffer\n");
	if (!OMXComponentUtils::changeComponentState(m_encComponent, OMX_StateExecuting)) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_encComponent);

	return true;
}

bool VGAProviderSystem::initScaleOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pScaleHandle,
			(OMX_STRING) "OMX.TI.VPSSM3.VFPC.INDTXSCWB", m_scaleComponent,
			&m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pScaleHandle == NULL)) {
		return false;
	}
	m_scaleComponent->handle = m_pScaleHandle;

	if (!setScaleParams()) {
		return false;
	}
	OMX_SendCommand(m_pScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_INPUT_PORT_START_INDEX, NULL);

	OMXComponentUtils::waitPortSem(m_scaleComponent);

	OMX_SendCommand(m_pScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);
	OMXComponentUtils::waitPortSem(m_scaleComponent);

	return true;
}

bool VGAProviderSystem::initEncodeOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pEncHandle, (OMX_STRING)"OMX.TI.DUCATI.VIDENC",
			m_encComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pEncHandle == NULL)) {
		return FALSE;
	}

	m_encComponent->handle = m_pEncHandle;
	printf("before set vga encode params \n");

	return setEncodeParams();
}

bool VGAProviderSystem::startOMXThread() {
	pthread_attr_init(&m_scaleComponent->ThreadAttr);
	OMXComponentUtils::initialOutputResources(m_scaleComponent);
	if (0 != pthread_create(&m_scaleComponent->outDataStrmThrdId,
		&m_scaleComponent->ThreadAttr,
		scaleOutWriteTask,
		this)) {
		return false;
	}
	printf("start vga scale component thread\n");

	pthread_attr_init(&m_encComponent->ThreadAttr);
	OMXComponentUtils::initialOutputResources(m_encComponent);
	if (0
			!= pthread_create(&m_encComponent->outDataStrmThrdId,
					&m_encComponent->ThreadAttr, VGAEncodeOutWriteTask,
					this)) {
		return false;
	}
	printf("after create vga encode out thread\n");
	return true;
}

bool VGAProviderSystem::stopOMXThread() {
	OMX_S32 ret_value;
	m_bExitThread = true;
	int size = m_inVGAWidth * m_inVGAHeight * 3/2;
	unsigned char* pTempBuffer = new unsigned char[size];
	memset(pTempBuffer, 0, size);
	for (int i = m_nVGAScaleInputIndex; i < m_scaleComponent->inPortParams->nBufferCountActual + 1; ++i) {
		setVGAScaleData(pTempBuffer, size);
		usleep(40000);
	}
	OMX_BUFFERHEADERTYPE pBufferOut;
	write(m_scaleComponent->outPortParams->opBufPipe[1], &pBufferOut, sizeof(pBufferOut));

	SempUtils::semp_pend(m_scaleComponent->eos_sem);
	printf("after exit vga scale exit\n");

	m_bExitVGAEncodeThread = true;

	for (int i = m_nVGAEncodeInputIndex; i < m_encComponent->inPortParams->nBufferCountActual + 1; ++i) {
		setVGAEncodeData(pTempBuffer, size);
		usleep(40000);
	}
	write(m_encComponent->outPortParams->opBufPipe[1],	&pBufferOut, sizeof(pBufferOut));

	SempUtils::semp_pend(m_encComponent->eos_sem);
	printf("after exit vga encode thread\n");


	delete []pTempBuffer;
	pTempBuffer = NULL;

	usleep(40000);

	OMX_ERRORTYPE eError = OMX_SendCommand(m_pScaleHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_scaleComponent);

	eError = OMX_SendCommand(m_pScaleHandle, OMX_CommandStateSet,
		OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	if (!freeScaleBuffers() ){
		return false;
	}

	eError = OMX_SendCommand(m_pEncHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_encComponent);

	eError = OMX_SendCommand(m_pEncHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	if (OMX_ErrorNone != eError) {
		return false;
	}
	if (!freeEncodeBuffers() ) {
		return false;
	}

	if (!freeOMXHandles() ) {
		return false;
	}

	pthread_join(m_scaleComponent->outDataStrmThrdId, NULL);

	printf("before wait end vga enc thread\n");
	pthread_join(m_encComponent->outDataStrmThrdId, NULL);
	pthread_attr_destroy(&m_encComponent->ThreadAttr);
	printf("after wait end vga enc thread\n");

	return true;
}

bool VGAProviderSystem::setScaleParams() {
	printf("set VGA Scalar params\n");
	if (NULL == m_pScaleHandle) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_PARAM_BUFFER_MEMORYTYPE memTypeCfg;
	OMX_PARAM_PORTDEFINITIONTYPE paramPort;
	OMX_PARAM_VFPC_NUMCHANNELPERHANDLE sNumChPerHandle;
	OMX_CONFIG_ALG_ENABLE algEnable;
	OMX_CONFIG_VIDCHANNEL_RESOLUTION chResolution;

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError = OMX_SetParameter (m_pScaleHandle,
			(OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
			&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		return false;
	}
	printf("set VGA Scalar params step 1\n");

	/* Setting Memory type at output port to Raw Memory */
	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError = OMX_SetParameter (m_pScaleHandle,
			(OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
			&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	printf("set VGA Scalar params step 2\n");
	/* set input height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

	OMX_GetParameter(m_pScaleHandle, OMX_IndexParamPortDefinition,
			&paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_inVGAWidth, 16);
	paramPort.format.video.nFrameHeight = m_inVGAHeight;

	paramPort.format.video.nStride = UTIL_ALIGN(m_inVGAWidth, 16);
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	paramPort.nBufferSize = (paramPort.format.video.nStride * paramPort.format.video.nFrameHeight * 3) >> 1;

	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
	eError = OMX_SetParameter (m_pScaleHandle,
			OMX_IndexParamPortDefinition,
			&paramPort);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	printf("set VGA Scalar params step 4\n");

	/* set output height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	OMX_GetParameter(m_pScaleHandle, OMX_IndexParamPortDefinition,
			&paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_inVGAWidth, 16);
	paramPort.format.video.nFrameHeight = m_inVGAHeight;

	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
	/* scalar buffer pitch should be multiple of 16 */
	paramPort.format.video.nStride = UTIL_ALIGN(m_inVGAWidth * 2, 16);

	paramPort.nBufferSize = paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight;

	eError =
	OMX_SetParameter (m_pScaleHandle, OMX_IndexParamPortDefinition,
			&paramPort);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	printf("set VGA Scalar params step 5\n");

	/* set number of channles */
	OMX_INIT_PARAM(&sNumChPerHandle);
	sNumChPerHandle.nNumChannelsPerHandle = 1;
	eError = OMX_SetParameter (m_pScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexParamVFPCNumChPerHandle,
			&sNumChPerHandle);
	if (eError != OMX_ErrorNone) {
		printf("set VGA Scale num channel per handle failed %x\n", eError);
		return false;
	}

	/* set VFPC input and output resolution information */
	printf("set input resolution \n");

	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_inVGAWidth, 16);
	chResolution.Frm0Height = m_inVGAHeight;
	chResolution.Frm0Pitch = UTIL_ALIGN(m_inVGAWidth, 16);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = UTIL_ALIGN(m_inVGAWidth, 16);
	chResolution.FrmCropHeight = m_inVGAHeight;
	chResolution.eDir = OMX_DirInput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("set VGA Scale input resolution failed %x\n", eError);
		return false;
	}

	printf("set output resolution \n");
	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_inVGAWidth, 16);
	chResolution.Frm0Height = m_inVGAHeight;

	chResolution.Frm0Pitch = UTIL_ALIGN(m_inVGAWidth * 2, 16);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = 0;
	chResolution.FrmCropHeight = 0;
	chResolution.eDir = OMX_DirOutput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("set VGA Scale output resolution failed %x\n", eError);
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

bool VGAProviderSystem::setEncodeParams() {
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
	eError = OMX_GetParameter(m_pEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* set the actual number of buffers required */
	tPortDef.nBufferCountActual = KEncodeComponentInOutPutNum;
	/* set the video format settings */

	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(m_inVGAWidth, 32);
	tPortDef.format.video.nFrameHeight = m_inVGAHeight;
	tPortDef.format.video.nStride = UTIL_ALIGN(m_inVGAWidth, 32);
	tPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	tPortDef.format.video.xFramerate = (m_pKernel->getSetting()->getVideoFrameRate()<< 16);
	/* settings for OMX_IndexParamVideoPortFormat */
	tPortDef.nBufferSize = (tPortDef.format.video.nStride
			* tPortDef.format.video.nFrameHeight * 3) >> 1;

	eError = OMX_SetParameter(m_pEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf("failed to set Encode OMX_IndexParamPortDefinition for input \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for input \n");

	OMX_INIT_PARAM(&tPortDef);

	tPortDef.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* settings for OMX_IndexParamPortDefinition */
	/* set the actual number of buffers required */

	tPortDef.nBufferCountActual = KEncodeComponentInOutPutNum;
	tPortDef.format.video.cMIMEType = "H264";
	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(m_inVGAWidth, 32);
	tPortDef.format.video.nFrameHeight = m_inVGAHeight;
	tPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	tPortDef.format.video.xFramerate = (m_pKernel->getSetting()->getVideoFrameRate() << 16);
	tPortDef.format.video.nBitrate = m_pKernel->getSetting()->getVGAEncBitrate();
	/* settings for OMX_IndexParamVideoPortFormat */

	eError = OMX_SetParameter(m_pEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
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
	tVidEncBitRate.nTargetBitrate = m_pKernel->getSetting()->getVGAEncBitrate();
	eError = OMX_SetParameter(m_pEncHandle, OMX_IndexParamVideoBitrate,&tVidEncBitRate);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Set the profile and level for H264 */
	OMX_INIT_PARAM(&tProfileLevel);
	tProfileLevel.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pEncHandle, OMX_IndexParamVideoProfileLevelCurrent, &tProfileLevel);

	if (m_pKernel->getSetting()->getLiveEncHighProfile()) {
		tProfileLevel.eProfile = OMX_VIDEO_AVCProfileHigh;
	} else {
		tProfileLevel.eProfile = OMX_VIDEO_AVCProfileBaseline; //OMX_VIDEO_AVCProfileHigh
	}
	tProfileLevel.eLevel = OMX_VIDEO_AVCLevel42;

	eError =
	OMX_SetParameter(m_pEncHandle, OMX_IndexParamVideoProfileLevelCurrent,
			&tProfileLevel);
	if (eError != OMX_ErrorNone)
		return false;

	OMX_INIT_PARAM(&tEncoderPreset);
	tEncoderPreset.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset, &tEncoderPreset);
	tEncoderPreset.eEncodingModePreset = OMX_Video_Enc_High_Speed_Med_Quality;
	tEncoderPreset.eRateControlPreset = OMX_Video_RC_Low_Delay;
	eError = OMX_SetParameter(m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset, &tEncoderPreset);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_INIT_PARAM(&tDynParams);

	tDynParams.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	if (m_pKernel->getSetting()->getLiveEncHighProfile()) {
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval =
				25;
	} else {
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.generateHeader = XDM_GENERATE_HEADER; // sps/pps
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.forceFrame = IVIDEO_IDR_FRAME;
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval = 25;
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.interFrameInterval = 1;
	}

	eError = OMX_SetParameter (m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	OMX_INIT_PARAM (&tStaticParam);

	tStaticParam.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter (m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	tStaticParam.videoStaticParams.h264EncStaticParams.videnc2Params.encodingPreset = XDM_USER_DEFINED;

	tStaticParam.videoStaticParams.h264EncStaticParams.numTemporalLayer = IH264_TEMPORAL_LAYERS_1;

	if (m_pKernel->getSetting()->getLiveEncHighProfile()) {
		tStaticParam.videoStaticParams.h264EncStaticParams.transformBlockSize = IH264_TRANSFORM_8x8;
		tStaticParam.videoStaticParams.h264EncStaticParams.entropyCodingMode = IH264_ENTROPYCODING_CABAC;
	} else {
		tStaticParam.videoStaticParams.h264EncStaticParams.transformBlockSize = IH264_TRANSFORM_4x4;
		tStaticParam.videoStaticParams.h264EncStaticParams.entropyCodingMode = IH264_ENTROPYCODING_CAVLC;
	}
	tStaticParam.videoStaticParams.h264EncStaticParams.intraCodingParams.chromaIntra8x8Enable |= 1;

	 /* for the mask bits, please refer to codec user guide */
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
	if (m_pKernel->getSetting()->getLiveEncHighProfile()) {
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.videoFullRangeFlag = 0;
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.timingInfoPresentFlag = 1;
	} else {
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.videoFullRangeFlag = 1;
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.timingInfoPresentFlag = 0;
	}

	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.hrdParamsPresentFlag = 0;
	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.numUnitsInTicks = 1000;

	eError = OMX_SetParameter (m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	return (OMX_ErrorNone == eError);
}


bool VGAProviderSystem::freeOMXHandles() {
	OMX_ERRORTYPE eError = OMX_FreeHandle(m_pScaleHandle);
	if ((eError != OMX_ErrorNone)) {
		return false;
	}
	printf(" scalar free handle \n");

	eError = OMX_FreeHandle(m_pEncHandle);
	if (OMX_ErrorNone != eError) {
		return false;
	}
	printf("free vga enc handle\n");
	return true;
}

bool VGAProviderSystem::freeScaleBuffers() {
	if (NULL == m_scaleComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (OMX_U32 j = 0; j < m_scaleComponent->inPortParams->nBufferCountActual; j++) {
		eError = OMX_FreeBuffer(m_pScaleHandle, OMX_VFPC_INPUT_PORT_START_INDEX,
							m_scaleComponent->inPortParams->pInBuff[j]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	for (OMX_U32 j = 0; j < m_scaleComponent->outPortParams->nBufferCountActual; j++) {
		eError = OMX_FreeBuffer(m_pScaleHandle, OMX_VFPC_OUTPUT_PORT_START_INDEX,
							m_scaleComponent->outPortParams->pOutBuff[j]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		Memory_free (m_heap, m_scaleComponent->outPortParams->pOutBuff[j]->pBuffer,
							m_scaleComponent->outPortParams->nBufferSize);
	}

	OMXComponentUtils::waitStateSem(m_scaleComponent);
	return true;
}

bool VGAProviderSystem::freeEncodeBuffers() {
	if (NULL == m_encComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_encComponent->inPortParams->nBufferCountActual;
			i++) {
		eError = OMX_FreeBuffer (m_pEncHandle,
				OMX_VIDENC_INPUT_PORT,
				m_encComponent->inPortParams->pInBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	for (OMX_U32 i = 0; i < m_encComponent->outPortParams->nBufferCountActual;
			i++) {
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

void VGAProviderSystem::setVGAScaleData(unsigned char* pPhyAddr, int size) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;

	if (m_nVGAScaleInputIndex <  m_scaleComponent->inPortParams->nBufferCountActual) {
		OMX_PTR pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)m_scaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]->pBuffer);

		m_pKernel->getEdmaInstance()->copy(pTempPhyAddr, pPhyAddr, m_inVGAWidth* 3/2, m_inVGAWidth * 3/2, size, 1);

		m_scaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]->nFilledLen = size;
		m_scaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]->nOffset = 0;
		m_scaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]->nAllocLen = size;
		m_scaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]->nInputPortIndex = 0;

		err = OMX_EmptyThisBuffer (m_pScaleHandle,
				m_scaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]);
		m_nVGAScaleInputIndex++;
		return;
	}

	/* Read empty buffer pointer from the pipe */
	read(m_scaleComponent->inPortParams->ipBufPipe[0], &pBufferIn, sizeof(pBufferIn));
	OMX_PTR pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)pBufferIn->pBuffer);
	m_pKernel->getEdmaInstance()->copy(pTempPhyAddr, pPhyAddr,
				m_inVGAWidth * 3/2, m_inVGAWidth * 3 /2, size, 1);

	pBufferIn->nFilledLen = size;
	pBufferIn->nOffset = 0;
	pBufferIn->nAllocLen = size;
	pBufferIn->nInputPortIndex = 0;

	err = OMX_EmptyThisBuffer (m_scaleComponent->handle, pBufferIn);
	if (OMX_ErrorNone != err) {
		/* put back the frame in pipe and wait for state change */
		write(m_scaleComponent->inPortParams->ipBufPipe[1], &pBufferIn, sizeof(pBufferIn));
		printf("exit vga scale read thread\n");
	}
}

void VGAProviderSystem::setVGAEncodeData(unsigned char* pPhyAddr, int size) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;
	OMX_PTR pTempPhyAddr;
	if (m_nVGAEncodeInputIndex < m_encComponent->inPortParams->nBufferCountActual) {
		pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)m_encComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->pBuffer);
		m_pKernel->getEdmaInstance()->copy((unsigned char*)pTempPhyAddr, pPhyAddr,
			m_inVGAWidth * 3/2, m_inVGAWidth * 3/2, size, 1);

		m_encComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nFilledLen = size;
		m_encComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nOffset = 0;
		m_encComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nAllocLen = size;
		m_encComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nInputPortIndex = 0;

		err = OMX_EmptyThisBuffer (m_pEncHandle,
				m_encComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]);

		m_nVGAEncodeInputIndex++;
		return;
	}

	read(m_encComponent->inPortParams->ipBufPipe[0], &pBufferIn, sizeof(pBufferIn));

	pTempPhyAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy((uint32_t) pBufferIn->pBuffer);

	m_pKernel->getEdmaInstance()->copy((unsigned char*)pTempPhyAddr, pPhyAddr,
			m_inVGAWidth * 3/2, m_inVGAWidth * 3/2, size, 1);

	pBufferIn->nFilledLen = size;
	pBufferIn->nOffset = 0;
	pBufferIn->nAllocLen = size;
	pBufferIn->nInputPortIndex = 0;

	err = OMX_EmptyThisBuffer (m_encComponent->handle, pBufferIn);

	if (OMX_ErrorNone != err) {
		/* put back the frame in pipe and wait for state change */
		write(m_encComponent->inPortParams->ipBufPipe[1], &pBufferIn,
				sizeof(pBufferIn));
		printf("exit thread\n");
	}
}

void VGAProviderSystem::onVGAScaleWriteTask() {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	printf("on vga scale write task\n");
	while (1) {
		read(m_scaleComponent->outPortParams->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
		if (m_bExitThread) {
			SempUtils::semp_post(m_scaleComponent->eos_sem);
			pthread_exit (NULL);
			break;
		}
		for (vector<IVGAProviderObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
			(*it)->onProviderYUYV(pBufferOut->pBuffer, m_inVGAWidth, m_inVGAHeight);
		}

	    err = OMX_FillThisBuffer (m_scaleComponent->handle, pBufferOut);
		if (OMX_ErrorNone != err) {
			write(m_scaleComponent->outPortParams->opBufPipe[1], &pBufferOut, sizeof(pBufferOut));

			SempUtils::semp_post(m_scaleComponent->eos_sem);
			pthread_exit(NULL);
			break;
		}
	}
}

void VGAProviderSystem::onVGAEncodeWriteTask() {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;

	printf("on vga encode write task\n");
	unsigned int nvgaEncodeCount = 0;
	while (1) {
		read(m_encComponent->outPortParams->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
	    if (m_bExitVGAEncodeThread)  {
		    SempUtils::semp_post(m_encComponent->eos_sem);
		    printf("exit vga encode write thread\n");
		    pthread_exit(NULL);
		    break;
		}
		m_pKernel->getLiveCourseManager()->sendData(m_pKernel->getSetting()->getLCVGAPath().c_str(), (const char*)pBufferOut->pBuffer
				, pBufferOut->nFilledLen, MTG_DATA_TYPE_VIDEO, eLCVGAVideo);
		for (vector<IVGAProviderObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
			(*it)->onProviderH264(pBufferOut->pBuffer, pBufferOut->nFilledLen);
		}

		nvgaEncodeCount++;
	    if (0 == nvgaEncodeCount%25) {
			OMX_CONFIG_INTRAREFRESHVOPTYPE tConfigIntraRefreshVopType;
			OMX_INIT_PARAM(&tConfigIntraRefreshVopType);
			tConfigIntraRefreshVopType.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
			tConfigIntraRefreshVopType.IntraRefreshVOP = OMX_TRUE;
			err = OMX_SetConfig(m_pEncHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tConfigIntraRefreshVopType);
			if (OMX_ErrorNone != err) {
				//return false;
			}
	    }

		err = OMX_FillThisBuffer (m_encComponent->handle, pBufferOut);

		if (OMX_ErrorNone != err) {
			write(m_encComponent->outPortParams->opBufPipe[1],
					&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client \n");
			pthread_exit(NULL);

		}
	}
}

void VGAProviderSystem::onNotifyVGAFMT(CaptureVGA* pCaptureHandle, int width, int height) {
	return;
}

void VGAProviderSystem::onCaptureVGABuffer(CaptureVGA* pCaptureHandle, unsigned char* pNV12Buffer, int width, int height) {
	if (m_bFreezeVGA) {
		return;
	}
	int size = width * height * 3/2;
	if (!m_bgetVGABuffer )  {
		printf("get vga buffer\n");

		m_vgaCapturebefTime = CommonUtils::GetTickCount();
		m_bgetVGABuffer = true;
		m_nVgaCaptureMicroSecond = 0;
	}
	m_vgaCaptureaftTime = CommonUtils::GetTickCount();
	long long diff = m_vgaCaptureaftTime - m_vgaCapturebefTime;
	if (diff < m_nVgaCaptureMicroSecond) {
		return;
	} else {
		m_nVgaCaptureMicroSecond += 40;
		if (m_bVGAUnder768) {
			unsigned char* py = pNV12Buffer;
			unsigned char* pu = py + width * height;
			memset(py + width * (720), 0x10, (height - 720) * width);
			memset(pu + width * (720) /2, 0x80, (height - 720) * width/2 );
		}
		setVGAScaleData(pNV12Buffer - buffer_addr[0] + (unsigned char*)VGA_BASE_ADDR, size);
		for (vector<IVGAProviderObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
			(*it)->onProviderNV12(pNV12Buffer, width, height);
		}
		setVGAEncodeData(pNV12Buffer - buffer_addr[0] + (unsigned char*)VGA_BASE_ADDR, size);
	}
	return;
}

void VGAProviderSystem::setResolution720P(bool bUnder720P) {
	m_bVGAUnder768 = bUnder720P;
}

void VGAProviderSystem::onTimeout(Timer* timer) {
	if (timer != &m_captureVGATimer) {
		return;
	}
	m_vgaMutex.lock();
	if (NULL == m_pCaptureVGAHandle) {
		m_vgaMutex.unlock();
		return;
	}

	if (m_bStartCaptureVGASucess) {
		m_vgaMutex.unlock();
		return;
	}

	if (m_pCaptureVGAHandle->init()) {
		m_bVGAUnder768 = false;
		m_bStartCaptureVGASucess = m_pCaptureVGAHandle->start();
	}
	m_vgaMutex.unlock();
}


bool VGAProviderSystem::doFreezeVGA(bool bFreezeVGA) {
	m_bFreezeVGA = bFreezeVGA;
	return true;
}
