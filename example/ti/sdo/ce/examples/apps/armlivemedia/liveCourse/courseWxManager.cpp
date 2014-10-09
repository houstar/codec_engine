#include "courseWxManager.h"
#include "../utils/liveCourseUtils.h"
#include "liveCourseManager.h"

void CourseWxManager::onRecvMtgData(MtgClient* pMtg, const char* path,
				const char*data, int dataLen, int datatype, int action) {
	if (datatype == MTG_DATA_TYPE_AUDIO) {
		//for (vector<ILiveCourseManagerObserver*>::iterator it = m_vecObservers.begin();
		//	 it != m_vecObservers.end(); ++it) {
		//	(*it)->onRecvMtgSpx(path, data, dataLen);
		//}

		if (NULL != m_pSpxListener)
			m_pSpxListener->onRecvMtgSpx(path, data, dataLen);

	} else if (datatype == MTG_DATA_TYPE_VIDEO) {
		m_pCourseClient->subscribe(path, true);
		printf("subscribe off path is %s\n", path);
	} else if (datatype == MTG_DATA_TYPE_BINARY) {
		if (NULL == path || NULL == data) {
			return;
		}
		if (strncmp(m_strEnterPath.c_str(), path, m_strEnterPath.length()) == 0) {
			procRecvEnterCmd(data, dataLen);
		} else if (strncmp(m_strLeavePath.c_str(), path, m_strLeavePath.length()) == 0) {
			procRecvLeaveCmd(data, dataLen);
		} else if (strncmp(m_strCourseStatusPath.c_str(), path, m_strCourseStatusPath.length()) == 0) {
			procRecvCourseStatusCmd(data, dataLen);
		} else if (strncmp(m_strClassStatusPath.c_str(), path, m_strClassStatusPath.length()) == 0) {
			procRecvClassStatusCmd(data, dataLen);
		} else if (strncmp(m_strSubscribeListPath.c_str(), path, m_strSubscribeListPath.length()) == 0) {
			procRecvSubscribeListCmd(path, data, dataLen);
		} else if (strncmp(m_strWebMonitorPath.c_str(), path, m_strWebMonitorPath.length()) == 0) {
			procRecvWebMonitorCmd(data, dataLen);
		} else {
		}
	}
}

void CourseWxManager::onJoinCallback(MtgClient* pMtg, bool bJoinOK) {
	if (!bJoinOK) {
		printf("course wx manager join failed\n");
		return;
	}
	printf("course wx manager join ok\n");
	fflush(stdout);
	m_pCourseClient->createAudioThread();

	printf("before create livecourse paths\n");
	fflush(stdout);
	createPaths();
	printf("after create livecourse paths\n");
	fflush(stdout);

	sendEnter();

	sendVideoList();
}

void CourseWxManager::onTimeout(Timer* pTimer) {
	if (pTimer == &m_timer) {
		m_mutex.lock();
		if (NULL != m_pCourseClient) {
			if (!m_pCourseClient->getJoinStatus()) {
				destroyPaths();
				delete m_pCourseClient;
				m_pCourseClient = NULL;
				printf("delete course client mtg\n");
				m_pCourseClient = new MtgClient();
				if (NULL == m_pCourseClient) {
					m_mutex.unlock();
					return;
				}
				m_pCourseClient->init(m_strMtgIp.c_str(), m_courseID, m_userId);
				m_pCourseClient->addListener(this);
				m_pCourseClient->joinConf();
				printf("reconnect course wx\n");
			} else {
				if (m_bExitCourse) {
					m_role = eLCRoleNone;
					m_bExitCourse = false;
					if (NULL != m_pCourseClient) {
						delete m_pCourseClient;
						m_pCourseClient = NULL;
					}
				}

			}
		}
		m_mutex.unlock();
	}
}

CourseWxManager::CourseWxManager()
: m_pSetting(NULL)
, m_pCourseClient(NULL)
, m_bTimerValid(false)
, m_role(eLCRoleNone)
, m_userId(0)
, m_courseID(0)
, m_pSpxListener(NULL)
, m_strMtgIp("")
, m_bExitCourse(false) {
	memset(m_bSendLCMediaType, false, sizeof(bool) * eLCMediaTypeCount);
	m_bSendLCMediaType[eLCSpeexAudio] = true;
	m_bSendLCMediaType[eLCAACAudio] = true;
	m_mutex.init();
	memset(&m_lcWebMessage, 0, sizeof(tagLiveMtgWebMessage));
}

