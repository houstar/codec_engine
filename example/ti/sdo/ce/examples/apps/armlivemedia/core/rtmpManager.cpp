#include "rtmpManager.h"

RtmpManager::RtmpManager()
: m_pSetting(NULL)
, m_rtmpHandle(NULL)
, m_bLCLive(false){

}

RtmpManager::~RtmpManager() {

}

bool RtmpManager::init(Setting* pSetting) {
	m_pSetting = pSetting;
	return true;
}

bool RtmpManager::setLCLive(bool bLCLive) {
	m_bLCLive = bLCLive;
	return true;
}

bool RtmpManager::uninit() {
	if (NULL != m_rtmpHandle) {
		m_rtmpHandle->uninit();
		delete m_rtmpHandle;
		m_rtmpHandle = NULL;
	}
	return true;
}

bool RtmpManager::startRtmp() {
	if (m_bLCLive) {
		if (NULL == m_rtmpHandle) {
			printf("before rtmp handle init lc live\n");
			m_rtmpHandle = new RtmpHandle();
			m_rtmpHandle->init(m_pSetting->getLCRtmpUrl().c_str(),
					m_pSetting->getLiveOutWidth(),
					m_pSetting->getLiveOutHeight(),
					m_pSetting->getVideoFrameRate(),
					1, //2,
					m_pSetting->getAudioSampleRate(),
					16);
			printf("after rtmp handle init\n");
		}
	} else {
		if (m_pSetting->getLiveEncEnable() ) {
			if (m_pSetting->getRtmpEnable() && NULL == m_rtmpHandle) {
				m_rtmpHandle = new RtmpHandle();
				printf("before rtmp handle init record\n");
				m_rtmpHandle->init(m_pSetting->getInterRtmpUrl().c_str(),
					m_pSetting->getLiveOutWidth(),
					m_pSetting->getLiveOutHeight(),
					m_pSetting->getVideoFrameRate(),
					1, //2,
					m_pSetting->getAudioSampleRate(),
					16);
				printf("after rtmp handle init\n");
			}
		}
	}

	return true;
}

bool RtmpManager::stopRtmp() {
	if (NULL != m_rtmpHandle) {
		m_rtmpHandle->uninit();
		delete m_rtmpHandle;
		m_rtmpHandle = NULL;
	}
	return true;
}

bool RtmpManager::pushAudio(const char* pAACBuf, int len) {
	if (NULL == m_rtmpHandle) {
		return false;
	}
	return m_rtmpHandle->pushAudio((char*)pAACBuf, len);
}

bool RtmpManager::pushVideo(const char* pH264Buf, int len) {
	if (NULL == m_rtmpHandle) {
		return false;
	}
	return m_rtmpHandle->pushVideo((char*)pH264Buf, len);
}
