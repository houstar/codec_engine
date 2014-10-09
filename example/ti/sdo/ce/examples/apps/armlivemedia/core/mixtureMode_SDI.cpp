#include <signal.h>
#include "mixtureMode_SDI.h"

#include "OMXComponentUtils.h"
#include "OMXCommonDef.h"
#include "msgq.h"
#include "constDefines.h"
#include "../utils/colorConvert.h"
#include "osdManager.h"
#include "v4lDisplay.h"

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>
#include "../utils/commonUtils.h"
#include "membufUtils.h"
#include "../kernel.h"
#include "../constStringDefine.h"
#include "audioEncodeFactory.h"


#define KTeacherLiveWidth 704
#define KTeacherLiveHeight 576


extern long long gHDMIDisplayAddr;
extern long long gVGADisplayAddr;

extern unsigned char *buffer_addr[MAX_BUFFER];
extern "C" Ptr DomxCore_mapUsrVirtualAddr2phy (UInt32 pUsrVirtAddr);

static OMX_PTR MyMapUsrVirtualAddr2phy(uint32_t pUsrVirtAddr) {
	//printf("before MapUsrAddr2phy %x\n", pUsrVirtAddr);
	OMX_PTR ptr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy(pUsrVirtAddr);
	//printf("after MapUsrAddr2phy %x\n", pUsrVirtAddr);
	return ptr;
}


extern long long gbackTImagePhyAddr;
extern long long gbackTSImagePhyAddr;
extern long long gbackTVImagePhyAddr;
extern long long gbackTSVImagePhyAddr;
extern long long glogoImagePhyAddr;
extern long long gcourseImagePhyAddr;
extern long long gheadImagePhyAddr;
extern long long gtailImagePhyAddr;
extern int		glogoWidth;
extern int		glogoHeight;
extern int		gcourseImageWidth;
extern int		gcourseImageHeight;

void* MixtureMode_SDI::OutputBitStreamWriteTask(void *threadsArg) {
	stConnInConnOutThreadArgs *pInOutParam = (stConnInConnOutThreadArgs*) threadsArg;
	ComponentWrapper *thisComp = pInOutParam->pComponent;
	MixtureMode_SDI* pSelf = (MixtureMode_SDI*) pInOutParam->pParent;
	pSelf->onWriteTask(thisComp);
	return NULL;
}

void* MixtureMode_SDI::SDIScaleOutputChannel1(void* threadsArg) {
	stConnInConnOutThreadArgs *pInOutParam = (stConnInConnOutThreadArgs*) threadsArg;
	ComponentWrapper *thisComp = pInOutParam->pComponent;
	MixtureMode_SDI* pSelf = (MixtureMode_SDI*) pInOutParam->pParent;
	pSelf->onSDIScaleChannel1(thisComp);
	return NULL;
}

void* MixtureMode_SDI::SDIScaleOutputChannel2(void* threadsArg) {
	stConnInConnOutThreadArgs *pInOutParam = (stConnInConnOutThreadArgs*) threadsArg;
	ComponentWrapper *thisComp = pInOutParam->pComponent;
	MixtureMode_SDI* pSelf = (MixtureMode_SDI*) pInOutParam->pParent;
	pSelf->onSDIScaleChannel2(thisComp);
	return NULL;
}

void* MixtureMode_SDI::LiveOutputBitStreamWriteTask(void *threadsArg) {
	stConnInConnOutThreadArgs *pInOutParam = (stConnInConnOutThreadArgs*) threadsArg;
	ComponentWrapper *thisComp = pInOutParam->pComponent;
	MixtureMode_SDI* pSelf = (MixtureMode_SDI*) pInOutParam->pParent;
	pSelf->onLiveWriteTask(thisComp);
	return NULL;
}

void* MixtureMode_SDI::VGAScaleOutWriteTask(void* threadsArg) {
	MixtureMode_SDI* pSelf = (MixtureMode_SDI*) threadsArg;
	pSelf->onVGAScaleWriteTask();
	return NULL;
}

void* MixtureMode_SDI::HeadTailScaleInputBitStreamReadTask(void* threadsArg) {
	MixtureMode_SDI* pSelf = (MixtureMode_SDI*) threadsArg;
	pSelf->onHeadTailScaleReadTask();
	return NULL;
}

void* MixtureMode_SDI::HeadTailScaleWriteTask(void* threadsArg) {
	MixtureMode_SDI* pSelf = (MixtureMode_SDI*) threadsArg;
	pSelf->onHeadTailScaleWriteTask();
	return NULL;
}

void* MixtureMode_SDI::VGAEncodeOutWriteTask(void* threadsArg) {
	MixtureMode_SDI* pSelf = (MixtureMode_SDI*)threadsArg;
	if (NULL != pSelf) {
		pSelf->onVGAEncodeWriteTask();
	}
	return NULL;
}


void* MixtureMode_SDI::InputMixStreamReadTask(void* threadsArg) {
	stConnInConnOutThreadArgs *pInOutParam = (stConnInConnOutThreadArgs*) threadsArg;
	ComponentWrapper *thisComp = pInOutParam->pComponent;
	MixtureMode_SDI* pSelf = (MixtureMode_SDI*) pInOutParam->pParent;
	pSelf->onMixReadTask(thisComp);
	return NULL;
}

void* MixtureMode_SDI::SDIEncOutputWriteTask(void* threadsArg) {
	stConnInConnOutThreadArgs *pInOutParam = (stConnInConnOutThreadArgs*) threadsArg;
	ComponentWrapper *thisComp = pInOutParam->pComponent;
	MixtureMode_SDI* pSelf = (MixtureMode_SDI*) pInOutParam->pParent;
	pSelf->onSDIEncWriteTask(thisComp);
	return NULL;
}

void* MixtureMode_SDI::ConnInConnOutTask(void *threadsArg) {
	stConnInConnOutThreadArgs *pInOutParam =
			(stConnInConnOutThreadArgs*) threadsArg;
	ComponentWrapper *thisComp = pInOutParam->pComponent;
	MixtureMode_SDI* pSelf = (MixtureMode_SDI*) pInOutParam->pParent;
	pSelf->onConnectTask(thisComp);
	return NULL;
}

void MixtureMode_SDI::onTimeout(Timer* pTimer) {
	if (pTimer == &m_ExitTimer) {
		m_ExitTimer.stop();
		m_ExitTimer.uninit();
		m_ExitTimerValid = false;
		if (!m_bUpHalfInited) {
			switchScene();
		}
	} else if (pTimer == &m_checkAudioSyncTimer) {
		m_bcheckAudioSyncEnable = true;
	} else if (pTimer == &m_ResumeTimer) {
		m_ResumeTimer.stop();
		m_ResumeTimer.uninit();
		m_ResumeTimerValid = false;
	} else if (pTimer == &m_headDelayTimer) {
		m_headDelayTimer.stop();
		m_headDelayTimer.uninit();
		m_headDelayTimerValid = false;

		m_bUpHalfInited = true;

		m_exchangeMutex.lock();
		m_bHeadState = false;
		m_bTailState = false;
		if (OMXComponentUtils::changeComponentState(m_headTailScaleComponent, OMX_StatePause)) {
			OMXComponentUtils::waitStateSem(m_headTailScaleComponent);
		} else {
			printf("change head tail scale component pause failed\n");
		}
		changeBackImage();


		m_exchangeMutex.unlock();

		if (!m_bCouldExit) {
			m_bCouldExit = true;
			m_ExitTimer.stop();
			m_ExitTimer.uninit();
			m_ExitTimerValid = false;
			printf("**********\n");
			printf("mixture mode counld exit \n");
		}
	}

}

void MixtureMode_SDI::notifySDIProviderBuffer(int index, unsigned char* pNV12Buffer, int width, int height) {
	if (m_bPaused || m_bTailState) {
		return;
	}

	int size = width * height * 2;
	if (index == 0) {
		setSDIDEIData(pNV12Buffer - buffer_addr[0] + (unsigned char*)VGA_BASE_ADDR, size, 0);
		if (m_bDirectCopyChannel1) {
			m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr,
					pNV12Buffer - buffer_addr[0] + (unsigned char*)VGA_BASE_ADDR,
					HD_WIDTH * 2, HD_WIDTH * 2,
					size, 1);
		}
	} else if (index == 1) {
		setSDIDEIData(pNV12Buffer - buffer_addr[0] + (unsigned char*)VGA_BASE_ADDR, size, 1);
		if (m_bDirectCopyChannel2) {
			m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr
					, pNV12Buffer - buffer_addr[0] + (unsigned char*)VGA_BASE_ADDR,
					HD_WIDTH * 2, HD_HEIGHT * 2,
						size, 1);
		}
	}

}

bool MixtureMode_SDI::onProviderNV12(unsigned char* pNV12Buffer, int width, int height) {
	if (m_bPaused || m_bTailState || m_bVGAFreeze) {
		return true;
	}

	int size = width * height * 3/2;
	setVGAScaleData(pNV12Buffer - buffer_addr[0] + (unsigned char*)VGA_BASE_ADDR, size);
	if (m_pSetting->getVGAEncodeEnable()) {
		setVGAEncodeData(pNV12Buffer - buffer_addr[0] + (unsigned char*)VGA_BASE_ADDR, size);
	}
	return true;
}

bool MixtureMode_SDI::onProviderYUYV(unsigned char* pYUYVBuffer, int width, int height) {
	return true;
}

bool MixtureMode_SDI::onVgaStatus(EVGAStatus status) {
	if (status == eVGAStatusChange) {
		//int size = m_inVGAWidth * m_inVGAHeight * 3/2;
		m_exchangeMutex.lock();
		bool bTeacherShow = (m_teacherSceneInfo.dwRight != m_teacherSceneInfo.dwLeft)
						&& (m_teacherSceneInfo.dwBottom != m_teacherSceneInfo.dwTop);
		bool bStudentShow = (m_studentSceneInfo.dwRight != m_studentSceneInfo.dwLeft)
						&& (m_studentSceneInfo.dwBottom != m_studentSceneInfo.dwTop);
		bool bDeviceShow = (m_deviceSceneInfo.dwRight != m_deviceSceneInfo.dwLeft)
						&& (m_deviceSceneInfo.dwBottom != m_deviceSceneInfo.dwTop);

		int nWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
		int nHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;
		if (bDeviceShow) {
			if (bTeacherShow || bStudentShow) {
				if (bTeacherShow && bStudentShow) {
					m_pKernel->getEdmaInstance()->copy((unsigned char*)m_exchangeImagePhyAddr + m_deviceSceneInfo.dwTop * m_outWidth * 2
						+ m_deviceSceneInfo.dwLeft * 2, (unsigned char*)gbackTSVImagePhyAddr + m_deviceSceneInfo.dwTop * m_outWidth * 2
						+ m_deviceSceneInfo.dwLeft * 2, nWidth * 2, m_outWidth * 2, m_outWidth * 2, m_outWidth * 2,
						nWidth * nHeight * 2, 1);
				} else {
					m_pKernel->getEdmaInstance()->copy((unsigned char*)m_exchangeImagePhyAddr + m_deviceSceneInfo.dwTop * m_outWidth * 2
						+ m_deviceSceneInfo.dwLeft * 2, (unsigned char*)gbackTVImagePhyAddr + m_deviceSceneInfo.dwTop * m_outWidth * 2
						+ m_deviceSceneInfo.dwLeft * 2, nWidth * 2, m_outWidth * 2, m_outWidth * 2, m_outWidth * 2,
						nWidth * nHeight * 2, 1);
				}
			} else {
				m_pKernel->getEdmaInstance()->copy((unsigned char*)m_exchangeImagePhyAddr + m_deviceSceneInfo.dwTop * m_outWidth * 2
					+ m_deviceSceneInfo.dwLeft * 2, (unsigned char*)gbackTImagePhyAddr + m_deviceSceneInfo.dwTop * m_outWidth * 2
					+ m_deviceSceneInfo.dwLeft * 2, nWidth * 2, m_outWidth*2,
					nWidth * nHeight * 2, 1);
			}
		}
		m_exchangeMutex.unlock();
	}
	return true;
}

void MixtureMode_SDI::OnAAC(char* aac, int len) {
}

void MixtureMode_SDI::OnSpeex(char* spx, int len) {
}

void MixtureMode_SDI::onAACEncoded(unsigned char* pAACBuf, int len) {
	if (m_bPaused) {
		int	nAudioTimeStamp = 1000 * 1024.0 / (m_pSetting->getAudioSampleRate()) * m_nAudioCountInMP4File;
		int	nVideoTimeStamp = 1000.0 / m_nFrameRate * m_nVideoCountInMP4File;
		if (nVideoTimeStamp <= nAudioTimeStamp) {
			return;
		}
	}

	if (m_nAudioFrames <= m_pSetting->getAudioDelayFrames()) {
		m_nAudioFrames++;
		return;
	}
#ifndef MTG_SEND_THREAD
	//m_pKernel->getLiveCourseManager()->sendData(m_pSetting->getLCAACPath().c_str(), (const char*)pAACBuf, len, MTG_DATA_TYPE);
#endif

	m_mp4Mutex.lock();
	if (NULL != m_mp4OutputHandle) {
		m_mp4OutputHandle->writeAACAudio((char*)pAACBuf, len);
	}
	m_mp4Mutex.unlock();

	if (NULL != m_rtmpHandle) {
		m_rtmpHandle->pushAudio((char*)pAACBuf, len);
	}

	if (-1 != m_aacfd) {
		write(m_aacfd, pAACBuf, len);
	}

	m_nAudioCountInMP4File++;
}

OMX_ERRORTYPE MixtureMode_SDI::procCallbackEventHandler(OMX_HANDLETYPE hComponent,
		OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1,
		OMX_U32 nData2, OMX_PTR pEventData) {
	ComponentWrapper *comp = (ComponentWrapper *) ptrAppData;
	MixtureMode_SDI* pSelf = NULL;
	if (NULL != comp->pParent) {
		pSelf = (MixtureMode_SDI*)comp->pParent;
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
		if (NULL != pSelf) {
			pSelf->componentError((OMX_ERRORTYPE)nData1);
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MixtureMode_SDI::procCallbackEmptyBufferDone(OMX_HANDLETYPE hComponent,
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

OMX_ERRORTYPE MixtureMode_SDI::procCallbackFillBufferDone(OMX_HANDLETYPE hComponent,
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


MixtureMode_SDI::MixtureMode_SDI(Kernel* pKernel) : BaseMode(pKernel),
		m_nEncBitRate(2000000), m_maxDecode(2), m_bExit(false),
		m_rtmpHandle(NULL),
		m_bPaused(false) {

	m_enc_frames = 0;

	m_ExitTimerValid = false;
	m_headDelayTimerValid = false;
	m_ResumeTimerValid = false;

	unsigned long nStartTime = CommonUtils::GetTickCount();

	m_pSDIDEIHandle = NULL;
	m_sdiDEIComponent = NULL;

	m_pSDIScaleHandle = NULL;
	m_sdiScaleComponent = NULL;

	m_pSDINfHandle = NULL;
	m_sdiNfComponent = NULL;

	m_pSDIEncHandle = NULL;
	m_sdiEncComponent = NULL;

	m_pVGAScaleHandle = NULL;
	m_pHeadTailScaleHandle = NULL;
	m_vgaScaleComponent = NULL;
	m_headTailScaleComponent = NULL;
	m_pVGAEncHandle = NULL;
	m_vgaEncComponent = NULL;

	m_inVGAWidth = 0;
	m_inVGAHeight = 0;

	m_mp4OutputHandle = NULL;
	for (int i = 0 ; i < KResourceNum; ++i) {
		m_resourceH264fd[i] = -1;
	}

	m_pDeiHandle = NULL;
	m_pLiveEncHandle = NULL;
	m_deiComponent = NULL;
	m_liveEncComponent = NULL;

	m_liveH264fd = -1;
	m_recordH264fd = -1;
	m_VGAH264fd = -1;
	m_aacfd = -1;

	m_bThreadsStart = false;

#ifdef ADDR_ALLOC_IN_HEAP
	m_backTImage = NULL;
	m_backTSImage = NULL;
	m_backTVImage = NULL;
	m_backTSVImage = NULL;
	m_logoImage = NULL;
	//m_logoImagePhyAddr = (unsigned char*)BASE_ADDR + LOGO_ADDR_OFFSET;
#else
	m_exchangeImagePhyAddr = BASE_ADDR + MIX_IMAGE_ADDR_OFFSET;
	m_preExchangeImagePhyAddr = BASE_ADDR + PRE_MIX_IMAGE_ADDR_OFFSET;
	gHDMIDisplayAddr = BASE_ADDR + HDMI_DISPLAY_OFFSET;
	gVGADisplayAddr = BASE_ADDR + VGA_DISPLAY_OFFSET;
#endif

	m_bInited = false;
	m_bExitVGAScaleThread = false;
	m_bExitVGAEncodeThread = false;
	m_bExitMixtureThread = false;
	m_bExitHeadTailScaleThread = false;

	m_IPCChannelMutex.init();
	m_bIPCChannelChanged = false;
	m_nReadSameChannelCount = 0;

	m_nSDIInputIndex[0] = 0;
	m_nSDIInputIndex[1] = 0;

	m_nVGAScaleInputIndex = 0;
	m_nVGAEncodeInputIndex = 0;
	m_nMixInputIndex = 0;

	m_nAudioCountInMP4File = 0;
	m_nVideoCountInMP4File = 0;

	m_bDirectCopyChannel1 = false;
	m_bDirectCopyChannel2 = false;

	unsigned long nEndTime = CommonUtils::GetTickCount();
	m_bPreExchangeImageValid = false;
	printf("init consume time %d\n", (int)(nEndTime - nStartTime));
}

MixtureMode_SDI::~MixtureMode_SDI() {
	uninit();

	m_IPCChannelMutex.uninit();
}

BOOL MixtureMode_SDI::init(Setting* pSetting) {
	if (NULL == pSetting || m_bInited) {
		return FALSE;
	}
	m_nInitTime = CommonUtils::GetTickCount();
	m_pSetting = pSetting;

	PRChannel* channel = m_pSetting->getIPCChannel();
	IPCInfo *info = m_pSetting->getIPCInfo(channel->dwTeacherChannel);
	if (NULL != info) {
		//m_inHeight[0] = info->dwIPCHeight;
		//m_inWidth[0] = info->dwIPCWidth;
		m_inHeight[0] = 1080;
		m_inWidth[0] = 1920;
	} else {
		m_inHeight[0] = 0;
		m_inWidth[0] = 0;
	}

	info = m_pSetting->getIPCInfo(channel->dwStudentChannel);
	if (NULL != info) {
		//m_inHeight[1] = info->dwIPCHeight;
		//m_inWidth[1] = info->dwIPCWidth;
		m_inHeight[1] = 1080;
		m_inWidth[1] = 1920;
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


	printf("init video in width %d, %d, %d, height %d,%d,%d\n", m_inWidth[0], m_inWidth[1], m_inWidth[2], m_inHeight[0], m_inHeight[1], m_inHeight[2]);

	m_outWidth = m_pSetting->getSaveWidth();
	m_outHeight = m_pSetting->getSaveHeight();
	m_nFrameRate = m_pSetting->getVideoFrameRate();
	m_nEncBitRate = m_pSetting->getVideoBitrate();

	m_liveOutWidth = m_pSetting->getLiveOutWidth();
	m_liveOutHeight = m_pSetting->getLiveOutHeight();

	m_inVGAWidth = 1024;
	m_inVGAHeight = 768;

	memcpy(&m_IPCChannel, m_pSetting->getIPCChannel(), sizeof(PRChannel));
	memcpy(&m_OldIPCChannel, m_pSetting->getOldIPCChannel(), sizeof(PRChannel));

	printf("before live enc enable\n");
	if (m_pSetting->getLiveEncEnable() ) {
		if (m_pSetting->getRtmpEnable() && NULL == m_rtmpHandle) {
			m_rtmpHandle = new RtmpHandle();
			printf("before rtmp handle init\n");
			/*m_rtmpHandle->init(m_pSetting->getRtmpUrl().c_str(),
							m_liveOutWidth,
							m_liveOutHeight,
							m_pSetting->getVideoFrameRate(),
							2,
							m_pSetting->getAudioSampleRate(),
							16);*/
		}
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

	m_strMp4FileName = m_pSetting->getSaveFileName();

	m_pCb.EventHandler = procCallbackEventHandler;
	m_pCb.EmptyBufferDone = procCallbackEmptyBufferDone;
	m_pCb.FillBufferDone = procCallbackFillBufferDone;

	m_vgaChangedMutex.init();
	sem_init(&m_inputMixWaitSem, 0, 0);

	m_heap = m_pKernel->getHeapObjectInstance();

	m_nAudioFrames = 0;

	m_enc_frames = 0;

    m_nTotalEncCount = 0;

    m_binputMixInited = false;

    m_exchangeMutex.init();
    m_headTailMutex.init();

	m_outScaleWidth[0] = 0;
	m_outScaleHeight[0] = 0;

	m_outScaleWidth[1] = 0;
	m_outScaleHeight[1] = 0;

	m_outVGAWidth = 0;
	m_outVGAHeight = 0;

	createComponents();
	allocInOutLowerHalfParams();

	m_mixbeftime = CommonUtils::GetTickCount();
	m_mixafttime = m_mixbeftime;
	m_nTotalEncCount = 0;

	m_nLiveTotalEncCount = 0;
	m_mixTotalCount = 0;

	m_bTeacherContainStud = false;
	m_bTeacherContainVGA = false;

	m_bStudContainTeacher = false;
	m_bStudContainVGA = false;

	m_bVGAContainTeacher = false;
	m_bVGAContainStud = false;

	m_pContainTeacherImage = NULL;
	m_pContainStudImage = NULL;
	m_pContainVGAImage = NULL;

	m_bHeadState = false;
	m_bTailState = false;

	m_bUpHalfInited = false;
	m_bCouldExit = false;

	m_bInited = true;
	printf("inited ok\n");

	allocInOutUpperHalfParams();

	openFileHandles();
	if (!initUpperHalfOMXHandle() ) {
		if (NULL != m_pKernel) {
			m_pKernel->NotifyAssistMessage(gStrRecordFailed);
		}

		system("reboot -f");
		return false;
	}
	if (!initLowerHalfOMXHandle()) {
		if (NULL != m_pKernel) {
			m_pKernel->NotifyAssistMessage(gStrRecordFailed);
		}
		system("reboot -f");
		return false;
	}

	printf("before init vga component\n");
	if (!initVGAComponent()) {
		if (NULL != m_pKernel) {
			m_pKernel->NotifyAssistMessage(gStrRecordFailed);
		}
		system("reboot -f");
		return false;
	}
	printf("after init vga component\n");
	m_nInitEndTime = CommonUtils::GetTickCount();
	printf("init diff time %d\n", (int)(m_nInitEndTime - m_nInitTime));
	return TRUE;
}

BOOL MixtureMode_SDI::uninit() {
	if (!m_bExit) {
		return FALSE;
	}
	printf("before free in out lower half params\n");
	fflush(stdout);
	freeInOutLowerHalfParams();
	printf("free in out lower half params\n");
	fflush(stdout);
	freeInOutUpperHalfParams();
	printf("base mode before destroy components \n");
	destroyComponents();
	uninitVGAComponent();

	printf("base mode after destroy components \n");

	m_bExit = false;
	m_vgaChangedMutex.uninit();
	sem_destroy(&m_inputMixWaitSem);
	printf("semp deinit vga sem\n");


	printf("memory free vga frame buffers\n");

#ifdef ADDR_ALLOC_IN_HEAP
	if (NULL != m_backTImage) {
		Memory_free(m_heap, m_backTImage, m_outWidth * m_outHeight * 2);
		m_backTImage = NULL;
	}
	if (NULL != m_backTSImage) {
		Memory_free(m_heap, m_backTSImage, m_outWidth * m_outHeight * 2);
		m_backTSImage = NULL;
	}
	if (NULL != m_backTVImage) {
		Memory_free(m_heap, m_backTVImage, m_outWidth * m_outHeight * 2);
		m_backTVImage = NULL;
	}
	if (NULL != m_backTSVImage) {
		Memory_free(m_heap, m_backTSVImage, m_outWidth * m_outHeight * 2);
		m_backTSVImage = NULL;
	}

#endif
	printf("after free back image\n");

	m_exchangeMutex.uninit();
	m_headTailMutex.uninit();

#ifdef ADDR_ALLOC_IN_HEAP
	if (NULL != m_pExchangeImage) {
		Memory_free(m_heap, m_pExchangeImage, m_outWidth * m_outHeight * 2);
		m_pExchangeImage = NULL;
	}
	printf("after free exchange image\n");
#endif

	if (NULL != m_pContainTeacherImage) {
		Memory_free(m_heap, m_pContainTeacherImage, m_nContainTeacherSize);
		m_pContainTeacherImage = NULL;
		SempUtils::semp_deinit(&m_containTeacherSem);
	}
	if (NULL != m_pContainStudImage) {
		Memory_free(m_heap, m_pContainStudImage, m_nContainStudSize);
		m_pContainStudImage = NULL;
		SempUtils::semp_deinit(&m_containStudSem);
	}

	if (NULL != m_pContainVGAImage) {
		Memory_free(m_heap, m_pContainVGAImage, m_nContainVGASize);
		m_pContainVGAImage = NULL;
		SempUtils::semp_deinit(&m_containVGASem);
	}

	if (NULL != m_rtmpHandle) {
		m_rtmpHandle->uninit();
		delete m_rtmpHandle;
		m_rtmpHandle = NULL;
		printf("rtmp uninit ok\n");
	}

	m_bInited = false;

	printf("uninited ok\n");
	return TRUE;
}

bool MixtureMode_SDI::switchScene() {
	if (m_pSetting->getHeaderEnable()) {
		m_pSetting->setHeadEnable(false);

		calcScalerWidthHeight();
		dynamicConfigScaleComponent();

		m_headDelayTimer.init();
		m_headDelayTimer.setInterval(500);
		m_headDelayTimer.start(this);
		m_headDelayTimerValid = true;
	} else {
		printf("before command switch scene\n");
		dynamicSwitchScene();
		printf("after command switch scene\n");
	}
	return true;
}

bool MixtureMode_SDI::isComponentCreated() {
	return m_bUpHalfInited;
}

bool MixtureMode_SDI::IPCChannelChanged() {
	m_IPCChannelMutex.lock();
	m_bIPCChannelChanged = true;
	memcpy(&m_IPCChannel, m_pSetting->getIPCChannel(), sizeof(PRChannel));
	memcpy(&m_OldIPCChannel, m_pSetting->getOldIPCChannel(),sizeof(PRChannel));

	m_bStudentChannelNormal = false;
	m_bTeacherChannelNormal = false;
	m_IPCChannelMutex.unlock();
	return true;
}

bool MixtureMode_SDI::doFreezeVGA(bool bFreeze) {
	printf("doFreezeVGA %d\n", bFreeze);
	m_bVGAFreeze = bFreeze;
	return true;
}

bool MixtureMode_SDI::componentError(OMX_ERRORTYPE error) {

	return true;
}

BOOL MixtureMode_SDI::initSDIDEIOMXHandle() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	eError = OMX_GetHandle(&m_pSDIDEIHandle,
			(OMX_STRING) OMX_VFPC_DEIDUALOUT_HIGH_COMP_NAME, m_sdiDEIComponent,
			&m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pSDIDEIHandle == NULL)) {
		return FALSE;
	}
	m_sdiDEIComponent->handle = m_pSDIDEIHandle;
	printf(" dei component is created \n");
	if (!setSDIDEIParams()) {
		return FALSE;
	}

	printf("enable dei input port \n");

	OMX_SendCommand(m_pSDIDEIHandle, OMX_CommandPortEnable,
		OMX_VFPC_INPUT_PORT_START_INDEX, NULL);

	OMXComponentUtils::waitPortSem(m_sdiDEIComponent);

	printf("enable dei output port 0 \n");
	OMX_SendCommand(m_pSDIDEIHandle, OMX_CommandPortEnable,
		OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);
	OMXComponentUtils::waitPortSem(m_sdiDEIComponent);

	printf("enable dei output port 1 \n");
	OMX_SendCommand(m_pSDIDEIHandle, OMX_CommandPortEnable,
		OMX_VFPC_OUTPUT_PORT_START_INDEX + 1, NULL);
	OMXComponentUtils::waitPortSem(m_sdiDEIComponent);

	OMX_SendCommand(m_pSDIDEIHandle, OMX_CommandPortEnable,
		OMX_VFPC_INPUT_PORT_START_INDEX + 1, NULL);

	OMXComponentUtils::waitPortSem(m_sdiDEIComponent);

	printf("enable dei output port 0 \n");
	OMX_SendCommand(m_pSDIDEIHandle, OMX_CommandPortEnable,
		OMX_VFPC_OUTPUT_PORT_START_INDEX + 2, NULL);
	OMXComponentUtils::waitPortSem(m_sdiDEIComponent);

	printf("enable dei output port 1 \n");
	OMX_SendCommand(m_pSDIDEIHandle, OMX_CommandPortEnable,
		OMX_VFPC_OUTPUT_PORT_START_INDEX + 3, NULL);
	OMXComponentUtils::waitPortSem(m_sdiDEIComponent);
	return TRUE;
}

BOOL MixtureMode_SDI::initSDIScaleOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pSDIScaleHandle,
			(OMX_STRING) "OMX.TI.VPSSM3.VFPC.INDTXSCWB",
			m_sdiScaleComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pSDIScaleHandle == NULL)) {
		return FALSE;
	}
	m_sdiScaleComponent->handle = m_pSDIScaleHandle;

	if (!setSDIScaleParams()) {
		return FALSE;
	}
	OMX_SendCommand(m_pSDIScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_INPUT_PORT_START_INDEX, NULL);
	OMXComponentUtils::waitPortSem(m_sdiScaleComponent);

	OMX_SendCommand(m_pSDIScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);
	OMXComponentUtils::waitPortSem(m_sdiScaleComponent);

	OMX_SendCommand(m_pSDIScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_INPUT_PORT_START_INDEX + 1, NULL);
	OMXComponentUtils::waitPortSem(m_sdiScaleComponent);

	printf("enable scalar output port \n");

	OMX_SendCommand(m_pSDIScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX + 1, NULL);
	OMXComponentUtils::waitPortSem(m_sdiScaleComponent);

	return TRUE;
}

BOOL MixtureMode_SDI::initSDINfOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pSDINfHandle,
			(OMX_STRING) "OMX.TI.VPSSM3.VFPC.NF", m_sdiNfComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pSDINfHandle == NULL)) {
		return FALSE;
	}
	m_sdiNfComponent->handle = m_pSDINfHandle;
	setSDINfParams();

	/* enable input and output port */
	/* as per openmax specs all the ports should be enabled by default but EZSDK
	 OMX component does not enable it hence we manually need to enable it. */
	OMX_SendCommand(m_pSDINfHandle, OMX_CommandPortEnable,
			OMX_VFPC_INPUT_PORT_START_INDEX, NULL);

	/* wait for both ports to get enabled, event handler would be notified from
	 the component after enabling the port, which inturn would post this
	 semaphore */

	OMXComponentUtils::waitPortSem(m_sdiNfComponent);
	printf("enable NF output port \n");
	OMX_SendCommand(m_pSDINfHandle, OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);

	OMXComponentUtils::waitPortSem(m_sdiNfComponent);
	return TRUE;
}

