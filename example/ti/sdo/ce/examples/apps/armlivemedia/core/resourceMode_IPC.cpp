#include "resourceMode_IPC.h"
#include "OMXComponentUtils.h"
#include "msgq.h"
#include "../utils/commonUtils.h"
#include "../kernel.h"
#include "../constStringDefine.h"
#include "audioEncodeFactory.h"

bool ResourceModeHandle_IPC::onProviderNV12(unsigned char* pNV12Buffer, int width, int height) {
	return true;
}

bool ResourceModeHandle_IPC::onProviderYUYV(unsigned char* pYUYVBuffer, int width, int height) {
	return true;
}

bool ResourceModeHandle_IPC::onProviderH264(unsigned char* pH264Buffer, int size) {
	if (NULL != m_pResourceMP4File[2]) {
		m_pResourceMP4File[2]->writeH264Video((char*)pH264Buffer, size);
	}
	return true;
}

bool ResourceModeHandle_IPC::onVgaStatus(EVGAStatus status) {
	return true;
}

void ResourceModeHandle_IPC::onTimeout(Timer* pTimer) {
}


void ResourceModeHandle_IPC::onAACEncoded(unsigned char* pAACBuf, int len) {
	if (m_bPaused) {
		return;
	}
	if (-1 != m_aacfd) {
		write(m_aacfd, pAACBuf, len);
	}
	for (int i = 0; i < KResourceNum; ++i) {
		m_pResourceMP4File[i]->writeAACAudio((char*)pAACBuf, len);
	}
}

void ResourceModeHandle_IPC::OnAAC(char* pAACBuf, int len) {
	if (m_bPaused) {
		return;
	}
	if (-1 != m_aacfd) {
		write(m_aacfd, pAACBuf, len);
	}
	for (int i = 0; i < KResourceNum; ++i) {
		m_pResourceMP4File[i]->writeAACAudio((char*)pAACBuf, len);
	}
}

void ResourceModeHandle_IPC::OnSpeex(char* spx, int len) {
	m_pKernel->getLiveCourseManager()->sendData(m_pSetting->getLCSpeexPath().c_str(),
			(const char*)spx, len, MTG_DATA_TYPE_AUDIO, eLCSpeexAudio);
}

bool ResourceModeHandle_IPC::onNotifyStreamVideo(const char* pData, int len, bool bIFrame, int index, int channel) {
	if (m_bPaused || index < 0 || index > KResourceNum) {
		return false;
	}

	if (NULL != m_pResourceMP4File[index]) {
		m_pResourceMP4File[index]->writeH264Video((char*)pData, len);
	}
	return true;
}

ResourceModeHandle_IPC::ResourceModeHandle_IPC(Kernel* pKernel)
: BaseMode(pKernel)
, m_pAudioEncode(NULL) {
	m_bExit = false;
	m_bPaused = false;


	for (int i = 0; i < KResourceNum; ++i) {
		m_pResourceMP4File[i] = NULL;
	}

	m_rtmpHandle = NULL;

	m_aacfd = -1;
}

ResourceModeHandle_IPC::~ResourceModeHandle_IPC() {
	uninit();
}

