#include "resourceMode_SDI.h"
#include "OMXComponentUtils.h"
#include "msgq.h"
#include "../utils/commonUtils.h"
#include "../kernel.h"
#include "../constStringDefine.h"
#include "audioEncodeFactory.h"

extern unsigned char *buffer_addr[MAX_BUFFER];
extern "C" Ptr DomxCore_mapUsrVirtualAddr2phy (UInt32 pUsrVirtAddr);


void* ResourceModeHandle_SDI::OutputBitStreamWriteTask(void *threadsArg) {
	stConnInConnOutThreadArgs *pInOutParam =
			(stConnInConnOutThreadArgs*) threadsArg;
	ComponentWrapper *thisComp = pInOutParam->pComponent;
	ResourceModeHandle_SDI* pSelf = (ResourceModeHandle_SDI*) pInOutParam->pParent;
	if (NULL != pSelf) {
		pSelf->onWriteTask(thisComp);
	}
	return NULL;
}

void* ResourceModeHandle_SDI::ConnInConnOutTask(void *threadsArg) {
	stConnInConnOutThreadArgs *pInOutParam =
			(stConnInConnOutThreadArgs*) threadsArg;
	ComponentWrapper *thisComp = pInOutParam->pComponent;
	ResourceModeHandle_SDI* pSelf = (ResourceModeHandle_SDI*) pInOutParam->pParent;
	if (NULL != pSelf) {
		pSelf->onConnectTask(thisComp);
	}
	return NULL;
}

OMX_ERRORTYPE ResourceModeHandle_SDI::procCallbackEventHandler(OMX_HANDLETYPE hComponent,
		OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1,
		OMX_U32 nData2, OMX_PTR pEventData) {
	ComponentWrapper *comp = (ComponentWrapper *) ptrAppData;

	printf("got event\n");
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
	} else if (eEvent == OMX_EventError) {
		printf("get OMX error\n");
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE ResourceModeHandle_SDI::procCallbackEmptyBufferDone(OMX_HANDLETYPE hComponent,
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
			printf("Error writing into local Pipe!\n");
			eError = OMX_ErrorNotReady;
			return eError;
		}
	}

	return eError;
}

