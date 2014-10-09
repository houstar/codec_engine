#include "classWxManager.h"
#include "../utils/liveCourseUtils.h"
#include "livemtgConstDefine.h"
#include "../utils/commonUtils.h"
#include "../kernel.h"

void ClassWxManager::onRecvMtgData(MtgClient* pMtg, const char* path,
				const char*data, int dataLen, int datatype, int action) {
	if (datatype == MTG_DATA_TYPE_VIDEO) {
		m_pClassClient->subscribe(path, false);
	}
	if (datatype != MTG_DATA_TYPE_BINARY) {
		return;
	}

	if (NULL == path || NULL == data) {
		return;
	}
	if (strncmp(m_strEnterPath.c_str(), path, m_strEnterPath.length()) == 0) {
		procRecvEnterCmd(data, dataLen);
	} else if (strncmp(m_strCourseStatusPath.c_str(), path, m_strCourseStatusPath.length()) == 0) {
		procRecvCourseStatusCmd(data, dataLen);
	} else if (strncmp(m_strRtmpPath.c_str(), path, m_strRtmpPath.length()) == 0) {
		procRecvRtmpPathCmd(data, dataLen);
	}
}

void ClassWxManager::onJoinCallback(MtgClient* pMtg, bool bJoinOK) {
	if (!bJoinOK) {
		printf("class wx manager join failed\n");
		return;
	}
	printf("class wx manager join ok\n");
	fflush(stdout);

	m_pClassClient->createAudioThread();
	createPaths();

}

void ClassWxManager::onTimeout(Timer* pTimer) {
	if (pTimer == &m_timer) {
		m_mutex.lock();
		if (NULL != m_pClassClient) {
			if (!m_pClassClient->getJoinStatus()) {
				destroyPaths();
				m_pClassClient->uninit();
				delete m_pClassClient;
				printf("delete class client mtg\n");
				m_pClassClient = new MtgClient();
				int confID = LiveCourseUtils::generate_course_id(m_strRoomId);
				m_pClassClient->init(m_strMtgIp.c_str(), confID, m_userId);
				m_pClassClient->addListener(this);
				m_pClassClient->joinConf();
				printf("reconnect class wx\n");
			}
		}
		m_mutex.unlock();
	}
}

ClassWxManager::ClassWxManager()
: m_pKernel(NULL)
, m_pSetting(NULL)
, m_pClassClient(NULL)
, m_bTimerValid(false)
, m_userId(0) {
	m_mutex.init();
}

ClassWxManager::~ClassWxManager() {
	uninit();
	m_mutex.uninit();
}

bool ClassWxManager::init(Kernel* pKernel, string strMtgIp, string strRoomId) {
	m_pKernel = pKernel;
	if (NULL == m_pKernel) {
		return false;
	}
	m_pSetting = m_pKernel->getSetting();
	m_strMtgIp = strMtgIp;
	m_strRoomId = strRoomId;

	m_strEnterPath = string(LIVEMTG_PATH_CMD_ENTER) + string("/") + m_strRoomId;
	m_strLeavePath = string(LIVEMTG_PATH_CMD_LEAVE) + string("/") + m_strRoomId;
	m_strRtmpPath = string(LIVEMTG_PATH_CMD_RTMP) + string("/") + m_strRoomId;
	m_strCourseStatusPath = string(LIVEMTG_PATH_CMD_COURSESTATUS) + string("/") + m_strRoomId;

	m_strResponsePath = m_strEnterPath + string("/test");

	m_userId = LiveCourseUtils::generate_uid(m_strRoomId, LIVEMTG_USER_TYPE_IC_PROVIDER);
	m_pClassClient = new MtgClient();
	int confID = LiveCourseUtils::generate_course_id(m_strRoomId);
	m_pClassClient->init(m_strMtgIp.c_str(), confID, m_userId);
	m_pClassClient->addListener(this);
	m_pClassClient->joinConf();


	m_timer.init();
	m_timer.start(this);
	m_timer.setInterval(60000);
	m_bTimerValid = true;
	return true;
}