BOOL ResourceModeHandle_IPC::init(Setting* pSetting) {
	if (NULL == pSetting) {
		return FALSE;
	}

	m_pSetting = pSetting;
	PRChannel* channel = m_pSetting->getIPCChannel();
	IPCInfo *info = m_pSetting->getIPCInfo(channel->dwTeacherChannel);
	if (NULL != info) {
		m_inHeight[0] = info->dwIPCHeight;
		m_inWidth[0] = info->dwIPCWidth;
	} else {
		m_inHeight[0] = 0;
		m_inWidth[0] = 0;
	}

	info = m_pSetting->getIPCInfo(channel->dwStudentChannel);
	if (NULL != info) {
		m_inHeight[1] = info->dwIPCHeight;
		m_inWidth[1] = info->dwIPCWidth;
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

#ifndef AUDIO_CODECENGINE
	m_pAudioEncode = AudioEncodeFactory::createInstance(true);
	m_pAudioEncode->addListener(this);
	m_pAudioEncode->init(2, m_pSetting->getAudioSampleRate(), 16, m_pSetting->getAACBitRate()
			, m_pSetting->getPCMNonBlock(), m_pSetting->getAudioDeviceName());
#else
	m_pAudioEncode = new codec_engine();
	m_pAudioEncode->addListener(this);
#endif

	printf("after init audio encode\n");

	m_strMp4FileName = m_pSetting->getSaveFileName();

	m_inVGAWidth = 1024;
	m_inVGAHeight = 768;

	m_streamVideoManager.init(m_pKernel);
	m_streamVideoManager.addlistener(this);

	printf("resource mode init ok\n");
	return TRUE;
}

BOOL ResourceModeHandle_IPC::uninit() {
	if (!m_bExit) {
		return FALSE;
	}
	m_bExit = false;
	printf("resource mode uninit\n");
	return TRUE;
}

BOOL ResourceModeHandle_IPC::start() {
	if (m_bExit) {
		return FALSE;
	}

	openFileHandles();
	m_pKernel->getVGAProviderSystemInstance()->addListener(this);


#ifndef CODEC_ENGINE_ENABLE
	if (NULL != m_pAudioEncode) {
		m_pKernel->getAudioCaptureInstance()->addListener(m_pAudioEncode);
		m_pAudioEncode->start();
	}
#endif
	printf("after audio encode start\n");
	m_streamVideoManager.openVideos();
	return TRUE;
}

BOOL ResourceModeHandle_IPC::stop() {
	m_pKernel->getVGAProviderSystemInstance()->removeListener(this);

	printf("before stop audio encode\n");
#ifndef CODEC_ENGINE_ENABLE
	if (NULL != m_pAudioEncode) {
		m_pKernel->getAudioCaptureInstance()->removeListener(m_pAudioEncode);

		m_pAudioEncode->stop();
		delete m_pAudioEncode;
		m_pAudioEncode = NULL;
	}
#endif
	m_streamVideoManager.closeVideos();
	printf("close videos\n");

	closeFileHandles();
	printf("uninit mp4 rtmp handle\n");


	uninit();
	printf("after stop\n");
	return TRUE;
}

BOOL ResourceModeHandle_IPC::pause() {
	m_bPaused = true;
	return TRUE;
}

BOOL ResourceModeHandle_IPC::resume() {
	m_bPaused = false;
	return TRUE;
}

BOOL ResourceModeHandle_IPC::doMovieHead() {
	return TRUE;
}

BOOL ResourceModeHandle_IPC::doMovieTail() {
	return TRUE;
}

bool ResourceModeHandle_IPC::switchScene() {
	return true;
}

bool ResourceModeHandle_IPC::isComponentCreated() {
	return true;
}

void ResourceModeHandle_IPC::openFileHandles() {
	char buf[255] = {0};
	for (int i = 0; i < KResourceNum; ++i) {
		if (NULL == m_pResourceMP4File[i]) {
			sprintf(buf, "%s_%d.mp4", m_strMp4FileName.c_str(), i);
			m_pResourceMP4File[i] = new Mp4Output();
			if (i < 2) {
#ifndef CODEC_ENGINE_ENABLE
				m_pResourceMP4File[i]->init(buf, m_inWidth[i], m_inHeight[i],
					25, 2, m_pSetting->getAudioSampleRate(), 16);
#else
				m_pResourceMP4File[i]->init(buf, m_inWidth[i], m_inHeight[i],
					25, 1, m_pSetting->getAudioSampleRate(), 16);
#endif
			} else {
#ifndef CODEC_ENGINE_ENABLE
				m_pResourceMP4File[i]->init(buf, m_inVGAWidth, m_inVGAHeight,
						25, 2, m_pSetting->getAudioSampleRate(), 16);
#else
				m_pResourceMP4File[i]->init(buf, m_inVGAWidth, m_inVGAHeight,
						25, 1, m_pSetting->getAudioSampleRate(), 16);
#endif
			}
		}
	}
	if (-1 == m_aacfd) {
		sprintf(buf, "%s.aac", m_strMp4FileName.c_str());
		m_aacfd = open(buf, O_LARGEFILE | O_APPEND | O_RDWR | O_CREAT, 0666);
	}
}

void ResourceModeHandle_IPC::closeFileHandles() {
	for (int i = 0; i < KResourceNum; ++i) {
		if (NULL != m_pResourceMP4File[i]) {
			m_pResourceMP4File[i]->uninit();
			delete m_pResourceMP4File[i];
			m_pResourceMP4File[i] = NULL;
		}
	}
	if (-1 != m_aacfd) {
		close(m_aacfd);
		m_aacfd = -1;
	}

	printf("mp4 uninit\n");
}