OMX_ERRORTYPE ResourceModeHandle_SDI::procCallbackFillBufferDone(OMX_HANDLETYPE hComponent,
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

void ResourceModeHandle_SDI::notifySDIProviderBuffer(int aIndex, unsigned char* pYUYVBuffer, int width, int height) {
	if (m_bPaused || m_bTailState) {
		return;
	}

	int size = width * height * 2;
	if (aIndex == 0) {
		setSDIData(pYUYVBuffer - buffer_addr[0] + (unsigned char*)VGA_BASE_ADDR, size, 0, 0);
	} else if (aIndex == 1) {
		setSDIData(pYUYVBuffer - buffer_addr[0] + (unsigned char*)VGA_BASE_ADDR, size, 1, 0);
	}
}

void ResourceModeHandle_SDI::OnAAC(char* aac, int len) {

}

void ResourceModeHandle_SDI::OnSpeex(char* spx, int len) {

}

void ResourceModeHandle_SDI::onAACEncoded(unsigned char* pAACBuf, int len) {
	if (m_bPaused) {
		return;
	}
	fwrite(pAACBuf, 1, len, m_pAACFile);
	for (int i = 0; i < KResourceNum; ++i) {
		if (i == 2) {
			if (m_bThreadsStart) {
				SempUtils::semp_pend(&m_resourceMP4Sem[i]);
				m_pResourceMP4File[i]->writeAACAudio((char*)pAACBuf, len);
				SempUtils::semp_post(&m_resourceMP4Sem[i]);
			}
		} else {
			SempUtils::semp_pend(&m_resourceMP4Sem[i]);
			m_pResourceMP4File[i]->writeAACAudio((char*)pAACBuf, len);
			SempUtils::semp_post(&m_resourceMP4Sem[i]);
		}
	}
}

bool ResourceModeHandle_SDI::onProviderNV12(unsigned char* pNV12Buffer, int width, int height) {
	if (m_bPaused) {
		return true;
	}

	int size = width * height * 3 / 2;
	if (width != m_inVGAWidth && height != m_inVGAHeight) {
		return true;
	}
	setVGAEncodeData(pNV12Buffer - buffer_addr[0] + (unsigned char*)VGA_BASE_ADDR, size);
	return true;
}

bool ResourceModeHandle_SDI::onProviderYUYV(unsigned char* pYUYVBuffer, int width, int height) {
	return true;
}

bool ResourceModeHandle_SDI::onVgaStatus(EVGAStatus status) {
	return true;
}

void ResourceModeHandle_SDI::onTimeout(Timer* pTimer) {
}

ResourceModeHandle_SDI::ResourceModeHandle_SDI(Kernel* pKernel) : BaseMode(pKernel)  {
	m_nVGAEncodeInputIndex = 0;

	m_bExit = false;
	m_bPaused = false;
	m_bTailState = false;
	m_liveOutWidth = 1280;
	m_liveOutHeight = 720;


	for (int i = 0; i < KResourceNum; ++i) {
		//m_pVideoInterface[i] = NULL;
		m_pResourceMP4File[i] = NULL;
	}
	for (int i = 0; i < KMaxSDICount; ++i) {
		m_pSDIDEIHandle[i] = NULL;
		m_pSDIEncHandle[i] = NULL;
		m_SDIDEIComponent[i] = NULL;
		m_SDIEncComponent[i] = NULL;
		m_nSDIInputIndex[i] = 0;
	}

	m_pVGAEncHandle = NULL;
	m_VGAEncComponent = NULL;
	m_rtmpHandle = NULL;

	m_pAACFile = NULL;
	m_pRecordH264File = NULL;

	m_bThreadsStart = false;
}

ResourceModeHandle_SDI::~ResourceModeHandle_SDI() {
	uninit();
}

BOOL ResourceModeHandle_SDI::init(Setting* pSetting) {
	if (NULL == pSetting) {
		return FALSE;
	}

	m_pSetting = pSetting;
	PRChannel* channel = m_pSetting->getIPCChannel();
	IPCInfo *info = m_pSetting->getIPCInfo(channel->dwTeacherChannel);
	if (NULL != info) {
		//m_inHeight[0] = info->dwIPCHeight;
		//m_inWidth[0] = info->dwIPCWidth;
		m_inWidth[0] = 1920;
		m_inHeight[0] = 1080;
	} else {
		m_inHeight[0] = 0;
		m_inWidth[0] = 0;
	}

	info = m_pSetting->getIPCInfo(channel->dwStudentChannel);
	if (NULL != info) {
		//m_inHeight[1] = info->dwIPCHeight;
		//m_inWidth[1] = info->dwIPCWidth;
		m_inWidth[1] = 1920;
		m_inHeight[1] = 1080;
	} else {
		m_inHeight[1] = 0;
		m_inWidth[1] = 0;
	}

	info = m_pSetting->getIPCInfo(channel->dwDeviceChannel);
	if (NULL != info) {
		m_inHeight[2] = info->dwIPCHeight;
		m_inWidth[2] = info->dwIPCWidth;
	} else {
		m_inHeight[2] = 0;
		m_inWidth[2] = 0;
	}

	m_nFrameRate = m_pSetting->getVideoFrameRate();
	m_nEncBitRate = m_pSetting->getVGAEncBitrate();

	if (m_nEncBitRate >= 2048000) {
		m_nEncBitRate = 2048000; //do not set great than 4096000,it will be get black screen.
	}


#ifndef AUDIO_CODECENGINE
	m_pAudioEncode = AudioEncodeFactory::createInstance(true);
	m_pAudioEncode->addListener(this);
	m_pAudioEncode->init(2, m_pSetting->getAudioSampleRate(), 16, m_pSetting->getAACBitRate()
			, m_pSetting->getPCMNonBlock(), m_pSetting->getAudioDeviceName());
#else
	m_pAudioEncode = new codec_engine();
	m_pAudioEncode->addListener(this);
	m_pKernel->getLiveCourseManager()->addListener(m_pAudioEncode);
#endif

	printf("after init audio encode\n");

	m_sdiProvider.init();
	m_sdiProvider.addListener(this);

	m_strMp4FileName = m_pSetting->getSaveFileName();

	m_pCb.EventHandler = procCallbackEventHandler;
	m_pCb.EmptyBufferDone = procCallbackEmptyBufferDone;
	m_pCb.FillBufferDone = procCallbackFillBufferDone;

	m_heap = m_pKernel->getHeapObjectInstance();
	//m_heap = (IHeap_Object*)SharedRegion_getHeap(IL_CLIENT_SR);

	printf("resource mode init ok\n");

	m_inVGAWidth = 1024;
	m_inVGAHeight = 768;

	createComponents();

	allocInOutParams();

	if (!initOMXHandle()) {
		return FALSE;
	}

	return TRUE;
}

BOOL ResourceModeHandle_SDI::uninit() {
	if (!m_bExit) {
		return FALSE;
	}
	if (m_bThreadsStart) {
		freeInOutParams();
		destroyComponents();
		m_bThreadsStart = false;
	}
	m_bExit = false;

	printf("resource mode uninit\n");
	return TRUE;
}

BOOL ResourceModeHandle_SDI::start() {
	if (m_bExit) {
		return FALSE;
	}
	printf("resource mode handle start\n");
	openFileHandles();

	if (NULL != m_pAudioEncode) {
		m_pAudioEncode->start();
	}
	printf("after audio encode start\n");

	m_pKernel->getVGAProviderSystemInstance()->addListener(this);
	m_sdiProvider.open();

	startOMXThread();
	printf("after start OMX Thread\n");
	m_bThreadsStart = true;

	return TRUE;
}

BOOL ResourceModeHandle_SDI::stop() {
	m_pKernel->getVGAProviderSystemInstance()->removeListener(this);
	m_sdiProvider.close();

	stopOMXThread();
	printf("before stop audio encode\n");
	if (NULL != m_pAudioEncode) {
		m_pAudioEncode->stop();
	}

	closeFileHandles();
	printf("uninit mp4 rtmp handle\n");

	uninit();
	printf("after stop\n");
	return TRUE;
}

BOOL ResourceModeHandle_SDI::pause() {
	m_bPaused = true;
	return TRUE;
}

BOOL ResourceModeHandle_SDI::resume() {
	m_bPaused = false;
	return TRUE;
}

BOOL ResourceModeHandle_SDI::doMovieHead() {
	return TRUE;
}

BOOL ResourceModeHandle_SDI::doMovieTail() {
	m_bTailState = true;
	return TRUE;
}

bool ResourceModeHandle_SDI::switchScene() {
	return true;
}

bool ResourceModeHandle_SDI::isComponentCreated() {
	return m_bThreadsStart;
}


bool ResourceModeHandle_SDI::startOMXThread() {
	pthread_attr_init(&m_VGAEncComponent->ThreadAttr);
	OMXComponentUtils::initialOutputResources(m_VGAEncComponent);
	m_vgaEncParams.pParent = this;
	m_vgaEncParams.pComponent = m_VGAEncComponent;
	if (0 != pthread_create(&m_VGAEncComponent->outDataStrmThrdId,
				&m_VGAEncComponent->ThreadAttr,
				OutputBitStreamWriteTask,
				&m_vgaEncParams)) {
		printf("Create_Task failed !");
		return false;
	}
	printf("vga encode output write task created\n");

	for (int i = 0; i < KMaxSDICount; i++) {
		pthread_attr_init(&m_SDIEncComponent[i]->ThreadAttr);
		OMXComponentUtils::initialOutputResources(m_SDIEncComponent[i]);
		m_SDIEncParams[i].pParent = this;
		m_SDIEncParams[i].pComponent = m_SDIEncComponent[i];
		if (0 != pthread_create(&m_SDIEncComponent[i]->outDataStrmThrdId,
					&m_SDIEncComponent[i]->ThreadAttr,
					OutputBitStreamWriteTask,
					&m_SDIEncParams[i])) {
			printf("Create_Task failed !");
			return false;
		}
		printf(" sdi enc output thread created \n ");

		pthread_attr_init(&m_SDIEncComponent[i]->ThreadAttr);
		if (0 != pthread_create(&m_SDIEncComponent[i]->connDataStrmThrdId,
				&m_SDIEncComponent[i]->ThreadAttr,
				ConnInConnOutTask,
				&m_SDIEncParams[i])) {
			printf("create SDI Encode task failed\n");
			return false;
		}

		pthread_attr_init(&m_SDIDEIComponent[i]->ThreadAttr);
		OMXComponentUtils::initialOutputResources(m_SDIDEIComponent[i]);
		m_SDIDEIParams[i].pParent = this;
		m_SDIDEIParams[i].pComponent = m_SDIDEIComponent[i];
		if (0 != pthread_create(&m_SDIDEIComponent[i]->connDataStrmThrdId,
				&m_SDIDEIComponent[i]->ThreadAttr,
				ConnInConnOutTask,
				&m_SDIDEIParams[i])) {
			printf("Create_Task failed !");
			return false;
		}

		printf(" SDI dei connect thread created \n ");
	}
	return true;
}

bool ResourceModeHandle_SDI::stopOMXThread() {
	if (!m_bThreadsStart) {
		return false;
	}
	OMX_ERRORTYPE eError;
	OMX_S32 ret_value;
	m_bExit = true;

	PipeMsg pipeMsg;
	pipeMsg.cmd = ePipeCmdExit;

	for (int i = 0; i < KMaxSDICount; i++) {
		write(m_SDIDEIComponent[i]->localPipe[1], &pipeMsg, sizeof(PipeMsg));
		write(m_SDIEncComponent[i]->localPipe[1], &pipeMsg, sizeof(PipeMsg));
	}
	for (int i =0 ; i < KMaxSDICount; ++i) {
		SempUtils::semp_pend(m_SDIEncComponent[i]->eos_sem);
	}

	int size = m_inVGAWidth * m_inVGAHeight * 3/2;
	unsigned char* pTempBuffer = new unsigned char[size];
	for (int i = m_nVGAEncodeInputIndex; i < m_VGAEncComponent->inPortParams->nBufferCountActual + 1; ++i) {
		setVGAEncodeData(pTempBuffer, size);
		usleep(40000);
	}
	delete []pTempBuffer;
	SempUtils::semp_pend(m_VGAEncComponent->eos_sem);
	eError = OMX_SendCommand(m_pVGAEncHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	OMXComponentUtils::waitStateSem(m_VGAEncComponent);
	printf("enc state idle\n");


	for (int i = 0; i < KMaxSDICount; i++) {
		eError = OMX_SendCommand (m_pSDIDEIHandle[i], OMX_CommandStateSet,
				OMX_StateIdle, NULL);
		if (eError != OMX_ErrorNone) {
			return FALSE;
		}
		OMXComponentUtils::waitStateSem(m_SDIDEIComponent[i]);
		printf(" sdi nf state idle \n ");

		eError = OMX_SendCommand (m_pSDIEncHandle[i], OMX_CommandStateSet,
				OMX_StateIdle, NULL);
		if (eError != OMX_ErrorNone) {
			return 0;
		}
		OMXComponentUtils::waitStateSem(m_SDIEncComponent[i]);
		printf(" Scalar state idle \n ");
	}

	for (int i = 0; i < KMaxSDICount; ++i) {
		eError = OMX_SendCommand(m_pSDIDEIHandle[i], OMX_CommandStateSet,
				OMX_StateLoaded, NULL);
		if (eError != OMX_ErrorNone) {
			return 0;
		}
		printf("before free dec buffers\n");
		if (!freeSDIDEIBuffers(i)) {
			return 0;
		}
		printf("after free dec buffers\n");

		eError = OMX_SendCommand(m_pSDIEncHandle[i], OMX_CommandStateSet,
				OMX_StateLoaded, NULL);
		if (eError != OMX_ErrorNone) {
			return 0;
		}
		if (!freeSDIEncBuffers(i)) {
			return 0;
		}
	}

	eError = OMX_SendCommand(m_pVGAEncHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (OMX_ErrorNone != eError) {
		return 0;
	}
	if (!freeEncBuffers() ) {
		return 0;
	}
	printf("enc state loaded\n");

	if (!freeOMXHandles() ) {
		return false;
	}

	for (int i = 0; i < KMaxSDICount; ++i) {
		pthread_join(m_SDIDEIComponent[i]->connDataStrmThrdId, NULL);
		pthread_join(m_SDIEncComponent[i]->connDataStrmThrdId, NULL);
		pthread_join(m_SDIEncComponent[i]->outDataStrmThrdId, NULL);
	}
	pthread_join(m_VGAEncComponent->outDataStrmThrdId, (void**) &ret_value);
	return true;
}

bool ResourceModeHandle_SDI::createComponents() {
	OMXComponentUtils::initComponent(m_VGAEncComponent);
	strcpy(m_VGAEncComponent->name, "VGAEnc");
	for (int i = 0; i < KMaxSDICount; ++i) {
		OMXComponentUtils::initComponent(m_SDIDEIComponent[i]);
		sprintf(m_SDIDEIComponent[i]->name, "SDIDEI_%d\n", i);
		OMXComponentUtils::initComponent(m_SDIEncComponent[i]);
		sprintf(m_SDIEncComponent[i]->name, "SDIEnc_%d\n", i);
	}

	m_VGAEncComponent->numInport = 1;
	m_VGAEncComponent->numOutport = 1;
	m_VGAEncComponent->startOutportIndex = 1;

	for (int i = 0; i < KMaxSDICount; ++i) {
		m_SDIDEIComponent[i]->numInport = 1;
		m_SDIDEIComponent[i]->numOutport = 2;
		m_SDIDEIComponent[i]->startOutportIndex = OMX_VFPC_NUM_OUTPUT_PORTS;

		m_SDIEncComponent[i]->numInport = 1;
		m_SDIEncComponent[i]->numOutport = 1;
		m_SDIEncComponent[i]->startOutportIndex = 1;
	}
	return true;
}

bool ResourceModeHandle_SDI::destroyComponents() {
	OMXComponentUtils::uninitComponent(m_VGAEncComponent);
	for (int i = 0; i < KMaxSDICount; ++i) {
		OMXComponentUtils::uninitComponent(m_SDIDEIComponent[i]);
		OMXComponentUtils::uninitComponent(m_SDIEncComponent[i]);
	}
	return true;
}

void ResourceModeHandle_SDI::allocInOutParams() {
	OMXComponentUtils::allocInOutParams(m_VGAEncComponent);
	for (int i = 0; i < KMaxSDICount; ++i) {
		OMXComponentUtils::allocInOutParams(m_SDIDEIComponent[i]);
		OMXComponentUtils::allocInOutParams(m_SDIEncComponent[i]);
	}
	InportParams *inPortParamsPtr;
	OutportParams *outPortParamsPtr;
	for (OMX_U32 i = 0; i < m_VGAEncComponent->numInport; ++i) {
		inPortParamsPtr = m_VGAEncComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		inPortParamsPtr->nBufferSize = ((UTIL_ALIGN(m_inVGAWidth, 32))
				* ((UTIL_ALIGN(m_inVGAHeight, 32))) * 3) >> 1;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (OMX_U32 i = 0; i < m_VGAEncComponent->numOutport; ++i) {
		outPortParamsPtr = m_VGAEncComponent->outPortParams + i;
		outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		outPortParamsPtr->nBufferSize =  ((UTIL_ALIGN(m_inVGAHeight, 32))
				* (UTIL_ALIGN(m_inVGAWidth, 32)) * 3) >> 1;
		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int*) m_VGAEncComponent->localPipe);

	for (int i = 0; i < KMaxSDICount; ++i) {
		for (OMX_U32 j = 0; j < m_SDIDEIComponent[i]->numInport; ++j) {
			inPortParamsPtr = m_SDIDEIComponent[i]->inPortParams + j;
			inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum; //m_pSetting->getBufferCountActual()
			inPortParamsPtr->nBufferSize = (UTIL_ALIGN(HD_WIDTH, 32)) * (UTIL_ALIGN(HD_HEIGHT, 32)) * 2;
			pipe((int *) inPortParamsPtr->ipBufPipe);
		}

		for (OMX_U32 j = 0; j < m_SDIDEIComponent[i]->numOutport; ++j) {
			outPortParamsPtr = m_SDIDEIComponent[i]->outPortParams + j;
			outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			if (0 == j % 2) {
				outPortParamsPtr->nBufferSize = (UTIL_ALIGN(HD_WIDTH, 32))
						* (UTIL_ALIGN(HD_HEIGHT, 32)) * 2;
			} else {
				outPortParamsPtr->nBufferSize = (UTIL_ALIGN(HD_WIDTH, 32)) * (UTIL_ALIGN(HD_HEIGHT, 32)) * 3/2;
			}
			pipe((int *) outPortParamsPtr->opBufPipe);
		}
		pipe((int *) m_SDIDEIComponent[i]->localPipe);

		for (OMX_U32 j = 0; j < m_SDIEncComponent[i]->numInport; ++j) {
			inPortParamsPtr = m_SDIEncComponent[i]->inPortParams + j;
			inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			inPortParamsPtr->nBufferSize = (UTIL_ALIGN(HD_WIDTH, 32)) * (UTIL_ALIGN(HD_HEIGHT, 32)) * 3/2;
			pipe((int *) inPortParamsPtr->ipBufPipe);
		}
		for (OMX_U32 j = 0; j < m_SDIEncComponent[i]->numOutport; ++j) {
			outPortParamsPtr = m_SDIEncComponent[i]->outPortParams + j;
			outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			outPortParamsPtr->nBufferSize = HD_WIDTH * HD_HEIGHT * 3/2;
			pipe((int *) outPortParamsPtr->opBufPipe);
		}
		pipe((int *) m_SDIEncComponent[i]->localPipe);
	}

}

void ResourceModeHandle_SDI::freeInOutParams() {
	OMXComponentUtils::freeInOutParams(m_VGAEncComponent);
	for (int i = 0; i < KMaxSDICount; ++i) {
		OMXComponentUtils::freeInOutParams(m_SDIDEIComponent[i]);
		OMXComponentUtils::freeInOutParams(m_SDIEncComponent[i]);
	}
}

BOOL ResourceModeHandle_SDI::initOMXHandle() {
	OMX_ERRORTYPE eError;
	if (!initEncodeOMXHandle()) {
		printf("init encode OMX handle failed\n");
		return FALSE;
	}
	printf("int encode OMX handle OK width %d, height %d\n", m_inVGAWidth, m_inVGAHeight);

	if (!initSDIDEIOMXHandle()) {
		printf("init SDI DEI OMX Handle failed\n");
		return FALSE;
	}
	printf("init SDI DEI OMX Handle ok\n");

	if (!initSDIEncOMXHandle()) {
		printf("init SDI ENC OMX Handle failed\n");
		return FALSE;
	}
	printf("init SDI ENC OMX Handle OK\n");


	establistConnections();

	for (int i = 0; i < KMaxSDICount; i++) {
		eError = OMX_SendCommand(m_pSDIDEIHandle[i], OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (OMX_ErrorNone != eError) {
			printf("set live scale handle idle error %x\n", eError);
			return false;
		}
		for (OMX_U32 j = 0; j < m_SDIDEIComponent[i]->inPortParams->nBufferCountActual; ++j) {
			eError = OMX_AllocateBuffer (m_pSDIDEIHandle[i],
					&m_SDIDEIComponent[i]->inPortParams->pInBuff[j],
					OMX_VFPC_INPUT_PORT_START_INDEX,
					this,
					m_SDIDEIComponent[i]->inPortParams->nBufferSize);

			if (eError != OMX_ErrorNone) {
				printf("can not use buffer index %d ,error %x\n", i, eError);
				return false;
			}
		}
		for (OMX_U32 j = 0; j < m_SDIDEIComponent[i]->numOutport; ++j) {
			OutportParams *outPortParamsPtr = m_SDIDEIComponent[i]->outPortParams + j;
			for (int k = 0; k < outPortParamsPtr->nBufferCountActual; ++k) {
				m_pHeapBuffer = Memory_alloc(m_heap, outPortParamsPtr->nBufferSize, 128, NULL);
				Memory_set(m_pHeapBuffer, 0, outPortParamsPtr->nBufferSize);
				if (NULL == m_pHeapBuffer) {
					printf("Buffer alloc failed\n");
					return false;
				}
				eError = OMX_UseBuffer(m_pSDIDEIHandle[i],
						&outPortParamsPtr->pOutBuff[k],
						OMX_VFPC_OUTPUT_PORT_START_INDEX + j, this,
						outPortParamsPtr->nBufferSize,
						(OMX_U8*)m_pHeapBuffer);
				if (OMX_ErrorNone != eError) {
					printf("live scale out port use buffer error %x\n", eError);
				}
			}
		}
		printf("before wait sdi dei component sem\n");
		OMXComponentUtils::waitStateSem(m_SDIDEIComponent[i]);
		printf("after wait sdi dei component sem\n");


		eError = OMX_SendCommand (m_SDIEncComponent[i]->handle, OMX_CommandStateSet,
						OMX_StateIdle, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}

		OutportParams *outPortParamsPtr = m_SDIDEIComponent[i]->outPortParams + 1;
		for (OMX_U32 j = 0; j < m_SDIEncComponent[i]->inPortParams->nBufferCountActual; j++) {
			eError = OMX_UseBuffer (m_pSDIEncHandle[i],
					&m_SDIEncComponent[i]->inPortParams->pInBuff[j],
					OMX_VIDENC_INPUT_PORT,
					this,
					outPortParamsPtr->nBufferSize,
					outPortParamsPtr->pOutBuff[j]->pBuffer);

			if (eError != OMX_ErrorNone) {
				return false;
			}
		}
		printf(" sdi enc input port use buffer done \n ");

		for (OMX_U32 j = 0; j < m_SDIEncComponent[i]->outPortParams->nBufferCountActual; j++) {
			m_pHeapBuffer = Memory_alloc (m_heap, m_SDIEncComponent[i]->outPortParams->nBufferSize, 128, NULL);
			if (m_pHeapBuffer == NULL) {
				printf ("Buffer Allocation Failed \n");
				return false;
			}
			eError = OMX_UseBuffer (m_pSDIEncHandle[i],
                    &m_SDIEncComponent[i]->outPortParams->pOutBuff[j],
                    OMX_VIDENC_OUTPUT_PORT, this,
                    m_SDIEncComponent[i]->outPortParams->nBufferSize,
                    (OMX_U8*)m_pHeapBuffer);
			if (eError != OMX_ErrorNone) {
				printf("dec use heap buffer error %x \n", eError);
				return false;
			}
		}
		printf(" sdi enc outport buffers allocated \n ");
		OMXComponentUtils::waitStateSem(m_SDIEncComponent[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		printf(" sdi enc state IDLE \n ");
	}

	eError = OMX_SendCommand (m_pVGAEncHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	for (OMX_U32 i = 0; i < m_VGAEncComponent->inPortParams->nBufferCountActual; ++i) {
		eError = OMX_AllocateBuffer (m_pVGAEncHandle,
				&m_VGAEncComponent->inPortParams->pInBuff[i],
				OMX_VIDENC_INPUT_PORT,
				this,
				m_VGAEncComponent->inPortParams->nBufferSize);

		if (eError != OMX_ErrorNone) {
			return FALSE;
		}
	}
	printf(" encoder input port use buffer done \n ");

	for (OMX_U32 i = 0; i < m_VGAEncComponent->outPortParams->nBufferCountActual; ++i) {
		eError = OMX_AllocateBuffer (m_pVGAEncHandle,
				&m_VGAEncComponent->outPortParams->pOutBuff[i],
				OMX_VIDENC_OUTPUT_PORT, this,
				m_VGAEncComponent->outPortParams->nBufferSize);
		if (eError != OMX_ErrorNone) {
			return FALSE;
		}
	}

	printf(" encoder outport buffers allocated \n ");
	OMXComponentUtils::waitStateSem(m_VGAEncComponent);

	printf(" Encoder state IDLE \n ");


	for (int i = 0; i < KMaxSDICount; i++) {
		eError = OMX_SendCommand(m_pSDIEncHandle[i], OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}

		OMXComponentUtils::waitStateSem(m_SDIEncComponent[i]);

		printf(" SDI ENC state execute \n ");
		eError = OMX_SendCommand(m_pSDIDEIHandle[i], OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}

		OMXComponentUtils::waitStateSem(m_SDIDEIComponent[i]);

		printf("sdi dei state execute \n ");
	}


	eError = OMX_SendCommand(m_pVGAEncHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}

	OMXComponentUtils::waitStateSem(m_VGAEncComponent);
	return TRUE;
}

BOOL ResourceModeHandle_SDI::initSDIDEIOMXHandle() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (int i = 0; i < KMaxSDICount; ++i) {
		OMX_ERRORTYPE eError = OMX_GetHandle(&m_pSDIDEIHandle[i],
		(OMX_STRING) OMX_VFPC_DEIDUALOUT_HIGH_COMP_NAME, m_SDIDEIComponent[i],
		&m_pCb);

		if ((eError != OMX_ErrorNone) || (m_pSDIDEIHandle[i] == NULL)) {
			return FALSE;
		}
		m_SDIDEIComponent[i]->handle = m_pSDIDEIHandle[i];
		printf(" dei component is created \n");
		if (!setSDIDEIParams(i)) {
			return FALSE;
		}

		printf("enable dei input port \n");

		OMX_SendCommand(m_pSDIDEIHandle[i], OMX_CommandPortEnable,
			OMX_VFPC_INPUT_PORT_START_INDEX, NULL);

		OMXComponentUtils::waitPortSem(m_SDIDEIComponent[i]);

		printf("enable dei output port 0 \n");
		OMX_SendCommand(m_pSDIDEIHandle[i], OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);
		OMXComponentUtils::waitPortSem(m_SDIDEIComponent[i]);

		printf("enable dei output port 1 \n");
		OMX_SendCommand(m_pSDIDEIHandle[i], OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX + 1, NULL);
		OMXComponentUtils::waitPortSem(m_SDIDEIComponent[i]);
	}
	return TRUE;
}


BOOL ResourceModeHandle_SDI::initSDIEncOMXHandle() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (int i = 0; i < KMaxSDICount; ++i) {
		eError = OMX_GetHandle(&m_pSDIEncHandle[i],
				(OMX_STRING) "OMX.TI.DUCATI.VIDENC", m_SDIEncComponent[i], &m_pCb);

		if ((eError != OMX_ErrorNone) || (m_pSDIEncHandle[i] == NULL)) {
			return FALSE;
		}

		m_SDIEncComponent[i]->handle = m_pSDIEncHandle[i];
		printf("before set encode params \n");
		if (!setSDIEncParams(i)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL ResourceModeHandle_SDI::initEncodeOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pVGAEncHandle,
			(OMX_STRING) "OMX.TI.DUCATI.VIDENC", m_VGAEncComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pVGAEncHandle == NULL)) {
		return FALSE;
	}

	m_VGAEncComponent->handle = m_pVGAEncHandle;
	printf("before set encode params \n");
	return setEncodeParams();
}

bool ResourceModeHandle_SDI::setEncodeParams() {
	if (NULL == m_pVGAEncHandle) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorUndefined;
	OMX_VIDEO_PARAM_PROFILELEVELTYPE tProfileLevel;
	OMX_VIDEO_PARAM_ENCODER_PRESETTYPE tEncoderPreset;
	OMX_VIDEO_PARAM_BITRATETYPE tVidEncBitRate;
	OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
	OMX_VIDEO_CONFIG_DYNAMICPARAMS tDynParams;

	OMX_INIT_PARAM(&tPortDef);
	tPortDef.nPortIndex = OMX_VIDENC_INPUT_PORT;
	eError = OMX_GetParameter(m_pVGAEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* set the actual number of buffers required */
	tPortDef.nBufferCountActual = KOtherComponetInOutPutNum;
	/* set the video format settings */

	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(m_inVGAWidth, 32);
	tPortDef.format.video.nFrameHeight = m_inVGAHeight;
	tPortDef.format.video.nStride = UTIL_ALIGN(m_inVGAWidth, 32);
	tPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	/* settings for OMX_IndexParamVideoPortFormat */
	tPortDef.nBufferSize = (tPortDef.format.video.nStride
			* tPortDef.format.video.nFrameHeight * 3) >> 1;

	eError = OMX_SetParameter(m_pVGAEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf(
				"failed to set Encode OMX_IndexParamPortDefinition for input \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for input \n");

	OMX_INIT_PARAM(&tPortDef);

	tPortDef.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pVGAEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* settings for OMX_IndexParamPortDefinition */
	/* set the actual number of buffers required */

	tPortDef.nBufferCountActual = KOtherComponetInOutPutNum;
	tPortDef.format.video.cMIMEType = "H264";
	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(m_inVGAWidth, 32);
	tPortDef.format.video.nFrameHeight = m_inVGAHeight;
	tPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	tPortDef.format.video.nBitrate = m_nEncBitRate;
	/* settings for OMX_IndexParamVideoPortFormat */

	eError = OMX_SetParameter(m_pVGAEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf( "failed to set Encode OMX_IndexParamPortDefinition for output \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for output \n");

	/* For changing bit rate following index can be used */
	OMX_INIT_PARAM(&tVidEncBitRate);

	tVidEncBitRate.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pVGAEncHandle, OMX_IndexParamVideoBitrate,
			&tVidEncBitRate);

	tVidEncBitRate.eControlRate = OMX_Video_ControlRateVariable;
	tVidEncBitRate.nTargetBitrate = m_nEncBitRate;
	eError = OMX_SetParameter(m_pVGAEncHandle, OMX_IndexParamVideoBitrate,
			&tVidEncBitRate);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Set the profile and level for H264 */
	OMX_INIT_PARAM(&tProfileLevel);
	tProfileLevel.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pVGAEncHandle, OMX_IndexParamVideoProfileLevelCurrent,
			&tProfileLevel);

	tProfileLevel.eProfile = OMX_VIDEO_AVCProfileHigh;
	tProfileLevel.eLevel = OMX_VIDEO_AVCLevel42;

	eError = OMX_SetParameter(m_pVGAEncHandle, OMX_IndexParamVideoProfileLevelCurrent,
			&tProfileLevel);
	if (eError != OMX_ErrorNone)
		return false;

	OMX_INIT_PARAM(&tEncoderPreset);
	tEncoderPreset.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pVGAEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset,
					&tEncoderPreset);
	tEncoderPreset.eEncodingModePreset = OMX_Video_Enc_High_Speed_Med_Quality;

	tEncoderPreset.eRateControlPreset = OMX_Video_RC_Low_Delay;
	eError = OMX_SetParameter(m_pVGAEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset,
					&tEncoderPreset);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_INIT_PARAM(&tDynParams);

	tDynParams.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pVGAEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval =
			25;

	eError = OMX_SetParameter (m_pVGAEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	return (OMX_ErrorNone == eError);
}

bool ResourceModeHandle_SDI::setSDIEncParams(int aSdiEncIndex) {
	if (aSdiEncIndex < 0 || aSdiEncIndex >= KMaxSDICount || NULL == m_pSDIEncHandle[aSdiEncIndex]) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorUndefined;
	OMX_VIDEO_PARAM_PROFILELEVELTYPE tProfileLevel;
	OMX_VIDEO_PARAM_ENCODER_PRESETTYPE tEncoderPreset;
	OMX_VIDEO_PARAM_BITRATETYPE tVidEncBitRate;
	OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
	OMX_VIDEO_CONFIG_DYNAMICPARAMS tDynParams;

	OMX_INIT_PARAM(&tPortDef);
	tPortDef.nPortIndex = OMX_VIDENC_INPUT_PORT;
	eError = OMX_GetParameter(m_pSDIEncHandle[aSdiEncIndex], OMX_IndexParamPortDefinition, &tPortDef);
	/* set the actual number of buffers required */
	tPortDef.nBufferCountActual = KOtherComponetInOutPutNum;
	/* set the video format settings */

	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(m_inWidth[aSdiEncIndex], 32);
	tPortDef.format.video.nFrameHeight = m_inHeight[aSdiEncIndex];
	tPortDef.format.video.nStride = UTIL_ALIGN(m_inWidth[aSdiEncIndex], 32);
	tPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	/* settings for OMX_IndexParamVideoPortFormat */
	tPortDef.nBufferSize = (tPortDef.format.video.nStride
			* tPortDef.format.video.nFrameHeight * 3) >> 1;

	eError = OMX_SetParameter(m_pSDIEncHandle[aSdiEncIndex], OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf("failed to set Encode OMX_IndexParamPortDefinition for input \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for input \n");

	OMX_INIT_PARAM(&tPortDef);

	tPortDef.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pSDIEncHandle[aSdiEncIndex], OMX_IndexParamPortDefinition, &tPortDef);
	/* settings for OMX_IndexParamPortDefinition */
	/* set the actual number of buffers required */

	tPortDef.nBufferCountActual = KOtherComponetInOutPutNum;
	tPortDef.format.video.cMIMEType = "H264";
	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(m_inWidth[aSdiEncIndex], 32);
	tPortDef.format.video.nFrameHeight = m_inHeight[aSdiEncIndex];
	tPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	tPortDef.format.video.nBitrate = m_pSetting->getVideoBitrate();
	/* settings for OMX_IndexParamVideoPortFormat */

	eError = OMX_SetParameter(m_pSDIEncHandle[aSdiEncIndex], OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf( "failed to set Encode OMX_IndexParamPortDefinition for output \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for output \n");

	/* For changing bit rate following index can be used */
	OMX_INIT_PARAM(&tVidEncBitRate);

	tVidEncBitRate.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pSDIEncHandle[aSdiEncIndex], OMX_IndexParamVideoBitrate,
			&tVidEncBitRate);

	tVidEncBitRate.eControlRate = OMX_Video_ControlRateVariable;
	tVidEncBitRate.nTargetBitrate = m_pSetting->getVideoBitrate();
	eError = OMX_SetParameter(m_pSDIEncHandle[aSdiEncIndex], OMX_IndexParamVideoBitrate,
			&tVidEncBitRate);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Set the profile and level for H264 */
	OMX_INIT_PARAM(&tProfileLevel);
	tProfileLevel.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pSDIEncHandle[aSdiEncIndex], OMX_IndexParamVideoProfileLevelCurrent,
			&tProfileLevel);

	tProfileLevel.eProfile = OMX_VIDEO_AVCProfileHigh;
	tProfileLevel.eLevel = OMX_VIDEO_AVCLevel42;

	eError = OMX_SetParameter(m_pSDIEncHandle[aSdiEncIndex], OMX_IndexParamVideoProfileLevelCurrent,
			&tProfileLevel);
	if (eError != OMX_ErrorNone)
		return false;

	OMX_INIT_PARAM(&tEncoderPreset);
	tEncoderPreset.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pSDIEncHandle[aSdiEncIndex], (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset,
					&tEncoderPreset);
	tEncoderPreset.eEncodingModePreset = OMX_Video_Enc_High_Speed_Med_Quality;

	tEncoderPreset.eRateControlPreset = OMX_Video_RC_Low_Delay;
	eError = OMX_SetParameter(m_pSDIEncHandle[aSdiEncIndex], (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset,
					&tEncoderPreset);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_INIT_PARAM(&tDynParams);

	tDynParams.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pSDIEncHandle[aSdiEncIndex], (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval =
			25;

	eError = OMX_SetParameter (m_pSDIEncHandle[aSdiEncIndex], (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	return (OMX_ErrorNone == eError);
}

bool ResourceModeHandle_SDI::setSDIDEIParams(int nSdiDEIIndex) {
	if (nSdiDEIIndex < 0 || nSdiDEIIndex > 2) {
		return false;
	}
	if (NULL == m_pSDIDEIHandle[nSdiDEIIndex]) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_PARAM_BUFFER_MEMORYTYPE memTypeCfg;
	OMX_PARAM_PORTDEFINITIONTYPE paramPort;
	OMX_PARAM_VFPC_NUMCHANNELPERHANDLE sNumChPerHandle;
	OMX_CONFIG_ALG_ENABLE algEnable;
	OMX_CONFIG_VIDCHANNEL_RESOLUTION chResolution;

	OMX_CONFIG_SUBSAMPLING_FACTOR sSubSamplinginfo = { 0 };

	OMX_INIT_PARAM(&sSubSamplinginfo);

	sSubSamplinginfo.nSubSamplingFactor = 1;
	eError = OMX_SetConfig (m_pSDIDEIHandle[nSdiDEIIndex], ( OMX_INDEXTYPE )
		( OMX_TI_IndexConfigSubSamplingFactor ), &sSubSamplinginfo );

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError = OMX_SetParameter (m_pSDIDEIHandle[nSdiDEIIndex], (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType, &memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at input port\n");
		return false;
	}
	printf("set input mem buffer type\n");

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError = OMX_SetParameter (m_pSDIDEIHandle[nSdiDEIIndex], (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType, &memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at output port\n");
		return false;
	}
	printf("set output 0 mem buffer type\n");

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError = OMX_SetParameter (m_pSDIDEIHandle[nSdiDEIIndex], (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType, &memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at output port\n");
		return false;
	}
	printf("set output 1 mem buffer type\n");

	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

	OMX_GetParameter(m_pSDIDEIHandle[nSdiDEIIndex], OMX_IndexParamPortDefinition, &paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_inWidth[nSdiDEIIndex], 32);
	paramPort.format.video.nFrameHeight = m_inHeight[nSdiDEIIndex];

	paramPort.format.video.nStride = UTIL_ALIGN(m_inWidth[nSdiDEIIndex] * 2, 32);
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
	paramPort.format.video.xFramerate = (m_nFrameRate << 16);
	paramPort.nBufferSize = paramPort.format.video.nStride
				* paramPort.format.video.nFrameHeight;

	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;

	OMX_SetParameter(m_pSDIDEIHandle[nSdiDEIIndex], OMX_IndexParamPortDefinition, &paramPort);
	printf("set inport param \n");

	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	OMX_GetParameter(m_pSDIDEIHandle[nSdiDEIIndex], OMX_IndexParamPortDefinition, &paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_liveOutWidth, 32);
	paramPort.format.video.nFrameHeight = m_liveOutHeight;
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
	paramPort.format.video.xFramerate = (m_nFrameRate << 16);
	paramPort.nBufferAlignment = 0;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;

	paramPort.format.video.nStride = UTIL_ALIGN(m_liveOutWidth * 2, 32);
	paramPort.nBufferSize = paramPort.format.video.nStride
				* paramPort.format.video.nFrameHeight;

	OMX_SetParameter(m_pSDIDEIHandle[nSdiDEIIndex], OMX_IndexParamPortDefinition, &paramPort);
	printf("set outport 0 param\n");

	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1;
	OMX_GetParameter(m_pSDIDEIHandle[nSdiDEIIndex], OMX_IndexParamPortDefinition, &paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_inWidth[nSdiDEIIndex], 32);
	paramPort.format.video.nFrameHeight = m_inHeight[nSdiDEIIndex];
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	paramPort.format.video.xFramerate = (m_nFrameRate << 16);
	paramPort.nBufferAlignment = 0;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
	paramPort.format.video.nStride = UTIL_ALIGN(m_inWidth[nSdiDEIIndex], 32);

	paramPort.nBufferSize = (paramPort.format.video.nStride
				* paramPort.format.video.nFrameHeight * 3) >> 1;

	OMX_SetParameter(m_pSDIDEIHandle[nSdiDEIIndex], OMX_IndexParamPortDefinition, &paramPort);

	printf("set number of channels\n");

	OMX_INIT_PARAM(&sNumChPerHandle);
	sNumChPerHandle.nNumChannelsPerHandle = 1;
	eError = OMX_SetParameter (m_pSDIDEIHandle[nSdiDEIIndex],
		(OMX_INDEXTYPE)
		OMX_TI_IndexParamVFPCNumChPerHandle,
		&sNumChPerHandle);
	if (eError != OMX_ErrorNone) {
		printf("failed to set num of channels\n");
		return false;
	}

	printf("set input resolution\n");

	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_inWidth[nSdiDEIIndex], 32);
	chResolution.Frm0Height = m_inHeight[nSdiDEIIndex];
	chResolution.Frm0Pitch = UTIL_ALIGN(m_inWidth[nSdiDEIIndex] * 2, 32);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = m_inWidth[nSdiDEIIndex];
	chResolution.FrmCropHeight = m_inHeight[nSdiDEIIndex];

	chResolution.eDir = OMX_DirInput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pSDIDEIHandle[nSdiDEIIndex],
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("failed to set input channel resolution\n");
		return false;
	}

	printf("set output resolution\n");
	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_liveOutWidth, 32);
	chResolution.Frm0Height = m_liveOutHeight;
	chResolution.Frm0Pitch = UTIL_ALIGN(m_liveOutWidth * 2, 32);
	chResolution.Frm1Width = UTIL_ALIGN(m_inWidth[nSdiDEIIndex], 32);
	chResolution.Frm1Height = m_inHeight[nSdiDEIIndex];
	chResolution.Frm1Pitch = UTIL_ALIGN(m_inWidth[nSdiDEIIndex], 32);
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = 0;
	chResolution.FrmCropHeight = 0;
	chResolution.eDir = OMX_DirOutput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pSDIDEIHandle[nSdiDEIIndex],
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

	eError = OMX_SetConfig (m_pSDIDEIHandle[nSdiDEIIndex],
			(OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable,
			&algEnable);
	if (eError != OMX_ErrorNone) {
		printf("failed to disable algo by pass mode\n");
		return false;
	}

	return true;
}

void ResourceModeHandle_SDI::openFileHandles() {
	char buf[255] = {0};
	for (int i = 0; i < KResourceNum; ++i) {
		if (NULL == m_pResourceMP4File[i]) {
			sprintf(buf, "%s_%d.mp4", m_strMp4FileName.c_str(), i);
			m_pResourceMP4File[i] = new Mp4Output();
#ifndef AUDIO_CODECENGINE
			if (i < 2) {
				m_pResourceMP4File[i]->init(buf, m_inWidth[i], m_inHeight[i],
					m_nFrameRate,2, m_pSetting->getAudioSampleRate(), 16);
			} else {
				m_pResourceMP4File[i]->init(buf, m_inVGAWidth, m_inVGAHeight,
						m_nFrameRate, 1, m_pSetting->getAudioSampleRate(), 16);
			}
#else
			if (i < 2) {
				m_pResourceMP4File[i]->init(buf, m_inWidth[i], m_inHeight[i],
					m_nFrameRate,2, m_pSetting->getAudioSampleRate(), 16);
			} else {
				m_pResourceMP4File[i]->init(buf, m_inVGAWidth, m_inVGAHeight,
					m_nFrameRate, 1, m_pSetting->getAudioSampleRate(), 16);
			}
#endif


			SempUtils::semp_init(&m_resourceMP4Sem[i], 1);
		}
	}
	if (NULL == m_pAACFile) {
		sprintf(buf, "%s.aac", m_strMp4FileName.c_str());
		m_pAACFile = fopen(buf, "wb+");
	}
}

void ResourceModeHandle_SDI::closeFileHandles() {
	for (int i = 0; i < KResourceNum; ++i) {
		if (NULL != m_pResourceMP4File[i]) {
			m_pResourceMP4File[i]->uninit();
			delete m_pResourceMP4File[i];
			m_pResourceMP4File[i] = NULL;
			SempUtils::semp_deinit(&m_resourceMP4Sem[i]);
		}
	}
	if (NULL != m_pAACFile) {
		fclose(m_pAACFile);
		m_pAACFile = NULL;
	}

	printf("mp4 uninit\n");
}

void ResourceModeHandle_SDI::setSDIData(unsigned char* pPhyAddr, int size, int aSDIIndex, int channel) {
	unsigned int dataRead = 0;
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;
	ComponentWrapper* deiComp = m_SDIDEIComponent[aSDIIndex];

	int i = m_nSDIInputIndex[aSDIIndex];
	if (i < deiComp->inPortParams->nBufferCountActual) {
		deiComp->inPortParams->pInBuff[i]->nFilledLen = size;
		deiComp->inPortParams->pInBuff[i]->nOffset = 0;
		deiComp->inPortParams->pInBuff[i]->nAllocLen = size;
		deiComp->inPortParams->pInBuff[i]->nInputPortIndex = 0;

		OMX_PTR pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)deiComp->inPortParams->pInBuff[i]->pBuffer);
		m_pKernel->getEdmaInstance()->copy(pTempPhyAddr, pPhyAddr, m_inWidth[aSDIIndex]*2, m_inWidth[aSDIIndex] *2, size, 1);

		err = OMX_EmptyThisBuffer (m_pSDIDEIHandle[aSDIIndex],
			m_SDIDEIComponent[aSDIIndex]->inPortParams->pInBuff[i]);
		m_nSDIInputIndex[aSDIIndex]++;
		return;
	}
	/* Read empty buffer pointer from the pipe */
	read(deiComp->inPortParams->ipBufPipe[0], &pBufferIn, sizeof(pBufferIn));


	OMX_PTR pTempPhyAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy( (uint32_t) pBufferIn->pBuffer);

	m_pKernel->getEdmaInstance()->copy((unsigned char*)pTempPhyAddr, pPhyAddr,
			m_inWidth[aSDIIndex] * 2, m_inWidth[aSDIIndex] * 2, size, 1);

	pBufferIn->nFilledLen = size;

	/* Pass the input buffer to the component */
	err = OMX_EmptyThisBuffer (deiComp->handle, pBufferIn);

	if (OMX_ErrorNone != err) {
		/* put back the frame in pipe and wait for state change */
		write(deiComp->inPortParams->ipBufPipe[1], &pBufferIn, sizeof(pBufferIn));
		printf("sdi nf exit thread\n");
		printf("exit %s read thread\n", deiComp->name);
	}
}


void ResourceModeHandle_SDI::setVGAEncodeData(unsigned char* pPhyAddr, int size) {
	unsigned int dataRead = 0;
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;

	if (m_nVGAEncodeInputIndex < m_VGAEncComponent->inPortParams->nBufferCountActual) {
		OMX_PTR pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)m_VGAEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->pBuffer);

		m_pKernel->getEdmaInstance()->copy(pTempPhyAddr, pPhyAddr,
				m_inVGAWidth * 3/2, m_inVGAWidth * 3/2, size, 1);

		m_VGAEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nFilledLen = size;
		m_VGAEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nOffset = 0;
		m_VGAEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nAllocLen = size;
		m_VGAEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nInputPortIndex = 0;


		err = OMX_EmptyThisBuffer (m_pVGAEncHandle, m_VGAEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]);

		m_nVGAEncodeInputIndex++;
		return;
	}
	/* Read empty buffer pointer from the pipe */
	read(m_VGAEncComponent->inPortParams->ipBufPipe[0], &pBufferIn, sizeof(pBufferIn));
	OMX_PTR pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)pBufferIn->pBuffer);

	m_pKernel->getEdmaInstance()->copy(pTempPhyAddr, pPhyAddr,
			m_inVGAWidth * 3/2, m_inVGAWidth * 3/2, size, 1);


	pBufferIn->nFilledLen = size;
	pBufferIn->nOffset = 0;
	pBufferIn->nAllocLen = size;
	pBufferIn->nInputPortIndex = 0;

	/* Pass the input buffer to the component */
	err = OMX_EmptyThisBuffer (m_VGAEncComponent->handle, pBufferIn);

	if (OMX_ErrorNone != err) {
			/* put back the frame in pipe and wait for state change */
		write(m_VGAEncComponent->inPortParams->ipBufPipe[1], &pBufferIn,
					sizeof(pBufferIn));
		printf("exit thread\n");
	}
}

void ResourceModeHandle_SDI::onWriteTask(ComponentWrapper* aComp) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	printf("resource mode on write task\n");
	fflush(stdout);
	while (1) {
		read(aComp->outPortParams->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
		if (m_bExit)  {
			SempUtils::semp_post(aComp->eos_sem);
			printf("exit out thread\n");
		    pthread_exit(aComp);
		    break;
		 }
		if (!m_bPaused) {
			if (aComp == m_SDIEncComponent[0]) {
				if (NULL != m_pResourceMP4File[0]) {
					SempUtils::semp_pend(&m_resourceMP4Sem[0]);
					m_pResourceMP4File[0]->writeH264Video((char*)pBufferOut->pBuffer, pBufferOut->nFilledLen);
					SempUtils::semp_post(&m_resourceMP4Sem[0]);
				}
			} else if (aComp == m_SDIEncComponent[1]) {
				if (NULL != m_pResourceMP4File[1]) {
					SempUtils::semp_pend(&m_resourceMP4Sem[1]);
					m_pResourceMP4File[1]->writeH264Video((char*)pBufferOut->pBuffer, pBufferOut->nFilledLen);
					SempUtils::semp_post(&m_resourceMP4Sem[1]);
				}
			} else if (aComp == m_VGAEncComponent) {
				if (NULL != m_pResourceMP4File[2]) {
					SempUtils::semp_pend(&m_resourceMP4Sem[2]);
					m_pResourceMP4File[2]->writeH264Video((char*)pBufferOut->pBuffer, pBufferOut->nFilledLen);
					SempUtils::semp_post(&m_resourceMP4Sem[2]);
				}
			}
 		}


	    err = OMX_FillThisBuffer (aComp->handle, pBufferOut);
		if (OMX_ErrorNone != err) {
			write(aComp->outPortParams->opBufPipe[1],	&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client %x\n", err);
			SempUtils::semp_post(aComp->eos_sem);
			pthread_exit(aComp);
			break;
		}
	}
}

void ResourceModeHandle_SDI::onConnectTask(ComponentWrapper *aComponent) {
	PipeMsg pipeMsg;
	OMX_ERRORTYPE err = OMX_ErrorNone;
	printf("onConnectTask\n");
	while (1) {
		/* Read from its own local Pipe */
		read(aComponent->localPipe[0], &pipeMsg, sizeof(PipeMsg));
		switch (pipeMsg.cmd) {
		case ePipeCmdExit:
			printf("exiting thread %s\n ", aComponent->name);

			SempUtils::semp_post(aComponent->eos_sem);
			pthread_exit(NULL);
			break;
		case ePipeCmdEmptyThisBuffer:
			err = OMXComponentUtils::procPipeCmdEmptyThisBuffer(aComponent, &pipeMsg);
			if (OMX_ErrorNone != err) {
				write(aComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
				printf("exit thread\n");

				SempUtils::semp_post(aComponent->eos_sem);
				pthread_exit(NULL);
			}

			break;
		case ePipeCmdFillThisBuffer:
			err = OMXComponentUtils::procPipeCmdFillThisBuffer(aComponent, &pipeMsg);
			if (OMX_ErrorNone != err) {
				write(aComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
				printf("exit thread\n");

				SempUtils::semp_post(aComponent->eos_sem);
				pthread_exit(NULL);
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

bool ResourceModeHandle_SDI::freeSDIDEIBuffers(int aIndex) {
	if (aIndex < 0 || aIndex >= KMaxSDICount || NULL == m_SDIDEIComponent[aIndex]) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_SDIDEIComponent[aIndex]->inPortParams->nBufferCountActual; i++) {
			eError = OMX_FreeBuffer (m_pSDIDEIHandle[aIndex],
					OMX_VFPC_INPUT_PORT_START_INDEX,
					m_SDIDEIComponent[aIndex]->inPortParams->pInBuff[i]);
			if (eError != OMX_ErrorNone) {
				return false;
			}
		}

	for (OMX_U32 j = 0; j < m_SDIDEIComponent[aIndex]->numOutport; ++j) {
		OutportParams *outPortParamsPtr = m_SDIDEIComponent[aIndex]->outPortParams + j;
		for (OMX_U32 i = 0; i < outPortParamsPtr->nBufferCountActual; i++) {
			eError = OMX_FreeBuffer (m_pSDIDEIHandle[aIndex],
					OMX_VFPC_OUTPUT_PORT_START_INDEX + j,
					outPortParamsPtr->pOutBuff[i]);
			if (eError != OMX_ErrorNone) {
				return false;
			}
			Memory_free (m_heap, outPortParamsPtr->pOutBuff[i]->pBuffer,
					outPortParamsPtr->nBufferSize);
		}
	}

	OMXComponentUtils::waitStateSem(m_SDIDEIComponent[aIndex]);
	return true;
}

bool ResourceModeHandle_SDI::freeSDIEncBuffers(int aIndex) {
	if (aIndex < 0 || aIndex >= KMaxSDICount || NULL == m_SDIEncComponent[aIndex]) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (OMX_U32 i = 0; i < m_SDIEncComponent[aIndex]->inPortParams->nBufferCountActual;
			i++) {
		eError = OMX_FreeBuffer (m_pSDIEncHandle[aIndex],
				OMX_VIDENC_INPUT_PORT,
				m_SDIEncComponent[aIndex]->inPortParams->pInBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	for (OMX_U32 i = 0; i < m_SDIEncComponent[aIndex]->outPortParams->nBufferCountActual; i++) {
		eError = OMX_FreeBuffer (m_pSDIEncHandle[aIndex],
				OMX_VIDENC_OUTPUT_PORT,
				m_SDIEncComponent[aIndex]->outPortParams->pOutBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	OMXComponentUtils::waitStateSem(m_SDIEncComponent[aIndex]);
	return true;
}

bool ResourceModeHandle_SDI::freeEncBuffers() {
	if (NULL == m_VGAEncComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_VGAEncComponent->inPortParams->nBufferCountActual;
			i++) {
		eError = OMX_FreeBuffer (m_pVGAEncHandle,
				OMX_VIDENC_INPUT_PORT,
				m_VGAEncComponent->inPortParams->pInBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	for (OMX_U32 i = 0; i < m_VGAEncComponent->outPortParams->nBufferCountActual; i++) {
		eError = OMX_FreeBuffer (m_pVGAEncHandle,
				OMX_VIDENC_OUTPUT_PORT,
				m_VGAEncComponent->outPortParams->pOutBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	OMXComponentUtils::waitStateSem(m_VGAEncComponent);
	return true;
}


bool ResourceModeHandle_SDI::establistConnections() {
	for (OMX_U32 i = 0; i < KMaxSDICount; i++) {
		OMXComponentUtils::connectComponents(m_SDIDEIComponent[i],
				OMX_VFPC_OUTPUT_PORT_START_INDEX + 1, m_SDIEncComponent[i],
				OMX_VFPC_INPUT_PORT_START_INDEX);
	}
	return true;
}

bool ResourceModeHandle_SDI::freeOMXHandles() {
	OMX_ERRORTYPE eError = OMX_FreeHandle(m_pVGAEncHandle);
	if ((eError != OMX_ErrorNone)) {
		printf("free handle failed\n", eError);
		return false;
	}

	for (int i = 0; i < KMaxSDICount; ++i) {
		eError = OMX_FreeHandle(m_pSDIEncHandle[i]);
		if (OMX_ErrorNone != eError) {
			return false;
		}

		eError = OMX_FreeHandle(m_pSDIDEIHandle[i]);
		if (OMX_ErrorNone != eError) {
			return false;
		}
	}
	return true;
}
