#include "streamVideoManager.h"
#include "../kernel.h"


void StreamVideoManager::onH264(KVideoInterface* video, int channel, int frameType,
		void* data, int len) {
	int nIndex = -1;
	for (int i = 0; i < MAX_IPC; ++i) {
		if (video == m_pVideoInterface[i]) {
			nIndex = i;
			break;
		}
	}
	if (nIndex == -1) {
		return;
	}
	m_IPCChannelMutex.lock();
	if (m_bTeacherChannelChanged) {
		if (frameType == 0) {
			if (nIndex == m_IPCChannel.dwTeacherChannel) {
				m_bTeacherChannelChanged = false;
				m_OldIPCChannel.dwTeacherChannel = nIndex;
				m_pKernel->getSetting()->setOldIPCTeacherChannel(nIndex);
				m_IPCChannelMutex.unlock();
				sendListenerStreamVideo((const char*)data, len, frameType == 0, 0, nIndex, true);
				printf("teacher get IFrame\n");
				return;
			} else if (nIndex == m_OldIPCChannel.dwTeacherChannel) {
				m_IPCChannelMutex.unlock();
				sendListenerStreamVideo((const char*)data, len, frameType == 0, 0, nIndex);
				return;
			} else {
				m_IPCChannelMutex.unlock();
			}
		} else {
			if (nIndex == m_OldIPCChannel.dwTeacherChannel) {
				m_IPCChannelMutex.unlock();
				sendListenerStreamVideo((const char*)data, len, frameType == 0, 0, nIndex);
				return;
			} else {
				m_IPCChannelMutex.unlock();
			}
		}
	}
	if (m_bStudentChannelChanged) {
		if (frameType == 0) {
			if (nIndex == m_IPCChannel.dwStudentChannel) {
				m_bStudentChannelChanged = false;
				m_OldIPCChannel.dwStudentChannel = nIndex;
				m_pKernel->getSetting()->setOldIPCStudentChannel(nIndex);
				m_IPCChannelMutex.unlock();
				sendListenerStreamVideo((const char*)data, len, frameType == 0, 1, nIndex, true);
				printf("student get IFrame\n");
				return;
			} else if (nIndex == m_OldIPCChannel.dwStudentChannel) {
				m_IPCChannelMutex.unlock();
				sendListenerStreamVideo((const char*)data, len, frameType == 0, 1, nIndex);
				return;
			} else {
				m_IPCChannelMutex.unlock();
			}
		} else {
			if (nIndex == m_OldIPCChannel.dwStudentChannel) {
				m_IPCChannelMutex.unlock();
				sendListenerStreamVideo((const char*)data, len, frameType == 0, 1, nIndex);
				return;
			} else {
				m_IPCChannelMutex.unlock();
			}
		}
	}

	if (!m_bTeacherChannelChanged && !m_bStudentChannelChanged) {
		if (nIndex == m_IPCChannel.dwTeacherChannel || nIndex == m_IPCChannel.dwStudentChannel ) {
			int bTeacher = (nIndex == m_IPCChannel.dwTeacherChannel) ? 0 : 1;
			m_IPCChannelMutex.unlock();
			sendListenerStreamVideo((const char*)data, len, frameType == 0, bTeacher, nIndex);
		} else {
			m_IPCChannelMutex.unlock();
		}
	}
}

StreamVideoManager::StreamVideoManager()
: m_pKernel(NULL)  {
	for (int i = 0; i < MAX_IPC; ++i) {
		m_pVideoInterface[i] = NULL;
	}

	m_bTeacherChannelChanged = false;
	m_bStudentChannelChanged = false;
}

StreamVideoManager::~StreamVideoManager() {
	uninit();
	m_IPCChannelMutex.uninit();
}

bool StreamVideoManager::init(Kernel * pKernel) {
	m_pKernel = pKernel;
	memcpy(&m_IPCChannel, m_pKernel->getSetting()->getIPCChannel(), sizeof(PRChannel));
	memcpy(&m_OldIPCChannel, m_pKernel->getSetting()->getOldIPCChannel(), sizeof(PRChannel));

	m_IPCChannelMutex.init();

	m_bTeacherChannelChanged = false;
	m_bStudentChannelChanged = false;
	return true;
}

