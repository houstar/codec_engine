#include "subVideoManager.h"
#include "../kernel.h"

void SubVideoManager::onH264(KVideoInterface* video, int channel, int frameType,
		void* data, int len) {
	if (video == m_pTeacherSubInterface) {
		m_pKernel->getLiveCourseManager()->sendData(m_pKernel->getSetting()->getLCTeacherPath().c_str(),
				(char*)data, len, MTG_DATA_TYPE_VIDEO, eLCTeacherVideo);

	} else if (video == m_pStudentSubInterface) {
		m_pKernel->getLiveCourseManager()->sendData(m_pKernel->getSetting()->getLCStudentPath().c_str(),
				(char*)data, len, MTG_DATA_TYPE_VIDEO, eLCStudentVideo);
	}
}


SubVideoManager::SubVideoManager()
: m_pKernel(NULL)
, m_pTeacherSubInterface(NULL)
, m_pStudentSubInterface(NULL) {

}

SubVideoManager::~SubVideoManager() {
	uninit();
}

bool SubVideoManager::init(Kernel* pKernel) {
	m_pKernel = pKernel;
	memcpy(&m_IPCChannel, m_pKernel->getSetting()->getIPCChannel(), sizeof(PRChannel));
	return true;
}

bool SubVideoManager::uninit() {
	closeSubVideos();
	return true;
}


bool SubVideoManager::openSubVideos() {
	if (NULL == m_pTeacherSubInterface) {
		IPCInfo* info = m_pKernel->getSetting()->getIPCInfo(m_pKernel->getSetting()->getIPCChannel()->dwTeacherChannel);
		if (NULL != info) {
			m_pTeacherSubInterface = KVideoCreatorFactory::createInstance("HikVision");
			m_pTeacherSubInterface->setChannelID(0);
			m_pTeacherSubInterface->addListener(this);
			m_pTeacherSubInterface->getCardInfo()->setCardDescription(info->cIPCTypeName);
			char ipBuf[32] = {0};
			sprintf(ipBuf, "%d.%d.%d.%d", (info->dwIPCIP >> 24) & 0xFF,
							(info->dwIPCIP >> 16) & 0xFF,
							(info->dwIPCIP >> 8) & 0xFF,
							(info->dwIPCIP) & 0xFF );
			printf("the teacher ip is %s\n", ipBuf);
			m_pTeacherSubInterface->getCardInfo()->setIP(ipBuf);
			if (m_pTeacherSubInterface->open(true)) {
				printf("open teacher sub video ok\n");
			} else {
				printf("open teacher sub video failed\n");
				//return false;
			}
		}
	}

	if (NULL == m_pStudentSubInterface) {
		IPCInfo* info = m_pKernel->getSetting()->getIPCInfo(m_pKernel->getSetting()->getIPCChannel()->dwStudentChannel);
		if (NULL != info) {
			m_pStudentSubInterface = KVideoCreatorFactory::createInstance("HikVision");
			m_pStudentSubInterface->setChannelID(0);
			m_pStudentSubInterface->addListener(this);
			m_pStudentSubInterface->getCardInfo()->setCardDescription(info->cIPCTypeName);
			char ipBuf[32] = {0};
			sprintf(ipBuf, "%d.%d.%d.%d", (info->dwIPCIP >> 24) & 0xFF,
							(info->dwIPCIP >> 16) & 0xFF,
							(info->dwIPCIP >> 8) & 0xFF,
							(info->dwIPCIP) & 0xFF );
			printf("the student ip is %s\n", ipBuf);
			m_pStudentSubInterface->getCardInfo()->setIP(ipBuf);
			if (m_pStudentSubInterface->open(true)) {
				printf("open student sub video ok\n");
			} else {
				printf("open student sub video failed\n");
				//return false;
			}
		}
	}
	return true;
}

bool SubVideoManager::closeSubVideos() {
	if (NULL != m_pTeacherSubInterface) {
		KVideoCreatorFactory::destroyInstance(m_pTeacherSubInterface);
	}
	if (NULL != m_pStudentSubInterface) {
		KVideoCreatorFactory::destroyInstance(m_pStudentSubInterface);
	}
	return true;
}