bool ClassWxManager::uninit() {
	if (NULL != m_pClassClient) {
		delete m_pClassClient;
		m_pClassClient = NULL;
	}
	if (m_bTimerValid) {
		m_bTimerValid = false;
		m_timer.stop();
		m_timer.uninit();
	}
	return true;
}


bool ClassWxManager::createPaths() {
	if (NULL == m_pClassClient) {
		return false;
	}

	m_pClassClient->create(m_strResponsePath.c_str() , MTG_DATA_TYPE_BINARY);

	m_pClassClient->destory(m_pSetting->getLCRecordPath().c_str(), MTG_DATA_TYPE_VIDEO);
	if (m_pClassClient->create(m_pSetting->getLCRecordPath().c_str(), MTG_DATA_TYPE_VIDEO) ) {
	} else {
	}

	m_pClassClient->destory(m_pSetting->getLCVGAPath().c_str(), MTG_DATA_TYPE_VIDEO);
	if (m_pClassClient->create(m_pSetting->getLCVGAPath().c_str(), MTG_DATA_TYPE_VIDEO) ) {
		printf("vga mtg client create path ok\n");


	} else {
	}

	if (m_pClassClient->create(m_pSetting->getLCAACPath().c_str(), MTG_DATA_TYPE_AUDIO)) {
	} else {
	}
	return true;
}

bool ClassWxManager::destroyPaths() {
	if (NULL == m_pClassClient) {
		return false;
	}
	m_pClassClient->destory(m_strResponsePath.c_str() , MTG_DATA_TYPE_BINARY);
	m_pClassClient->destory(m_pSetting->getLCRecordPath().c_str(), MTG_DATA_TYPE_VIDEO);
	m_pClassClient->destory(m_pSetting->getLCVGAPath().c_str(), MTG_DATA_TYPE_VIDEO);
	m_pClassClient->destory(m_pSetting->getLCAACPath().c_str(), MTG_DATA_TYPE_AUDIO);

	return true;
}

bool ClassWxManager::sendData(const char* pPath, const char* pData, int dataLen,
		int dataType, ELiveCourseMediaType lcType) {
	m_mutex.lock();
	if (NULL != m_pClassClient) {
		if (eLCRecordVideo == lcType || eLCVGAVideo == lcType || eLCAACAudio == lcType) {
			m_pClassClient->sendData(pPath, pData, dataLen, dataType);
		}
	}
	m_mutex.unlock();
	return true;
}

bool ClassWxManager::procRecvEnterCmd(const char* pData, int len) {
	if (len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgEnter) ) {
		return false;
	}
	printf("receive enter path cmd len %d\n", len);

	tagLiveMtgHeader* pHeader = (tagLiveMtgHeader*)pData;
	if (pHeader->magic != LIVEMTG_HEADER_MAGIC || pHeader->cmdID != LIVEMTG_CMD_ID_ENTER) {
		return false;
	}
	if (pHeader->len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgEnter)) {
		return false;
	}
	printf("mtg header %d, %d, %d, %d\n", pHeader->magic, pHeader->len, pHeader->cmdID, pHeader->cmdFlag);
	tagLiveMtgEnter *pEnter = (tagLiveMtgEnter*)(pData + sizeof(tagLiveMtgHeader));
	printf("enter %s, type %d\n", pEnter->roomID, pEnter->userType);
	int userType = LIVEMTG_USER_TYPE_NULL;
	if (pEnter->userType == LIVEMTG_USER_TYPE_ASSISTANT_LIVE) {
		userType = LIVEMTG_USER_TYPE_VIDEO_LIVE;
	} else {
		userType = LIVEMTG_USER_TYPE_VIDEO_AUDENCE;
	}

	strcpy(m_pSetting->getSchool(), pEnter->school);
	strcpy(m_pSetting->getCampus(), pEnter->campus);
	strcpy(m_pSetting->getBuilding(), pEnter->building);
	strcpy(m_pSetting->getRoomName(), pEnter->roomName);

	for (vector<IClassWxObserver*>::iterator it = m_vecObservers.begin();
			it != m_vecObservers.end(); ++it) {
		(*it)->onRecvLCRole((ELiveCourseRole)userType);
	}

	if (NULL != m_pClassClient) {
		char buf[32] = {0};
		strcpy(buf, "recv enter");
		m_pClassClient->sendData(m_strResponsePath.c_str(), buf, 32, MTG_DATA_TYPE_BINARY);
	}

	return true;
}