CourseWxManager::~CourseWxManager() {
	uninit();
	m_mutex.unlock();
}

bool CourseWxManager::init(Setting* pSetting) {
	m_pSetting = pSetting;

	m_strEnterPath = string(LIVEMTG_PATH_CMD_ENTER) + string("/") + m_pSetting->getClassRoomID();
	m_strLeavePath = string(LIVEMTG_PATH_CMD_LEAVE) + string("/") + m_pSetting->getClassRoomID();
	m_strCourseStatusPath = string(LIVEMTG_PATH_CMD_COURSESTATUS) + string("/") + m_pSetting->getClassRoomID();
	m_strClassStatusPath = string(LIVEMTG_PATH_CMD_CLASSSTATUS) + string("/") + m_pSetting->getClassRoomID();
	m_strSubscribeListPath = string(LIVEMTG_PATH_CMD_SUBSCRIBELIST);
	m_strVideoListPath = string(LIVEMTG_PATH_CMD_VIDEOLIST) + string("/") + m_pSetting->getClassRoomID();
	m_strWebMonitorPath = string(LIVEMTG_PATH_CMD_WEBMESSAGE) + string("/") + m_pSetting->getClassRoomID();

	m_timer.init();
	m_timer.start(this);
	m_timer.setInterval(60000);
	m_bTimerValid = true;
	return true;
}

bool CourseWxManager::setRole(ELiveCourseRole role) {
	m_role = role;
	m_userId = LiveCourseUtils::generate_uid(m_pSetting->getClassRoomID(), m_role);
	return true;
}

bool CourseWxManager::setCourseInfo(int courseId, string strMtgIp) {
	if (m_courseID != courseId) {
		m_vecSubscribeInfos.clear();
	}
	if (strMtgIp.length() == 0) {
		return false;
	}
	m_mutex.lock();
	if (NULL != m_pCourseClient) {
		destroyPaths();
		printf("destroy paths\n");
		m_pCourseClient->uninit();
		delete m_pCourseClient;
		m_pCourseClient = NULL;
	}

	m_courseID = courseId;
	m_strMtgIp = strMtgIp;

	printf("before create course client object\n");
	m_pCourseClient = new MtgClient();
	if (NULL != m_pCourseClient) {
		m_pCourseClient->init(strMtgIp.c_str(), m_courseID, m_userId);
		m_pCourseClient->addListener(this);
		m_pCourseClient->joinConf();
	}
	printf("after create course client object\n");

	m_bExitCourse = false;
	m_mutex.unlock();
	return true;
}

bool CourseWxManager::uninit() {
	if (NULL != m_pCourseClient) {
		delete m_pCourseClient;
		m_pCourseClient = NULL;
	}
	if (m_bTimerValid) {
		m_bTimerValid = false;
		m_timer.stop();
		m_timer.uninit();
	}
	return true;
}

bool CourseWxManager::createPaths() {
	if (NULL == m_pCourseClient) {
		return false;
	}
	if (m_pCourseClient->create(m_pSetting->getLCRecordPath().c_str(), MTG_DATA_TYPE_VIDEO) ) {
		printf("record mtg client create path ok\n");
	} else {
		printf("record mtg client create path failed\n");
	}

	if (m_pCourseClient->create(m_pSetting->getLCVGAPath().c_str(), MTG_DATA_TYPE_VIDEO) ) {
		printf("vga mtg client create path ok\n");
	} else {
		printf("vga mtg client create path failed\n");
	}

	if (m_pCourseClient->create(m_pSetting->getLCTeacherPath().c_str(), MTG_DATA_TYPE_VIDEO)) {
		printf("mtg client create teacher path ok\n");
	} else {
		printf("mtg client create teacher path failed\n");
	}

	if (m_pCourseClient->create(m_pSetting->getLCStudentPath().c_str(), MTG_DATA_TYPE_VIDEO)) {
		printf("mtg client create student path ok\n");
	} else {
		printf("mtg client create student path failed\n");
	}

	if (m_pCourseClient->create(m_pSetting->getLCSpeexPath().c_str(), MTG_DATA_TYPE_AUDIO)) {
		printf("create course audio path ok\n");
	} else {
		printf("create course audio path failed\n");
	}

	char bufEnterPath[64] = {0};
	sprintf(bufEnterPath, "%s/%d", m_strEnterPath.c_str(), m_userId);
	if (m_pCourseClient->create(bufEnterPath, MTG_DATA_TYPE_BINARY)) {
		printf("create course enter path ok\n");
	} else {
		printf("create course enter path failed\n");
	}

	if (m_pCourseClient->create(m_strVideoListPath.c_str(), MTG_DATA_TYPE_BINARY)) {
		printf("create video list path ok\n");
	} else {
		printf("create video list path failed\n");
	}

	char bufUid[32] = {0};
	sprintf(bufUid, "%d", m_userId);
	//
	string strPath = m_strClassStatusPath + string(bufUid);
	if (m_pCourseClient->create(strPath.c_str(), MTG_DATA_TYPE_BINARY)) {
		printf("create class status path ok\n");
	} else {
		printf("create class status path failed\n");
	}

	return true;
}