bool StreamVideoManager::uninit() {
	closeVideos();
	return true;
}

bool StreamVideoManager::openVideos() {
	for (int i = 0; i < MAX_IPC; ++i) {
		IPCInfo* info = m_pKernel->getSetting()->getIPCInfo(i);
		if (NULL != info) {
			m_pVideoInterface[i] = KVideoCreatorFactory::createInstance("HikVision");
			if (NULL != m_pVideoInterface[i]) {
				m_pVideoInterface[i]->setChannelID(0);
				m_pVideoInterface[i]->addListener(this);
				m_pVideoInterface[i]->getCardInfo()->setCardDescription(info->cIPCTypeName);
				char ipBuf[32] = {0};
				sprintf(ipBuf, "%d.%d.%d.%d", (info->dwIPCIP >> 24) & 0xFF,
						(info->dwIPCIP >> 16) & 0xFF,
						(info->dwIPCIP >> 8) & 0xFF,
						(info->dwIPCIP) & 0xFF);
				m_pVideoInterface[i]->getCardInfo()->setIP(ipBuf);
				printf("the camera ip is %s\n", ipBuf);
				if (m_pVideoInterface[i]->open(info->dwSubCode)) {
					printf("open hikvision video ok index %d, subcode %d\n", i, info->dwSubCode);
				} else {
					printf("open hikvision video failed index %d, subcode %d\n", i, info->dwSubCode);
				}
			}
		}
	}


	return true;
}

bool StreamVideoManager::closeVideos() {
	for (int i = 0; i < MAX_IPC; ++i) {
		if (NULL != m_pVideoInterface[i]) {
			KVideoCreatorFactory::destroyInstance(m_pVideoInterface[i]);
		}
	}

	return true;
}

bool StreamVideoManager::IPCChannelChanged() {
	if (m_IPCChannel.dwTeacherChannel == m_pKernel->getSetting()->getIPCChannel()->dwTeacherChannel
			&& m_IPCChannel.dwStudentChannel == m_pKernel->getSetting()->getIPCChannel()->dwStudentChannel) {
		return false;
	}
	m_IPCChannelMutex.lock();
	if (m_IPCChannel.dwStudentChannel != m_pKernel->getSetting()->getIPCChannel()->dwStudentChannel) {
		m_bStudentChannelChanged = true;
	}
	if (m_IPCChannel.dwTeacherChannel != m_pKernel->getSetting()->getIPCChannel()->dwTeacherChannel) {
		m_bTeacherChannelChanged = true;
	}

	memcpy(&m_IPCChannel, m_pKernel->getSetting()->getIPCChannel(), sizeof(PRChannel));
	memcpy(&m_OldIPCChannel, m_pKernel->getSetting()->getOldIPCChannel(),sizeof(PRChannel));

	m_IPCChannelMutex.unlock();
	return true;
}

bool StreamVideoManager::addlistener(IStreamVideoManagerListener* plistener) {
	for (vector<IStreamVideoManagerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
		if ( *it == plistener) {
			return false;
		}
	}
	m_listeners.push_back(plistener);
	return true;
}

bool StreamVideoManager::removelistener(IStreamVideoManagerListener* plistener) {
	for (vector<IStreamVideoManagerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
		if (*it == plistener) {
			m_listeners.erase(it);
			return true;
		}
	}
	return true;
}

bool StreamVideoManager::sendListenerStreamVideo(const char* pData, int len, bool bIFrame,
		int index, int channel, bool bChannelChanged) {
	for (vector<IStreamVideoManagerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
		(*it)->onNotifyStreamVideo((const char*)pData, len, bIFrame, index, channel, bChannelChanged);
	}
	return true;

}

bool StreamVideoManager::getTeacherChannelChanged() {
	return m_bTeacherChannelChanged;
}

bool StreamVideoManager::getStudentChannelChanged() {
	return m_bStudentChannelChanged;
}