bool ClassWxManager::procRecvCourseStatusCmd(const char* pData, int len) {
	if (len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgCourse)) {
		return false;
	}

	tagLiveMtgHeader* pHeader = (tagLiveMtgHeader*) pData;
	if (pHeader->magic != LIVEMTG_HEADER_MAGIC || pHeader->cmdID != LIVEMTG_CMD_ID_COURSE) {
		return false;
	}
	if (pHeader->len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgCourse)) {
		return false;
	}
	tagLiveMtgCourse* pStatus = (tagLiveMtgCourse*) (pData + sizeof(tagLiveMtgHeader));
	if (pStatus->courseID <= 0) {
		return false;
	}
	printf("receive course status , course id %d\n", pStatus->courseID);
	printf("the live course ip1 is %s\n", pStatus->wxIP1);
	printf("the live course ip2 is %s\n", pStatus->wxIP2);

	//if (!CommonUtils::isIPValid(pStatus->wxIP1)) {
	//	printf("receive bad command, the ip is wrong\n");
	//	return false;
	//}
	for (vector<IClassWxObserver*>::iterator it = m_vecObservers.begin();
			it != m_vecObservers.end(); ++it) {
		(*it)->onRecvCourseStatus(pStatus->courseID, pStatus->wxIP1);
	}

	if (NULL != m_pKernel) {
		strcpy(m_pSetting->getUtf8RecordParam()->cUtf8CourseName, pStatus->courseName);
		strcpy(m_pSetting->getUtf8RecordParam()->cUtf8TeacherName, pStatus->teacherName);
		strcpy(m_pSetting->getUtf8RecordParam()->cUtf8TitleName, pStatus->courseSecondName);
		m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_UTF8_RecordParam);
	}
	if (NULL != m_pClassClient) {
		char buf[32] = {0};
		strcpy(buf, "recv course");
		m_pClassClient->sendData(m_strResponsePath.c_str(), buf, 32, MTG_DATA_TYPE_BINARY);
	}
	return true;
}

bool ClassWxManager::procRecvRtmpPathCmd(const char* pData, int len) {
	if (len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgRtmp)) {
		return false;
	}

	tagLiveMtgHeader* pHeader = (tagLiveMtgHeader*) pData;
	if (pHeader->magic != LIVEMTG_HEADER_MAGIC || pHeader->cmdID != LIVEMTG_CMD_ID_RTMP) {
		return false;
	}
	if (pHeader->len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgRtmp)) {
		return false;
	}
	tagLiveMtgRtmp* pRtmp = (tagLiveMtgRtmp*) (pData + sizeof(tagLiveMtgHeader));
	m_pSetting->setLCRtmpUrl(pRtmp->url);
	printf("the rmtp url is %s\n", pRtmp->url);
	return true;
}

bool ClassWxManager::addListener(IClassWxObserver* pObserver) {
	for (vector<IClassWxObserver*>::iterator it = m_vecObservers.begin();
			it != m_vecObservers.end(); ++it) {
		if ( *it == pObserver) {
			return false;
		}
	}
	m_vecObservers.push_back(pObserver);
	return true;
}

bool ClassWxManager::removeListener(IClassWxObserver* pObserver) {
	for (vector<IClassWxObserver*>::iterator it = m_vecObservers.begin();
			it != m_vecObservers.end(); ++it) {
		if ( *it == pObserver) {
			m_vecObservers.erase(it);
			return true;
		}
	}
	return true;
}
