#include "liveCourseManager.h"
#include "livemtgConstDefine.h"
#include "../kernel.h"
#include "../KVideo/audio/codec_engine.h"

bool LiveCourseManager::onRecvLCRole(ELiveCourseRole role) {
#ifdef CODEC_ENGINE_ENABLE
	// audio stop->open
	if (NULL != m_pKernel->getCodecEngineInstance()) {
		m_pKernel->getCodecEngineInstance()->changestate();
		if (role == eLCRoleLive || role == eLCRoleAudence) {
			m_pKernel->getCodecEngineInstance()->startEchoCancel();
			m_pKernel->getCodecEngineInstance()->setMixRemoteEnable(true);
		}
	}
#endif	
	m_courseWxManager.setRole(role);
	if (role == eLCRoleLive || role == eLCRoleAudence) {
		m_pKernel->doStartLC(role);
	}
	m_pKernel->getHDMIDisplayManagerInstance()->setLiveCourseRole(role);
	m_pKernel->getRtmpManager()->setLCLive(role == eLCRoleLive);
	if (role == eLCRoleLive || role == eLCRoleAudence) {
			m_subVideoManager.init(m_pKernel);
			m_subVideoManager.openSubVideos();
	}
	return true;
}

bool LiveCourseManager::onCourseWxLeave() {
#ifdef CODEC_ENGINE_ENABLE
	// audio stop->open
	if (NULL != m_pKernel->getCodecEngineInstance()) {
		m_pKernel->getCodecEngineInstance()->changestate();
		m_pKernel->getCodecEngineInstance()->clearmembers();
		m_pKernel->getCodecEngineInstance()->stopEchoCancel();
		m_pKernel->getCodecEngineInstance()->setMixRemoteEnable(false);
	}
#endif
	
	m_pKernel->getHDMIDisplayManagerInstance()->setLiveCourseRole(eLCRoleNone);
	m_pKernel->getRtmpManager()->setLCLive(false);

	m_subVideoManager.closeSubVideos();
	printf("close sub videos\n");
	return true;
}

bool LiveCourseManager::onRecvCourseStatus(int courseId, string strLCMtgIp) {
	m_courseWxManager.setCourseInfo(courseId, strLCMtgIp);
	return true;
}

void LiveCourseManager::OnSpeex(char* data, int len)
{
	sendData(m_pSetting->getLCSpeexPath().c_str(), data, len,MTG_DATA_TYPE_AUDIO, eLCSpeexAudio);
}

LiveCourseManager::LiveCourseManager()
: m_pKernel(NULL)
, m_pSetting(NULL)
{

}

LiveCourseManager::~LiveCourseManager() {
	uninit();
}

bool LiveCourseManager::init(Kernel* pKernel, Setting* pSetting) {
	if (NULL == pSetting) {
		return false;
	}
	m_pKernel = pKernel;
	m_pSetting = pSetting;

	m_mtgHeartbeat.init();
	m_mtgHeartbeat.startThread();

	m_classWxManager.addListener(this);
	m_classWxManager.init(m_pKernel, m_pSetting->getMtgIp(), m_pSetting->getClassRoomID());

	m_courseWxManager.addListener(this);
	m_courseWxManager.init(m_pSetting);
	return true;
}

bool LiveCourseManager::uninit() {
	m_classWxManager.uninit();

	m_courseWxManager.uninit();

	m_mtgHeartbeat.stopThread();
	m_mtgHeartbeat.uninit();
	return true;
}

bool LiveCourseManager::addListener(ILiveCourseManagerObserver* pObserver) {
	for (vector<ILiveCourseManagerObserver*>::iterator it = m_vecObservers.begin();
			it != m_vecObservers.end(); ++it) {
		if ( *it == pObserver) {
			return false;
		}
	}
	m_vecObservers.push_back(pObserver);
	return true;
}

bool LiveCourseManager::removeListener(ILiveCourseManagerObserver* pObserver) {
	for (vector<ILiveCourseManagerObserver*>::iterator it = m_vecObservers.begin();
			it != m_vecObservers.end(); ++it) {
		if ( *it == pObserver) {
			m_vecObservers.erase(it);
			return true;
		}
	}
	return true;
}


bool LiveCourseManager::sendData(const char* pPath, const char* pData, int dataLen,
		int dataType, ELiveCourseMediaType lcType) {
	if (lcType < 0 || lcType >= eLCMediaTypeCount) {
		return false;
	}
	m_courseWxManager.sendData(pPath, pData, dataLen, dataType, lcType);

	if (m_pSetting->getMtgEnable()) {
		m_classWxManager.sendData(pPath, pData, dataLen, dataType, lcType);
	}

	return true;
}