bool CourseWxManager::destroyPaths() {
	if (NULL == m_pCourseClient) {
		return false;
	}
	m_pCourseClient->destory(m_pSetting->getLCRecordPath().c_str(), MTG_DATA_TYPE_VIDEO);
	m_pCourseClient->destory(m_pSetting->getLCVGAPath().c_str(), MTG_DATA_TYPE_VIDEO);
	m_pCourseClient->destory(m_pSetting->getLCTeacherPath().c_str(), MTG_DATA_TYPE_VIDEO);
	m_pCourseClient->destory(m_pSetting->getLCStudentPath().c_str(), MTG_DATA_TYPE_VIDEO);
	//m_pCourseClient->destory(m_pSetting->getLCSpeexPath().c_str(), MTG_DATA_TYPE_AUDIO);

	char bufEnterPath[64] = {0};
	sprintf(bufEnterPath, "%s/%d", m_strEnterPath.c_str(), m_userId);
	m_pCourseClient->destory(bufEnterPath, MTG_DATA_TYPE_BINARY);

	m_pCourseClient->destory(m_strVideoListPath.c_str(), MTG_DATA_TYPE_BINARY);

	char bufUid[32] = {0};
	sprintf(bufUid, "%d", m_userId);
	//
	string strPath = m_strClassStatusPath + string(bufUid);
	if (m_pCourseClient->destory(strPath.c_str(), MTG_DATA_TYPE_BINARY)) {
		printf("destroy class status path ok\n");
	} else {
		printf("destroy class status path failed\n");
	}
	return true;
}

bool CourseWxManager::sendEnter() {
	if (NULL == m_pCourseClient) {
		return false;
	}
	int len = sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgEnter);
	char* pBuf = new char[len + 1];
	tagLiveMtgHeader *header = (tagLiveMtgHeader*)pBuf;
	tagLiveMtgEnter *enterCmd = (tagLiveMtgEnter*) (pBuf + sizeof(tagLiveMtgHeader));
	header->magic = LIVEMTG_HEADER_MAGIC;
	header->cmdID = LIVEMTG_CMD_ID_ENTER;
	header->len = len;
	header->cmdFlag = 0;
	printf("send enter step 1\n");

	strcpy(enterCmd->school, m_pSetting->getSchool());
	strcpy(enterCmd->campus, m_pSetting->getCampus());
	strcpy(enterCmd->building, m_pSetting->getBuilding());
	strcpy(enterCmd->roomName, m_pSetting->getRoomName());
	strcpy(enterCmd->roomID, m_pSetting->getClassRoomID().c_str());
	enterCmd->userType = m_role;

	char bufEnterPath[64] = {0};
	sprintf(bufEnterPath, "%s/%d", m_strEnterPath.c_str(), m_userId);
	m_pCourseClient->sendData(bufEnterPath, pBuf, len, MTG_DATA_TYPE_BINARY);
	delete []pBuf;
	printf("after send enter\n");
	return true;
}