BOOL MixtureMode_SDI::initSDIEncOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pSDIEncHandle, (OMX_STRING)"OMX.TI.DUCATI.VIDENC",
			m_sdiEncComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pSDIEncHandle == NULL)) {
		return FALSE;
	}

	m_sdiEncComponent->handle = m_pSDIEncHandle;
	printf("before set vga encode params \n");

	return setSDIEncParams();
}



BOOL MixtureMode_SDI::initVGAScaleOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pVGAScaleHandle,
			(OMX_STRING) "OMX.TI.VPSSM3.VFPC.INDTXSCWB", m_vgaScaleComponent,
			&m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pVGAScaleHandle == NULL)) {
		return FALSE;
	}
	printf("after vga scale component get handle\n");
	m_vgaScaleComponent->handle = m_pVGAScaleHandle;

	if (!setVGAScaleParams()) {
		return FALSE;
	}
	OMX_SendCommand(m_pVGAScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_INPUT_PORT_START_INDEX, NULL);

	OMXComponentUtils::waitPortSem(m_vgaScaleComponent);

	OMX_SendCommand(m_pVGAScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);
	OMXComponentUtils::waitPortSem(m_vgaScaleComponent);
	return TRUE;
}

BOOL MixtureMode_SDI::initHeadTailScaleOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pHeadTailScaleHandle,
			(OMX_STRING) "OMX.TI.VPSSM3.VFPC.INDTXSCWB", m_headTailScaleComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pHeadTailScaleHandle == NULL)) {
		return FALSE;
	}
	m_headTailScaleComponent->handle = m_pHeadTailScaleHandle;

	if (!setHeadTailScaleParams()) {
		return FALSE;
	}
	OMX_SendCommand(m_pHeadTailScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_INPUT_PORT_START_INDEX, NULL);

	OMXComponentUtils::waitPortSem(m_headTailScaleComponent);

	OMX_SendCommand(m_pHeadTailScaleHandle, OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);
	OMXComponentUtils::waitPortSem(m_headTailScaleComponent);
	return TRUE;
}

BOOL MixtureMode_SDI::initNfOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pNfHandle,
			(OMX_STRING) "OMX.TI.VPSSM3.VFPC.NF", m_nfComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pNfHandle == NULL)) {
		return FALSE;
	}
	m_nfComponent->handle = m_pNfHandle;
	setNfParams();

	/* enable input and output port */
	/* as per openmax specs all the ports should be enabled by default but EZSDK
	 OMX component does not enable it hence we manually need to enable it. */
	OMX_SendCommand(m_pNfHandle, OMX_CommandPortEnable,
			OMX_VFPC_INPUT_PORT_START_INDEX, NULL);

	/* wait for both ports to get enabled, event handler would be notified from
	 the component after enabling the port, which inturn would post this
	 semaphore */

	OMXComponentUtils::waitPortSem(m_nfComponent);
	printf("enable NF output port \n");
	OMX_SendCommand(m_pNfHandle, OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);

	OMXComponentUtils::waitPortSem(m_nfComponent);
	return TRUE;
}

BOOL MixtureMode_SDI::initVGAEncOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pVGAEncHandle, (OMX_STRING)"OMX.TI.DUCATI.VIDENC",
			m_vgaEncComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pVGAEncHandle == NULL)) {
		return FALSE;
	}

	m_vgaEncComponent->handle = m_pVGAEncHandle;
	printf("before set vga encode params \n");

	return setVGAEncodeParams();
}

BOOL MixtureMode_SDI::initEncodeOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pEncHandle,
			(OMX_STRING) "OMX.TI.DUCATI.VIDENC", m_encComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pEncHandle == NULL)) {
		return FALSE;
	}

	m_encComponent->handle = m_pEncHandle;
	printf("before set encode params \n");

	return setEncodeParams();
}

BOOL MixtureMode_SDI::initLiveEncodeOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pLiveEncHandle,
			(OMX_STRING) "OMX.TI.DUCATI.VIDENC", m_liveEncComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pLiveEncHandle == NULL)) {
		return FALSE;
	}

	m_liveEncComponent->handle = m_pLiveEncHandle;
	return setLiveEncodeParams();
}