bool CourseWxManager::sendClassStatus(int classStatus) {
	if (m_role != eLCRoleLive) {
		return false;
	}
	if (NULL == m_pCourseClient) {
		return false;
	}
	printf("before liveCourseManager send class status %d\n", classStatus);
	fflush(stdout);

	int len = sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgClassStatus);
	char* pBuf = new char[len + 1];
	tagLiveMtgHeader *header = (tagLiveMtgHeader*)pBuf;
	tagLiveMtgClassStatus *status = (tagLiveMtgClassStatus*) (pBuf + sizeof(tagLiveMtgHeader));
	header->magic = LIVEMTG_HEADER_MAGIC;
	header->cmdID = LIVEMTG_CMD_ID_CLASSSTATUS;
	header->len = len;
	header->cmdFlag = 0;
	printf("step 1\n");
	fflush(stdout);
	memset(status, 0, sizeof(tagLiveMtgClassStatus));
	status->status = classStatus;
	status->courseID = m_courseID;
	strcpy(status->liveRoomID, m_pSetting->getClassRoomID().c_str());
	char bufUid[32] = {0};
	sprintf(bufUid, "%d", m_userId);
	//
	string strPath = m_strClassStatusPath + string(bufUid);
	printf("step 2\n");
	fflush(stdout);
	if (NULL != m_pCourseClient) {
		m_pCourseClient->sendData(strPath.c_str(), pBuf, len, MTG_DATA_TYPE_BINARY);
		printf("command client senddata\n");
	}
	delete []pBuf;
	printf("after liveCourseManager send class status %d\n", classStatus);
	printf("step 3\n");
	fflush(stdout);
	return true;
}

bool CourseWxManager::sendVideoList() {
	if (NULL == m_pCourseClient) {
		return false;
	}

	int len = sizeof(tagLiveMtgHeader) + VideoListItemCount * sizeof(tagLiveMtgVideoListItem);
	char* pBuf = new char[len + 1];
	memset(pBuf, 0, len);
	tagLiveMtgHeader *header = (tagLiveMtgHeader*)pBuf;

	header->magic = LIVEMTG_HEADER_MAGIC;
	header->cmdID = LIVEMTG_CMD_ID_VIDEOLIST;
	header->len = len;
	header->cmdFlag = 0;


	for (int i = 0; i < VideoListItemCount; ++i) {
		tagLiveMtgVideoListItem *videoListCmd = (tagLiveMtgVideoListItem*) (pBuf + sizeof(tagLiveMtgHeader) + i * sizeof(tagLiveMtgVideoListItem));
		if (0 == i) {
			memcpy(videoListCmd->pathL, m_pSetting->getLCRecordPath().c_str(), m_pSetting->getLCRecordPath().length());
		} else if (1 == i) {
			memcpy(videoListCmd->pathL, m_pSetting->getLCTeacherPath().c_str(), m_pSetting->getLCTeacherPath().length());
		} else if (2 == i) {
			memcpy(videoListCmd->pathM, m_pSetting->getLCVGAPath().c_str(), m_pSetting->getLCVGAPath().length());
		} else if (3 == i) {
			memcpy(videoListCmd->pathL, m_pSetting->getLCStudentPath().c_str(), m_pSetting->getLCStudentPath().length());
		}
	}

	m_pCourseClient->sendData(m_strVideoListPath.c_str(), pBuf, len, MTG_DATA_TYPE_BINARY);

	delete []pBuf;
	printf("send video list\n");

	return true;
}

bool CourseWxManager::sendData(const char* pPath, const char* pData, int dataLen,
		int dataType, ELiveCourseMediaType lcType) {
	m_mutex.lock();
	if (NULL != m_pCourseClient) {
		if (m_bSendLCMediaType[lcType] /*&& eLCAACAudio != lcType && eLCSpeexAudio != lcType*/) {
			m_pCourseClient->sendData(pPath, pData, dataLen, dataType);
		}
	}
	m_mutex.unlock();
	return true;
}

bool CourseWxManager::procRecvEnterCmd(const char* pData, int len) {
	if (len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgEnter) ) {
		return false;
	}
	return true;
}

bool CourseWxManager::procRecvLeaveCmd(const char* pData, int len) {
	if (len < sizeof(tagLiveMtgHeader)) {
		return false;
	}
	tagLiveMtgHeader* pHeader = (tagLiveMtgHeader*) pData;
	if (pHeader->magic != LIVEMTG_HEADER_MAGIC || pHeader->cmdID != LIVEMTG_CMD_ID_LEAVE) {
		return false;
	}
	printf("receive leave path cmd\n");

	for (vector<ICourseWxObserver*>::iterator it = m_vecObservers.begin();
			it != m_vecObservers.end(); ++it) {
		(*it)->onCourseWxLeave();
	}
	m_bExitCourse = true;
	return true;
}

bool CourseWxManager::procRecvCourseStatusCmd(const char* pData, int len) {
	if (len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgCourse)) {
		return false;
	}
	return true;
}

bool CourseWxManager::procRecvClassStatusCmd(const char* pData, int len) {
	printf("receive class status path cmd\n");
	if (len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgClassStatus)) {
		return false;
	}
	tagLiveMtgClassStatus* pStatus = (tagLiveMtgClassStatus*)(pData + sizeof(tagLiveMtgHeader));
	printf("status %d, courseID %d, roomID %s\n", pStatus->status, pStatus->courseID, pStatus->liveRoomID);
	if (pStatus->status == LIVEMTG_CLASSSTATUS_BEGIN) {
		if (strcmp(pStatus->liveRoomID, m_pSetting->getClassRoomID().c_str()) != 0) {
			//send command to kernel board to start
			//m_pKernel->getKernelBoardCommandHandle()->sendStateCmd(FLAG_AS_LIVESTART);
		}
	} else if (pStatus->status == LIVEMTG_CLASSSTATUS_END) {
		if (strcmp(pStatus->liveRoomID, m_pSetting->getClassRoomID().c_str()) != 0) {
			//send command to kernel board to stop
			//m_pKernel->getKernelBoardCommandHandle()->sendStateCmd(FLAG_AS_LIVESTOP);
		}
	}
	return true;
}

bool CourseWxManager::procRecvSubscribeListCmd(const char* pPath, const char* pData, int len) {
	if (len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgSubscribeListItem)) {
		return false;
	}
	tagLiveMtgHeader* pHeader = (tagLiveMtgHeader*) pData;
	if (pHeader->magic != LIVEMTG_HEADER_MAGIC || pHeader->cmdID != LIVEMTG_CMD_ID_SUBSCRIBELIST) {
		return false;
	}
	if (pHeader->len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgSubscribeListItem)) {
		return false;
	}
	printf("receive subscribe list path cmd******\n");

	for (int i = 0; i < m_vecSubscribeInfos.size(); ++i) {
		if (strcmp(pPath, m_vecSubscribeInfos.at(i).path.c_str()) == 0) {
			m_vecSubscribeInfos.erase(m_vecSubscribeInfos.begin() + i);
			break;
		}
	}

	int nOffset = sizeof(tagLiveMtgHeader);
	pHeader->len -= nOffset;

	stSubscribeInfo info;
	for (int i = 0; i < eLCMediaTypeCount; ++i) {
		info.bSubscibeLCMediaType[i] = false;
	}
	info.path = pPath;
	while (pHeader->len >= sizeof(tagLiveMtgSubscribeListItem)) {
		tagLiveMtgSubscribeListItem* pItem = (tagLiveMtgSubscribeListItem*)(pData + nOffset);
		printf("subscribe Path %s\n", pItem->path);
		if (strncmp(pItem->path, m_pSetting->getLCRecordPath().c_str(), m_pSetting->getLCRecordPath().length()) == 0) {
			info.bSubscibeLCMediaType[eLCRecordVideo] = true;
			printf("send record video enable\n");
		} else if (strncmp(pItem->path, m_pSetting->getLCVGAPath().c_str(), m_pSetting->getLCVGAPath().length()) == 0) {
			info.bSubscibeLCMediaType[eLCVGAVideo] = true;
			printf("send vga video enable\n");
		} else if (strncmp(pItem->path, m_pSetting->getLCTeacherPath().c_str(), m_pSetting->getLCTeacherPath().length()) == 0) {
			info.bSubscibeLCMediaType[eLCTeacherVideo] = true;
			printf("send teacher video enable\n");
		} else if (strncmp(pItem->path, m_pSetting->getLCStudentPath().c_str(), m_pSetting->getLCStudentPath().length()) == 0) {
			info.bSubscibeLCMediaType[eLCStudentVideo] = true;
		} else if (strncmp(pItem->path, m_pSetting->getLCSpeexPath().c_str(), m_pSetting->getLCSpeexPath().length()) == 0) {
			info.bSubscibeLCMediaType[eLCSpeexAudio] = true;
		}
		pHeader->len -= sizeof(tagLiveMtgSubscribeListItem);
		nOffset += sizeof(tagLiveMtgSubscribeListItem);
	}

	m_vecSubscribeInfos.push_back(info);

	memset(m_bSendLCMediaType, false, sizeof(bool) * eLCMediaTypeCount);
	m_bSendLCMediaType[eLCSpeexAudio] = true;
	m_bSendLCMediaType[eLCAACAudio] = true;
	for (int i = 0; i < m_vecSubscribeInfos.size(); ++i) {
		m_bSendLCMediaType[eLCRecordVideo] = m_bSendLCMediaType[eLCRecordVideo] || m_vecSubscribeInfos.at(i).bSubscibeLCMediaType[eLCRecordVideo];
		m_bSendLCMediaType[eLCVGAVideo] = m_bSendLCMediaType[eLCVGAVideo] || m_vecSubscribeInfos.at(i).bSubscibeLCMediaType[eLCVGAVideo];
		m_bSendLCMediaType[eLCTeacherVideo] = m_bSendLCMediaType[eLCTeacherVideo] || m_vecSubscribeInfos.at(i).bSubscibeLCMediaType[eLCTeacherVideo];
		m_bSendLCMediaType[eLCStudentVideo] = m_bSendLCMediaType[eLCStudentVideo] || m_vecSubscribeInfos.at(i).bSubscibeLCMediaType[eLCStudentVideo];
	}
	return true;
}