BOOL MixtureMode_SDI::initDeiOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pDeiHandle,
		(OMX_STRING) OMX_VFPC_DEIDUALOUT_HIGH_COMP_NAME, m_deiComponent,
		&m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pDeiHandle == NULL)) {
		return FALSE;
	}
	m_deiComponent->handle = m_pDeiHandle;
	printf(" dei component is created \n");
	if (!setDeiParams()) {
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

BOOL MixtureMode_SDI::initLiveNfOMXHandle() {
	OMX_ERRORTYPE eError = OMX_GetHandle(&m_pLiveNfHandle,
			(OMX_STRING) "OMX.TI.VPSSM3.VFPC.NF", m_liveNfComponent, &m_pCb);

	if ((eError != OMX_ErrorNone) || (m_pLiveNfHandle == NULL)) {
		return FALSE;
	}
	m_liveNfComponent->handle = m_pLiveNfHandle;
	setLiveNfParams();

	/* enable input and output port */
	/* as per openmax specs all the ports should be enabled by default but EZSDK
	 OMX component does not enable it hence we manually need to enable it. */
	OMX_SendCommand(m_pLiveNfHandle, OMX_CommandPortEnable,
			OMX_VFPC_INPUT_PORT_START_INDEX, NULL);

	/* wait for both ports to get enabled, event handler would be notified from
	 the component after enabling the port, which inturn would post this
	 semaphore */
	OMXComponentUtils::waitPortSem(m_liveNfComponent);

	printf("enable NF output port \n");
	OMX_SendCommand(m_pLiveNfHandle, OMX_CommandPortEnable,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);

	OMXComponentUtils::waitPortSem(m_liveNfComponent);

	return TRUE;
}

bool MixtureMode_SDI::setSDIDEIParams() {
	if (NULL == m_pSDIDEIHandle) {
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
	eError = OMX_SetConfig (m_pSDIDEIHandle, ( OMX_INDEXTYPE )
		( OMX_TI_IndexConfigSubSamplingFactor ), &sSubSamplinginfo );

	for (int i = 0; i < 2; ++i) {
		OMX_INIT_PARAM(&memTypeCfg);
		memTypeCfg.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX + i;
		memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
		eError = OMX_SetParameter (m_pSDIDEIHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType, &memTypeCfg);

		if (eError != OMX_ErrorNone) {
			printf("failed to set memory Type at input port\n");
			return false;
		}
		printf("set input mem buffer type\n");

		OMX_INIT_PARAM(&memTypeCfg);
		memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + i * 2;
		memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
		eError = OMX_SetParameter (m_pSDIDEIHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType, &memTypeCfg);

		if (eError != OMX_ErrorNone) {
			printf("failed to set memory Type at output port\n");
			return false;
		}
		printf("set output 0 mem buffer type\n");

		OMX_INIT_PARAM(&memTypeCfg);
		memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1 + i * 2;
		memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
		eError = OMX_SetParameter (m_pSDIDEIHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType, &memTypeCfg);

		if (eError != OMX_ErrorNone) {
			printf("failed to set memory Type at output port\n");
			return false;
		}
		printf("set output 1 mem buffer type\n");

		OMX_INIT_PARAM(&paramPort);
		paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX + i * 2;

		OMX_GetParameter(m_pSDIDEIHandle, OMX_IndexParamPortDefinition, &paramPort);
		paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX + i;
		paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_inWidth[i], 32);
		paramPort.format.video.nFrameHeight = m_inHeight[i];

		paramPort.format.video.nStride = UTIL_ALIGN(m_inWidth[i] * 2, 32);
		paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
		paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
		paramPort.format.video.xFramerate = (m_nFrameRate << 16);
		paramPort.nBufferSize = paramPort.format.video.nStride
					* paramPort.format.video.nFrameHeight;

		paramPort.nBufferAlignment = 0;
		paramPort.bBuffersContiguous = OMX_FALSE;
		paramPort.nBufferCountActual = KOtherComponetInOutPutNum;

		OMX_SetParameter(m_pSDIDEIHandle, OMX_IndexParamPortDefinition, &paramPort);
		printf("set inport param \n");

		OMX_INIT_PARAM(&paramPort);
		paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + i * 2;
		OMX_GetParameter(m_pSDIDEIHandle, OMX_IndexParamPortDefinition, &paramPort);

		paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + i * 2;
		paramPort.format.video.nFrameWidth = UTIL_ALIGN(KTeacherLiveWidth, 32);
		paramPort.format.video.nFrameHeight = KTeacherLiveHeight;
		paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
		paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
		paramPort.format.video.xFramerate = (m_nFrameRate << 16);
		paramPort.nBufferAlignment = 0;
		paramPort.nBufferCountActual = KOtherComponetInOutPutNum;

		paramPort.format.video.nStride = UTIL_ALIGN(KTeacherLiveWidth * 2, 32);
		paramPort.nBufferSize = paramPort.format.video.nStride
					* paramPort.format.video.nFrameHeight;

		OMX_SetParameter(m_pSDIDEIHandle, OMX_IndexParamPortDefinition, &paramPort);
		printf("set outport 0 param\n");

		OMX_INIT_PARAM(&paramPort);
		paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1 + i * 2;
		OMX_GetParameter(m_pSDIDEIHandle, OMX_IndexParamPortDefinition, &paramPort);

		paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1 + i * 2;
		paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_inWidth[i], 32);
		paramPort.format.video.nFrameHeight = m_inHeight[i];
		paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
		paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
		paramPort.format.video.xFramerate = (m_nFrameRate << 16);
		paramPort.nBufferAlignment = 0;
		paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
		paramPort.format.video.nStride = UTIL_ALIGN(m_inWidth[i], 32);

		paramPort.nBufferSize = (paramPort.format.video.nStride
					* paramPort.format.video.nFrameHeight * 3) >> 1;

		OMX_SetParameter(m_pSDIDEIHandle, OMX_IndexParamPortDefinition, &paramPort);

		printf("set number of channels\n");
	}

	OMX_INIT_PARAM(&sNumChPerHandle);
	sNumChPerHandle.nNumChannelsPerHandle = 2;
	eError = OMX_SetParameter (m_pSDIDEIHandle,
		(OMX_INDEXTYPE)
		OMX_TI_IndexParamVFPCNumChPerHandle,
		&sNumChPerHandle);
	if (eError != OMX_ErrorNone) {
		printf("failed to set num of channels\n");
		return false;
	}

	printf("set input resolution\n");

	for (int i = 0; i < 2; ++i) {
		OMX_INIT_PARAM(&chResolution);
		chResolution.Frm0Width = UTIL_ALIGN(m_inWidth[i], 32);
		chResolution.Frm0Height = m_inHeight[i];
		chResolution.Frm0Pitch = UTIL_ALIGN(m_inWidth[i] * 2, 32);
		chResolution.Frm1Width = 0;
		chResolution.Frm1Height = 0;
		chResolution.Frm1Pitch = 0;
		chResolution.FrmStartX = 0;
		chResolution.FrmStartY = 0;
		chResolution.FrmCropWidth = m_inWidth[i];
		chResolution.FrmCropHeight = m_inHeight[i];

		chResolution.eDir = OMX_DirInput;
		chResolution.nChId = i;

		eError = OMX_SetConfig (m_pSDIDEIHandle,
				(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
				&chResolution);
		if (eError != OMX_ErrorNone) {
			printf("failed to set input channel resolution\n");
			return false;
		}

		printf("set output resolution\n");
		OMX_INIT_PARAM(&chResolution);
		chResolution.Frm0Width = UTIL_ALIGN(KTeacherLiveWidth, 32);
		chResolution.Frm0Height = KTeacherLiveHeight;
		chResolution.Frm0Pitch = UTIL_ALIGN(KTeacherLiveWidth * 2, 32);
		chResolution.Frm1Width = UTIL_ALIGN(m_inWidth[i], 32);
		chResolution.Frm1Height = m_inHeight[i];
		chResolution.Frm1Pitch = UTIL_ALIGN(m_inWidth[i], 32);
		chResolution.FrmStartX = 0;
		chResolution.FrmStartY = 0;
		chResolution.FrmCropWidth = 0;
		chResolution.FrmCropHeight = 0;
		chResolution.eDir = OMX_DirOutput;
		chResolution.nChId = i;

		eError = OMX_SetConfig (m_pSDIDEIHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
		if (eError != OMX_ErrorNone) {
			printf("failed to set output channel resolution\n");
			return false;
		}
	}


	OMX_INIT_PARAM(&algEnable);
	algEnable.nPortIndex = 0;
	algEnable.nChId = 0;
	algEnable.bAlgBypass = OMX_TRUE;

	eError = OMX_SetConfig (m_pSDIDEIHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable,
			&algEnable);
	if (eError != OMX_ErrorNone) {
		printf("failed to disable algo by pass mode\n");
		return false;
	}

	return true;
}

bool MixtureMode_SDI::setSDIScaleParams() {
	if (NULL == m_pSDIScaleHandle) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_PARAM_BUFFER_MEMORYTYPE memTypeCfg;
	OMX_PARAM_PORTDEFINITIONTYPE paramPort;
	OMX_PARAM_VFPC_NUMCHANNELPERHANDLE sNumChPerHandle;
	OMX_CONFIG_ALG_ENABLE algEnable;
	OMX_CONFIG_VIDCHANNEL_RESOLUTION chResolution;

	for (int i = 0; i < 2; ++i) {
		OMX_INIT_PARAM(&memTypeCfg);
		memTypeCfg.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX + i;
		memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
		eError = OMX_SetParameter (m_pSDIScaleHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
							&memTypeCfg);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		OMX_INIT_PARAM(&memTypeCfg);
		memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + i;
		memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
		eError = OMX_SetParameter (m_pSDIScaleHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
							&memTypeCfg);
		if (eError != OMX_ErrorNone) {
			return false;
		}

		OMX_INIT_PARAM(&paramPort);
		paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX + i;

		OMX_GetParameter(m_pSDIScaleHandle, OMX_IndexParamPortDefinition, &paramPort);
		paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX + i;
		paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_inWidth[i], 32);
		paramPort.format.video.nFrameHeight = m_inHeight[i];

		paramPort.format.video.nStride = paramPort.format.video.nFrameWidth;
		paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
		paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
		paramPort.nBufferSize = (paramPort.format.video.nStride
				* m_inHeight[i] * 3) >> 1;

		paramPort.nBufferAlignment = 0;
		paramPort.bBuffersContiguous = OMX_FALSE;
		paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
		eError = OMX_SetParameter (m_pSDIScaleHandle, OMX_IndexParamPortDefinition, &paramPort);
		if (eError != OMX_ErrorNone) {
			return false;
		}

		/* set output height/width and color format */
		OMX_INIT_PARAM(&paramPort);
		paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + i;
		OMX_GetParameter(m_pSDIScaleHandle, OMX_IndexParamPortDefinition, &paramPort);

		paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + i;
		paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_outScaleWidth[i], 16);
		paramPort.format.video.nFrameHeight = m_outScaleHeight[i];

		paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
		paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
		paramPort.nBufferAlignment = 0;
		paramPort.bBuffersContiguous = OMX_FALSE;
		paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
		/* scalar buffer pitch should be multiple of 16 */
		paramPort.format.video.nStride = UTIL_ALIGN(m_outScaleWidth[i] * 2, 16);

		paramPort.nBufferSize = paramPort.format.video.nStride
				* paramPort.format.video.nFrameHeight;

		eError = OMX_SetParameter (m_pSDIScaleHandle, OMX_IndexParamPortDefinition,
				&paramPort);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}


	/* set number of channles */
	OMX_INIT_PARAM(&sNumChPerHandle);
	sNumChPerHandle.nNumChannelsPerHandle = 2;
	eError = OMX_SetParameter (m_pSDIScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexParamVFPCNumChPerHandle,
			&sNumChPerHandle);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* set VFPC input and output resolution information */
	for (int i = 0; i < 2; ++i) {
		printf("set input resolution \n");
		OMX_INIT_PARAM(&chResolution);
		chResolution.Frm0Width = UTIL_ALIGN(m_inWidth[i], 32);
		chResolution.Frm0Height = m_inHeight[i];
		chResolution.Frm0Pitch = UTIL_ALIGN(m_inWidth[i], 32);
		chResolution.Frm1Width = 0;
		chResolution.Frm1Height = 0;
		chResolution.Frm1Pitch = 0;
		chResolution.FrmStartX = 0;
		chResolution.FrmStartY = 0;
		chResolution.FrmCropWidth = m_inWidth[i];
		chResolution.FrmCropHeight = m_inHeight[i];
		chResolution.eDir = OMX_DirInput;
		chResolution.nChId = i;

		eError = OMX_SetConfig (m_pSDIScaleHandle,
				(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
				&chResolution);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		printf("set output resolution \n");
		OMX_INIT_PARAM(&chResolution);
		chResolution.Frm0Width = UTIL_ALIGN(m_outScaleWidth[i], 16);
		chResolution.Frm0Height = m_outScaleHeight[i];

		chResolution.Frm0Pitch = UTIL_ALIGN(m_outScaleWidth[i] * 2, 16);
		chResolution.Frm1Width = 0;
		chResolution.Frm1Height = 0;
		chResolution.Frm1Pitch = 0;
		chResolution.FrmStartX = 0;
		chResolution.FrmStartY = 0;
		chResolution.FrmCropWidth = 0;
		chResolution.FrmCropHeight = 0;
		chResolution.eDir = OMX_DirOutput;
		chResolution.nChId = i;

		eError = OMX_SetConfig (m_pSDIScaleHandle,
				(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
				&chResolution);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	/* disable algo bypass mode */
	OMX_INIT_PARAM(&algEnable);
	algEnable.nPortIndex = 0;
	algEnable.nChId = 0;
	algEnable.bAlgBypass = OMX_FALSE;

	eError = OMX_SetConfig (m_pSDIScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable, &algEnable);
	return true;
}

bool MixtureMode_SDI::setSDINfParams() {
	if (NULL == m_pSDINfHandle) {
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
	eError =
	OMX_SetParameter (m_pSDINfHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
			&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at input port\n");
		return false;
	}

	/* Setting Memory type at output port to Raw Memory */
	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError =
	OMX_SetParameter (m_pSDINfHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
			&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at output port\n");
		return false;
	}

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError = OMX_SetParameter (m_pSDINfHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
				&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at output port\n");
		return false;
	}

	/* set input height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

	OMX_GetParameter(m_pSDINfHandle, OMX_IndexParamPortDefinition, &paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(KTeacherLiveWidth, 32);
	paramPort.format.video.nFrameHeight = UTIL_ALIGN(KTeacherLiveHeight, 32);

	paramPort.format.video.nStride = UTIL_ALIGN(KTeacherLiveWidth * 2, 32);
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
	paramPort.nBufferSize = (paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight);

	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;

	OMX_SetParameter(m_pSDINfHandle, OMX_IndexParamPortDefinition, &paramPort);

	/* set output height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	OMX_GetParameter(m_pSDINfHandle, OMX_IndexParamPortDefinition, &paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(KTeacherLiveWidth, 32);
	paramPort.format.video.nFrameHeight = UTIL_ALIGN(KTeacherLiveHeight, 32);
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
	paramPort.format.video.nStride = UTIL_ALIGN(KTeacherLiveWidth, 32);

	paramPort.nBufferSize = (paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight * 3) >> 1;

	OMX_SetParameter(m_pSDINfHandle, OMX_IndexParamPortDefinition, &paramPort);

	OMX_INIT_PARAM(&sNumChPerHandle);
	sNumChPerHandle.nNumChannelsPerHandle = 1;
	eError = OMX_SetParameter (m_pSDINfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexParamVFPCNumChPerHandle,
			&sNumChPerHandle);
	if (eError != OMX_ErrorNone) {
		printf("failed to set num of channels\n");
		return false;
	}

	/* set VFPC input and output resolution information */
	printf("set input resolution \n");

	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(KTeacherLiveWidth, 32);
	chResolution.Frm0Height = UTIL_ALIGN(KTeacherLiveHeight, 32);
	chResolution.Frm0Pitch = UTIL_ALIGN(KTeacherLiveWidth * 2, 32);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = UTIL_ALIGN(KTeacherLiveWidth, 32);
	chResolution.FrmCropHeight = UTIL_ALIGN(KTeacherLiveHeight, 32);
	chResolution.eDir = OMX_DirInput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pSDINfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("failed to set input channel resolution\n");
	}

	printf("set output resolution \n");
	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(KTeacherLiveWidth, 32);
	chResolution.Frm0Height = UTIL_ALIGN(KTeacherLiveHeight, 32);
	chResolution.Frm0Pitch = UTIL_ALIGN(KTeacherLiveWidth, 32);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;

	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = KTeacherLiveHeight;
	chResolution.FrmCropWidth = 0;
	chResolution.FrmCropHeight = (UTIL_ALIGN(KTeacherLiveHeight, 32)) - KTeacherLiveHeight;
	chResolution.eDir = OMX_DirOutput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pSDINfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("failed to set output channel resolution\n");
		return false;
	}

	/* disable algo bypass mode */
	OMX_INIT_PARAM(&algEnable);
	algEnable.nPortIndex = 0;
	algEnable.nChId = 0;
	algEnable.bAlgBypass = OMX_FALSE;
	OMX_INIT_PARAM(&algEnable);

	eError = OMX_SetConfig (m_pSDINfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable, &algEnable);

	return (OMX_ErrorNone == eError);
}

bool MixtureMode_SDI::setSDIEncParams() {
	if (NULL == m_pSDIEncHandle) {
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
	eError = OMX_GetParameter(m_pSDIEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* set the actual number of buffers required */
	tPortDef.nBufferCountActual = KEncodeComponentInOutPutNum;
	/* set the video format settings */

	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(KTeacherLiveWidth, 32);
	tPortDef.format.video.nFrameHeight = KTeacherLiveHeight;
	tPortDef.format.video.nStride = UTIL_ALIGN(KTeacherLiveWidth, 32);
	tPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	/* settings for OMX_IndexParamVideoPortFormat */
	tPortDef.nBufferSize = (tPortDef.format.video.nStride
			* tPortDef.format.video.nFrameHeight * 3) >> 1;

	eError = OMX_SetParameter(m_pSDIEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf("failed to set Encode OMX_IndexParamPortDefinition for input \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for input \n");

	OMX_INIT_PARAM(&tPortDef);

	tPortDef.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pSDIEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* settings for OMX_IndexParamPortDefinition */
	/* set the actual number of buffers required */

	tPortDef.nBufferCountActual = KEncodeComponentInOutPutNum;
	tPortDef.format.video.cMIMEType = "H264";
	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(KTeacherLiveWidth, 32);
	tPortDef.format.video.nFrameHeight = KTeacherLiveHeight;
	tPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	tPortDef.format.video.nBitrate = m_pSetting->getVGAEncBitrate();
	/* settings for OMX_IndexParamVideoPortFormat */

	eError = OMX_SetParameter(m_pSDIEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf("failed to set Encode OMX_IndexParamPortDefinition for output \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for output \n");

	/* For changing bit rate following index can be used */
	OMX_INIT_PARAM(&tVidEncBitRate);

	tVidEncBitRate.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pSDIEncHandle, OMX_IndexParamVideoBitrate,
			&tVidEncBitRate);

	tVidEncBitRate.eControlRate = OMX_Video_ControlRateVariable;
	tVidEncBitRate.nTargetBitrate = m_pSetting->getVGAEncBitrate();
	eError = OMX_SetParameter(m_pSDIEncHandle, OMX_IndexParamVideoBitrate,&tVidEncBitRate);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Set the profile and level for H264 */
	OMX_INIT_PARAM(&tProfileLevel);
	tProfileLevel.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pSDIEncHandle, OMX_IndexParamVideoProfileLevelCurrent, &tProfileLevel);

	if (m_pSetting->getLiveEncHighProfile()) {
		tProfileLevel.eProfile = OMX_VIDEO_AVCProfileHigh;
	} else {
		tProfileLevel.eProfile = OMX_VIDEO_AVCProfileBaseline; //OMX_VIDEO_AVCProfileHigh
	}
	tProfileLevel.eLevel = OMX_VIDEO_AVCLevel42;

	eError =
	OMX_SetParameter(m_pSDIEncHandle, OMX_IndexParamVideoProfileLevelCurrent,
			&tProfileLevel);
	if (eError != OMX_ErrorNone)
		return false;

	OMX_INIT_PARAM(&tEncoderPreset);
	tEncoderPreset.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pSDIEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset, &tEncoderPreset);
	tEncoderPreset.eEncodingModePreset = OMX_Video_Enc_High_Speed_Med_Quality;
	tEncoderPreset.eRateControlPreset = OMX_Video_RC_Low_Delay;
	eError = OMX_SetParameter(m_pSDIEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset, &tEncoderPreset);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_INIT_PARAM(&tDynParams);

	tDynParams.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pSDIEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	if (m_pSetting->getLiveEncHighProfile()) {
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval =
				25;
	} else {
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.generateHeader = XDM_GENERATE_HEADER; // sps/pps
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.forceFrame = IVIDEO_IDR_FRAME;
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval = 25;
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.interFrameInterval = 1;
	}

	eError = OMX_SetParameter (m_pSDIEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	OMX_INIT_PARAM (&tStaticParam);

	tStaticParam.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter (m_pSDIEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	tStaticParam.videoStaticParams.h264EncStaticParams.videnc2Params.encodingPreset = XDM_USER_DEFINED;

	tStaticParam.videoStaticParams.h264EncStaticParams.numTemporalLayer = IH264_TEMPORAL_LAYERS_1;

	if (m_pSetting->getLiveEncHighProfile()) {
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
	if (m_pSetting->getLiveEncHighProfile()) {
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.videoFullRangeFlag = 0;
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.timingInfoPresentFlag = 1;
	} else {
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.videoFullRangeFlag = 1;
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.timingInfoPresentFlag = 0;
	}

	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.hrdParamsPresentFlag = 0;
	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.numUnitsInTicks = 1000;

	eError = OMX_SetParameter (m_pSDIEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	return (OMX_ErrorNone == eError);
}

bool MixtureMode_SDI::setVGAScaleParams() {
	printf("set VGA Scalar params\n");
	if (NULL == m_pVGAScaleHandle) {
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
	eError = OMX_SetParameter (m_pVGAScaleHandle,
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
	eError = OMX_SetParameter (m_pVGAScaleHandle,
			(OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
			&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	printf("set VGA Scalar params step 2\n");
	/* set input height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

	OMX_GetParameter(m_pVGAScaleHandle, OMX_IndexParamPortDefinition,
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
	eError = OMX_SetParameter (m_pVGAScaleHandle,
			OMX_IndexParamPortDefinition,
			&paramPort);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	printf("set VGA Scalar params step 4\n");

	/* set output height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	OMX_GetParameter(m_pVGAScaleHandle, OMX_IndexParamPortDefinition,
			&paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_outVGAWidth, 16);
	paramPort.format.video.nFrameHeight = m_outVGAHeight;

	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
	/* scalar buffer pitch should be multiple of 16 */
	paramPort.format.video.nStride = UTIL_ALIGN(m_outVGAWidth * 2, 16);

	paramPort.nBufferSize = paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight;

	eError =
	OMX_SetParameter (m_pVGAScaleHandle, OMX_IndexParamPortDefinition,
			&paramPort);
	if (eError != OMX_ErrorNone) {
		printf("set VGA Scale param out port definition failed\n");
		fflush(stdout);
		return false;
	}
	printf("set VGA Scalar params step 5\n");

	/* set number of channles */
	OMX_INIT_PARAM(&sNumChPerHandle);
	sNumChPerHandle.nNumChannelsPerHandle = 1;
	eError = OMX_SetParameter (m_pVGAScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexParamVFPCNumChPerHandle,
			&sNumChPerHandle);
	if (eError != OMX_ErrorNone) {
		printf("set VGA Scale num channel per handle failed %x\n", eError);
		fflush(stdout);
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

	eError = OMX_SetConfig (m_pVGAScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("set VGA Scale input resolution failed %x\n", eError);
		fflush(stdout);
		return false;
	}

	printf("set output resolution \n");
	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_outVGAWidth, 16);
	chResolution.Frm0Height = m_outVGAHeight;

	chResolution.Frm0Pitch = UTIL_ALIGN(m_outVGAWidth * 2, 16);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = 0;
	chResolution.FrmCropHeight = 0;
	chResolution.eDir = OMX_DirOutput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pVGAScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("set VGA Scale output resolution failed %x\n", eError);
		fflush(stdout);
		return false;
	}

	/* disable algo bypass mode */
	OMX_INIT_PARAM(&algEnable);
	algEnable.nPortIndex = 0;
	algEnable.nChId = 0;
	algEnable.bAlgBypass = OMX_FALSE;

	eError = OMX_SetConfig (m_pVGAScaleHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable, &algEnable);
	return true;
}

bool MixtureMode_SDI::setHeadTailScaleParams() {
	if (NULL == m_pHeadTailScaleHandle) {
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
	eError = OMX_SetParameter (m_pHeadTailScaleHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType, &memTypeCfg);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Setting Memory type at output port to Raw Memory */
	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError = OMX_SetParameter (m_pHeadTailScaleHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType, &memTypeCfg);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* set input height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

	OMX_GetParameter(m_pHeadTailScaleHandle, OMX_IndexParamPortDefinition, &paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = KHeadPicWidth;
	paramPort.format.video.nFrameHeight = KHeadPicHeight;

	paramPort.format.video.nStride = KHeadPicWidth;
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	paramPort.nBufferSize = (paramPort.format.video.nStride * KHeadPicHeight * 3) >> 1;

	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
	eError = OMX_SetParameter (m_pHeadTailScaleHandle, OMX_IndexParamPortDefinition, &paramPort);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* set output height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	OMX_GetParameter(m_pHeadTailScaleHandle, OMX_IndexParamPortDefinition, &paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_outWidth, 16);
	paramPort.format.video.nFrameHeight = m_outHeight;

	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
	/* scalar buffer pitch should be multiple of 16 */
	paramPort.format.video.nStride = UTIL_ALIGN(m_outWidth * 2, 16);

	paramPort.nBufferSize = paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight;

	eError = OMX_SetParameter (m_pHeadTailScaleHandle, OMX_IndexParamPortDefinition, &paramPort);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* set number of channles */
	OMX_INIT_PARAM(&sNumChPerHandle);
	sNumChPerHandle.nNumChannelsPerHandle = 1;
	eError = OMX_SetParameter (m_pHeadTailScaleHandle, (OMX_INDEXTYPE) OMX_TI_IndexParamVFPCNumChPerHandle, &sNumChPerHandle);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = KHeadPicWidth;
	chResolution.Frm0Height = KHeadPicHeight;
	chResolution.Frm0Pitch = KHeadPicWidth;
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = KHeadPicWidth;
	chResolution.FrmCropHeight = KHeadPicHeight;
	chResolution.eDir = OMX_DirInput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pHeadTailScaleHandle, (OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution, &chResolution);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_outWidth, 16);
	chResolution.Frm0Height = m_outHeight;

	chResolution.Frm0Pitch = UTIL_ALIGN(m_outWidth * 2, 16);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = 0;
	chResolution.FrmCropHeight = 0;
	chResolution.eDir = OMX_DirOutput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pHeadTailScaleHandle, (OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution, &chResolution);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* disable algo bypass mode */
	OMX_INIT_PARAM(&algEnable);
	algEnable.nPortIndex = 0;
	algEnable.nChId = 0;
	algEnable.bAlgBypass = OMX_FALSE;

	eError = OMX_SetConfig (m_pHeadTailScaleHandle, (OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable, &algEnable);
	return true;
}

bool MixtureMode_SDI::setVGAEncodeParams() {
	if (NULL == m_pVGAEncHandle) {
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
	eError = OMX_GetParameter(m_pVGAEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* set the actual number of buffers required */
	tPortDef.nBufferCountActual = KEncodeComponentInOutPutNum;
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
		printf("failed to set Encode OMX_IndexParamPortDefinition for input \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for input \n");

	OMX_INIT_PARAM(&tPortDef);

	tPortDef.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pVGAEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* settings for OMX_IndexParamPortDefinition */
	/* set the actual number of buffers required */

	tPortDef.nBufferCountActual = KEncodeComponentInOutPutNum;
	tPortDef.format.video.cMIMEType = "H264";
	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(m_inVGAWidth, 32);
	tPortDef.format.video.nFrameHeight = m_inVGAHeight;
	tPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	tPortDef.format.video.nBitrate = m_pSetting->getVGAEncBitrate();
	/* settings for OMX_IndexParamVideoPortFormat */

	eError = OMX_SetParameter(m_pVGAEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf("failed to set Encode OMX_IndexParamPortDefinition for output \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for output \n");

	/* For changing bit rate following index can be used */
	OMX_INIT_PARAM(&tVidEncBitRate);

	tVidEncBitRate.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pVGAEncHandle, OMX_IndexParamVideoBitrate,
			&tVidEncBitRate);

	tVidEncBitRate.eControlRate = OMX_Video_ControlRateVariable;
	tVidEncBitRate.nTargetBitrate = m_pSetting->getVGAEncBitrate();
	eError = OMX_SetParameter(m_pVGAEncHandle, OMX_IndexParamVideoBitrate,&tVidEncBitRate);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Set the profile and level for H264 */
	OMX_INIT_PARAM(&tProfileLevel);
	tProfileLevel.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pVGAEncHandle, OMX_IndexParamVideoProfileLevelCurrent, &tProfileLevel);

	if (m_pSetting->getLiveEncHighProfile()) {
		tProfileLevel.eProfile = OMX_VIDEO_AVCProfileHigh;
	} else {
		tProfileLevel.eProfile = OMX_VIDEO_AVCProfileBaseline; //OMX_VIDEO_AVCProfileHigh
	}
	tProfileLevel.eLevel = OMX_VIDEO_AVCLevel42;

	eError =
	OMX_SetParameter(m_pVGAEncHandle, OMX_IndexParamVideoProfileLevelCurrent,
			&tProfileLevel);
	if (eError != OMX_ErrorNone)
		return false;

	OMX_INIT_PARAM(&tEncoderPreset);
	tEncoderPreset.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pVGAEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset, &tEncoderPreset);
	tEncoderPreset.eEncodingModePreset = OMX_Video_Enc_High_Speed_Med_Quality;
	tEncoderPreset.eRateControlPreset = OMX_Video_RC_Low_Delay;
	eError = OMX_SetParameter(m_pVGAEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset, &tEncoderPreset);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_INIT_PARAM(&tDynParams);

	tDynParams.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pVGAEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	if (m_pSetting->getLiveEncHighProfile()) {
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval =
				25;
	} else {
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.generateHeader = XDM_GENERATE_HEADER; // sps/pps
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.forceFrame = IVIDEO_IDR_FRAME;
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval = 25;
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.interFrameInterval = 1;
	}

	eError = OMX_SetParameter (m_pVGAEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	OMX_INIT_PARAM (&tStaticParam);

	tStaticParam.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter (m_pVGAEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	tStaticParam.videoStaticParams.h264EncStaticParams.videnc2Params.encodingPreset = XDM_USER_DEFINED;

	tStaticParam.videoStaticParams.h264EncStaticParams.numTemporalLayer = IH264_TEMPORAL_LAYERS_1;

	if (m_pSetting->getLiveEncHighProfile()) {
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
	if (m_pSetting->getLiveEncHighProfile()) {
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.videoFullRangeFlag = 0;
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.timingInfoPresentFlag = 1;
	} else {
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.videoFullRangeFlag = 1;
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.timingInfoPresentFlag = 0;
	}

	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.hrdParamsPresentFlag = 0;
	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.numUnitsInTicks = 1000;

	eError = OMX_SetParameter (m_pVGAEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	return (OMX_ErrorNone == eError);
}

bool MixtureMode_SDI::setEncodeParams() {
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
	tPortDef.nBufferCountActual = KEncodeComponentInOutPutNum;
	/* set the video format settings */

	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(m_outWidth, 32);
	tPortDef.format.video.nFrameHeight = m_outHeight;
	tPortDef.format.video.nStride = UTIL_ALIGN(m_outWidth, 32);
	tPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	/* settings for OMX_IndexParamVideoPortFormat */
	tPortDef.nBufferSize = (tPortDef.format.video.nStride
			* tPortDef.format.video.nFrameHeight * 3) >> 1;

	eError =
	OMX_SetParameter(m_pEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf(
				"failed to set Encode OMX_IndexParamPortDefinition for input \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for input \n");

	OMX_INIT_PARAM(&tPortDef);

	tPortDef.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError =
	OMX_GetParameter(m_pEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* settings for OMX_IndexParamPortDefinition */
	/* set the actual number of buffers required */

	tPortDef.nBufferCountActual = KEncodeComponentInOutPutNum;
	tPortDef.format.video.cMIMEType = "H264";
	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(m_outWidth, 32);
	tPortDef.format.video.nFrameHeight = m_outHeight;
	tPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	tPortDef.format.video.nBitrate = m_nEncBitRate;
	/* settings for OMX_IndexParamVideoPortFormat */

	eError =
	OMX_SetParameter(m_pEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf(
				"failed to set Encode OMX_IndexParamPortDefinition for output \n");
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

	if (m_pSetting->getLiveEncHighProfile()) {
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
	eError =
			OMX_GetParameter(m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset,
					&tEncoderPreset);

	if (m_pSetting->getEncodeQualityEnable() ) {
		  tEncoderPreset.eEncodingModePreset = OMX_Video_Enc_Default;
		  tEncoderPreset.eRateControlPreset = OMX_Video_RC_None;
	} else {
		tEncoderPreset.eEncodingModePreset = OMX_Video_Enc_High_Speed_Med_Quality;
		tEncoderPreset.eRateControlPreset = OMX_Video_RC_Low_Delay;
	}

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
	if (m_pSetting->getLiveEncHighProfile()) {
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval = 25;
	} else {
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.generateHeader = XDM_GENERATE_HEADER; // sps/pps
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.forceFrame = IVIDEO_IDR_FRAME;
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval = 25;
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.interFrameInterval = 1;
	}
	tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval =
			25;


	eError = OMX_SetParameter (m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	OMX_INIT_PARAM (&tStaticParam);

	tStaticParam.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter (m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	if (m_pSetting->getEncodeQualityEnable()) {
		  tStaticParam.videoStaticParams.h264EncStaticParams.videnc2Params.encodingPreset = XDM_DEFAULT;
		  tStaticParam.videoStaticParams.h264EncStaticParams.videnc2Params.rateControlPreset = IVIDEO_LOW_DELAY;
	} else {
		tStaticParam.videoStaticParams.h264EncStaticParams.videnc2Params.encodingPreset = XDM_USER_DEFINED;

		tStaticParam.videoStaticParams.h264EncStaticParams.numTemporalLayer = IH264_TEMPORAL_LAYERS_1;

		if (m_pSetting->getLiveEncHighProfile()) {
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
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.videoFullRangeFlag = 0;
		 if (m_pSetting->getLiveEncHighProfile()) {
			tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.timingInfoPresentFlag = 1;
			tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.hrdParamsPresentFlag = 0;
		 } else {
			 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.timingInfoPresentFlag = 0;
			 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.hrdParamsPresentFlag = 1;
		 }
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.numUnitsInTicks = 1000;
	}


	eError = OMX_SetParameter (m_pEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	return (OMX_ErrorNone == eError);
}

bool MixtureMode_SDI::setLiveEncodeParams() {
	if (NULL == m_pLiveEncHandle) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorUndefined;
	OMX_VIDEO_PARAM_PROFILELEVELTYPE tProfileLevel;
	OMX_VIDEO_PARAM_ENCODER_PRESETTYPE tEncoderPreset;
	OMX_VIDEO_PARAM_BITRATETYPE tVidEncBitRate;
	OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
	OMX_VIDEO_CONFIG_DYNAMICPARAMS tDynParams;
	OMX_VIDEO_CONFIG_STATICPARAMS tStaticParam;

	OMX_INIT_PARAM(&tPortDef);
	tPortDef.nPortIndex = OMX_VIDENC_INPUT_PORT;
	eError = OMX_GetParameter(m_pLiveEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* set the actual number of buffers required */
	tPortDef.nBufferCountActual = KOtherComponetInOutPutNum;
	/* set the video format settings */

	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(m_liveOutWidth, 32);
	tPortDef.format.video.nFrameHeight = m_liveOutHeight;
	tPortDef.format.video.nStride = UTIL_ALIGN(m_liveOutWidth, 32);
	tPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	/* settings for OMX_IndexParamVideoPortFormat */
	tPortDef.nBufferSize = (tPortDef.format.video.nStride
			* tPortDef.format.video.nFrameHeight * 3) >> 1;

	eError = OMX_SetParameter(m_pLiveEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf(
				"failed to set Encode OMX_IndexParamPortDefinition for input \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for input \n");

	OMX_INIT_PARAM(&tPortDef);

	tPortDef.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pLiveEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	/* settings for OMX_IndexParamPortDefinition */
	/* set the actual number of buffers required */

	tPortDef.nBufferCountActual = KOtherComponetInOutPutNum;
	tPortDef.format.video.cMIMEType = "H264";
	tPortDef.format.video.nFrameWidth = UTIL_ALIGN(m_liveOutWidth, 32);
	tPortDef.format.video.nFrameHeight = m_liveOutHeight;
	tPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	tPortDef.format.video.xFramerate = (m_nFrameRate << 16);
	tPortDef.format.video.nBitrate = m_pSetting->getLiveEncBitrate();
	/* settings for OMX_IndexParamVideoPortFormat */

	eError = OMX_SetParameter(m_pLiveEncHandle, OMX_IndexParamPortDefinition, &tPortDef);
	if (eError != OMX_ErrorNone) {
		printf("failed to set Encode OMX_IndexParamPortDefinition for output \n");
		return false;
	}
	printf("set Encode OMX_IndexParamPortDefinition for output \n");

	/* For changing bit rate following index can be used */
	OMX_INIT_PARAM(&tVidEncBitRate);

	tVidEncBitRate.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pLiveEncHandle, OMX_IndexParamVideoBitrate,
			&tVidEncBitRate);

	tVidEncBitRate.eControlRate = OMX_Video_ControlRateVariable;
	tVidEncBitRate.nTargetBitrate = m_pSetting->getLiveEncBitrate();
	eError = OMX_SetParameter(m_pLiveEncHandle, OMX_IndexParamVideoBitrate,
			&tVidEncBitRate);

	if (eError != OMX_ErrorNone) {
		return false;
	}

	/* Set the profile and level for H264 */
	OMX_INIT_PARAM(&tProfileLevel);
	tProfileLevel.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pLiveEncHandle, OMX_IndexParamVideoProfileLevelCurrent,
			&tProfileLevel);

	if (m_pSetting->getLiveEncHighProfile()) {
		tProfileLevel.eProfile = OMX_VIDEO_AVCProfileHigh;
	} else {
		tProfileLevel.eProfile = OMX_VIDEO_AVCProfileBaseline; //OMX_VIDEO_AVCProfileHigh
	}
	tProfileLevel.eLevel = OMX_VIDEO_AVCLevel42;

	eError = OMX_SetParameter(m_pLiveEncHandle, OMX_IndexParamVideoProfileLevelCurrent,
			&tProfileLevel);
	if (eError != OMX_ErrorNone)
		return false;

	OMX_INIT_PARAM(&tEncoderPreset);
	tEncoderPreset.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
	eError = OMX_GetParameter(m_pLiveEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset,
					&tEncoderPreset);
	tEncoderPreset.eEncodingModePreset = OMX_Video_Enc_High_Speed_Med_Quality;
	tEncoderPreset.eRateControlPreset = OMX_Video_RC_Low_Delay;

	eError = OMX_SetParameter(m_pLiveEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset,
					&tEncoderPreset);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMX_INIT_PARAM(&tDynParams);

	tDynParams.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter(m_pLiveEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);


	if (m_pSetting->getLiveEncHighProfile()) {
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval = 25;
	} else {
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.generateHeader = XDM_GENERATE_HEADER; // sps/pps
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.forceFrame = IVIDEO_IDR_FRAME;
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval = 25;
		tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.interFrameInterval = 1;
	}

	eError = OMX_SetParameter (m_pLiveEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDynamicParams,
					&tDynParams);

	OMX_INIT_PARAM (&tStaticParam);

	tStaticParam.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

	eError = OMX_GetParameter (m_pLiveEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	tStaticParam.videoStaticParams.h264EncStaticParams.videnc2Params.encodingPreset = XDM_USER_DEFINED;

	tStaticParam.videoStaticParams.h264EncStaticParams.numTemporalLayer = IH264_TEMPORAL_LAYERS_1;

	if (m_pSetting->getLiveEncHighProfile()) {
		tStaticParam.videoStaticParams.h264EncStaticParams.transformBlockSize = IH264_TRANSFORM_8x8;
		tStaticParam.videoStaticParams.h264EncStaticParams.entropyCodingMode = IH264_ENTROPYCODING_CABAC;
	} else {
		tStaticParam.videoStaticParams.h264EncStaticParams.transformBlockSize = IH264_TRANSFORM_4x4;
		tStaticParam.videoStaticParams.h264EncStaticParams.entropyCodingMode = IH264_ENTROPYCODING_CAVLC;
	}
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
	 if (m_pSetting->getLiveEncHighProfile()) {
		tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.timingInfoPresentFlag = 1;
		tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.hrdParamsPresentFlag = 0;
	 } else {
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.timingInfoPresentFlag = 0;
		 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.hrdParamsPresentFlag = 1;
	 }


	 tStaticParam.videoStaticParams.h264EncStaticParams.vuiCodingParams.numUnitsInTicks = 1000;

	eError = OMX_SetParameter (m_pLiveEncHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoStaticParams,
	&tStaticParam);

	return (OMX_ErrorNone == eError);
}

bool MixtureMode_SDI::setDeiParams() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_PARAM_BUFFER_MEMORYTYPE memTypeCfg;
	OMX_PARAM_PORTDEFINITIONTYPE paramPort;
	OMX_PARAM_VFPC_NUMCHANNELPERHANDLE sNumChPerHandle;
	OMX_CONFIG_ALG_ENABLE algEnable;
	OMX_CONFIG_VIDCHANNEL_RESOLUTION chResolution;

	OMX_CONFIG_SUBSAMPLING_FACTOR sSubSamplinginfo = { 0 };

	OMX_INIT_PARAM(&sSubSamplinginfo);

	sSubSamplinginfo.nSubSamplingFactor = 1;
	eError = OMX_SetConfig (m_pDeiHandle, ( OMX_INDEXTYPE )
		( OMX_TI_IndexConfigSubSamplingFactor ),
		&sSubSamplinginfo );

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError = OMX_SetParameter (m_pDeiHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType, &memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at input port\n");
		return false;
	}

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError = OMX_SetParameter (m_pDeiHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType, &memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at output port\n");
		return false;
	}

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError = OMX_SetParameter (m_pDeiHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType, &memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at output port\n");
		return false;
	}

	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

	OMX_GetParameter(m_pDeiHandle, OMX_IndexParamPortDefinition, &paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_outWidth, 32);
	paramPort.format.video.nFrameHeight = m_outHeight;

	paramPort.format.video.nStride = UTIL_ALIGN(m_outWidth * 2, 32);
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
	paramPort.format.video.xFramerate = (m_nFrameRate << 16);
	paramPort.nBufferSize = paramPort.format.video.nStride
				* paramPort.format.video.nFrameHeight;

	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;

	OMX_SetParameter(m_pDeiHandle, OMX_IndexParamPortDefinition, &paramPort);

	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	OMX_GetParameter(m_pDeiHandle, OMX_IndexParamPortDefinition, &paramPort);

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

	OMX_SetParameter(m_pDeiHandle, OMX_IndexParamPortDefinition, &paramPort);

	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1;
	OMX_GetParameter(m_pDeiHandle, OMX_IndexParamPortDefinition, &paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX + 1;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_outWidth, 32);
	paramPort.format.video.nFrameHeight = m_outHeight;
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	paramPort.format.video.xFramerate = (m_nFrameRate << 16);
	paramPort.nBufferAlignment = 0;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
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
	chResolution.Frm0Width = UTIL_ALIGN(m_outWidth, 32);
	chResolution.Frm0Height = m_outHeight;
	chResolution.Frm0Pitch = UTIL_ALIGN(m_outWidth * 2, 32);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = m_outWidth;
	chResolution.FrmCropHeight = m_outHeight;

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
	chResolution.Frm0Width = UTIL_ALIGN(m_liveOutWidth, 32);
	chResolution.Frm0Height = m_liveOutHeight;
	chResolution.Frm0Pitch = UTIL_ALIGN(m_liveOutWidth * 2, 32);
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

	eError = OMX_SetConfig (m_pDeiHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable,
			&algEnable);
	if (eError != OMX_ErrorNone) {
		printf("failed to disable algo by pass mode\n");
		return false;
	}

	return true;
}

bool MixtureMode_SDI::setLiveNfParams() {
	if (NULL == m_pLiveNfHandle) {
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
	eError =
	OMX_SetParameter (m_pLiveNfHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
			&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at input port\n");
		return false;
	}

	/* Setting Memory type at output port to Raw Memory */
	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError =
	OMX_SetParameter (m_pLiveNfHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
			&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at output port\n");
		return false;
	}

	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError = OMX_SetParameter (m_pLiveNfHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
				&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at output port\n");
		return false;
	}

	/* set input height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

	OMX_GetParameter(m_pLiveNfHandle, OMX_IndexParamPortDefinition, &paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_liveOutWidth, 32);
	paramPort.format.video.nFrameHeight = UTIL_ALIGN(m_liveOutHeight, 32);

	paramPort.format.video.nStride = UTIL_ALIGN(m_liveOutWidth * 2, 32);
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
	paramPort.nBufferSize = (paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight);

	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;

	OMX_SetParameter(m_pLiveNfHandle, OMX_IndexParamPortDefinition, &paramPort);

	/* set output height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	OMX_GetParameter(m_pLiveNfHandle, OMX_IndexParamPortDefinition, &paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_liveOutWidth, 32);
	paramPort.format.video.nFrameHeight = UTIL_ALIGN(m_liveOutHeight, 32);
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;
	paramPort.format.video.nStride = UTIL_ALIGN(m_liveOutWidth, 32);

	paramPort.nBufferSize = (paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight * 3) >> 1;

	OMX_SetParameter(m_pLiveNfHandle, OMX_IndexParamPortDefinition, &paramPort);

	OMX_INIT_PARAM(&sNumChPerHandle);
	sNumChPerHandle.nNumChannelsPerHandle = 1;
	eError = OMX_SetParameter (m_pLiveNfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexParamVFPCNumChPerHandle,
			&sNumChPerHandle);
	if (eError != OMX_ErrorNone) {
		printf("failed to set num of channels\n");
		return false;
	}

	/* set VFPC input and output resolution information */
	printf("set input resolution \n");

	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_liveOutWidth, 32);
	chResolution.Frm0Height = UTIL_ALIGN(m_liveOutHeight, 32);
	chResolution.Frm0Pitch = UTIL_ALIGN(m_liveOutWidth * 2, 32);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = UTIL_ALIGN(m_liveOutWidth, 32);
	chResolution.FrmCropHeight = UTIL_ALIGN(m_liveOutHeight, 32);
	chResolution.eDir = OMX_DirInput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pLiveNfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("failed to set input channel resolution\n");
	}

	printf("set output resolution \n");
	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_liveOutWidth, 32);
	chResolution.Frm0Height = UTIL_ALIGN(m_liveOutHeight, 32);
	chResolution.Frm0Pitch = UTIL_ALIGN(m_liveOutWidth, 32);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;

	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = m_liveOutHeight;
	chResolution.FrmCropWidth = 0;
	chResolution.FrmCropHeight = (UTIL_ALIGN(m_liveOutHeight, 32)) - m_liveOutHeight;
	chResolution.eDir = OMX_DirOutput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pLiveNfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("failed to set output channel resolution\n");
		return false;
	}

	/* disable algo bypass mode */
	OMX_INIT_PARAM(&algEnable);
	algEnable.nPortIndex = 0;
	algEnable.nChId = 0;
	algEnable.bAlgBypass = OMX_FALSE;
	OMX_INIT_PARAM(&algEnable);

	eError = OMX_SetConfig (m_pLiveNfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable, &algEnable);

	return (OMX_ErrorNone == eError);
}

bool MixtureMode_SDI::setNfParams() {
	if (NULL == m_pNfHandle) {
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
	eError =
	OMX_SetParameter (m_pNfHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
			&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at input port\n");
		return false;
	}

	/* Setting Memory type at output port to Raw Memory */
	OMX_INIT_PARAM(&memTypeCfg);
	memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	eError =
	OMX_SetParameter (m_pNfHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamBuffMemType,
			&memTypeCfg);

	if (eError != OMX_ErrorNone) {
		printf("failed to set memory Type at output port\n");
		return false;
	}

	/* set input height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

	OMX_GetParameter(m_pNfHandle, OMX_IndexParamPortDefinition, &paramPort);
	paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_outWidth, 32);
	paramPort.format.video.nFrameHeight = UTIL_ALIGN(m_outHeight, 32);

	paramPort.format.video.nStride = UTIL_ALIGN(m_outWidth * 2, 32);
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
	paramPort.nBufferSize = (paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight);

	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KOtherComponetInOutPutNum;

	OMX_SetParameter(m_pNfHandle, OMX_IndexParamPortDefinition, &paramPort);

	/* set output height/width and color format */
	OMX_INIT_PARAM(&paramPort);
	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	OMX_GetParameter(m_pNfHandle, OMX_IndexParamPortDefinition, &paramPort);

	paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
	paramPort.format.video.nFrameWidth = UTIL_ALIGN(m_outWidth, 32);
	paramPort.format.video.nFrameHeight = UTIL_ALIGN(m_outHeight, 32);
	paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	paramPort.nBufferAlignment = 0;
	paramPort.bBuffersContiguous = OMX_FALSE;
	paramPort.nBufferCountActual = KEncodeComponentInOutPutNum;
	paramPort.format.video.nStride = UTIL_ALIGN(m_outWidth, 32);

	paramPort.nBufferSize = (paramPort.format.video.nStride
			* paramPort.format.video.nFrameHeight * 3) >> 1;

	OMX_SetParameter(m_pNfHandle, OMX_IndexParamPortDefinition, &paramPort);

	OMX_INIT_PARAM(&sNumChPerHandle);
	sNumChPerHandle.nNumChannelsPerHandle = 1;
	eError = OMX_SetParameter (m_pNfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexParamVFPCNumChPerHandle,
			&sNumChPerHandle);
	if (eError != OMX_ErrorNone) {
		printf("failed to set num of channels\n");
		return false;
	}

	/* set VFPC input and output resolution information */
	printf("set input resolution \n");

	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_outWidth, 32);
	chResolution.Frm0Height = UTIL_ALIGN(m_outHeight, 32);
	chResolution.Frm0Pitch = UTIL_ALIGN(m_outWidth * 2, 32);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;
	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = 0;
	chResolution.FrmCropWidth = UTIL_ALIGN(m_outWidth, 32);
	chResolution.FrmCropHeight = UTIL_ALIGN(m_outHeight, 32);
	chResolution.eDir = OMX_DirInput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pNfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("failed to set input channel resolution\n");
	}

	printf("set output resolution \n");
	OMX_INIT_PARAM(&chResolution);
	chResolution.Frm0Width = UTIL_ALIGN(m_outWidth, 32);
	chResolution.Frm0Height = UTIL_ALIGN(m_outHeight, 32);
	chResolution.Frm0Pitch = UTIL_ALIGN(m_outWidth, 32);
	chResolution.Frm1Width = 0;
	chResolution.Frm1Height = 0;
	chResolution.Frm1Pitch = 0;

	chResolution.FrmStartX = 0;
	chResolution.FrmStartY = m_outHeight;
	chResolution.FrmCropWidth = 0;
	chResolution.FrmCropHeight = (UTIL_ALIGN(m_outHeight, 32)) - m_outHeight;
	chResolution.eDir = OMX_DirOutput;
	chResolution.nChId = 0;

	eError = OMX_SetConfig (m_pNfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
			&chResolution);
	if (eError != OMX_ErrorNone) {
		printf("failed to set output channel resolution\n");
		return false;
	}

	/* disable algo bypass mode */
	OMX_INIT_PARAM(&algEnable);
	algEnable.nPortIndex = 0;
	algEnable.nChId = 0;
	algEnable.bAlgBypass = OMX_FALSE;
	OMX_INIT_PARAM(&algEnable);

	eError = OMX_SetConfig (m_pNfHandle,
			(OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable, &algEnable);

	return (OMX_ErrorNone == eError);

}

bool MixtureMode_SDI::freeSDIDEIBuffers() {
	if (NULL == m_sdiDEIComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (int i = 0; i < m_sdiDEIComponent->numInport; ++i) {
		InportParams* InportParamsPtr = m_sdiDEIComponent->inPortParams + i;
		for (OMX_U32 j = 0; j < InportParamsPtr->nBufferCountActual; j++) {
			eError = OMX_FreeBuffer (m_pSDIDEIHandle,
					OMX_VFPC_INPUT_PORT_START_INDEX + i,
					InportParamsPtr->pInBuff[j]);
			if (eError != OMX_ErrorNone) {
				return false;
			}
		}
	}

	for (OMX_U32 j = 0; j < m_sdiDEIComponent->numOutport; ++j) {
		OutportParams *outPortParamsPtr = m_sdiDEIComponent->outPortParams + j;
		for (OMX_U32 i = 0; i < outPortParamsPtr->nBufferCountActual; i++) {
			eError = OMX_FreeBuffer (m_pSDIDEIHandle,
					OMX_VFPC_OUTPUT_PORT_START_INDEX + j,
					outPortParamsPtr->pOutBuff[i]);
			if (eError != OMX_ErrorNone) {
				return false;
			}
			Memory_free (m_heap, outPortParamsPtr->pOutBuff[i]->pBuffer,
					outPortParamsPtr->nBufferSize);
		}
	}

	OMXComponentUtils::waitStateSem(m_sdiDEIComponent);
	return true;
}

bool MixtureMode_SDI::freeSDIScaleBuffers() {
	if (NULL == m_sdiScaleComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (int i = 0; i < 2; ++i) {
		InportParams *inPortParamsPtr = m_sdiScaleComponent->inPortParams + i;
		for (OMX_U32 j = 0; j < inPortParamsPtr->nBufferCountActual; j++) {
			eError = OMX_FreeBuffer(m_pSDIScaleHandle, OMX_VFPC_INPUT_PORT_START_INDEX + i,
							inPortParamsPtr->pInBuff[j]);
			if (eError != OMX_ErrorNone) {
				return false;
			}
		}

		OutportParams *outPortParamsPtr = m_sdiScaleComponent->outPortParams + i;
		for (OMX_U32 j = 0; j < outPortParamsPtr->nBufferCountActual; j++) {
			eError = OMX_FreeBuffer(m_pSDIScaleHandle, OMX_VFPC_OUTPUT_PORT_START_INDEX + i,
							outPortParamsPtr->pOutBuff[j]);
			if (eError != OMX_ErrorNone) {
				return 0;
			}
			Memory_free (m_heap, outPortParamsPtr->pOutBuff[j]->pBuffer,
					outPortParamsPtr->nBufferSize);
		}
	}

	OMXComponentUtils::waitStateSem(m_sdiScaleComponent);
	return true;
}

bool MixtureMode_SDI::freeSDINfBuffers() {
	if (NULL == m_sdiNfComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_sdiNfComponent->inPortParams->nBufferCountActual;
			i++) {
		eError = OMX_FreeBuffer (m_pSDINfHandle,
				OMX_VFPC_INPUT_PORT_START_INDEX,
				m_sdiNfComponent->inPortParams->pInBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	OutportParams* outportParamsPtr = NULL;
	for (OMX_U32 i = 0; i < m_sdiNfComponent->numOutport; ++i) {
		outportParamsPtr = m_sdiNfComponent->outPortParams + i;
		for (OMX_U32 j = 0; j < outportParamsPtr->nBufferCountActual; j++) {
			eError = OMX_FreeBuffer (m_pSDINfHandle,
					OMX_VFPC_OUTPUT_PORT_START_INDEX + i,
					m_sdiNfComponent->outPortParams->pOutBuff[j]);
			if (eError != OMX_ErrorNone) {
				return false;
			}
			Memory_free (m_heap, outportParamsPtr->pOutBuff[j]->pBuffer,
					outportParamsPtr->nBufferSize);
		}
	}

	OMXComponentUtils::waitStateSem(m_sdiNfComponent);
	return true;
}

bool MixtureMode_SDI::freeSDIEncBuffers() {
	if (NULL == m_sdiEncComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_sdiEncComponent->inPortParams->nBufferCountActual;
			i++) {
		eError = OMX_FreeBuffer (m_pSDIEncHandle,
				OMX_VIDENC_INPUT_PORT,
				m_sdiEncComponent->inPortParams->pInBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	for (OMX_U32 i = 0; i < m_sdiEncComponent->outPortParams->nBufferCountActual; i++) {
		eError = OMX_FreeBuffer (m_pSDIEncHandle,
				OMX_VIDENC_OUTPUT_PORT,
				m_sdiEncComponent->outPortParams->pOutBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	OMXComponentUtils::waitStateSem(m_sdiEncComponent);
	return true;
}

bool MixtureMode_SDI::freeVGAScaleBuffers() {
	if (NULL == m_vgaScaleComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (OMX_U32 j = 0; j < m_vgaScaleComponent->inPortParams->nBufferCountActual; j++) {
		eError = OMX_FreeBuffer(m_pVGAScaleHandle, OMX_VFPC_INPUT_PORT_START_INDEX,
						m_vgaScaleComponent->inPortParams->pInBuff[j]);
		if (eError != OMX_ErrorNone) {
			return 0;
		}
	}

	for (OMX_U32 j = 0; j < m_vgaScaleComponent->outPortParams->nBufferCountActual; j++) {
		eError = OMX_FreeBuffer(m_pVGAScaleHandle, OMX_VFPC_OUTPUT_PORT_START_INDEX,
						m_vgaScaleComponent->outPortParams->pOutBuff[j]);
		if (eError != OMX_ErrorNone) {
			return 0;
		}
		Memory_free (m_heap, m_vgaScaleComponent->outPortParams->pOutBuff[j]->pBuffer,
						m_vgaScaleComponent->outPortParams->nBufferSize);
	}

	OMXComponentUtils::waitStateSem(m_vgaScaleComponent);
	return true;
}

bool MixtureMode_SDI::freeHeadTailScaleBuffers() {
	if (NULL == m_headTailScaleComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (OMX_U32 j = 0; j < m_headTailScaleComponent->inPortParams->nBufferCountActual; j++) {
		eError = OMX_FreeBuffer(m_pHeadTailScaleHandle, OMX_VFPC_INPUT_PORT_START_INDEX,
				m_headTailScaleComponent->inPortParams->pInBuff[j]);
		if (eError != OMX_ErrorNone) {
			return 0;
		}
	}

	for (OMX_U32 j = 0; j < m_headTailScaleComponent->outPortParams->nBufferCountActual; j++) {
		eError = OMX_FreeBuffer(m_pHeadTailScaleHandle, OMX_VFPC_OUTPUT_PORT_START_INDEX,
				m_headTailScaleComponent->outPortParams->pOutBuff[j]);
		if (eError != OMX_ErrorNone) {
			return 0;
		}
		Memory_free (m_heap, m_headTailScaleComponent->outPortParams->pOutBuff[j]->pBuffer,
				m_headTailScaleComponent->outPortParams->nBufferSize);
	}

	OMXComponentUtils::waitStateSem(m_headTailScaleComponent);
	return true;
}

bool MixtureMode_SDI::freeNfBuffers() {
	if (NULL == m_nfComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_nfComponent->inPortParams->nBufferCountActual;
			i++) {
		eError = OMX_FreeBuffer (m_pNfHandle,
				OMX_VFPC_INPUT_PORT_START_INDEX,
				m_nfComponent->inPortParams->pInBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	OutportParams* outportParamsPtr = NULL;
	for (OMX_U32 i = 0; i < m_nfComponent->numOutport; ++i) {
		outportParamsPtr = m_nfComponent->outPortParams + i;
		for (OMX_U32 j = 0; j < outportParamsPtr->nBufferCountActual; j++) {
			eError = OMX_FreeBuffer (m_pNfHandle,
					OMX_VFPC_OUTPUT_PORT_START_INDEX + i,
					m_nfComponent->outPortParams->pOutBuff[j]);
			if (eError != OMX_ErrorNone) {
				return false;
			}
			Memory_free (m_heap, outportParamsPtr->pOutBuff[j]->pBuffer,
					outportParamsPtr->nBufferSize);
		}
	}

	OMXComponentUtils::waitStateSem(m_nfComponent);
	return true;
}

bool MixtureMode_SDI::freeEncBuffers() {
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

bool MixtureMode_SDI::freeLiveEncBuffers() {
	if (NULL == m_liveEncComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_liveEncComponent->inPortParams->nBufferCountActual;
			i++) {
		eError = OMX_FreeBuffer (m_pLiveEncHandle,
				OMX_VIDENC_INPUT_PORT,
				m_liveEncComponent->inPortParams->pInBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	for (OMX_U32 i = 0; i < m_liveEncComponent->outPortParams->nBufferCountActual;
			i++) {
		eError = OMX_FreeBuffer (m_pLiveEncHandle,
				OMX_VIDENC_OUTPUT_PORT,
				m_liveEncComponent->outPortParams->pOutBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	OMXComponentUtils::waitStateSem(m_liveEncComponent);
	return true;
}

bool MixtureMode_SDI::freeVGAEncBuffers() {
	if (NULL == m_vgaEncComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_vgaEncComponent->inPortParams->nBufferCountActual;
			i++) {
		eError = OMX_FreeBuffer (m_pVGAEncHandle,
				OMX_VIDENC_INPUT_PORT,
				m_vgaEncComponent->inPortParams->pInBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	for (OMX_U32 i = 0; i < m_vgaEncComponent->outPortParams->nBufferCountActual;
			i++) {
		eError = OMX_FreeBuffer (m_pVGAEncHandle,
				OMX_VIDENC_OUTPUT_PORT,
				m_vgaEncComponent->outPortParams->pOutBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	OMXComponentUtils::waitStateSem(m_vgaEncComponent);
	return true;
}


bool MixtureMode_SDI::freeDeiBuffers() {
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
			Memory_free (m_heap, outPortParamsPtr->pOutBuff[i]->pBuffer,
					outPortParamsPtr->nBufferSize);
		}
	}

	OMXComponentUtils::waitStateSem(m_deiComponent);
	return true;
}

bool MixtureMode_SDI::freeLiveNfBuffers() {
	if (NULL == m_liveNfComponent) {
		return false;
	}
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_liveNfComponent->inPortParams->nBufferCountActual; i++) {
		eError = OMX_FreeBuffer (m_pLiveNfHandle,
				OMX_VFPC_INPUT_PORT_START_INDEX,
				m_liveNfComponent->inPortParams->pInBuff[i]);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	OutportParams* outportParamsPtr = NULL;
	for (OMX_U32 i = 0; i < m_liveNfComponent->numOutport; ++i) {
		outportParamsPtr = m_liveNfComponent->outPortParams + i;
		for (OMX_U32 j = 0; j < outportParamsPtr->nBufferCountActual; j++) {
			eError = OMX_FreeBuffer (m_pLiveNfHandle,
					OMX_VFPC_OUTPUT_PORT_START_INDEX + i,
					m_liveNfComponent->outPortParams->pOutBuff[j]);
			if (eError != OMX_ErrorNone) {
				return false;
			}
			Memory_free (m_heap, outportParamsPtr->pOutBuff[j]->pBuffer,
					outportParamsPtr->nBufferSize);
		}
	}

	OMXComponentUtils::waitStateSem(m_liveNfComponent);
	return true;
}

BOOL MixtureMode_SDI::doMovieHead() {
	m_bHeadState = true;
	return true;

}

BOOL MixtureMode_SDI::doMovieTail() {
	printf("stop capture vga\n");
	if (m_bPaused) {
		resume();
	}

	if (OMXComponentUtils::changeComponentState(m_headTailScaleComponent, OMX_StateExecuting)) {
		OMXComponentUtils::waitStateSem(m_headTailScaleComponent);
	} else {
		printf("change head tail scale component running failed\n");
	}

	m_exchangeMutex.lock();
	m_bTailState = true;
	printf("do movie tail\n");

	m_teacherSceneInfo.dwLeft = 0;
	m_teacherSceneInfo.dwRight = 0;
	m_teacherSceneInfo.dwTop = 0;
	m_teacherSceneInfo.dwBottom = 0;

	m_studentSceneInfo.dwLeft = 0;
	m_studentSceneInfo.dwRight = 0;
	m_studentSceneInfo.dwTop = 0;
	m_studentSceneInfo.dwBottom = 0;

	m_studentSceneInfo.dwLeft = 0;
	m_studentSceneInfo.dwRight = 0;
	m_studentSceneInfo.dwTop = 0;
	m_studentSceneInfo.dwBottom = 0;
	m_exchangeMutex.unlock();

	return true;
}

bool MixtureMode_SDI::createComponents() {
	OMXComponentUtils::initComponent(m_sdiDEIComponent);
	strcpy(m_sdiDEIComponent->name, "sdiDEI");
	m_sdiDEIComponent->pParent = this;

	OMXComponentUtils::initComponent(m_sdiScaleComponent);
	strcpy(m_sdiScaleComponent->name, "sdiScale");
	m_sdiScaleComponent->pParent = this;

	OMXComponentUtils::initComponent(m_sdiNfComponent);
	strcpy(m_sdiNfComponent->name, "sdiNf");
	m_sdiNfComponent->pParent = this;

	OMXComponentUtils::initComponent(m_sdiEncComponent);
	strcpy(m_sdiEncComponent->name, "sdiEnc");
	m_sdiEncComponent->pParent = this;

	OMXComponentUtils::initComponent(m_vgaScaleComponent);
	strcpy(m_vgaScaleComponent->name, "vgaScale");
	m_vgaScaleComponent->pParent = this;

	OMXComponentUtils::initComponent(m_headTailScaleComponent);
	strcpy(m_headTailScaleComponent->name, "HeadScale");
	m_headTailScaleComponent->pParent = this;

	OMXComponentUtils::initComponent(m_encComponent);
	strcpy(m_encComponent->name, "enc");
	m_encComponent->pParent = this;
	if (m_pSetting->getLiveEncEnable()) {
		OMXComponentUtils::initComponent(m_deiComponent);
		strcpy(m_deiComponent->name, "dei");
		OMXComponentUtils::initComponent(m_liveNfComponent);
		strcpy(m_liveNfComponent->name, "liveNf");
		OMXComponentUtils::initComponent(m_liveEncComponent);
		strcpy(m_liveEncComponent->name, "liveEnc");
	} else {
		OMXComponentUtils::initComponent(m_nfComponent);
		strcpy(m_nfComponent->name, "nf");
	}


	if (m_pSetting->getVGAEncodeEnable()) {
		OMXComponentUtils::initComponent(m_vgaEncComponent);
		strcpy(m_vgaEncComponent->name, "vgaEncode");
	}

	m_sdiDEIComponent->numInport = 2;
	m_sdiDEIComponent->numOutport = 4;
	m_sdiDEIComponent->startOutportIndex = OMX_VFPC_NUM_OUTPUT_PORTS;

	m_sdiScaleComponent->numInport = 2;
	m_sdiScaleComponent->numOutport = 2;
	m_sdiScaleComponent->startOutportIndex = OMX_VFPC_NUM_OUTPUT_PORTS;

	m_sdiNfComponent->numInport = 1;
	m_sdiNfComponent->numOutport = 1;
	m_sdiNfComponent->startOutportIndex = OMX_VFPC_NUM_OUTPUT_PORTS;

	m_sdiEncComponent->numInport = 1;
	m_sdiEncComponent->numOutport = 1;
	m_sdiEncComponent->startOutportIndex = 1;

	m_vgaScaleComponent->numInport = 1;
	m_vgaScaleComponent->numOutport = 1;
	m_vgaScaleComponent->startOutportIndex = OMX_VFPC_NUM_OUTPUT_PORTS;

	m_headTailScaleComponent->numInport = 1;
	m_headTailScaleComponent->numOutport = 1;
	m_headTailScaleComponent->startOutportIndex = OMX_VFPC_NUM_OUTPUT_PORTS;

	m_encComponent->numInport = 1;
	m_encComponent->numOutport = 1;
	m_encComponent->startOutportIndex = 1;

	if (m_pSetting->getLiveEncEnable()) {
		m_deiComponent->numInport = 1;
		m_deiComponent->numOutport = 2;
		m_deiComponent->startOutportIndex = OMX_VFPC_NUM_OUTPUT_PORTS;

		m_liveNfComponent->numInport = 1;
		m_liveNfComponent->numOutport = 1;
		m_liveNfComponent->startOutportIndex = OMX_VFPC_NUM_OUTPUT_PORTS;

		m_liveEncComponent->numInport = 1;
		m_liveEncComponent->numOutport = 1;
		m_liveEncComponent->startOutportIndex = 1;
	} else {
		m_nfComponent->numInport = 1;
		m_nfComponent->numOutport = 1;
		m_nfComponent->startOutportIndex = OMX_VFPC_NUM_OUTPUT_PORTS;
	}


	if (m_pSetting->getVGAEncodeEnable()) {
		m_vgaEncComponent->numInport = 1;
		m_vgaEncComponent->numOutport = 1;
		m_vgaEncComponent->startOutportIndex = 1;
	}

	return true;
}

bool MixtureMode_SDI::destroyComponents() {
	OMXComponentUtils::uninitComponent(m_sdiScaleComponent);
	OMXComponentUtils::uninitComponent(m_sdiDEIComponent);
	OMXComponentUtils::uninitComponent(m_sdiNfComponent);
	OMXComponentUtils::uninitComponent(m_sdiEncComponent);

	OMXComponentUtils::uninitComponent(m_headTailScaleComponent);

	OMXComponentUtils::uninitComponent(m_encComponent);

	if (m_pSetting->getLiveEncEnable()) {
		OMXComponentUtils::uninitComponent(m_deiComponent);
		OMXComponentUtils::uninitComponent(m_liveNfComponent);
		OMXComponentUtils::uninitComponent(m_liveEncComponent);
	} else {
		OMXComponentUtils::uninitComponent(m_nfComponent);
	}
	return true;
}

bool MixtureMode_SDI::allocInOutLowerHalfParams() {
	InportParams *inPortParamsPtr;
	OutportParams *outPortParamsPtr;

	OMXComponentUtils::allocInOutParams(m_headTailScaleComponent);

	OMXComponentUtils::allocInOutParams(m_encComponent);
	if (m_pSetting->getLiveEncEnable()) {
		OMXComponentUtils::allocInOutParams(m_deiComponent);
		OMXComponentUtils::allocInOutParams(m_liveNfComponent);
		OMXComponentUtils::allocInOutParams(m_liveEncComponent);
	} else {
		OMXComponentUtils::allocInOutParams(m_nfComponent);
	}

	for (OMX_U32 j = 0; j < m_headTailScaleComponent->numInport; ++j) {
		inPortParamsPtr = m_headTailScaleComponent->inPortParams + j;
		inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		inPortParamsPtr->nBufferSize = (HD_WIDTH * HD_HEIGHT * 3) >> 1;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (OMX_U32 j = 0; j < m_headTailScaleComponent->numOutport; ++j) {
		outPortParamsPtr = m_headTailScaleComponent->outPortParams + j;
		outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		outPortParamsPtr->nBufferSize = HD_WIDTH * HD_HEIGHT * 2;

		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int *) m_headTailScaleComponent->localPipe);

	for (OMX_U32 i = 0; i < m_encComponent->numInport; ++i) {
		inPortParamsPtr = m_encComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual = KEncodeComponentInOutPutNum;
		inPortParamsPtr->nBufferSize = ((UTIL_ALIGN(m_outWidth, 32))
				* ((UTIL_ALIGN(m_outHeight, 32))) * 3) >> 1;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (OMX_U32 i = 0; i < m_encComponent->numOutport; ++i) {
		outPortParamsPtr = m_encComponent->outPortParams + i;
		outPortParamsPtr->nBufferCountActual = KEncodeComponentInOutPutNum;
		outPortParamsPtr->nBufferSize =  ((UTIL_ALIGN(m_outHeight, 32))
				* (UTIL_ALIGN(m_outWidth, 32)) * 3) >> 1;
		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int*) m_encComponent->localPipe);

	if (m_pSetting->getLiveEncEnable()) {
		for (OMX_U32 i = 0; i < m_deiComponent->numInport; ++i) {
			inPortParamsPtr = m_deiComponent->inPortParams + i;
			inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			inPortParamsPtr->nBufferSize = (UTIL_ALIGN(m_outWidth, 32))
					* (UTIL_ALIGN(m_outHeight, 32)) * 2;
			pipe((int*) inPortParamsPtr->ipBufPipe);
		}
		for (OMX_U32 i = 0; i < m_deiComponent->numOutport; ++i) {
			outPortParamsPtr = m_deiComponent->outPortParams + i;
			outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			if (0 == i % 2) {
				outPortParamsPtr->nBufferSize = (UTIL_ALIGN(m_liveOutWidth, 32))
						* (UTIL_ALIGN(m_liveOutHeight, 32)) * 2;
			} else {
				outPortParamsPtr->nBufferSize = (UTIL_ALIGN(m_outWidth, 32))
						* (UTIL_ALIGN(m_outHeight, 32)) * 3 /2;
			}
			pipe((int*)outPortParamsPtr->opBufPipe);
		}
		pipe((int*) m_deiComponent->localPipe);

		for (OMX_U32 i = 0; i < m_liveNfComponent->numInport; ++i) {
			inPortParamsPtr = m_liveNfComponent->inPortParams + i;
			inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			inPortParamsPtr->nBufferSize = (UTIL_ALIGN(m_liveOutWidth, 32))
					* (UTIL_ALIGN(m_liveOutHeight, 32)) * 2;
			pipe((int*) inPortParamsPtr->ipBufPipe);
		}
		for (OMX_U32 i = 0; i < m_liveNfComponent->numOutport; ++i) {
			outPortParamsPtr = m_liveNfComponent->outPortParams + i;
			outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			outPortParamsPtr->nBufferSize = (UTIL_ALIGN(m_liveOutWidth, 32))
					* (UTIL_ALIGN(m_liveOutHeight, 32)) * 3 / 2;
			pipe((int*)outPortParamsPtr->opBufPipe);
		}
		pipe((int*) m_liveNfComponent->localPipe);

		for (OMX_U32 i = 0; i < m_liveEncComponent->numInport; ++i) {
			inPortParamsPtr = m_liveEncComponent->inPortParams + i;
			inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			inPortParamsPtr->nBufferSize = ((UTIL_ALIGN(m_liveOutWidth, 32))
					* ((UTIL_ALIGN(m_liveOutHeight, 32))) * 3) >> 1;
			pipe((int *) inPortParamsPtr->ipBufPipe);
		}
		for (OMX_U32 i = 0; i < m_liveEncComponent->numOutport; ++i) {
			outPortParamsPtr = m_liveEncComponent->outPortParams + i;
			outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			outPortParamsPtr->nBufferSize =  ((UTIL_ALIGN(m_liveOutHeight, 32))
					* (UTIL_ALIGN(m_liveOutWidth, 32)) * 3) >> 1;
			pipe((int *) outPortParamsPtr->opBufPipe);
		}
		pipe((int*) m_liveEncComponent->localPipe);
	} else {
		for (OMX_U32 i = 0; i < m_nfComponent->numInport; ++i) {
			inPortParamsPtr = m_nfComponent->inPortParams + i;
			inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			inPortParamsPtr->nBufferSize = (UTIL_ALIGN (m_outWidth, 32))  * (UTIL_ALIGN(m_outHeight, 32)) * 2;
			pipe((int *) inPortParamsPtr->ipBufPipe);
		}
		for (OMX_U32 i = 0; i < m_nfComponent->numOutport; ++i) {
			outPortParamsPtr = m_nfComponent->outPortParams + i;
			outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			outPortParamsPtr->nBufferSize = ((UTIL_ALIGN(m_outWidth, 32))
					* ((UTIL_ALIGN(m_outHeight, 32))) * 3) >> 1;
			pipe((int *) outPortParamsPtr->opBufPipe);
		}
		pipe((int*) m_nfComponent->localPipe);
	}


	return true;
}

bool MixtureMode_SDI::allocInOutUpperHalfParams() {
	calcScalerWidthHeight();
	InportParams *inPortParamsPtr;
	OutportParams *outPortParamsPtr;

	OMXComponentUtils::allocInOutParams(m_sdiDEIComponent);
	OMXComponentUtils::allocInOutParams(m_sdiScaleComponent);
	OMXComponentUtils::allocInOutParams(m_sdiNfComponent);
	OMXComponentUtils::allocInOutParams(m_sdiEncComponent);

	OMXComponentUtils::allocInOutParams(m_vgaScaleComponent);

	if (m_pSetting->getVGAEncodeEnable()) {
		OMXComponentUtils::allocInOutParams(m_vgaEncComponent);
	}

	for (OMX_U32 j = 0; j < m_sdiDEIComponent->numInport; ++j) {
		inPortParamsPtr = m_sdiDEIComponent->inPortParams + j;
		inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum; //m_pSetting->getBufferCountActual()
		inPortParamsPtr->nBufferSize = (UTIL_ALIGN(HD_WIDTH, 32)) * (UTIL_ALIGN(HD_HEIGHT, 32)) * 2;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}

	for (OMX_U32 j = 0; j < m_sdiDEIComponent->numOutport; ++j) {
		outPortParamsPtr = m_sdiDEIComponent->outPortParams + j;
		outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		if (0 == j % 2) {
			outPortParamsPtr->nBufferSize = (UTIL_ALIGN(HD_WIDTH, 32))
						* (UTIL_ALIGN(HD_HEIGHT, 32)) * 2;
		} else {
			outPortParamsPtr->nBufferSize = (UTIL_ALIGN(HD_WIDTH, 32)) * (UTIL_ALIGN(HD_HEIGHT, 32)) * 3/2;
		}

		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int *) m_sdiDEIComponent->localPipe);

	for (OMX_U32 j = 0; j < m_sdiScaleComponent->numInport; ++j) {
		inPortParamsPtr = m_sdiScaleComponent->inPortParams + j;
		inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		inPortParamsPtr->nBufferSize = (UTIL_ALIGN(HD_WIDTH, 32)) * (UTIL_ALIGN(HD_HEIGHT, 32)) * 3/2;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (OMX_U32 j = 0; j < m_sdiScaleComponent->numOutport; ++j) {
		outPortParamsPtr = m_sdiScaleComponent->outPortParams + j;
		outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		outPortParamsPtr->nBufferSize = m_outWidth * m_outHeight * 2;
		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int *) m_sdiScaleComponent->localPipe);

	for (int i = 0; i < m_sdiNfComponent->numInport; ++i) {
		inPortParamsPtr = m_sdiNfComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		inPortParamsPtr->nBufferSize = (UTIL_ALIGN(HD_WIDTH, 32)) * (UTIL_ALIGN(HD_HEIGHT, 32)) * 2;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (int i = 0; i < m_sdiNfComponent->numOutport; ++i) {
		outPortParamsPtr = m_sdiNfComponent->outPortParams + i;
		outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		outPortParamsPtr->nBufferSize =(UTIL_ALIGN(KTeacherLiveWidth, 32)) * (UTIL_ALIGN(KTeacherLiveHeight, 32)) * 3 /2;
		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int*)m_sdiNfComponent->localPipe);

	for (int i = 0; i < m_sdiEncComponent->numInport; ++i) {
		inPortParamsPtr = m_sdiEncComponent->inPortParams + i;
		inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		inPortParamsPtr->nBufferSize = (UTIL_ALIGN(KTeacherLiveWidth, 32)) * (UTIL_ALIGN(KTeacherLiveHeight, 32)) * 3/ 2;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (int i = 0; i < m_sdiEncComponent->numOutport; ++i) {
		outPortParamsPtr = m_sdiEncComponent->outPortParams + i;
		outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		outPortParamsPtr->nBufferSize = (UTIL_ALIGN(KTeacherLiveWidth, 32)) * (UTIL_ALIGN(KTeacherLiveHeight, 32)) * 3 /2;
		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int*)m_sdiEncComponent->localPipe);

	for (OMX_U32 j = 0; j < m_vgaScaleComponent->numInport; ++j) {
		inPortParamsPtr = m_vgaScaleComponent->inPortParams + j;
		inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		inPortParamsPtr->nBufferSize = (HD_WIDTH * HD_HEIGHT * 3) >> 1;
		pipe((int *) inPortParamsPtr->ipBufPipe);
	}
	for (OMX_U32 j = 0; j < m_vgaScaleComponent->numOutport; ++j) {
		outPortParamsPtr = m_vgaScaleComponent->outPortParams + j;
		outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
		outPortParamsPtr->nBufferSize = m_outWidth * m_outHeight * 2;

		pipe((int *) outPortParamsPtr->opBufPipe);
	}
	pipe((int *) m_vgaScaleComponent->localPipe);


	if (m_pSetting->getVGAEncodeEnable()) {
		for (OMX_U32 i = 0; i < m_vgaEncComponent->numInport; ++i) {
			inPortParamsPtr = m_vgaEncComponent->inPortParams + i;
			inPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			inPortParamsPtr->nBufferSize = (UTIL_ALIGN(1280, 32))
					* (UTIL_ALIGN(1024, 32)) * 3 / 2;
			pipe((int*) inPortParamsPtr->ipBufPipe);
		}

		for (OMX_U32 i = 0; i < m_vgaEncComponent->numOutport; ++i) {
			outPortParamsPtr = m_vgaEncComponent->outPortParams + i;
			outPortParamsPtr->nBufferCountActual = KOtherComponetInOutPutNum;
			outPortParamsPtr->nBufferSize = ((UTIL_ALIGN(1280, 32))
					* ((UTIL_ALIGN(1024, 32))) * 3) >> 1;
			pipe((int *) outPortParamsPtr->opBufPipe);
		}
		pipe((int*) m_vgaEncComponent->localPipe);
	}

	return true;
}

bool MixtureMode_SDI::freeInOutLowerHalfParams() {
	if (m_pSetting->getLiveEncEnable()) {
		OMXComponentUtils::freeInOutParams(m_deiComponent);
		OMXComponentUtils::freeInOutParams(m_liveNfComponent);
		OMXComponentUtils::freeInOutParams(m_liveEncComponent);
	} else {
		OMXComponentUtils::freeInOutParams(m_nfComponent);
	}
	OMXComponentUtils::freeInOutParams(m_encComponent);
	OMXComponentUtils::freeInOutParams(m_headTailScaleComponent);
	return true;
}

bool MixtureMode_SDI::freeInOutUpperHalfParams() {
	OMXComponentUtils::freeInOutParams(m_sdiEncComponent);
	printf("after free sdi enc \n");
	fflush(stdout);

	OMXComponentUtils::freeInOutParams(m_sdiNfComponent);
	printf("after free sdi nf \n");
	fflush(stdout);

	OMXComponentUtils::freeInOutParams(m_sdiDEIComponent);
	printf("after free sdi dei \n");
	fflush(stdout);

	printf("before free sdi scale \n");
	fflush(stdout);
	OMXComponentUtils::freeInOutParams(m_sdiScaleComponent);
	printf("after free sdi scale \n");
	fflush(stdout);



	return true;
}

bool MixtureMode_SDI::initLowerHalf() {
	if (!initLowerHalfOMXHandle() ) {
		return false;
	}

	return startLowerHalfOMXThread();
}

bool MixtureMode_SDI::initUpperHalf() {
	if (!initUpperHalfOMXHandle() ) {
		return false;
	}
	return startUpperHalfOMXThread();
}

bool MixtureMode_SDI::initLowerHalfOMXHandle() {
	OMX_ERRORTYPE eError;
	if (!initHeadTailScaleOMXHandle() ) {
		return false;
	}
	printf("init head tail scale omx handle ok\n");

	if (m_pSetting->getLiveEncEnable()) {
		if (!initDeiOMXHandle() ) {
			printf("init live scale omx handle failed\n");
			return false;
		}

		if (!initLiveNfOMXHandle() ) {
			printf("init live nf omx handle failed\n");
			return false;
		}

		if (!initLiveEncodeOMXHandle() ){
			printf("init live encode omx handle failed\n");
			return false;
		}
		printf("init live encode omx handle ok\n");
	} else {
		if (!initNfOMXHandle()) {
			printf("init nf OMX handle failed\n");
			return false;
		}
	}

	if (!initEncodeOMXHandle()) {
		printf("init encode OMX handle failed\n");
		return false;
	}
	printf("init encode OMX handle OK\n");
	establishLowerHalfConnections();

	eError = OMX_SendCommand (m_headTailScaleComponent->handle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	for (OMX_U32 j = 0; j < m_headTailScaleComponent->inPortParams->nBufferCountActual; j++) {
		eError = OMX_AllocateBuffer (m_pHeadTailScaleHandle,
				&m_headTailScaleComponent->inPortParams->pInBuff[j],
				OMX_VFPC_INPUT_PORT_START_INDEX,
				this,
				m_headTailScaleComponent->inPortParams->nBufferSize);

		if (eError != OMX_ErrorNone) {
			printf("can not use buffer index %d ,error %x\n", j, eError);
			return false;
		}

		OMX_PTR phyAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy ((uint32_t)m_headTailScaleComponent->inPortParams->pInBuff[j]->pBuffer);
		m_pKernel->getEdmaInstance()->copy((unsigned char*)phyAddr, (unsigned char*)gheadImagePhyAddr,
				KHeadPicWidth * 2, KHeadPicWidth * 2 , KHeadPicWidth * KHeadPicHeight * 2, 1);
	}
	printf("head tail scale input port use buffer done \n ");
	for (OMX_U32 j = 0; j < m_headTailScaleComponent->outPortParams->nBufferCountActual; j++) {
		m_pHeapBuffer = Memory_alloc (m_heap, m_headTailScaleComponent->outPortParams->nBufferSize, 128, NULL);
		if (m_pHeapBuffer == NULL) {
			printf ("Buffer Allocation Failed \n");
			return false;
		}
		eError = OMX_UseBuffer (m_pHeadTailScaleHandle,
            &m_headTailScaleComponent->outPortParams->pOutBuff[j],
             OMX_VFPC_OUTPUT_PORT_START_INDEX, this,
             m_headTailScaleComponent->outPortParams->nBufferSize,
              (OMX_U8*)m_pHeapBuffer);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	printf("head tail scalar output port alloc buffer done\n");

	OMXComponentUtils::waitStateSem(m_headTailScaleComponent);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	if (m_pSetting->getLiveEncEnable() ) {
		eError = OMX_SendCommand(m_pDeiHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (OMX_ErrorNone != eError) {
			printf("set live scale handle idle error %x\n", eError);
			return false;
		}
		for (OMX_U32 i = 0; i < m_deiComponent->inPortParams->nBufferCountActual; ++i) {
			eError = OMX_AllocateBuffer (m_pDeiHandle,
					&m_deiComponent->inPortParams->pInBuff[i],
					OMX_VFPC_INPUT_PORT_START_INDEX,
					this,
					m_deiComponent->inPortParams->nBufferSize);

			if (eError != OMX_ErrorNone) {
				printf("can not use buffer index %d ,error %x\n", i, eError);
				return false;
			}
		}
		for (OMX_U32 i = 0; i < m_deiComponent->numOutport; ++i) {
			OutportParams *outPortParamsPtr = m_deiComponent->outPortParams + i;
			for (int j = 0; j < outPortParamsPtr->nBufferCountActual; ++j) {
				m_pHeapBuffer = Memory_alloc(m_heap, outPortParamsPtr->nBufferSize, 128, NULL);
				Memory_set(m_pHeapBuffer, 0, outPortParamsPtr->nBufferSize);
				if (NULL == m_pHeapBuffer) {
					printf("Buffer alloc failed\n");
					return false;
				}
				eError = OMX_UseBuffer(m_pDeiHandle,
						&outPortParamsPtr->pOutBuff[j],
						OMX_VFPC_OUTPUT_PORT_START_INDEX + i, this,
						outPortParamsPtr->nBufferSize,
						(OMX_U8*)m_pHeapBuffer);
				if (OMX_ErrorNone != eError) {
					printf("live scale out port use buffer error %x\n", eError);
				}
			}
		}
		printf("before wait live scale component sem\n");
		OMXComponentUtils::waitStateSem(m_deiComponent);
		printf("after wait live scale component sem\n");


		eError = OMX_SendCommand (m_pLiveNfHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (eError != OMX_ErrorNone) {
			printf("set live nf handle to idle failed %x\n", eError);
			return false;
		}

		OutportParams *outPortParamsPtr = m_deiComponent->outPortParams;
		for (OMX_U32 i = 0; i < m_liveNfComponent->inPortParams->nBufferCountActual; i++) {
			eError = OMX_UseBuffer (m_pLiveNfHandle,
						&m_liveNfComponent->inPortParams->pInBuff[i],
						OMX_VFPC_INPUT_PORT_START_INDEX,
						this,
						outPortParamsPtr->nBufferSize,
						outPortParamsPtr->pOutBuff[i]->pBuffer);

			if (eError != OMX_ErrorNone) {
				printf("live nf use buffer failed %x\n", eError);
				return false;
			}
		}
		printf(" Noise Filter input port use buffer done \n ");

		for (OMX_U32 i = 0; i < m_liveNfComponent->numOutport; ++i) {
			OutportParams *outPortParamsPtr = m_liveNfComponent->outPortParams + i;
			for (OMX_U32 j = 0; j < outPortParamsPtr->nBufferCountActual; j++) {
				m_pHeapBuffer = Memory_alloc (m_heap, outPortParamsPtr->nBufferSize, 128, NULL);
				Memory_set(m_pHeapBuffer, 0, outPortParamsPtr->nBufferSize);
				if (m_pHeapBuffer == NULL) {
					printf ("Buffer Allocation Failed \n");
					return false;
				}
				eError = OMX_UseBuffer (m_pLiveNfHandle,
			         &outPortParamsPtr->pOutBuff[j],
			         OMX_VFPC_OUTPUT_PORT_START_INDEX + i, this,
			         outPortParamsPtr->nBufferSize,
			         (OMX_U8*)m_pHeapBuffer);
				if (eError != OMX_ErrorNone) {
					printf("live nf use heap buffer error %x \n", eError);
					return false;
				}
			}
		}
		printf(" Noise Filter outport buffers allocated \n ");
		OMXComponentUtils::waitStateSem(m_liveNfComponent);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		printf(" Noise Filter state IDLE \n ");

		eError = OMX_SendCommand (m_pLiveEncHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		outPortParamsPtr = m_liveNfComponent->outPortParams;
		for (OMX_U32 i = 0; i < m_liveEncComponent->inPortParams->nBufferCountActual; ++i) {
			eError = OMX_UseBuffer (m_pLiveEncHandle,
					&m_liveEncComponent->inPortParams->pInBuff[i],
					OMX_VIDENC_INPUT_PORT,
					this,
					outPortParamsPtr->nBufferSize,
					outPortParamsPtr->pOutBuff[i]->pBuffer);

			if (eError != OMX_ErrorNone) {
				return false;
			}
		}
		printf("live encoder input port use buffer done \n ");

		for (OMX_U32 i = 0; i < m_liveEncComponent->outPortParams->nBufferCountActual; ++i) {
			eError = OMX_AllocateBuffer (m_pLiveEncHandle,
						&m_liveEncComponent->outPortParams->pOutBuff[i],
						OMX_VIDENC_OUTPUT_PORT, this,
						m_liveEncComponent->outPortParams->nBufferSize);
			if (eError != OMX_ErrorNone) {
				return false;
			}
		}
		printf("live encoder outport buffers allocated \n ");
		OMXComponentUtils::waitStateSem(m_liveEncComponent);
		printf("get live enc componet state sem\n");
	} else {
		eError = OMX_SendCommand (m_pNfHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		for (OMX_U32 i = 0; i < m_nfComponent->inPortParams->nBufferCountActual; i++) {
			eError = OMX_AllocateBuffer (m_pNfHandle,
					&m_nfComponent->inPortParams->pInBuff[i],
					OMX_VFPC_INPUT_PORT_START_INDEX,
					this,
					m_nfComponent->inPortParams->nBufferSize);

			if (eError != OMX_ErrorNone) {
				printf("can not use buffer index %d ,error %x\n", i, eError);
				return false;
			}
		}
		printf(" Noise Filter input port use buffer done \n ");

		for (OMX_U32 i = 0; i < m_nfComponent->numOutport; ++i) {
			OutportParams *outPortParamsPtr = m_nfComponent->outPortParams + i;
			for (OMX_U32 j = 0; j < outPortParamsPtr->nBufferCountActual; j++) {
					m_pHeapBuffer = Memory_alloc (m_heap, outPortParamsPtr->nBufferSize, 128, NULL);
					if (m_pHeapBuffer == NULL) {
						printf ("Buffer Allocation Failed \n");
						return false;
					}
					Memory_set(m_pHeapBuffer, 0, outPortParamsPtr->nBufferSize);
					eError = OMX_UseBuffer (m_pNfHandle,
		                    &outPortParamsPtr->pOutBuff[j],
		                    OMX_VFPC_OUTPUT_PORT_START_INDEX + i, this,
		                    outPortParamsPtr->nBufferSize,
		                    (OMX_U8*)m_pHeapBuffer);
					if (eError != OMX_ErrorNone) {
						printf("nf use heap buffer error %x \n", eError);
						return false;
					}

			}
		}
		printf(" Noise Filter outport buffers allocated \n ");
		OMXComponentUtils::waitStateSem(m_nfComponent);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		printf(" Noise Filter state IDLE \n ");
	}


	eError = OMX_SendCommand (m_pEncHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	for (OMX_U32 i = 0; i < m_encComponent->inPortParams->nBufferCountActual; ++i) {
		OutportParams* outPortParamsPtr;
		if (m_pSetting->getLiveEncEnable() ) {
			outPortParamsPtr = m_deiComponent->outPortParams + 1;
		} else {
			outPortParamsPtr = m_nfComponent->outPortParams;
		}

		eError = OMX_UseBuffer (m_pEncHandle,
				&m_encComponent->inPortParams->pInBuff[i],
				OMX_VIDENC_INPUT_PORT,
				this,
				outPortParamsPtr->nBufferSize,
				outPortParamsPtr->pOutBuff[i]->pBuffer);

		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	printf(" encoder input port use buffer done \n ");

	for (OMX_U32 i = 0; i < m_encComponent->outPortParams->nBufferCountActual;
			++i) {
		eError = OMX_AllocateBuffer (m_pEncHandle,
				&m_encComponent->outPortParams->pOutBuff[i],
				OMX_VIDENC_OUTPUT_PORT, this,
				m_encComponent->outPortParams->nBufferSize);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}

	printf(" encoder outport buffers allocated \n ");
	OMXComponentUtils::waitStateSem(m_encComponent);

	printf(" Encoder state IDLE \n ");



	eError = OMX_SendCommand(m_pEncHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMXComponentUtils::waitStateSem(m_encComponent);

	printf(" encoder state execute \n ");

	if (m_pSetting->getLiveEncEnable() ) {
		eError = OMX_SendCommand(m_pDeiHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		OMXComponentUtils::waitStateSem(m_deiComponent);

		eError = OMX_SendCommand(m_pLiveNfHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		OMXComponentUtils::waitStateSem(m_liveNfComponent);

		eError = OMX_SendCommand(m_pLiveEncHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}

		OMXComponentUtils::waitStateSem(m_liveEncComponent);
	} else {
		eError = OMX_SendCommand(m_pNfHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}

		OMXComponentUtils::waitStateSem(m_nfComponent);

		printf(" nf  state execute \n ");
	}

	eError = OMX_SendCommand(m_pHeadTailScaleHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_headTailScaleComponent);

	return true;
}


bool MixtureMode_SDI::initUpperHalfOMXHandle() {
	OMX_ERRORTYPE eError;

	if (!initSDIDEIOMXHandle()) {
		return false;
	}
	printf("init sdi nf omx handle ok\n");

	if (!initSDIScaleOMXHandle()) {
		return false;
	}
	printf("init Scale OMX Handle OK\n");

	if (!initSDINfOMXHandle()) {
		return false;
	}
	printf("init SDI Nf OMX Handle OK\n");

	if (!initSDIEncOMXHandle()) {
		return false;
	}
	printf("init SDI Enc OMX Handle ok\n");

	establistUpperHalfConnections();
	printf("end establish up Connections \n");

	eError = OMX_SendCommand(m_pSDIDEIHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (OMX_ErrorNone != eError) {
		printf("set live scale handle idle error %x\n", eError);
		return false;
	}
	for (int i = 0; i < m_sdiDEIComponent->numInport; ++i) {
		InportParams* inPortParamsPtr = m_sdiDEIComponent->inPortParams + i;
		for (OMX_U32 j = 0; j < inPortParamsPtr->nBufferCountActual; ++j) {
			eError = OMX_AllocateBuffer (m_pSDIDEIHandle,
					&inPortParamsPtr->pInBuff[j],
					OMX_VFPC_INPUT_PORT_START_INDEX + i,
					this,
					inPortParamsPtr->nBufferSize);

			if (eError != OMX_ErrorNone) {
				printf("can not use buffer index %d ,error %x\n", i, eError);
				return false;
			}
		}
	}
	for (OMX_U32 j = 0; j < m_sdiDEIComponent->numOutport; ++j) {
		OutportParams *outPortParamsPtr = m_sdiDEIComponent->outPortParams + j;
		for (int k = 0; k < outPortParamsPtr->nBufferCountActual; ++k) {
			m_pHeapBuffer = Memory_alloc(m_heap, outPortParamsPtr->nBufferSize, 128, NULL);
			Memory_set(m_pHeapBuffer, 0, outPortParamsPtr->nBufferSize);
			if (NULL == m_pHeapBuffer) {
				printf("Buffer alloc failed\n");
				return false;
			}
			eError = OMX_UseBuffer(m_pSDIDEIHandle,
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
	OMXComponentUtils::waitStateSem(m_sdiDEIComponent);
	printf("after wait sdi dei component sem\n");


	eError = OMX_SendCommand (m_sdiScaleComponent->handle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	for (int i = 0 ; i < m_sdiScaleComponent->numInport; ++i) {
		OutportParams *outPortParamsPtr = m_sdiDEIComponent->outPortParams + 1 + i * 2;
		InportParams* inPortParamsPtr = m_sdiScaleComponent->inPortParams + i;
		for (OMX_U32 j = 0; j < inPortParamsPtr->nBufferCountActual; j++) {
			eError = OMX_UseBuffer (m_pSDIScaleHandle,
					&inPortParamsPtr->pInBuff[j],
					OMX_VFPC_INPUT_PORT_START_INDEX + i,
					this,
					outPortParamsPtr->nBufferSize,
					outPortParamsPtr->pOutBuff[j]->pBuffer);

			if (eError != OMX_ErrorNone) {
				return false;
			}
		}
		printf(" Scalar input port use buffer done \n ");

		outPortParamsPtr = m_sdiScaleComponent->outPortParams + i;
		for (OMX_U32 j = 0; j < outPortParamsPtr->nBufferCountActual; j++) {
			m_pHeapBuffer = Memory_alloc (m_heap, outPortParamsPtr->nBufferSize, 128, NULL);
			if (m_pHeapBuffer == NULL) {
				printf ("Buffer Allocation Failed \n");
				return false;
			}
			eError = OMX_UseBuffer (m_pSDIScaleHandle,
	                &outPortParamsPtr->pOutBuff[j],
	                OMX_VFPC_OUTPUT_PORT_START_INDEX + i, this,
	                outPortParamsPtr->nBufferSize,
	                (OMX_U8*)m_pHeapBuffer);
			if (eError != OMX_ErrorNone) {
				printf("dec use heap buffer error %x \n", eError);
				return false;
			}

		}
	}

	printf(" scalar outport buffers allocated \n ");
	OMXComponentUtils::waitStateSem(m_sdiScaleComponent);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	printf(" scalar state IDLE \n ");

	eError = OMX_SendCommand (m_sdiNfComponent->handle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	for (OMX_U32 j = 0; j < m_sdiNfComponent->inPortParams->nBufferCountActual; j++) {
		eError = OMX_UseBuffer (m_pSDINfHandle,
				&m_sdiNfComponent->inPortParams->pInBuff[j],
				OMX_VFPC_INPUT_PORT_START_INDEX,
				this,
				m_sdiDEIComponent->outPortParams->nBufferSize,
				m_sdiDEIComponent->outPortParams->pOutBuff[j]->pBuffer);

		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	printf(" sdi nf input port use buffer done \n ");

	for (OMX_U32 j = 0; j < m_sdiNfComponent->outPortParams->nBufferCountActual; j++) {
		m_pHeapBuffer = Memory_alloc (m_heap, m_sdiNfComponent->outPortParams->nBufferSize, 128, NULL);
		if (m_pHeapBuffer == NULL) {
			printf ("Buffer Allocation Failed \n");
			return false;
		}
		eError = OMX_UseBuffer (m_pSDINfHandle,
                &m_sdiNfComponent->outPortParams->pOutBuff[j],
                OMX_VFPC_OUTPUT_PORT_START_INDEX, this,
                m_sdiNfComponent->outPortParams->nBufferSize,
                (OMX_U8*)m_pHeapBuffer);
		if (eError != OMX_ErrorNone) {
			printf("dec use heap buffer error %x \n", eError);
			return false;
		}
	}

	OMXComponentUtils::waitStateSem(m_sdiNfComponent);

	eError = OMX_SendCommand (m_sdiEncComponent->handle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	for (OMX_U32 j = 0; j < m_sdiEncComponent->inPortParams->nBufferCountActual; j++) {
		eError = OMX_UseBuffer (m_pSDIEncHandle,
				&m_sdiEncComponent->inPortParams->pInBuff[j],
				OMX_VIDENC_INPUT_PORT,
				this,
				m_sdiNfComponent->outPortParams->nBufferSize,
				m_sdiNfComponent->outPortParams->pOutBuff[j]->pBuffer);

		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	printf(" sdi enc input port use buffer done \n ");

	for (OMX_U32 i = 0; i < m_sdiEncComponent->outPortParams->nBufferCountActual; i++) {
		eError = OMX_AllocateBuffer (m_pSDIEncHandle,
				&m_sdiEncComponent->outPortParams->pOutBuff[i],
				OMX_VIDENC_OUTPUT_PORT, this,
				m_sdiEncComponent->outPortParams->nBufferSize);
		if (eError != OMX_ErrorNone) {
			return false;
		}
	}
	printf("before get sdi enc sem\n");
	OMXComponentUtils::waitStateSem(m_sdiEncComponent);
	printf("after get sdi enc sem\n");

	eError = OMX_SendCommand(m_pSDIEncHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (OMX_ErrorNone != eError) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_sdiEncComponent);

	eError = OMX_SendCommand(m_pSDINfHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (OMX_ErrorNone != eError) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_sdiNfComponent);

	eError = OMX_SendCommand(m_pSDIScaleHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMXComponentUtils::waitStateSem(m_sdiScaleComponent);

	printf(" sdi scale state execute \n ");

	eError = OMX_SendCommand(m_pSDIDEIHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	OMXComponentUtils::waitStateSem(m_sdiDEIComponent);
	printf("sdi enc state execute \n ");



	return true;
}


bool MixtureMode_SDI::establishLowerHalfConnections() {
	if (m_pSetting->getLiveEncEnable()) {
		OMXComponentUtils::connectComponents(m_deiComponent,
				OMX_VFPC_OUTPUT_PORT_START_INDEX, m_liveNfComponent,
				OMX_VFPC_INPUT_PORT_START_INDEX);

		OMXComponentUtils::connectComponents(m_liveNfComponent,
				OMX_VFPC_OUTPUT_PORT_START_INDEX, m_liveEncComponent,
				OMX_VIDENC_INPUT_PORT);

		OMXComponentUtils::connectComponents(m_deiComponent,
				OMX_VFPC_OUTPUT_PORT_START_INDEX  + 1, m_encComponent,
				OMX_VFPC_INPUT_PORT_START_INDEX);
	} else {
		OMXComponentUtils::connectComponents(m_nfComponent,
				OMX_VFPC_OUTPUT_PORT_START_INDEX, m_encComponent,
				OMX_VIDENC_INPUT_PORT);
	}

	return true;
}

bool MixtureMode_SDI::establistUpperHalfConnections() {
	for (OMX_U32 i = 0; i < m_maxDecode; i++) {
		OMXComponentUtils::connectComponents(m_sdiDEIComponent,
				OMX_VFPC_OUTPUT_PORT_START_INDEX + 1 + i * 2, m_sdiScaleComponent,
				OMX_VFPC_INPUT_PORT_START_INDEX + i);
	}
	OMXComponentUtils::connectComponents(m_sdiDEIComponent,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, m_sdiNfComponent,
			OMX_VFPC_INPUT_PORT_START_INDEX);
	OMXComponentUtils::connectComponents(m_sdiNfComponent,
			OMX_VFPC_OUTPUT_PORT_START_INDEX, m_sdiEncComponent,
			OMX_VIDENC_INPUT_PORT);
	return true;
}

bool MixtureMode_SDI::startLowerHalfOMXThread() {
	pthread_attr_init(&m_encComponent->ThreadAttr);
	m_encParams.pParent = this;
	m_encParams.pComponent = m_encComponent;
	OMXComponentUtils::initialOutputResources(m_encComponent);
	if (0 != pthread_create(&m_encComponent->outDataStrmThrdId,
				&m_encComponent->ThreadAttr,
				OutputBitStreamWriteTask,
				&m_encParams)) {
		printf("Create_Task failed !");
		return false;
	}
	printf(" file write thread created \n ");

	pthread_attr_init(&m_encComponent->ThreadAttr);
	if (0 != pthread_create(&m_encComponent->connDataStrmThrdId,
				&m_encComponent->ThreadAttr,
				ConnInConnOutTask, &m_encParams)) {
		printf("Create_Task failed !");
		return false;
	}
	printf(" encode connect thread created \n ");

	if (m_pSetting->getLiveEncEnable() ) {
		pthread_attr_init(&m_liveEncComponent->ThreadAttr);
		m_liveEncParams.pParent = this;
		m_liveEncParams.pComponent = m_liveEncComponent;
		OMXComponentUtils::initialOutputResources(m_liveEncComponent);
		if (0 != pthread_create(&m_liveEncComponent->outDataStrmThrdId,
					&m_liveEncComponent->ThreadAttr,
					LiveOutputBitStreamWriteTask,
					&m_liveEncParams)) {
			printf("Create_Task failed !");
			return false;
		}
		printf("live file write thread created \n ");

		pthread_attr_init(&m_liveEncComponent->ThreadAttr);
		if (0 != pthread_create(&m_liveEncComponent->connDataStrmThrdId,
					&m_liveEncComponent->ThreadAttr,
					ConnInConnOutTask, &m_liveEncParams)) {
			printf("Create_Task failed !");
			return false;
		}
		printf("live encode connect thread created \n ");

		OMXComponentUtils::initialOutputResources(m_liveNfComponent);
		m_liveNfParams.pParent = this;
		m_liveNfParams.pComponent = m_liveNfComponent;
		pthread_attr_init(&m_liveNfComponent->ThreadAttr);
		if (0 != pthread_create(&m_liveNfComponent->connDataStrmThrdId,
				&m_liveNfComponent->ThreadAttr,
				ConnInConnOutTask,
				&m_liveNfParams)) {
				printf("Create_Task failed !");
			return false;
		}

		pthread_attr_init(&m_deiComponent->ThreadAttr);
		m_deiParams.pParent = this;
		m_deiParams.pComponent = m_deiComponent;
		if (0 != pthread_create(&m_deiComponent->inDataStrmThrdId,
				&m_deiComponent->ThreadAttr,
				InputMixStreamReadTask,
				&m_deiParams)) {
			printf("create thread failed\n");
			return false;
		}

		pthread_attr_init(&m_deiComponent->ThreadAttr);
		OMXComponentUtils::initialOutputResources(m_deiComponent);

		if (0 != pthread_create(&m_deiComponent->connDataStrmThrdId,
				&m_deiComponent->ThreadAttr,
				ConnInConnOutTask,
				&m_deiParams)) {
			printf("Create_Task failed !");
			return false;
		}
	} else {
		pthread_attr_init(&m_nfComponent->ThreadAttr);
		m_nfParams.pParent = this;
		m_nfParams.pComponent = m_nfComponent;
		if (0 != pthread_create(&m_nfComponent->inDataStrmThrdId,
				&m_nfComponent->ThreadAttr,
				InputMixStreamReadTask,
				&m_nfParams)) {
			printf("create thread failed\n");
			return false;
		}

		pthread_attr_init(&m_nfComponent->ThreadAttr);
		OMXComponentUtils::initialOutputResources(m_nfComponent);
		if (0 != pthread_create(&m_nfComponent->connDataStrmThrdId,
				&m_nfComponent->ThreadAttr,
				ConnInConnOutTask, &m_nfParams)) {
			printf("Create_Task failed !");
			return false;
		}
	}

	printf("before create head tail in thread\n");
	pthread_attr_init(&m_headTailScaleComponent->ThreadAttr);
	if (0 != pthread_create(&m_headTailScaleComponent->inDataStrmThrdId,
				&m_headTailScaleComponent->ThreadAttr,
				HeadTailScaleInputBitStreamReadTask,
				this)) {
		printf("Create_Task failed !");
		return false;
	}
	printf("after create head tail in thread\n");

	pthread_attr_init(&m_headTailScaleComponent->ThreadAttr);
	OMXComponentUtils::initialOutputResources(m_headTailScaleComponent);
	if (0 != pthread_create(&m_headTailScaleComponent->outDataStrmThrdId,
			&m_headTailScaleComponent->ThreadAttr,
			HeadTailScaleWriteTask,
			this)) {
		return false;
	}
	printf("start head tail scale component thread\n");

	return true;
}

bool MixtureMode_SDI::startUpperHalfOMXThread() {

	pthread_attr_init(&m_sdiDEIComponent->ThreadAttr);

	OMXComponentUtils::initialOutputResources(m_sdiDEIComponent);
	m_sdiDEIParams.pParent = this;
	m_sdiDEIParams.pComponent = m_sdiDEIComponent;
	if (0 != pthread_create(&m_sdiDEIComponent->connDataStrmThrdId,
			&m_sdiDEIComponent->ThreadAttr,
			ConnInConnOutTask,
			&m_sdiDEIParams)) {
		printf("Create_Task failed !");
		return false;
	}
	printf(" decoder connect thread created \n ");

	pthread_attr_init(&m_sdiScaleComponent->ThreadAttr);
	m_sdiScaleParams.pParent = this;
	m_sdiScaleParams.pComponent = m_sdiScaleComponent;
	OMXComponentUtils::initialOutputResources(m_sdiScaleComponent);
	if (0 != pthread_create(&m_sdiScaleComponent->connDataStrmThrdId,
			&m_sdiScaleComponent->ThreadAttr,
			ConnInConnOutTask,
			&m_sdiScaleParams)) {
		printf("create task failed\n");
		return false;
	}

	pthread_attr_init(&m_sdiScaleComponent->ThreadAttr);
	if (0 != pthread_create(&m_sdiScaleComponent->outDataThreadId[0],
				&m_sdiScaleComponent->ThreadAttr,
				SDIScaleOutputChannel1,
				&m_sdiScaleParams)) {
		printf("Create_Task failed !");
		return false;
	}

	pthread_attr_init(&m_sdiScaleComponent->ThreadAttr);
	if (0 != pthread_create(&m_sdiScaleComponent->outDataStrmThrdId,
			&m_sdiScaleComponent->ThreadAttr,
			SDIScaleOutputChannel2,
			&m_sdiScaleParams)) {
		printf("create sdi scale output channel 2 failed\n");
		return false;
	}

	printf(" scalar connect thread created \n ");

	pthread_attr_init(&m_sdiEncComponent->ThreadAttr);
	m_sdiEncParams.pParent = this;
	m_sdiEncParams.pComponent = m_sdiEncComponent;
	OMXComponentUtils::initialOutputResources(m_sdiEncComponent);
	if (0 != pthread_create(&m_sdiEncComponent->outDataStrmThrdId,
				&m_sdiEncComponent->ThreadAttr,
				SDIEncOutputWriteTask,
				&m_sdiEncParams)) {
		printf("Create_Task failed !");
		return false;
	}
	printf("sdi enc write thread created \n ");

	pthread_attr_init(&m_sdiEncComponent->ThreadAttr);
	if (0 != pthread_create(&m_sdiEncComponent->connDataStrmThrdId,
				&m_sdiEncComponent->ThreadAttr,
				ConnInConnOutTask, &m_sdiEncParams)) {
		printf("Create_Task failed !");
		return false;
	}
	printf("sdi encode connect thread created \n ");

	OMXComponentUtils::initialOutputResources(m_sdiNfComponent);
	m_sdiNfParams.pParent = this;
	m_sdiNfParams.pComponent = m_sdiNfComponent;
	pthread_attr_init(&m_sdiNfComponent->ThreadAttr);
	if (0 != pthread_create(&m_sdiNfComponent->connDataStrmThrdId,
			&m_sdiNfComponent->ThreadAttr,
			ConnInConnOutTask,
			&m_sdiNfParams)) {
			printf("Create_Task failed !");
		return false;
	}


	startVGAComponentThread();

	m_bThreadsStart = true;
	return true;
}

bool MixtureMode_SDI::stopOMXThread() {
	PipeMsg pipeMsg;
	OMX_ERRORTYPE eError;
	OMX_S32 ret_value;
	if (m_bPaused) {
		changeToRunning(false);
	}

	printf("begin stop omx thread\n");
	stopVGAComponentThread();
	printf("after stop vga component thread\n");
	fflush(stdout);

	m_bExitMixtureThread = true;
	if (m_pSetting->getLiveEncEnable() ) {
		SempUtils::semp_pend(m_deiComponent->eos_sem);
	} else {
		SempUtils::semp_pend(m_nfComponent->eos_sem);
	}

	m_bExitHeadTailScaleThread = true;
	SempUtils::semp_pend(m_headTailScaleComponent->eos_sem);
	SempUtils::semp_pend(m_headTailScaleComponent->eos_sem);

	pipeMsg.cmd = ePipeCmdExit;

	if (m_pSetting->getLiveEncEnable() ) {
		write(m_deiComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
		write(m_liveNfComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
		write(m_liveEncComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg) );
	}

	if (!m_pSetting->getLiveEncEnable() ) {
		write(m_nfComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
	}
	write(m_encComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));

	write(m_sdiDEIComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
	write(m_sdiScaleComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
	write(m_sdiNfComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));
	write(m_sdiEncComponent->localPipe[1], &pipeMsg, sizeof(PipeMsg));

	m_bExit = true;
	//wait for out thread exit
	SempUtils::semp_pend(m_encComponent->eos_sem);
	SempUtils::semp_pend(m_sdiEncComponent->eos_sem);

	if (m_pSetting->getLiveEncEnable() ) {
		SempUtils::semp_pend(m_liveEncComponent->eos_sem);
	}

	printf("before get sdi scale eos sem\n");
	write(m_sdiScaleComponent->outPortParams[0].opBufPipe[1], &pipeMsg, sizeof(PipeMsg));
	SempUtils::semp_pend(m_sdiScaleComponent->eos_sem);
	write(m_sdiScaleComponent->outPortParams[1].opBufPipe[1], &pipeMsg, sizeof(PipeMsg));
	SempUtils::semp_pend(m_sdiScaleComponent->eos_sem);
	printf("after get sdi scale eos sem\n");

	eError = OMX_SendCommand (m_pSDIDEIHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_sdiDEIComponent);
	printf(" sdi nf state idle \n ");


	eError = OMX_SendCommand (m_pSDIScaleHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	OMXComponentUtils::waitStateSem(m_sdiScaleComponent);
	printf(" Scalar state idle \n ");

	eError = OMX_SendCommand(m_pSDINfHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (OMX_ErrorNone != eError) {
		return 0;
	}
	OMXComponentUtils::waitStateSem(m_sdiNfComponent);

	eError = OMX_SendCommand(m_pSDIEncHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (OMX_ErrorNone != eError) {
		return 0;
	}
	OMXComponentUtils::waitStateSem(m_sdiEncComponent);

	eError = OMX_SendCommand(m_pHeadTailScaleHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_headTailScaleComponent);

	if (m_pSetting->getLiveEncEnable() ) {
		eError = OMX_SendCommand(m_pDeiHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (OMX_ErrorNone != eError) {
			return 0;
		}
		OMXComponentUtils::waitStateSem(m_deiComponent);

		eError = OMX_SendCommand(m_pLiveNfHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (OMX_ErrorNone != eError) {
			return 0;
		}
		OMXComponentUtils::waitStateSem(m_liveNfComponent);

		eError = OMX_SendCommand(m_pLiveEncHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (eError != OMX_ErrorNone) {
			return 0;
		}
		OMXComponentUtils::waitStateSem(m_liveEncComponent);
	} else {
		eError = OMX_SendCommand(m_pNfHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (eError != OMX_ErrorNone) {
			return 0;
		}
		OMXComponentUtils::waitStateSem(m_nfComponent);
		printf("nf state idle\n");
	}

	eError = OMX_SendCommand(m_pEncHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	OMXComponentUtils::waitStateSem(m_encComponent);
	printf("enc state idle\n");
	fflush(stdout);

	if (m_pSetting->getLiveEncEnable() ) {
		printf("before set live enc state loaded\n");

		eError = OMX_SendCommand(m_pLiveEncHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
		if (OMX_ErrorNone != eError) {
			printf("set live enc state loaded failed %x\n", eError);
			return 0;
		}
		if (!freeLiveEncBuffers() ) {
			printf("set live enc buffers failed\n");
			return 0;
		}

		printf("before set live nf state loaded\n");

		eError = OMX_SendCommand(m_pLiveNfHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
		if (OMX_ErrorNone != eError) {
			printf("set live nf state loaded failed %x\n", eError);
			return 0;
		}
		printf("before free live nf buffers\n");

		if (!freeLiveNfBuffers() ) {
			printf("free live nf buffers failed\n");
			return 0;
		}

		eError = OMX_SendCommand(m_pEncHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
		if (OMX_ErrorNone != eError) {
			printf("set enc loaded failed\n");
			return 0;
		}
		if (!freeEncBuffers() ) {
			printf("free enc buffers failed\n");
			return 0;
		}
		printf("enc state loaded\n");

		eError = OMX_SendCommand(m_pDeiHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
		if (OMX_ErrorNone != eError) {
			printf("set dei state loaded failed %x\n", eError);
			return 0;
		}
		printf("before free dei buffers\n");

		if (!freeDeiBuffers() ) {
			printf("free dei buffers failed\n");
			return 0;
		}
		printf("after free dei buffers\n");

	} else {
		eError = OMX_SendCommand(m_pEncHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
		if (OMX_ErrorNone != eError) {
			printf("set enc loaded failed\n");
			return 0;
		}
		if (!freeEncBuffers() ) {
			printf("free enc buffers failed\n");
			return 0;
		}
		printf("enc state loaded\n");

		eError = OMX_SendCommand(m_pNfHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
		if (OMX_ErrorNone != eError) {
			return 0;
		}
		if (!freeNfBuffers() ) {
			return 0;
		}
		printf("nf state loaded\n");
	}


	eError = OMX_SendCommand(m_pSDIDEIHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return 0;
	}
	printf("before free dec buffers\n");
	if (!freeSDIDEIBuffers()) {
		return 0;
	}
	printf("after free dec buffers\n");


	eError = OMX_SendCommand(m_pSDIScaleHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	if (!freeSDIScaleBuffers()) {
		return FALSE;
	}

	eError = OMX_SendCommand(m_pSDINfHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	if (!freeSDINfBuffers()) {
		return FALSE;
	}

	eError = OMX_SendCommand(m_pSDIEncHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	if (!freeSDIEncBuffers()) {
		return FALSE;
	}



	eError = OMX_SendCommand(m_pHeadTailScaleHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	if (!freeHeadTailScaleBuffers()) {
		return FALSE;
	}


	if (!freeOMXHandles() ) {
		return false;
	}
	printf("free handles ok\n");

	pthread_cancel(m_sdiDEIComponent->connDataStrmThrdId);
	pthread_join(m_sdiDEIComponent->connDataStrmThrdId, NULL);
	pthread_cancel(m_sdiScaleComponent->connDataStrmThrdId);
	pthread_join(m_sdiScaleComponent->connDataStrmThrdId, NULL);
	pthread_cancel(m_sdiScaleComponent->outDataThreadId[0]);
	pthread_join(m_sdiScaleComponent->outDataThreadId[0], NULL);
	pthread_cancel(m_sdiScaleComponent->outDataStrmThrdId);
	pthread_join(m_sdiScaleComponent->outDataStrmThrdId, NULL);
	pthread_cancel(m_sdiNfComponent->connDataStrmThrdId);
	pthread_join(m_sdiNfComponent->connDataStrmThrdId, NULL);
	pthread_cancel(m_sdiEncComponent->connDataStrmThrdId);
	pthread_join(m_sdiEncComponent->connDataStrmThrdId, NULL);
	pthread_cancel(m_sdiEncComponent->outDataStrmThrdId);
	pthread_join(m_sdiEncComponent->outDataStrmThrdId, NULL);
	printf("after end vga scale thread\n");

	pthread_join(m_headTailScaleComponent->inDataStrmThrdId, NULL);
	pthread_join(m_headTailScaleComponent->outDataStrmThrdId, NULL);

	if (m_pSetting->getLiveEncEnable() ) {
		pthread_join(m_deiComponent->inDataStrmThrdId, NULL);
		printf("after end dei in thread\n");

		pthread_join(m_deiComponent->connDataStrmThrdId, NULL);
		printf("after end dei connect thread\n");

		pthread_join(m_liveNfComponent->connDataStrmThrdId, NULL);
		printf("after end nf connect thread\n");

		pthread_join(m_liveEncComponent->connDataStrmThrdId, NULL);
		printf("after end live enc connect thread\n");

		pthread_join(m_liveEncComponent->outDataStrmThrdId, NULL);
		printf("after end live enc out thread\n");
	} else {
		pthread_join(m_nfComponent->inDataStrmThrdId, NULL);
		pthread_join(m_nfComponent->connDataStrmThrdId, NULL);
	}
	printf("before enc connect thread\n");

	pthread_cancel(m_encComponent->connDataStrmThrdId);
	pthread_join(m_encComponent->connDataStrmThrdId, NULL);
	printf("after enc out thread\n");

	pthread_cancel(m_encComponent->outDataStrmThrdId);
	pthread_join(m_encComponent->outDataStrmThrdId, NULL);
	printf("after end encode thread \n");

	printf("after stop omx thread\n");
	return true;
}

bool MixtureMode_SDI::freeOMXHandles() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	eError = OMX_FreeHandle(m_pSDIDEIHandle);
	if ((eError != OMX_ErrorNone)) {
		printf("free sdi dei handle failed %x\n", eError);
		return false;
	}
	printf(" sdi dei free handle \n");

	eError = OMX_FreeHandle(m_pSDIScaleHandle);
	if ((eError != OMX_ErrorNone)) {
		return false;
	}
	printf(" scalar free handle \n");

	eError = OMX_FreeHandle(m_pSDINfHandle);
	if ((eError != OMX_ErrorNone)) {
		return false;
	}
	printf("sdi nf free handle \n");

	eError = OMX_FreeHandle(m_pSDIEncHandle);
	if ((eError != OMX_ErrorNone)) {
		return false;
	}
	printf("sdi enc free handle \n");

	eError = OMX_FreeHandle(m_pHeadTailScaleHandle);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	if (m_pSetting->getLiveEncEnable() ) {
		eError = OMX_FreeHandle(m_pDeiHandle);
		if (OMX_ErrorNone != eError) {
			return false;
		}

		eError = OMX_FreeHandle(m_pLiveNfHandle);
		if (OMX_ErrorNone != eError) {
			return false;
		}

		eError = OMX_FreeHandle(m_pLiveEncHandle);
		if (OMX_ErrorNone != eError) {
			return false;
		}
	} else {
		eError = OMX_FreeHandle(m_pNfHandle);
		if ((eError != OMX_ErrorNone)) {
			return false;
		}
	}
	eError = OMX_FreeHandle(m_pEncHandle);
	if ((eError != OMX_ErrorNone)) {
		return false;
	}


	return true;
}
BOOL MixtureMode_SDI::start() {
	if (m_bExit) {
		return FALSE;
	}


	if (!m_pAudioEncode->start()) {
		if (NULL != m_pKernel) {
			m_pKernel->NotifyAssistMessage(gStrRecordFailed);
		}
		system("reboot -f");
	}

	if (m_pSetting->getHeaderEnable() ) {
		doMovieHead();
	}
	startLowerHalfOMXThread();
	startUpperHalfOMXThread();

	m_bCouldExit = true;

	m_pKernel->getVGAProviderSystemInstance()->addListener(this);

	m_pKernel->getSDIProviderSystemInstance()->addListener(this);

	m_ExitTimer.init();
	m_ExitTimer.setInterval(12000);
	m_ExitTimer.start(this);
	m_ExitTimerValid = true;

	printf("check audio timer init\n");
	fflush(stdout);
	m_checkAudioSyncTimer.init();
	m_checkAudioSyncTimer.setInterval(300000); //5 min
	m_checkAudioSyncTimer.start(this);
	m_checkAudioSyncTimerValid = true;
	printf("check audio timer init end\n");
	fflush(stdout);
	return TRUE;
}

BOOL MixtureMode_SDI::stop() {
	if (!m_bCouldExit) {
		return FALSE;
	}

	m_bCouldExit = TRUE;

	if (m_ResumeTimerValid) {
		m_ResumeTimer.stop();
		m_ResumeTimer.uninit();
		m_ResumeTimerValid = false;
	}
	if (m_checkAudioSyncTimerValid) {
		m_checkAudioSyncTimer.stop();
		m_checkAudioSyncTimer.uninit();
		m_checkAudioSyncTimerValid = false;
	}

	if (m_headDelayTimerValid) {
		m_headDelayTimer.stop();
		m_headDelayTimer.uninit();
		m_headDelayTimerValid = false;
	}
	if (m_ExitTimerValid) {
		m_ExitTimer.stop();
		m_ExitTimer.uninit();
		m_ExitTimerValid = false;
	}

	stopOMXThread();

	m_pKernel->getVGAProviderSystemInstance()->removeListener(this);
	m_pKernel->getSDIProviderSystemInstance()->removeListener(this);
	printf("after vga remove listener\n");
	fflush(stdout);


	if (NULL != m_pAudioEncode) {
		m_pAudioEncode->stop();
		delete m_pAudioEncode;
		m_pAudioEncode = NULL;
	}

	closeFileHandles();
	printf("close file handle\n");
	fflush(stdout);
	if (m_pSetting->getHDMIDisplayEnable()) {
		m_displayHandle.uninitv4lhandle();
	}

	printf("after uninitv4lhandle before uninit\n");
	uninit();
	printf("after stop\n");
	return TRUE;
}

bool MixtureMode_SDI::pauseLowerHalf() {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	if (m_pSetting->getLiveEncEnable() ) {
		eError = OMX_SendCommand(m_pDeiHandle, OMX_CommandStateSet, OMX_StatePause, NULL);
		if (OMX_ErrorNone != eError) {
			return false;
		}
		OMXComponentUtils::waitStateSem(m_deiComponent);

		eError = OMX_SendCommand(m_pLiveNfHandle, OMX_CommandStateSet, OMX_StatePause, NULL);
		if (OMX_ErrorNone != eError) {
			return false;
		}
		OMXComponentUtils::waitStateSem(m_liveNfComponent);

	} else {
		eError = OMX_SendCommand(m_pNfHandle, OMX_CommandStateSet, OMX_StatePause, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		OMXComponentUtils::waitStateSem(m_nfComponent);
		printf("nf component pause ok\n");
	}

	return true;
}

bool MixtureMode_SDI::resumeLowerHalf() {
	m_mixbeftime = CommonUtils::GetTickCount();
	m_mixafttime = m_mixbeftime;
	m_mixTotalCount = 0;


	m_livebeftime = m_mixbeftime;
	m_liveafttime = m_livebeftime;
	m_nLiveTotalEncCount = 0;

	m_nTotalEncCount = 0;

	m_headTailScaleReadaftTime = m_mixbeftime;
	m_headTailScaleReadbefTime = m_mixbeftime;
	m_headTailScaleReadExpectTime = 0;

	OMX_ERRORTYPE eError = OMX_ErrorNone;
	if (m_pSetting->getLiveEncEnable() ) {
		eError = OMX_SendCommand(m_pDeiHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (OMX_ErrorNone != eError) {
			return false;
		}
		OMXComponentUtils::waitStateSem(m_deiComponent);

		eError = OMX_SendCommand(m_pLiveNfHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (OMX_ErrorNone != eError) {
			return false;
		}
		OMXComponentUtils::waitStateSem(m_liveNfComponent);

	} else {
		eError = OMX_SendCommand(m_pNfHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		OMXComponentUtils::waitStateSem(m_nfComponent);
		printf("nf component pause ok\n");
	}


	/*eError = OMX_SendCommand(m_pHeadTailScaleHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (OMX_ErrorNone != eError) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_headTailScaleComponent);*/
	return true;
}


BOOL MixtureMode_SDI::pause() {
	m_bPaused = true;

	//m_streamVideoManager.closeVideos();

	OMX_ERRORTYPE eError = OMX_ErrorNone;

	if (m_pSetting->getLiveEncEnable() ) {
		eError = OMX_SendCommand(m_pDeiHandle, OMX_CommandStateSet, OMX_StatePause, NULL);
		if (OMX_ErrorNone != eError) {
			return FALSE;
		}
		OMXComponentUtils::waitStateSem(m_deiComponent);

		eError = OMX_SendCommand(m_pLiveNfHandle, OMX_CommandStateSet, OMX_StatePause, NULL);
		if (OMX_ErrorNone != eError) {
			return FALSE;
		}
		OMXComponentUtils::waitStateSem(m_liveNfComponent);

	} else {
		eError = OMX_SendCommand(m_pNfHandle, OMX_CommandStateSet, OMX_StatePause, NULL);
		if (eError != OMX_ErrorNone) {
			return FALSE;
		}
		OMXComponentUtils::waitStateSem(m_nfComponent);
		printf("nf component pause ok\n");
	}

	eError = OMX_SendCommand(m_pVGAScaleHandle, OMX_CommandStateSet,
			OMX_StatePause, NULL);
	if (OMX_ErrorNone != eError) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_vgaScaleComponent);

	/*eError = OMX_SendCommand(m_pHeadTailScaleHandle, OMX_CommandStateSet, OMX_StatePause, NULL);
	if (OMX_ErrorNone != eError) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_headTailScaleComponent);*/

	//do not change to enc component to pause,because it will be get error
	/*eError = OMX_SendCommand(m_pEncHandle, OMX_CommandStateSet,
			OMX_StatePause, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	SempUtils::semp_pend(m_encComponent->state_sem);*/



	return TRUE;
}

BOOL MixtureMode_SDI::changeToRunning(bool bOpenVideos) {
	m_bPaused = false;

	m_mixbeftime = CommonUtils::GetTickCount();
	m_mixafttime = m_mixbeftime;
	m_mixTotalCount = 0;


	m_livebeftime = m_mixbeftime;
	m_liveafttime = m_livebeftime;
	m_nLiveTotalEncCount = 0;

	m_nTotalEncCount = 0;

	OMX_ERRORTYPE eError = OMX_ErrorNone;

	if (m_pSetting->getLiveEncEnable() ) {
		eError = OMX_SendCommand(m_pDeiHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (OMX_ErrorNone != eError) {
			return FALSE;
		}
		OMXComponentUtils::waitStateSem(m_deiComponent);

		eError = OMX_SendCommand(m_pLiveNfHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (OMX_ErrorNone != eError) {
			return FALSE;
		}
		OMXComponentUtils::waitStateSem(m_liveNfComponent);

	} else {
		eError = OMX_SendCommand(m_pNfHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (eError != OMX_ErrorNone) {
			return FALSE;
		}

		OMXComponentUtils::waitStateSem(m_nfComponent);
		printf("nf component pause ok\n");
	}


	eError = OMX_SendCommand(m_pVGAScaleHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (OMX_ErrorNone != eError) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_vgaScaleComponent);

	/*eError = OMX_SendCommand(m_pHeadTailScaleHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (OMX_ErrorNone != eError) {
		return FALSE;
	}
	OMXComponentUtils::waitStateSem(m_headTailScaleComponent);*/

	/*eError = OMX_SendCommand(m_pEncHandle, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	if (eError != OMX_ErrorNone) {
		return FALSE;
	}
	SempUtils::semp_pend(m_encComponent->state_sem);*/

	return TRUE;
}

BOOL MixtureMode_SDI::resume() {
	if (m_ResumeTimerValid) {
		m_ResumeTimer.stop();
		m_ResumeTimer.uninit();
		m_ResumeTimerValid = false;
	}
	m_ResumeTimer.init();
	m_ResumeTimer.setInterval(400);
	m_ResumeTimer.start(this);
	m_ResumeTimerValid = true;

	return changeToRunning(true);
}

void MixtureMode_SDI::onVGAScaleWriteTask() {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	printf("on vga scale write task\n");
	while (1) {
		read(m_vgaScaleComponent->outPortParams->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
		if (m_bExitVGAScaleThread) {
			SempUtils::semp_post(m_vgaScaleComponent->eos_sem);
			printf("exit %s write thread\n", m_vgaScaleComponent->name);

			pthread_exit (NULL);
			break;
		}

		m_exchangeMutex.lock();
		if (!m_bHeadState && !m_bTailState) {
			if (m_deviceSceneInfo.dwLeft != m_deviceSceneInfo.dwRight
					&& m_deviceSceneInfo.dwTop != m_deviceSceneInfo.dwBottom) {
				OMX_PTR pOutAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy(
						(uint32_t) pBufferOut->pBuffer);

				if (m_bTeacherContainVGA || m_bStudContainVGA) {
					if (pBufferOut->nFilledLen == m_nContainVGASize) {
						SempUtils::semp_pend(&m_containVGASem);
						m_pKernel->getEdmaInstance()->copy(m_containVGAImagePhyAddr, pOutAddr,
								m_outVGAWidth * 2, m_outVGAWidth * 2, pBufferOut->nFilledLen, 1);
						SempUtils::semp_post(&m_containVGASem);
					}
				} else {
					if (pBufferOut->nFilledLen == m_outVGAWidth * m_outVGAHeight * 2) {
						if (m_ResumeTimerValid) {
							m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr
											+ m_deviceSceneInfo.dwTop * m_outWidth * 2
											+ m_deviceSceneInfo.dwLeft * 2, pOutAddr,
									m_outVGAWidth * 2, m_outWidth * 2,
									pBufferOut->nFilledLen, 1);
						}
						if (m_bTailState) {
							printf("vga scale copy ******\n");
						}
					}
					if (m_bVGAContainTeacher) {
						SempUtils::semp_pend(&m_containTeacherSem);
						m_pKernel->getEdmaInstance()->copy(
								(char*) m_exchangeImagePhyAddr
										+ m_teacherSceneInfo.dwTop * m_outWidth * 2
										+ m_teacherSceneInfo.dwLeft * 2,
								m_containTeacherImagePhyAddr,
								m_outScaleWidth[0] * 2, m_outWidth * 2,
								m_nContainTeacherSize, 1);
						SempUtils::semp_post(&m_containTeacherSem);
					} else if (m_bVGAContainStud) {
						SempUtils::semp_pend(&m_containStudSem);
						m_pKernel->getEdmaInstance()->copy(
								(char*) m_exchangeImagePhyAddr
										+ m_studentSceneInfo.dwTop * m_outWidth * 2
										+ m_studentSceneInfo.dwLeft * 2,
								m_containStudImagePhyAddr, m_outScaleWidth[1] * 2,
								m_outWidth * 2, m_nContainStudSize, 1);
						SempUtils::semp_post(&m_containStudSem);
					}
				}
			}
		}
		m_exchangeMutex.unlock();

	    err = OMX_FillThisBuffer (m_vgaScaleComponent->handle, pBufferOut);

		if (OMX_ErrorNone != err) {
			write(m_vgaScaleComponent->outPortParams->opBufPipe[1],	&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client %x\n", err);

			SempUtils::semp_post(m_vgaScaleComponent->eos_sem);
			pthread_exit(NULL);
			break;
		}
	}
}

void MixtureMode_SDI::onVGAEncodeWriteTask() {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;

	printf("on vga encode write task\n");
	unsigned int m_nvgaEncodeCount = 0;
	while (1) {
		read(m_vgaEncComponent->outPortParams->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
	    if (m_bExitVGAEncodeThread)  {
		    	SempUtils::semp_post(m_vgaEncComponent->eos_sem);
		    	printf("exit vga encode write thread\n");

		    	pthread_exit(NULL);
		    	break;
		 }
		if (!m_bPaused) {
			m_pKernel->getLiveCourseManager()->sendData(m_pSetting->getLCVGAPath().c_str(), (const char*)pBufferOut->pBuffer
					, pBufferOut->nFilledLen, MTG_DATA_TYPE_VIDEO, eLCVGAVideo);
			//if (-1 != m_vgaH264fd) {
			//	write(m_vgaH264fd, pBufferOut->pBuffer, pBufferOut->nFilledLen);
			//}
		}

	    m_nvgaEncodeCount++;
	    if (0 == m_nvgaEncodeCount%25) {
			OMX_CONFIG_INTRAREFRESHVOPTYPE tConfigIntraRefreshVopType;
			OMX_INIT_PARAM(&tConfigIntraRefreshVopType);
			tConfigIntraRefreshVopType.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
			tConfigIntraRefreshVopType.IntraRefreshVOP = OMX_TRUE;
			err = OMX_SetConfig(m_pVGAEncHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tConfigIntraRefreshVopType);
			if (OMX_ErrorNone != err) {
				//return false;
			}
	    }

		err = OMX_FillThisBuffer (m_vgaEncComponent->handle, pBufferOut);

		if (OMX_ErrorNone != err) {
			write(m_vgaEncComponent->outPortParams->opBufPipe[1],
					&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client \n");
			pthread_exit(NULL);

		}
	}
}

void MixtureMode_SDI::onSDIEncWriteTask(ComponentWrapper* aComponent) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	unsigned int nLiveCount = 0;
	while (1) {
		read(aComponent->outPortParams->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
	    if (m_bExit)  {
	    	SempUtils::semp_post(aComponent->eos_sem);
	    	//pthread_exit(NULL);
	    	printf("exit live encode thread\n");
	    	break;
	    }

		if (!m_bPaused) {
			if (nLiveCount > m_pSetting->getLiveDeleteFrameCount()) {
				if (-1 != m_resourceH264fd[0]) {
					write(m_resourceH264fd[0], pBufferOut->pBuffer, pBufferOut->nFilledLen);
				}
				m_pKernel->getLiveCourseManager()->sendData(m_pSetting->getLCTeacherPath().c_str(), (const char*)pBufferOut->pBuffer
						, pBufferOut->nFilledLen, MTG_DATA_TYPE_VIDEO, eLCRecordVideo);
			} else if (nLiveCount == m_pSetting->getLiveDeleteFrameCount()) {
				OMX_CONFIG_INTRAREFRESHVOPTYPE tConfigIntraRefreshVopType;
				OMX_INIT_PARAM(&tConfigIntraRefreshVopType);
				tConfigIntraRefreshVopType.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
				tConfigIntraRefreshVopType.IntraRefreshVOP = OMX_TRUE;
				err = OMX_SetConfig(m_pSDIEncHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tConfigIntraRefreshVopType);
				if (OMX_ErrorNone != err) {
					//return false;
				}
			}
		}
		nLiveCount++;

	    if (m_pSetting->getLiveEncVOPInter() > 0) {
		    if (0 == m_nLiveTotalEncCount%m_pSetting->getLiveEncVOPInter()) {
				OMX_CONFIG_INTRAREFRESHVOPTYPE tConfigIntraRefreshVopType;
				OMX_INIT_PARAM(&tConfigIntraRefreshVopType);
				tConfigIntraRefreshVopType.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
				tConfigIntraRefreshVopType.IntraRefreshVOP = OMX_TRUE;
				err = OMX_SetConfig(m_pSDIEncHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tConfigIntraRefreshVopType);
				if (OMX_ErrorNone != err) {
					//return false;
				}
		    }
	    }

		err = OMX_FillThisBuffer (aComponent->handle, pBufferOut);

		if (OMX_ErrorNone != err) {
			write(aComponent->outPortParams->opBufPipe[1],
					&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client \n");
			pthread_exit(NULL);
			break;
		}
	}
}

void MixtureMode_SDI::onLiveWriteTask(ComponentWrapper *aComponent) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	unsigned int nLiveCount = 0;
	while (1) {
		read(aComponent->outPortParams->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
	    if (m_bExit)  {
	    	SempUtils::semp_post(aComponent->eos_sem);
	    	//pthread_exit(NULL);
	    	printf("exit live encode thread\n");
	    	break;
	    }

		if (!m_bPaused) {
			if (nLiveCount > m_pSetting->getLiveDeleteFrameCount()) {
				if (NULL != m_rtmpHandle) {
					m_rtmpHandle->pushVideo((char*)pBufferOut->pBuffer, pBufferOut->nFilledLen);
				}
				if (-1 != m_liveH264fd) {
					write(m_liveH264fd, pBufferOut->pBuffer, pBufferOut->nFilledLen);
				}
				m_pKernel->getLiveCourseManager()->sendData(m_pSetting->getLCRecordPath().c_str(), (const char*)pBufferOut->pBuffer
						, pBufferOut->nFilledLen, MTG_DATA_TYPE_VIDEO, eLCRecordVideo);
			} else if (nLiveCount == m_pSetting->getLiveDeleteFrameCount()) {
				OMX_CONFIG_INTRAREFRESHVOPTYPE tConfigIntraRefreshVopType;
				OMX_INIT_PARAM(&tConfigIntraRefreshVopType);
				tConfigIntraRefreshVopType.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
				tConfigIntraRefreshVopType.IntraRefreshVOP = OMX_TRUE;
				err = OMX_SetConfig(m_pLiveEncHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tConfigIntraRefreshVopType);
				if (OMX_ErrorNone != err) {
					//return false;
				}
			}
		}
		nLiveCount++;
	    m_nLiveTotalEncCount++;
	    if (m_pSetting->getLiveEncVOPInter() > 0) {
		    if (0 == m_nLiveTotalEncCount%m_pSetting->getLiveEncVOPInter()) {
				OMX_CONFIG_INTRAREFRESHVOPTYPE tConfigIntraRefreshVopType;
				OMX_INIT_PARAM(&tConfigIntraRefreshVopType);
				tConfigIntraRefreshVopType.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
				tConfigIntraRefreshVopType.IntraRefreshVOP = OMX_TRUE;
				err = OMX_SetConfig(m_pLiveEncHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tConfigIntraRefreshVopType);
				if (OMX_ErrorNone != err) {
					//return false;
				}
		    }
	    }

		err = OMX_FillThisBuffer (aComponent->handle, pBufferOut);

		if (OMX_ErrorNone != err) {
			write(aComponent->outPortParams->opBufPipe[1],
					&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client \n");
			pthread_exit(NULL);
			break;
		}
	}
}

void MixtureMode_SDI::onHeadTailScaleWriteTask() {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	unsigned int nScaleCount = 0;
	while (1) {
		read(m_headTailScaleComponent->outPortParams->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
		if (m_bExitHeadTailScaleThread)  {
			SempUtils::semp_post(m_headTailScaleComponent->eos_sem);
			printf("exit %s write thread\n", m_headTailScaleComponent->name);

			pthread_exit(NULL);
			break;
		}

		if (!m_binputMixInited) {
			OMX_PTR pTempBuffer = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy ((uint32_t)pBufferOut->pBuffer);
			if (nScaleCount == 0) {
				//FILE* pFile = fopen("/media/sda/test.yuv", "wb+");
				//fwrite(buffer_addr[0] + m_exchangeImagePhyAddr - BASE_ADDR, 1, pBufferOut->nFilledLen, pFile);
				//fclose(pFile);

				m_exchangeMutex.lock();
				m_pKernel->getEdmaInstance()->copy((char*)m_exchangeImagePhyAddr, pTempBuffer,
					m_outWidth * 2, m_outWidth * 2 , pBufferOut->nFilledLen, 1);
				m_exchangeMutex.unlock();
			}

			if (!setMixData((unsigned char*)pTempBuffer, pBufferOut->nFilledLen)) {
				sem_post(&m_inputMixWaitSem);
				printf("post wait sem\n");
				fflush(stdout);
				m_binputMixInited = true;
			}
			nScaleCount++;
		} else {
			m_exchangeMutex.lock();
			if (m_bTailState || m_bHeadState) {
				OMX_PTR pTempBuffer = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy ((uint32_t)pBufferOut->pBuffer);
				if (pBufferOut->nFilledLen == m_outWidth * m_outHeight * 2) {
					m_pKernel->getEdmaInstance()->copy((char*)m_exchangeImagePhyAddr, pTempBuffer,
							m_outWidth * 2, m_outWidth * 2 , pBufferOut->nFilledLen, 1);
				}

			}
			m_exchangeMutex.unlock();
		}

	    err = OMX_FillThisBuffer (m_headTailScaleComponent->handle, pBufferOut);
			/*struct timespec tp;
			if(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp)){
				perror("2");
			}
			printf("componenent name %s >>%ds.%dns\n", aComponent->name, tp.tv_sec,tp.tv_nsec);*/
			if (OMX_ErrorNone != err) {
				write(m_headTailScaleComponent->outPortParams->opBufPipe[1],&pBufferOut, sizeof(pBufferOut));
				printf(" waiting for action from IL Client %x\n", err);
				SempUtils::semp_post(m_headTailScaleComponent->eos_sem);
				pthread_exit(NULL);
				break;
			}
	}

}

void MixtureMode_SDI::onSDIScaleChannel1(ComponentWrapper* aComponent) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	OutportParams *outPortParamsPtr = m_sdiScaleComponent->outPortParams;
	while (1) {
		read(outPortParamsPtr->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
		if (m_bExit)  {
			SempUtils::semp_post(aComponent->eos_sem);
			printf("exit %s write thread\n", aComponent->name);
		    pthread_exit(NULL);
		    break;
		}
		m_exchangeMutex.lock();
		if (!m_bHeadState && !m_bTailState) {
			if (m_teacherSceneInfo.dwLeft != m_teacherSceneInfo.dwRight
					&& m_teacherSceneInfo.dwTop != m_teacherSceneInfo.dwBottom && !m_ResumeTimerValid) {
				//printf("before DomxCore_mapUsrVirtualAddr2phy 6\n");
				OMX_PTR pOutAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy( (uint32_t) pBufferOut->pBuffer);
				//printf("after DomxCore_mapUsrVirtualAddr2phy 6\n");

				if (m_bStudContainTeacher || m_bVGAContainTeacher) {
					if (pBufferOut->nFilledLen == m_outScaleWidth[0] * m_outScaleHeight[0] * 2) {
						SempUtils::semp_pend(&m_containTeacherSem);
						m_pKernel->getEdmaInstance()->copy(m_containTeacherImagePhyAddr, pOutAddr,
								m_outScaleWidth[0] * 2, m_outScaleWidth[0] * 2,
								pBufferOut->nFilledLen, 1);
						SempUtils::semp_post(&m_containTeacherSem);
					}
				} else {
					if (pBufferOut->nFilledLen == m_outScaleWidth[0] * m_outScaleHeight[0] * 2 && !m_bDirectCopyChannel1) {
						m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr
										+ m_teacherSceneInfo.dwTop * m_outWidth * 2
										+ m_teacherSceneInfo.dwLeft * 2, pOutAddr,
								m_outScaleWidth[0] * 2, m_outWidth * 2,
								pBufferOut->nFilledLen, 1);
					}

					if (m_bTeacherContainStud) {
						SempUtils::semp_pend(&m_containStudSem);
						m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr + m_studentSceneInfo.dwTop * m_outWidth * 2
										+ m_studentSceneInfo.dwLeft * 2,
								m_containStudImagePhyAddr, m_outScaleWidth[1] * 2, m_outWidth * 2,
								m_nContainStudSize, 1);
						SempUtils::semp_post(&m_containStudSem);
					} else if (m_bTeacherContainVGA) {
						SempUtils::semp_pend(&m_containVGASem);
						m_pKernel->getEdmaInstance()->copy( (char*) m_exchangeImagePhyAddr + m_deviceSceneInfo.dwTop * m_outWidth * 2
								+ m_deviceSceneInfo.dwLeft * 2,
								m_containVGAImagePhyAddr, m_outVGAWidth * 2,
								m_outWidth * 2, m_nContainVGASize, 1);
						SempUtils::semp_post(&m_containVGASem);
					}
				}
			}
		}
		m_exchangeMutex.unlock();

	    err = OMX_FillThisBuffer (aComponent->handle, pBufferOut);
		/*struct timespec tp;
		if(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp)){
			perror("2");
		}
		printf("componenent name %s >>%ds.%dns\n", aComponent->name, tp.tv_sec,tp.tv_nsec);*/
		if (OMX_ErrorNone != err) {
			write(outPortParamsPtr->opBufPipe[1],	&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client %x\n", err);
			SempUtils::semp_post(aComponent->eos_sem);
			pthread_exit(NULL);
			break;
		}
	}
}

void MixtureMode_SDI::onSDIScaleChannel2(ComponentWrapper* aComponent) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	OutportParams *outPortParamsPtr;
	while (1) {
		outPortParamsPtr = m_sdiScaleComponent->outPortParams + 1;
		read(outPortParamsPtr->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
		if (m_bExit)  {
			SempUtils::semp_post(aComponent->eos_sem);
			printf("exit %s write thread\n", aComponent->name);
		    pthread_exit(NULL);
		    break;
		}
		m_exchangeMutex.lock();
		if (!m_bHeadState && ! m_bTailState) {
			if (m_studentSceneInfo.dwLeft != m_studentSceneInfo.dwRight
					&& m_studentSceneInfo.dwTop != m_studentSceneInfo.dwBottom && !m_ResumeTimerValid) {
				//printf("before DomxCore_mapUsrVirtualAddr2phy 8\n");
				OMX_PTR pOutAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy((uint32_t) pBufferOut->pBuffer);
				//printf("after DomxCore_mapUsrVirtualAddr2phy 8\n");

				if (m_bTeacherContainStud || m_bVGAContainStud) {
					if (pBufferOut->nFilledLen == m_nContainStudSize) {
						SempUtils::semp_pend(&m_containStudSem);
						m_pKernel->getEdmaInstance()->copy(m_containStudImagePhyAddr, pOutAddr,
								m_outScaleWidth[1] * 2, m_outScaleWidth[1] * 2,
								pBufferOut->nFilledLen, 1);
						SempUtils::semp_post(&m_containStudSem);
					}
				} else {
					if (pBufferOut->nFilledLen == m_outScaleWidth[1] * m_outScaleHeight[1] * 2 && !m_bDirectCopyChannel2) {
						m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr
										+ m_studentSceneInfo.dwTop * m_outWidth * 2
										+ m_studentSceneInfo.dwLeft * 2, pOutAddr,
								m_outScaleWidth[1] * 2, m_outWidth * 2,
								pBufferOut->nFilledLen, 1);
					}
					if (m_bStudContainTeacher) {
						SempUtils::semp_pend(&m_containTeacherSem);
						m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr
										+ m_teacherSceneInfo.dwTop * m_outWidth * 2
										+ m_teacherSceneInfo.dwLeft * 2,
								m_containTeacherImagePhyAddr,
								m_outScaleWidth[0] * 2, m_outWidth * 2,
								m_nContainTeacherSize, 1);
						SempUtils::semp_post(&m_containTeacherSem);
					} else if (m_bStudContainVGA) {
						SempUtils::semp_pend(&m_containVGASem);
						m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr
										+ m_deviceSceneInfo.dwTop * m_outWidth * 2
										+ m_deviceSceneInfo.dwLeft * 2,
								m_containVGAImagePhyAddr, m_outVGAWidth * 2,
								m_outWidth * 2, m_nContainVGASize, 1);
						SempUtils::semp_post(&m_containVGASem);
					}

				}
			}
		}
		m_exchangeMutex.unlock();
	    err = OMX_FillThisBuffer (aComponent->handle, pBufferOut);
		/*struct timespec tp;
		if(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp)){
			perror("2");
		}
		printf("componenent name %s >>%ds.%dns\n", aComponent->name, tp.tv_sec,tp.tv_nsec);*/
		if (OMX_ErrorNone != err) {
			write(outPortParamsPtr->opBufPipe[1],	&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client %x\n", err);
			SempUtils::semp_post(aComponent->eos_sem);
			pthread_exit(NULL);
			break;
		}
	}
}

void MixtureMode_SDI::onWriteTask(ComponentWrapper *aComponent) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;

	OutportParams *outPortParamsPtr = aComponent->outPortParams;
	int index = 0;
	while (1) {
		read(outPortParamsPtr->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
		if (m_bExit)  {
			SempUtils::semp_post(aComponent->eos_sem);
			printf("exit %s write thread\n", aComponent->name);
			pthread_exit(NULL);
			break;
		}
		m_enc_frames++;
		if (!m_bPaused) {
			if (NULL != m_mp4OutputHandle) {
				if (m_enc_frames > m_pSetting->getDeleteFrameCount()) {
					m_mp4Mutex.lock();
					m_mp4OutputHandle->writeH264Video(
								(const char*) pBufferOut->pBuffer, pBufferOut->nFilledLen);
					m_mp4Mutex.unlock();
					if (-1 != m_recordH264fd) {
						write(m_recordH264fd, pBufferOut->pBuffer, pBufferOut->nFilledLen);
					}
					m_nVideoCountInMP4File++;
				} else if (m_enc_frames == m_pSetting->getDeleteFrameCount()) {
					OMX_CONFIG_INTRAREFRESHVOPTYPE tConfigIntraRefreshVopType;
					OMX_INIT_PARAM(&tConfigIntraRefreshVopType);
					tConfigIntraRefreshVopType.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
					tConfigIntraRefreshVopType.IntraRefreshVOP = OMX_TRUE;
					err = OMX_SetConfig(m_pEncHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tConfigIntraRefreshVopType);
					if (OMX_ErrorNone != err) {
						//return false;
					}
				}
			}

		}
		m_nTotalEncCount++;
		if (m_pSetting->getLiveEncVOPInter()) {
			if (0 == m_nTotalEncCount % 25) {
				OMX_CONFIG_INTRAREFRESHVOPTYPE tConfigIntraRefreshVopType;
				OMX_INIT_PARAM(&tConfigIntraRefreshVopType);
				tConfigIntraRefreshVopType.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
				tConfigIntraRefreshVopType.IntraRefreshVOP = OMX_TRUE;
				err = OMX_SetConfig(m_pEncHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tConfigIntraRefreshVopType);
				if (OMX_ErrorNone != err) {
					//return false;
				}
			}
		}

	    err = OMX_FillThisBuffer (aComponent->handle, pBufferOut);
		/*struct timespec tp;
		if(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp)){
			perror("2");
		}
		printf("componenent name %s >>%ds.%dns\n", aComponent->name, tp.tv_sec,tp.tv_nsec);*/
		if (OMX_ErrorNone != err) {
			write(outPortParamsPtr->opBufPipe[1],	&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client %x\n", err);
			SempUtils::semp_post(aComponent->eos_sem);
			pthread_exit(NULL);
			break;
		}
	}
}

void MixtureMode_SDI::setSDIDEIData(unsigned char* pPhyAddr, int size, int aSDIIndex) {
	unsigned int dataRead = 0;
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;
	ComponentWrapper* deiComp = m_sdiDEIComponent;

	int i = m_nSDIInputIndex[aSDIIndex];

	InportParams* InportParamsPtr = deiComp->inPortParams + aSDIIndex;
	if (i < deiComp->inPortParams->nBufferCountActual) {
		InportParamsPtr->pInBuff[i]->nFilledLen = size;
		InportParamsPtr->pInBuff[i]->nOffset = 0;
		InportParamsPtr->pInBuff[i]->nAllocLen = size;
		InportParamsPtr->pInBuff[i]->nInputPortIndex = 0;

		OMX_PTR pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)InportParamsPtr->pInBuff[i]->pBuffer);
		m_pKernel->getEdmaInstance()->copy(pTempPhyAddr, pPhyAddr, m_inWidth[aSDIIndex]*2, m_inWidth[aSDIIndex] *2, size, 1);

		err = OMX_EmptyThisBuffer (m_pSDIDEIHandle,
				InportParamsPtr->pInBuff[i]);
		m_nSDIInputIndex[aSDIIndex]++;
		return;
	}
	/* Read empty buffer pointer from the pipe */
	read(InportParamsPtr->ipBufPipe[0], &pBufferIn, sizeof(pBufferIn));


	OMX_PTR pTempPhyAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy( (uint32_t) pBufferIn->pBuffer);

	m_pKernel->getEdmaInstance()->copy((unsigned char*)pTempPhyAddr, pPhyAddr,
			m_inWidth[aSDIIndex] * 2, m_inWidth[aSDIIndex] * 2, size, 1);

	pBufferIn->nFilledLen = size;

	/*if (m_bControlSwitchScene) {
		if (m_teacherSceneInfo.dwLeft != m_teacherSceneInfo.dwRight) {
			if (channel == m_IPCChannel.dwTeacherChannel) {
				m_nReadSameChannelCount++;
				if (m_nReadSameChannelCount == KDecodeCmponentInOutNum) {
					printf("before switch scene in fill date, t channel\n");
					dynamicSwitchScene();
					printf("switch scene in fill date, t channel %d\n", channel);
					m_bControlSwitchScene = false;
					m_nReadSameChannelCount = 0;
				}
			}
		} else {
			if (m_studentSceneInfo.dwLeft != m_studentSceneInfo.dwRight) {
				if (channel == m_IPCChannel.dwStudentChannel) {
					m_nReadSameChannelCount++;
					if (m_nReadSameChannelCount == KDecodeCmponentInOutNum) {
						printf("before switch scene in fill date, s channel\n");
						if (NULL != m_pLogFile) {
							strcpy(m_logBuffer, "before switch scene in fill date, s channel\n");
							fwrite(m_logBuffer, 1, strlen(m_logBuffer), m_pLogFile);
						}

						dynamicSwitchScene();
						printf("switch scene in fill date, s channel %d\n", channel);
						if (NULL != m_pLogFile) {
							sprintf(m_logBuffer, "switch scene in fill date, s channel %d\n", channel);
							fwrite(m_logBuffer, 1, strlen(m_logBuffer), m_pLogFile);
						}

						m_bControlSwitchScene = false;
						m_nReadSameChannelCount = 0;
					}
				}
			}
		}
	}*/

	/* Pass the input buffer to the component */
	err = OMX_EmptyThisBuffer (deiComp->handle, pBufferIn);

	if (OMX_ErrorNone != err) {
		/* put back the frame in pipe and wait for state change */
		write(InportParamsPtr->ipBufPipe[1], &pBufferIn, sizeof(pBufferIn));
		printf("sdi nf exit thread\n");
		printf("exit %s read thread\n", deiComp->name);
	}
}


void MixtureMode_SDI::setVGAScaleData(unsigned char* pPhyAddr, int size) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;

	if (m_nVGAScaleInputIndex <  m_vgaScaleComponent->inPortParams->nBufferCountActual) {
		OMX_PTR pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)m_vgaScaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]->pBuffer);

		m_pKernel->getEdmaInstance()->copy(pTempPhyAddr, pPhyAddr, m_inVGAWidth* 3/2, m_inVGAWidth * 3/2, size, 1);

		m_vgaScaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]->nFilledLen = size;
		m_vgaScaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]->nOffset = 0;
		m_vgaScaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]->nAllocLen = size;
		m_vgaScaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]->nInputPortIndex = 0;

		err = OMX_EmptyThisBuffer (m_pVGAScaleHandle,
				m_vgaScaleComponent->inPortParams->pInBuff[m_nVGAScaleInputIndex]);
		m_nVGAScaleInputIndex++;
		return;
	}

	/* Read empty buffer pointer from the pipe */
	read(m_vgaScaleComponent->inPortParams->ipBufPipe[0], &pBufferIn, sizeof(pBufferIn));

	OMX_PTR pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)pBufferIn->pBuffer);

	m_pKernel->getEdmaInstance()->copy(pTempPhyAddr, pPhyAddr,
			m_inVGAWidth * 3/2, m_inVGAWidth * 3 /2, size, 1);


	pBufferIn->nFilledLen = size;
	pBufferIn->nOffset = 0;
	pBufferIn->nAllocLen = size;
	pBufferIn->nInputPortIndex = 0;

	err = OMX_EmptyThisBuffer (m_vgaScaleComponent->handle, pBufferIn);

	if (OMX_ErrorNone != err) {
		/* put back the frame in pipe and wait for state change */
		write(m_vgaScaleComponent->inPortParams->ipBufPipe[1], &pBufferIn,
				sizeof(pBufferIn));
		printf("exit vga scale read thread\n");
	}
}

void MixtureMode_SDI::setVGAEncodeData(unsigned char* pPhyAddr, int size) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;
	OMX_PTR pTempPhyAddr;
	if (m_nVGAEncodeInputIndex < m_vgaEncComponent->inPortParams->nBufferCountActual) {
		pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)m_vgaEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->pBuffer);
		m_pKernel->getEdmaInstance()->copy((unsigned char*)pTempPhyAddr, pPhyAddr,
			m_inVGAWidth * 3/2, m_inVGAWidth * 3/2, size, 1);

		m_vgaEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nFilledLen = size;
		m_vgaEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nOffset = 0;
		m_vgaEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nAllocLen = size;
		m_vgaEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]->nInputPortIndex = 0;

		err = OMX_EmptyThisBuffer (m_pVGAEncHandle,
				m_vgaEncComponent->inPortParams->pInBuff[m_nVGAEncodeInputIndex]);

		m_nVGAEncodeInputIndex++;
		return;
	}

	read(m_vgaEncComponent->inPortParams->ipBufPipe[0], &pBufferIn, sizeof(pBufferIn));

	pTempPhyAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy( (uint32_t) pBufferIn->pBuffer);

	m_pKernel->getEdmaInstance()->copy((unsigned char*)pTempPhyAddr, pPhyAddr,
			m_inVGAWidth * 3/2, m_inVGAWidth * 3/2, size, 1);

	pBufferIn->nFilledLen = size;
	pBufferIn->nOffset = 0;
	pBufferIn->nAllocLen = size;
	pBufferIn->nInputPortIndex = 0;

	err = OMX_EmptyThisBuffer (m_vgaEncComponent->handle, pBufferIn);

	if (OMX_ErrorNone != err) {
		/* put back the frame in pipe and wait for state change */
		write(m_vgaEncComponent->inPortParams->ipBufPipe[1], &pBufferIn,
				sizeof(pBufferIn));
		printf("exit thread\n");
	}
}

bool MixtureMode_SDI::setMixData(unsigned char* pPhyAddr, int size) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	ComponentWrapper* pComponent = m_pSetting->getLiveEncEnable() ? m_deiComponent : m_nfComponent;
	OMX_PTR pTempAddr;
	if (m_nMixInputIndex < pComponent->inPortParams->nBufferCountActual) {
		pTempAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy ((uint32_t)pComponent->inPortParams->pInBuff[m_nMixInputIndex]->pBuffer);
		m_pKernel->getEdmaInstance()->copy(pTempAddr, (unsigned char*)pPhyAddr,
				m_outWidth * 2, m_outWidth * 2 , size, 1);

		pComponent->inPortParams->pInBuff[m_nMixInputIndex]->nFilledLen = size;
		pComponent->inPortParams->pInBuff[m_nMixInputIndex]->nOffset = 0;
		pComponent->inPortParams->pInBuff[m_nMixInputIndex]->nAllocLen = size;
		pComponent->inPortParams->pInBuff[m_nMixInputIndex]->nInputPortIndex = 0;
		err = OMX_EmptyThisBuffer (pComponent->handle, pComponent->inPortParams->pInBuff[m_nMixInputIndex]);
		m_nMixInputIndex++;
		return true;
	}
	return false;
}

void MixtureMode_SDI::onMixReadTask(ComponentWrapper *aComponent) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;

	int frameSize = m_outWidth * m_outHeight * 2;
	timespec tv;
	tv.tv_sec = time(NULL) + 10;
	tv.tv_nsec = 0;

	sem_timedwait(&m_inputMixWaitSem, &tv);
	printf("on mix read task get sem\n");

	if (m_bExitMixtureThread) {
		SempUtils::semp_post(aComponent->eos_sem);
		printf("exit %s read thread\n", aComponent->name);
		pthread_exit(NULL);
		return;
	}

	OMX_PTR phyAddr;


	int nUsecondsPerFrame = 1000000/m_nFrameRate;
	m_mixbeftime = CommonUtils::GetTickCount();
	m_mixafttime = m_mixbeftime;

	tagRecordTemplate* pTemplate = m_pSetting->getRecordTemplate(m_outWidth, m_outHeight);
	bool bShowLogo = pTemplate->bVideoLogoShow;
	bool bCoursePicShow = pTemplate->bVideoCoursePicShow;

	if (m_preExchangeImagePhyAddr)
	if (m_pSetting->getHDMIDisplayEnable()) {
		m_exchangeMutex.lock();
		for (int i = 0; i < DISPLAY_MAX_BUFFER; ++i) {
			m_pKernel->getEdmaInstance()->copy((unsigned char*)gHDMIDisplayAddr + i * m_outWidth * m_outHeight * 2, (unsigned char*)m_exchangeImagePhyAddr,
					m_outWidth * 2, m_outWidth * 2 , m_outWidth * m_outHeight * 2, 1);
		}
		m_exchangeMutex.unlock();
		m_displayHandle.initv4lhandle(m_outWidth, m_outHeight, m_pSetting->getHDMIDisplayDeviceName().c_str(), 1);
	}

	long long imageAddr = m_exchangeImagePhyAddr;
	while (1) {
		read(aComponent->inPortParams->ipBufPipe[0], &pBufferIn, sizeof(pBufferIn));

		if (m_bExitMixtureThread) {
			SempUtils::semp_post(aComponent->eos_sem);
			printf("exit %s read thread\n", aComponent->name);
			pthread_exit(NULL);
			break;
		}

		phyAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy ((uint32_t)pBufferIn->pBuffer);

		while (m_mixafttime - m_mixbeftime < m_mixTotalCount * 40) {
			usleep(1000);
			m_mixafttime = CommonUtils::GetTickCount();
		}
		m_mixTotalCount++;

		if (m_bPreExchangeImageValid) {
			imageAddr = m_preExchangeImagePhyAddr;
			m_bPreExchangeImageValid = false;
		} else {
			imageAddr = m_exchangeImagePhyAddr;
		}
		m_exchangeMutex.lock();
		if (m_bTailState || m_bHeadState) {
			m_pKernel->getEdmaInstance()->copy(phyAddr, (unsigned char*)m_exchangeImagePhyAddr,
					m_outWidth * 2, m_outWidth * 2 , m_outWidth * m_outHeight * 2, 1);
		} else {
			m_pKernel->getEdmaInstance()->copy(phyAddr, (unsigned char*)imageAddr,
						m_outWidth * 2, m_outWidth * 2 , m_outWidth * m_outHeight * 2, 1);
		}
		m_exchangeMutex.unlock();

		if (bShowLogo) {
			m_pKernel->getEdmaInstance()->copy((char*)phyAddr +
					pTemplate->VideoLogo.dwPosX * 2 + pTemplate->VideoLogo.dwPosY * m_outWidth * 2,
					(unsigned char*)glogoImagePhyAddr, glogoWidth * 2, m_outWidth * 2, glogoWidth * glogoHeight * 2, 1);
		}
		if (bCoursePicShow && !(m_bTailState || m_bHeadState)) {

//#ifdef ALPHA_TEST
			/* unsigned char* pRGB32 = new unsigned char[m_courseImageWidth * m_courseImageHeight * 4];
			 unsigned char* pYUYV = new unsigned char[m_courseImageWidth * m_courseImageHeight * 2];
			 //可以优化，不需要两次拷贝
			 for (int i = 0; i < m_courseImageHeight; ++i) {
				 memcpy(pYUYV + i * m_courseImageWidth * 2,
						 pBufferIn->pBuffer + pTemplate->VideoCoursePic.dwPosX * 2 + (i + pTemplate->VideoCoursePic.dwPosY) * m_outWidth * 2,
						 m_courseImageWidth * 2);
			 }
			 ColorConvertUtils::yuyv_to_rgb32(pRGB32, pYUYV, m_courseImageWidth, m_courseImageHeight);
			 unsigned char* ps1 = buffer_addr[0] + m_courseImagePhyAddr - BASE_ADDR;
			 unsigned char r, g, b;
			 unsigned char alpha;
			 int nOffset1 = 0;
			 for (int i = 0; i < m_courseImageHeight; ++i) {
				 for (int j = 0; j < m_courseImageWidth; ++j) {
					 alpha = ps1[nOffset1 + 3];
					 pRGB32[nOffset1] = ps1[nOffset1 + 0] * alpha / 255 + pRGB32[nOffset1 + 0] * (255 - alpha)/255;
					 pRGB32[nOffset1 + 1] = ps1[nOffset1 + 1] * alpha / 255 + pRGB32[nOffset1 + 1] * (255 - alpha)/255;
					 pRGB32[nOffset1 + 2] = ps1[nOffset1 + 2] * alpha / 255 + pRGB32[nOffset1 + 2] * (255 - alpha)/255;

					 nOffset1 += 4;
				 }
			 }

			 ColorConvertUtils::rgb32_to_yuyv(pYUYV, pRGB32, m_courseImageWidth, m_courseImageHeight);

			 for (int i = 0; i < m_courseImageHeight; ++i) {
				 memcpy(pBufferIn->pBuffer + pTemplate->VideoCoursePic.dwPosX * 2 + (i + pTemplate->VideoCoursePic.dwPosY) * m_outWidth * 2,
						 pYUYV + i * m_courseImageWidth * 2,
						 m_courseImageWidth * 2);
			 }



			 delete []pRGB32;
			 pRGB32 = NULL;
			 delete []pYUYV;
			 pYUYV = NULL;
#else*/
			m_pKernel->getEdmaInstance()->copy((char*)phyAddr +
							pTemplate->VideoCoursePic.dwPosX * 2 + pTemplate->VideoCoursePic.dwPosY * m_outWidth * 2,
							(unsigned char*)gcourseImagePhyAddr, gcourseImageWidth * 2, m_outWidth * 2, gcourseImageWidth * gcourseImageHeight * 2, 1);
//#endif

		}


		if (m_pSetting->getHDMIDisplayEnable()) {
			m_displayHandle.doV4LDisplay((const char*)pBufferIn->pBuffer);
		}

		err = OMX_EmptyThisBuffer (aComponent->handle, pBufferIn);
		if (OMX_ErrorNone != err) {
			write(aComponent->inPortParams->ipBufPipe[1], &pBufferIn,
					sizeof(pBufferIn));
			printf("mix read task exit thread\n");
			SempUtils::semp_post(aComponent->eos_sem);
			printf("exit %s read thread\n", aComponent->name);
			pthread_exit(NULL);
		}
	}
}

void MixtureMode_SDI::onHeadTailScaleReadTask() {
	unsigned int dataRead = 0;
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn = NULL;
	OMX_BUFFERHEADERTYPE pBufferOut;

	m_headTailScaleReadExpectTime = 0;

	printf("before on head tail scale read task\n");

	int nMicrosecondsPerFrame = 1000/m_nFrameRate;
	for (OMX_U32 i = 0; i < m_headTailScaleComponent->inPortParams->nBufferCountActual; i++) {
		if (!initHeadTailInputResources(i)) {
			i--;
		}
		if (m_bExitHeadTailScaleThread) {
			SempUtils::semp_post(m_headTailScaleComponent->eos_sem);
			write(m_headTailScaleComponent->outPortParams->opBufPipe[1], &pBufferOut, sizeof(pBufferOut));
			printf("exit head tail scale read thread\n");
			pthread_exit(NULL);
			return;
		}
	}
	printf("after init head tail input resources\n");
	m_headTailScaleReadbefTime = CommonUtils::GetTickCount();
	while (1) {
		/* Read empty buffer pointer from the pipe */
		read(m_headTailScaleComponent->inPortParams->ipBufPipe[0], &pBufferIn, sizeof(pBufferIn));
		if (m_bExitHeadTailScaleThread) {
			SempUtils::semp_post(m_headTailScaleComponent->eos_sem);
			write(m_headTailScaleComponent->outPortParams->opBufPipe[1], &pBufferOut, sizeof(pBufferOut));
			printf("exit head tail scale read task\n");
			pthread_exit(NULL);
			break;
		}

		dataRead = fillHeadTailScaleData(pBufferIn);
		m_headTailScaleReadaftTime = CommonUtils::GetTickCount();

		while (m_headTailScaleReadaftTime - m_headTailScaleReadbefTime < m_headTailScaleReadExpectTime) {
			usleep(1000);
			m_headTailScaleReadaftTime = CommonUtils::GetTickCount();
		}
		m_headTailScaleReadExpectTime += nMicrosecondsPerFrame;

		/*if (dataRead <= 0 || m_bExit) {
			printf("No data available for Read\n");
			pBufferIn->nFlags |= OMX_BUFFERFLAG_EOS;
			err = OMX_EmptyThisBuffer (m_vgaScaleComponent->handle, pBufferIn);
			pthread_exit(m_vgaScaleComponent);
			break;
		}*/
		err = OMX_EmptyThisBuffer (m_headTailScaleComponent->handle, pBufferIn);

		if (OMX_ErrorNone != err) {
			/* put back the frame in pipe and wait for state change */
			write(m_headTailScaleComponent->inPortParams->ipBufPipe[1], &pBufferIn,
					sizeof(pBufferIn));
			printf("exit head tail scale read thread\n");
			SempUtils::semp_post(m_headTailScaleComponent->eos_sem);
			pthread_exit(NULL);
		}
	}
}

void MixtureMode_SDI::onConnectTask(ComponentWrapper *aComponent) {
	PipeMsg pipeMsg;
	OMX_ERRORTYPE err = OMX_ErrorNone;
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

bool MixtureMode_SDI::initHeadTailInputResources(int aInputIndex) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	int frameSize = KHeadPicWidth * KHeadPicHeight * 3 /2;

	OMX_PTR pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)m_headTailScaleComponent->inPortParams->pInBuff[aInputIndex]->pBuffer);

	m_headTailMutex.lock();
	m_pKernel->getEdmaInstance()->copy(pTempPhyAddr, (unsigned char*)gheadImagePhyAddr, KHeadPicWidth * 3/2, KHeadPicWidth * 3/2, frameSize, 1);
	m_headTailMutex.unlock();

	m_headTailScaleComponent->inPortParams->pInBuff[aInputIndex]->nFilledLen = frameSize;
	m_headTailScaleComponent->inPortParams->pInBuff[aInputIndex]->nOffset = 0;
	m_headTailScaleComponent->inPortParams->pInBuff[aInputIndex]->nAllocLen = frameSize;
	m_headTailScaleComponent->inPortParams->pInBuff[aInputIndex]->nInputPortIndex = 0;


	err = OMX_EmptyThisBuffer (m_pHeadTailScaleHandle,
			m_headTailScaleComponent->inPortParams->pInBuff[aInputIndex]);
	return true;
}

int MixtureMode_SDI::fillHeadTailScaleData(OMX_BUFFERHEADERTYPE* pBuf) {
	int frameSize = KHeadPicWidth * KHeadPicHeight * 3 /2;
	OMX_PTR pTempPhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)pBuf->pBuffer);

	m_headTailMutex.lock();
	if (m_bTailState) {
		m_pKernel->getEdmaInstance()->copy(pTempPhyAddr, (unsigned char*)gtailImagePhyAddr,
			KHeadPicWidth * 3/2, KHeadPicWidth * 3 /2, frameSize, 1);
	} else {
		m_pKernel->getEdmaInstance()->copy(pTempPhyAddr, (unsigned char*)gheadImagePhyAddr,
			KHeadPicWidth * 3/2, KHeadPicWidth * 3 /2, frameSize, 1);
	}
	m_headTailMutex.unlock();

	pBuf->nFilledLen = frameSize;
	pBuf->nOffset = 0;
	pBuf->nAllocLen = frameSize;
	pBuf->nInputPortIndex = 0;

	/* Update the buffer header with buffer filled length and alloc length */
	pBuf->nFilledLen = frameSize;

	return frameSize;
}



void MixtureMode_SDI::dynamicSwitchScene() {
	if (!m_bThreadsStart) {
		return;
	}
	m_exchangeMutex.lock();
	printf("before calcScalerWidthHeight\n");

	if (calcScalerWidthHeight()) {
		printf("after calcScalerWidthHeight\n");
		dynamicConfigScaleComponent();
		m_bPreExchangeImageValid = true;
		m_pKernel->getEdmaInstance()->copy((unsigned char*)m_preExchangeImagePhyAddr, (unsigned char*)m_exchangeImagePhyAddr,
				m_outWidth * 2, m_outWidth * 2, m_outWidth* m_outHeight * 2, 1);
		changeBackImage();
	} else {
		printf("after calcScalerWidthHeight not change\n");
	}
	m_exchangeMutex.unlock();
}

void MixtureMode_SDI::changeBackImage() {
	bool bTeacherShow = (m_teacherSceneInfo.dwRight != m_teacherSceneInfo.dwLeft)
		&& (m_teacherSceneInfo.dwBottom != m_teacherSceneInfo.dwTop);
	bool bStudentShow = (m_studentSceneInfo.dwRight != m_studentSceneInfo.dwLeft)
		&& (m_studentSceneInfo.dwBottom != m_studentSceneInfo.dwTop);
	bool bVGAShow = (m_deviceSceneInfo.dwRight != m_deviceSceneInfo.dwLeft)
		&& (m_deviceSceneInfo.dwBottom != m_deviceSceneInfo.dwTop);

	if (bVGAShow) {
		if (bTeacherShow || bStudentShow) {
			if (bTeacherShow && bStudentShow) {
				m_pKernel->getEdmaInstance()->copy((unsigned char*)m_exchangeImagePhyAddr, (unsigned char*)gbackTSVImagePhyAddr, m_outWidth * 2, m_outWidth*2,
							m_outWidth * m_outHeight * 2, 1);
				printf("after copy tsv image \n");
			} else {
				m_pKernel->getEdmaInstance()->copy((unsigned char*)m_exchangeImagePhyAddr, (unsigned char*)gbackTVImagePhyAddr, m_outWidth * 2, m_outWidth*2,
							m_outWidth * m_outHeight * 2, 1);
			}
		} else {
			m_pKernel->getEdmaInstance()->copy((unsigned char*)m_exchangeImagePhyAddr, (unsigned char*)gbackTImagePhyAddr, m_outWidth * 2, m_outWidth*2,
				m_outWidth * m_outHeight * 2, 1);
		}
	} else {
		if (bTeacherShow || bStudentShow) {
			if (bTeacherShow && bStudentShow) {
				m_pKernel->getEdmaInstance()->copy((unsigned char*)m_exchangeImagePhyAddr, (unsigned char*)gbackTSImagePhyAddr, m_outWidth * 2, m_outWidth*2,
						m_outWidth * m_outHeight * 2, 1);
			} else {
				m_pKernel->getEdmaInstance()->copy((unsigned char*)m_exchangeImagePhyAddr, (unsigned char*)gbackTImagePhyAddr, m_outWidth * 2, m_outWidth*2,
						m_outWidth * m_outHeight * 2, 1);
			}
		}
	}
}

bool MixtureMode_SDI::calcScalerWidthHeight() {
	bool bRet = false;
	int nOutScaleWidth0 = m_outScaleWidth[0];
	int nOutScaleHeight0 = m_outScaleHeight[0];
	int nOutScaleWidth1 = m_outScaleWidth[1];
	int nOutScaleHeight1 = m_outScaleHeight[1];
	int nOutVGAWidth = m_outVGAWidth;
	int nOutVGAHeight = m_outVGAHeight;

	memcpy(&m_teacherSceneInfo, m_pSetting->getTeacherSceneVideoInfo(), sizeof(SceneVideoInfo));
	memcpy(&m_studentSceneInfo, m_pSetting->getStudentSceneVideoInfo(), sizeof(SceneVideoInfo));
	memcpy(&m_deviceSceneInfo, m_pSetting->getDeviceSceneVideoInfo(), sizeof(SceneVideoInfo));

	m_outScaleWidth[0] = m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft;
	m_outScaleHeight[0] = m_teacherSceneInfo.dwBottom - m_teacherSceneInfo.dwTop;

	m_bDirectCopyChannel1 = false;
	m_bDirectCopyChannel2 = false;
	if (m_outScaleWidth[0] == HD_WIDTH && m_outScaleHeight[0] == HD_HEIGHT) {
		m_bDirectCopyChannel1 = true;
	}

	m_outScaleWidth[1] = m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft;
	m_outScaleHeight[1] = m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop;
	if (m_outScaleWidth[1] == HD_WIDTH && m_outScaleHeight[1] == HD_HEIGHT) {
		m_bDirectCopyChannel2 = true;
	}

	m_outVGAWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
	m_outVGAHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;

	m_bTeacherContainStud = false;
	m_bTeacherContainVGA = false;

	m_bStudContainTeacher = false;
	m_bStudContainVGA = false;

	m_bVGAContainTeacher = false;
	m_bVGAContainStud = false;

	bRet = 	(nOutScaleWidth0 != m_outScaleWidth[0])
	   || (nOutScaleHeight0 != m_outScaleHeight[0])
	   || (nOutScaleWidth1 != m_outScaleWidth[1])
	   || (nOutScaleHeight1 != m_outScaleHeight[1])
	   || (nOutVGAWidth != m_outVGAWidth)
	   || (nOutVGAHeight != m_outVGAHeight);
	return bRet;
}

bool MixtureMode_SDI::dynamicConfigScaleComponent() {
	OMX_CONFIG_VIDCHANNEL_RESOLUTION chResolution;
	m_vgaChangedMutex.lock();
	for (int i = 0; i < 3; ++i) {
		OMX_INIT_PARAM(&chResolution);
		if (2 == i) {
			chResolution.Frm0Width = UTIL_ALIGN(m_outVGAWidth, 16);
			chResolution.Frm0Height = m_outVGAHeight;

			chResolution.Frm0Pitch = UTIL_ALIGN(m_outVGAWidth * 2, 16);
		} else {
			chResolution.Frm0Width = UTIL_ALIGN(m_outScaleWidth[i], 16);
			chResolution.Frm0Height = m_outScaleHeight[i];

			chResolution.Frm0Pitch = UTIL_ALIGN(m_outScaleWidth[i] * 2, 16);
		}
		printf("scale out width %d, height %d\n", chResolution.Frm0Width, chResolution.Frm0Height);

		chResolution.Frm1Width = 0;
		chResolution.Frm1Height = 0;
		chResolution.Frm1Pitch = 0;
		chResolution.FrmStartX = 0;
		chResolution.FrmStartY = 0;
		chResolution.FrmCropWidth = 0;
		chResolution.FrmCropHeight = 0;
		chResolution.eDir = OMX_DirOutput;
		if (i == 1) {
			chResolution.nChId = 1;
		} else {
			chResolution.nChId = 0;
		}


		OMX_ERRORTYPE eError = OMX_ErrorNone;
		if (2 == i) {
			 eError = OMX_SetConfig (m_pVGAScaleHandle,
				(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
				&chResolution);
		} else {
			 eError = OMX_SetConfig (m_pSDIScaleHandle,
				(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
				&chResolution);
		}

		if (eError != OMX_ErrorNone) {
			printf("dynamicConfigScaleComponent failed\n");
			return false;
		}
	}
	m_vgaChangedMutex.unlock();
	printf("dynamicConfigScaleComponent sucess \n");
	return true;
}

bool MixtureMode_SDI::dynamicConfigVGAScaleComponent() {
	OMX_CONFIG_VIDCHANNEL_RESOLUTION chResolution;
	OMX_ERRORTYPE eError;
	OMX_INIT_PARAM(&chResolution);

		chResolution.Frm0Width = UTIL_ALIGN(m_inVGAWidth, 16);
		chResolution.Frm0Height = m_inVGAHeight;
		chResolution.Frm0Pitch = UTIL_ALIGN(m_inVGAWidth, 16);
		chResolution.Frm1Width = 0;
		chResolution.Frm1Height = 0;
		chResolution.Frm1Pitch = 0;
		chResolution.FrmStartX = 0;
		chResolution.FrmStartY = 0;
		chResolution.FrmCropWidth = m_inVGAWidth;
		chResolution.FrmCropHeight = m_inVGAHeight;
		chResolution.eDir = OMX_DirInput;
		chResolution.nChId = 0;

		eError = OMX_SetConfig(m_pVGAScaleHandle,
				(OMX_INDEXTYPE ) OMX_TI_IndexConfigVidChResolution, &chResolution);
		if (eError != OMX_ErrorNone) {
			return false;
		}

		chResolution.Frm0Width = UTIL_ALIGN(m_outVGAWidth, 16);
		chResolution.Frm0Height = m_outVGAHeight;
		chResolution.Frm0Pitch = UTIL_ALIGN(m_outVGAWidth * 2, 16);

		chResolution.Frm1Width = 0;
		chResolution.Frm1Height = 0;
		chResolution.Frm1Pitch = 0;
		chResolution.FrmStartX = 0;
		chResolution.FrmStartY = 0;
		chResolution.FrmCropWidth = 0;
		chResolution.FrmCropHeight = 0;
		chResolution.eDir = OMX_DirOutput;
		chResolution.nChId = 0;

		eError = OMX_SetConfig (m_pVGAScaleHandle,
				(OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
				&chResolution);
		if (eError != OMX_ErrorNone) {
			printf("dynamicConfigScaleComponent failed\n");
			return false;
		}
	printf("dynamicConfigScaleComponent sucess \n");
	return true;
}


void MixtureMode_SDI::openFileHandles() {
	if (NULL == m_mp4OutputHandle) {
		m_mp4OutputHandle = new Mp4Output();
		m_mp4OutputHandle->init(m_strMp4FileName, m_outWidth, m_outHeight,
					m_nFrameRate, 2, m_pSetting->getAudioSampleRate(), 16);
		m_mp4Mutex.init();
	}
	char buf[255] = {0};

	if (-1 == m_aacfd) {
		sprintf(buf, "%s.aac", m_strMp4FileName.c_str());
		m_aacfd = open(buf, O_LARGEFILE | O_APPEND | O_RDWR | O_CREAT, 0666);
	}
	if (-1 == m_recordH264fd) {
		sprintf(buf, "%s.h264", m_strMp4FileName.c_str());
		m_recordH264fd = open(buf, O_LARGEFILE | O_APPEND | O_RDWR | O_CREAT, 0666);
	}
	if (m_pSetting->getVGAEncodeEnable()) {
		if (-1 == m_VGAH264fd) {
			sprintf(buf, "%s.vga.h264", m_strMp4FileName.c_str());
			m_VGAH264fd = open(buf, O_LARGEFILE | O_APPEND | O_RDWR | O_CREAT, 0666);
		}
	}

	for (int i = 0; i < m_maxDecode; ++i) {
		if (-1 == m_resourceH264fd[i]) {
			sprintf(buf, "%s_%d.H264", m_strMp4FileName.c_str(), i);
			m_resourceH264fd[i] = open(buf, O_LARGEFILE | O_APPEND | O_RDWR | O_CREAT, 0666);
		}
	}

	if (m_pSetting->getLiveEncEnable() ) {
		if (-1 == m_liveH264fd) {
			sprintf(buf, "%s_live.h264", m_strMp4FileName.c_str());
			m_liveH264fd = open(buf, O_LARGEFILE | O_APPEND | O_RDWR | O_CREAT, 0666);
		}
	}
}

void MixtureMode_SDI::closeFileHandles() {
	if (NULL != m_mp4OutputHandle) {
		m_mp4OutputHandle->uninit();
		delete m_mp4OutputHandle;
		m_mp4OutputHandle = NULL;
		m_mp4Mutex.uninit();
		printf("mp4 unint \n");
	}
	for (int i = 0; i < m_maxDecode; ++i) {
		if (-1 != m_resourceH264fd[i]) {
			close(m_resourceH264fd[i]);
			m_resourceH264fd[i] = -1;
		}
	}
	if (-1 != m_liveH264fd) {
		close(m_liveH264fd);
		m_liveH264fd = -1;
	}
	if (-1 != m_aacfd) {
		close(m_aacfd);
		m_aacfd = -1;
	}
	if (-1 != m_recordH264fd) {
		close(m_recordH264fd);
		m_recordH264fd = -1;
	}
	if (-1 != m_VGAH264fd) {
		close(m_VGAH264fd);
		m_VGAH264fd = -1;
	}
}

bool MixtureMode_SDI::initVGAComponent() {
	if (!initVGAScaleOMXHandle() ) {
		return false;
	}

	if (!OMXComponentUtils::changeComponentState(m_vgaScaleComponent, OMX_StateIdle)) {
		return false;
	}

	OMX_ERRORTYPE eError = OMX_ErrorNone;
	for (OMX_U32 j = 0; j < m_vgaScaleComponent->inPortParams->nBufferCountActual; j++) {
		eError = OMX_AllocateBuffer (m_pVGAScaleHandle,
				&m_vgaScaleComponent->inPortParams->pInBuff[j],
				OMX_VFPC_INPUT_PORT_START_INDEX,
				this,
				m_vgaScaleComponent->inPortParams->nBufferSize);

		if (eError != OMX_ErrorNone) {
			printf("can not use buffer index %d ,error %x\n", j, eError);
			return false;
		}
	}

	for (OMX_U32 j = 0; j < m_vgaScaleComponent->outPortParams->nBufferCountActual; j++) {
		m_pHeapBuffer = Memory_alloc (m_heap,
					m_vgaScaleComponent->outPortParams->nBufferSize, 128, NULL);
			if (m_pHeapBuffer == NULL) {
				printf ("Buffer Allocation Failed \n");
				return false;
			}
			eError = OMX_UseBuffer (m_pVGAScaleHandle,
                    &m_vgaScaleComponent->outPortParams->pOutBuff[j],
                    OMX_VFPC_OUTPUT_PORT_START_INDEX, this,
                    m_vgaScaleComponent->outPortParams->nBufferSize,
                    (OMX_U8*)m_pHeapBuffer);
			if (eError != OMX_ErrorNone) {
				return false;
			}
	}
	printf("vga scalar output port alloc buffer done\n");
	OMXComponentUtils::waitStateSem(m_vgaScaleComponent);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	if (!OMXComponentUtils::changeComponentState(m_vgaScaleComponent, OMX_StateExecuting)) {
		printf("vga scale to executing failed\n");
		return false;
	}
	OMXComponentUtils::waitStateSem(m_vgaScaleComponent);

	if (m_pSetting->getVGAEncodeEnable()) {
		if (!initVGAEncOMXHandle()) {
			printf("init vga encode failed\n");
			return false;
		}
		printf("init vga enc omx handle ok\n");
		if (!OMXComponentUtils::changeComponentState(m_vgaEncComponent, OMX_StateIdle)) {
			return false;
		}

		for (OMX_U32 j = 0; j < m_vgaEncComponent->inPortParams->nBufferCountActual; ++j) {
			eError = OMX_AllocateBuffer (m_pVGAEncHandle,
					&m_vgaEncComponent->inPortParams->pInBuff[j],
					OMX_VIDENC_INPUT_PORT,
					this,
					m_vgaEncComponent->inPortParams->nBufferSize);

			if (eError != OMX_ErrorNone) {
				printf("can not use buffer index %d ,error %x\n", j, eError);
				return false;
			}
		}

		for (OMX_U32 j = 0; j < m_vgaEncComponent->outPortParams->nBufferCountActual; ++j) {
			eError = OMX_AllocateBuffer (m_pVGAEncHandle,
						&m_vgaEncComponent->outPortParams->pOutBuff[j],
						OMX_VIDENC_OUTPUT_PORT, this,
						m_vgaEncComponent->outPortParams->nBufferSize);
			if (eError != OMX_ErrorNone) {
				return false;
			}
		}
		OMXComponentUtils::waitStateSem(m_vgaEncComponent);
		printf("after allocate vga enc buffer\n");
		if (!OMXComponentUtils::changeComponentState(m_vgaEncComponent, OMX_StateExecuting)) {
			return false;
		}
		OMXComponentUtils::waitStateSem(m_vgaEncComponent);
	}
	return true;
}

bool MixtureMode_SDI::uninitVGAComponent() {
	OMXComponentUtils::freeInOutParams(m_vgaScaleComponent);
	if (m_pSetting->getVGAEncodeEnable()) {
		OMXComponentUtils::freeInOutParams(m_vgaEncComponent);
	}
	OMXComponentUtils::uninitComponent(m_vgaScaleComponent);
	if (m_pSetting->getVGAEncodeEnable()) {
		OMXComponentUtils::uninitComponent(m_vgaEncComponent);
	}
	return true;
}

bool MixtureMode_SDI::stopVGAComponentThread() {
	OMX_S32 ret_value;
	m_bExitVGAScaleThread = true;
	int size = m_inVGAWidth * m_inVGAHeight * 3/2;
	unsigned char* pTempBuffer = new unsigned char[size];
	memset(pTempBuffer, 0, size);
	for (int i = m_nVGAScaleInputIndex; i < m_vgaScaleComponent->inPortParams->nBufferCountActual + 1; ++i) {
		setVGAScaleData(pTempBuffer, size);
		usleep(40000);
	}
	OMX_BUFFERHEADERTYPE pBufferOut;
	write(m_vgaScaleComponent->outPortParams->opBufPipe[1],	&pBufferOut, sizeof(pBufferOut));

	SempUtils::semp_pend(m_vgaScaleComponent->eos_sem);
	printf("after exit vga scale exit\n");
	m_bExitVGAEncodeThread = true;

	if (m_pSetting->getVGAEncodeEnable()) {
		for (int i = m_nVGAEncodeInputIndex; i < m_vgaEncComponent->inPortParams->nBufferCountActual + 1; ++i) {
			setVGAEncodeData(pTempBuffer, size);
			usleep(40000);
		}
		write(m_vgaEncComponent->outPortParams->opBufPipe[1],	&pBufferOut, sizeof(pBufferOut));

		SempUtils::semp_pend(m_vgaEncComponent->eos_sem);
		printf("after exit vga encode thread\n");
	}

	delete []pTempBuffer;

	OMX_ERRORTYPE eError = OMX_SendCommand(m_pVGAScaleHandle, OMX_CommandStateSet,
			OMX_StateIdle, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	OMXComponentUtils::waitStateSem(m_vgaScaleComponent);

	if (m_pSetting->getVGAEncodeEnable()) {
		eError = OMX_SendCommand(m_pVGAEncHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (eError != OMX_ErrorNone) {
			return false;
		}
		OMXComponentUtils::waitStateSem(m_vgaEncComponent);
	}
	eError = OMX_SendCommand(m_pVGAScaleHandle, OMX_CommandStateSet,
			OMX_StateLoaded, NULL);
	if (eError != OMX_ErrorNone) {
		return false;
	}
	if (!freeVGAScaleBuffers() ){
		return false;
	}

	if (m_pSetting->getVGAEncodeEnable()) {
		eError = OMX_SendCommand(m_pVGAEncHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
		if (OMX_ErrorNone != eError) {
			return false;
		}
		if (!freeVGAEncBuffers() ) {
			return false;
		}
	}

	eError = OMX_FreeHandle(m_pVGAScaleHandle);
	if (eError != OMX_ErrorNone) {
		return false;
	}

	if (m_pSetting->getVGAEncodeEnable()) {
		eError = OMX_FreeHandle(m_pVGAEncHandle);
		if (OMX_ErrorNone != eError) {
			return false;
		}
	}
	pthread_join(m_vgaScaleComponent->outDataStrmThrdId, NULL);
	if (m_pSetting->getVGAEncodeEnable()) {
		printf("before wait end vga enc thread\n");
		pthread_join(m_vgaEncComponent->outDataStrmThrdId, NULL);
		printf("after wait end vga enc thread\n");
	}
	return true;
}

bool MixtureMode_SDI::startVGAComponentThread() {
	m_bExitVGAScaleThread = false;
	m_bExitVGAEncodeThread = false;

	pthread_attr_init(&m_vgaScaleComponent->ThreadAttr);
	OMXComponentUtils::initialOutputResources(m_vgaScaleComponent);
	if (0 != pthread_create(&m_vgaScaleComponent->outDataStrmThrdId,
			&m_vgaScaleComponent->ThreadAttr,
			VGAScaleOutWriteTask,
			this)) {
		return false;
	}
	printf("start vga scale component thread\n");


	if (m_pSetting->getVGAEncodeEnable()) {
		pthread_attr_init(&m_vgaEncComponent->ThreadAttr);
		OMXComponentUtils::initialOutputResources(m_vgaEncComponent);
		if (0
				!= pthread_create(&m_vgaEncComponent->outDataStrmThrdId,
						&m_vgaEncComponent->ThreadAttr, VGAEncodeOutWriteTask,
						this)) {
			return false;
		}
		printf("after create vga encode out thread\n");
	}
	return true;
}