bool CourseWxManager::procRecvWebMonitorCmd(const char* pData, int len) {
	if (len < sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgWebMessage)) {
		return false;
	}
	tagLiveMtgHeader* pHeader = (tagLiveMtgHeader*) pData;
	if (pHeader->magic != LIVEMTG_HEADER_MAGIC || pHeader->cmdID != LIVEMTG_CMD_ID_WEBMESSAGE) {
		return false;
	}
	tagLiveMtgWebMessage* pWebMessage = (tagLiveMtgWebMessage*)(pData + sizeof(tagLiveMtgHeader));
	memcpy(&m_lcWebMessage, pWebMessage, sizeof(tagLiveMtgWebMessage));
	m_pSetting->setEventId(m_lcWebMessage.EventID);
	m_vecStreamInfos.clear();
	int nOffset = sizeof(tagLiveMtgHeader) + sizeof(tagLiveMtgWebMessage);
	int nExtraLen = pHeader->len - nOffset;
	tagStreamInfo streamInfo;
	while (nExtraLen >= sizeof(tagStreamInfo)) {
		tagStreamInfo* pStreamInfo = (tagStreamInfo*)(pData + nOffset);
		memcpy(&streamInfo, pStreamInfo, sizeof(tagStreamInfo));
		nExtraLen -= sizeof(tagStreamInfo);
		nOffset += sizeof(tagStreamInfo);
		m_vecStreamInfos.push_back(streamInfo);
	}
	return true;
}


bool CourseWxManager::setSpxListener(ILiveCourseManagerObserver* pObserver)
{
	m_pSpxListener = pObserver;
	return true;
}

bool CourseWxManager::removeSpxListener()
{
	m_pSpxListener = NULL;
	return true;
}


bool CourseWxManager::addListener(ICourseWxObserver* pObserver) {
	for (vector<ICourseWxObserver*>::iterator it = m_vecObservers.begin();
			it != m_vecObservers.end(); ++it) {
		if ( *it == pObserver) {
			return false;
		}
	}
	m_vecObservers.push_back(pObserver);
	return true;
}

bool CourseWxManager::removeListener(ICourseWxObserver* pObserver) {
	for (vector<ICourseWxObserver*>::iterator it = m_vecObservers.begin();
			it != m_vecObservers.end(); ++it) {
		if ( *it == pObserver) {
			m_vecObservers.erase(it);
			return true;
		}
	}
	return true;
}


unsigned long CourseWxManager::getSendCount() {
	m_mutex.lock();
	if (NULL == m_pCourseClient) {
		m_mutex.unlock();
		return 0;
	}

	unsigned long nRet = m_pCourseClient->getSendCount();
	m_mutex.unlock();
	return nRet;
}

unsigned long CourseWxManager::getRecvCount() {
	m_mutex.lock();
	if (NULL == m_pCourseClient) {
		m_mutex.unlock();
		return 0;
	}

	unsigned long nRet = m_pCourseClient->getRecvCount();
	m_mutex.unlock();
	return nRet;
}

unsigned long CourseWxManager::getSendLossCount() {
	m_mutex.lock();
	if (NULL == m_pCourseClient) {
		m_mutex.unlock();
		return 0;
	}

	unsigned long nRet = m_pCourseClient->getSendLossCount();
	m_mutex.unlock();
	return nRet;
}

unsigned long CourseWxManager::getRecvLossCount() {
	m_mutex.lock();
	if (NULL == m_pCourseClient) {
		m_mutex.unlock();
		return 0;
	}

	unsigned long nRet = m_pCourseClient->getRecvLossCount();
	m_mutex.unlock();
	return nRet;
}
