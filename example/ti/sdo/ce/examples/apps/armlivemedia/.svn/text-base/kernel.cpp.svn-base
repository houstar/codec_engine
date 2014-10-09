#include "kernel.h"
#include "log.h"
#include "./core/msgq.h"
#include "./core/osdManager.h"
#include <signal.h>
#include "./KVideo/captureVga.h"
#include "./core/recordModeFactory.h"
#include <unistd.h>
#include "ACCWebService.h"
#include "./utils/commonUtils.h"
#include "constStringDefine.h"
#include "./utils/codeConverter.h"
#include "./utils/colorConvert.h"
#include "./liveCourse/livemtgConstDefine.h"
#include "./utils/liveCourseUtils.h"
#include "mtgFunc.h"
#include "hikFunc.h"

#ifdef CODEC_ENGINE_ENABLE
#include "./KVideo/audio/codec_engine.h"
#endif

extern unsigned char *buffer_addr[MAX_BUFFER];

long long gbackTImagePhyAddr;
long long gbackTSImagePhyAddr;
long long gbackTVImagePhyAddr;
long long gbackTSVImagePhyAddr;
long long glogoImagePhyAddr;
long long gcourseImagePhyAddr;
long long gheadImagePhyAddr;
long long gtailImagePhyAddr;
int		glogoWidth;
int		glogoHeight;
int		gcourseImageWidth;
int		gcourseImageHeight;
long long gHDMIDisplayAddr = 0;
long long gVGADisplayAddr = 0;

void* uploadFileThread(void* pParam) {
	Kernel* pSelf = (Kernel*) pParam;
	if (NULL != pSelf) {
		pSelf->uploadFile();
	}
	return NULL;
}

void* moveFileThread(void* pParam) {
	Kernel* pSelf = (Kernel*) pParam;
	if (NULL != pSelf) {
		pSelf->moveFile();
	}
	return NULL;
}

unsigned int nDiskCheckMinute = 0;


Kernel::Kernel() : m_RecordModeHandle(NULL), m_bReadyCourseOver(false) {
	m_commandOrder = eCommandNone;
	m_threadId = 0;
	m_vgaFreezeTimerValid = false;
	m_moveFileThreadId = 0;

	m_outWidth = 0;
	m_outHeight = 0;

	memset(m_cUtf8CourseName, 0 , 50);
	memset(m_cUtf8TeacherName, 0, 50);
	memset(m_cUtf8TitleName, 0, 50);

	m_nlastVGAChannel = 0;
	m_pModelStatus = NULL;

#ifdef CODEC_ENGINE_ENABLE
	m_pAudioCodecEngine = NULL;
#endif
}

Kernel::~Kernel() {
	this->uninit();
}

bool Kernel::init() {
	printf("init lib kernel.\n");
	fflush(stdout);
	m_setting.readSetting();
	printf("after read setting\n");

	if (!loadMtgFunc()) {
		return false;
	}
	if (!loadHikFunc()) {
		return false;
	}
	m_rtmpManager.init(&m_setting);

	char bufLogPath[255] = {0};
	time_t curTime = time(NULL);
	tm* t = localtime(&curTime);
	printf("local time is %4d-%02d-%02d %02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
			t->tm_hour, t->tm_min, t->tm_sec);

	m_memBufUtils.open();
	glogoImagePhyAddr = BASE_ADDR + LOGO_ADDR_OFFSET;
	gcourseImagePhyAddr = BASE_ADDR + COURSE_ADDR_OFFSET;
	gbackTImagePhyAddr = BASE_ADDR + TEACHER_BACKIMAGE_ADDR_OFFSET;
	gbackTSImagePhyAddr = BASE_ADDR + TEACHER_STUD_BACKIMAGE_ADDR_OFFSET;
	gbackTVImagePhyAddr = BASE_ADDR + TEACHER_VGA_BACKIMAGE_ADDR_OFFSET;
	gbackTSVImagePhyAddr = BASE_ADDR + TEACHER_STUD_VGA_BACKIMAGE_ADDR_OFFSET;
	gheadImagePhyAddr = BASE_ADDR + HEAD_ADDR_OFFSET;
	gtailImagePhyAddr = BASE_ADDR + TAIL_ADDR_OFFSET;

	SempUtils::semp_init(&m_commandSem, 1);
	m_commandTimer.init();
	m_commandTimer.start(this);
	m_commandTimer.setInterval(1000);

	m_assistcommandHandle.init(&m_setting);
	m_kernelboardcommndHandle.init(&m_setting, &m_function);
	m_function.init(this);

	m_recordList.init(this);
	printf("record list manager init\n");

	int nDiskSpace = CommonUtils::getDiskFreeSpace(m_setting.getSavePath().c_str());
	if (-1 != nDiskSpace) {
		printf("disk space is %d G\n", nDiskSpace);
		if (nDiskSpace <= m_setting.getDiskSpaceAlertSize()) {
			NotifyAssistMessage(gStrDiskfull); //磁盘空间满

			printf("auto delete files %d\n", m_setting.getAutoDeleteFiles());
			if (m_setting.getAutoDeleteFiles()) {
				time_t currentTime = time(NULL);
				time_t preTime = currentTime -  m_setting.getDaysBeforeCanDelete() * 24 * 60 *60;
				//tm* t = localtime(&preTime);
				//CommonUtils::changeUtcToBeijing(t);
				//preTime = mktime(t);
			    //m_recordList.deleteItemsBeforeTime(preTime);
				printf("before delete dir month %d, day %d, hour %d\n", t->tm_mon + 1, t->tm_mday, t->tm_hour);
			    CommonUtils::delete_dir(m_setting.getSavePath().c_str(), preTime);
			}
		}
	}

	m_diskCheckTimer.init();
	m_diskCheckTimer.start(this);
	m_diskCheckTimer.setInterval(1000 * 60); // 1 minute


	//m_vgaStateHandle.init(this, 1000);
	printf("end read setting\n");

	m_hdmiDisplayManager.init(&m_setting);

	m_bInited = true;
	if (m_setting.getDestPath().length() > 0) {
		pthread_attr_init(&m_moveFileThreadAttr);
		if (0 != pthread_create(&m_moveFileThreadId, &m_moveFileThreadAttr,
				moveFileThread, this)) {

		}
		printf("create move file thread\n");
		m_moveFileMutex.init();
	}

	m_heap = (IHeap_Object*)SharedRegion_getHeap(IL_CLIENT_SR);

	m_edmaUtils.init();
	printf("edma utils init\n");

	//m_audioPlayback.init("default", 2, 16, m_setting.getAudioSampleRate());
	//m_audioPlayback.start();

	//m_liveCourseManager.init(this, &m_setting);
	//printf("live course manager init\n");

	m_monitorSender.init(&m_setting, &m_liveCourseManager);

#ifndef CODEC_ENGINE_ENABLE
	m_liveCourseManager.init(this, &m_setting);
	printf("live course manager init\n");
	m_audioCapture.init(2, m_setting.getAudioSampleRate(), 16, m_setting.getPCMNonBlock(),
						m_setting.getAudioDeviceName().c_str());
	m_audioCapture.addListener(this);
	m_audioCapture.start();
#else
	if (NULL == m_pAudioCodecEngine) {
		m_pAudioCodecEngine = new codec_engine();		
	}
	printf("codec_engine::init start\n");
	if (!m_pAudioCodecEngine->init((char*)(buffer_addr[0])))
		printf("codec_engine::init error\n");
	
	if (!m_pAudioCodecEngine->start())
		printf("codec_engine::start error\n");
	printf("codec_engine::start done.\n");

	m_liveCourseManager.init(this, &m_setting);
	printf("live course manager init\n");
	// spx in
	m_liveCourseManager.getCourseWxManager()->setSpxListener(m_pAudioCodecEngine);

	m_pAudioCodecEngine->setSpxListener(&m_liveCourseManager);
#endif
	m_vgaDisplay.initv4lhandle(1024, 768, m_setting.getVGADisplayDeviceName().c_str(), 2);
	printf("vga display init\n");
	m_vgaProviderSystem.init(this);
	m_vgaProviderSystem.addListener(this);
	m_vgaProviderSystem.start();
	printf("start vga provider system\n");
	fflush(stdout);

#ifdef CAMERA_SDI
	m_sdiVideoProvider.init();
	m_sdiVideoProvider.open();
#endif
	return true;
}

bool Kernel::uninit() {
	m_memBufUtils.close();
	SempUtils::semp_deinit(&m_commandSem);
	if (m_vgaFreezeTimerValid) {
		m_vgaFreezeTimer.stop();
		m_vgaFreezeTimer.uninit();
		m_vgaFreezeTimerValid = false;
	}

	m_bInited = false;
	if (0 != m_moveFileThreadId) {
		pthread_join(m_moveFileThreadId, NULL);
		pthread_attr_destroy(&m_moveFileThreadAttr);
		m_moveFileThreadId = 0;

		m_moveFileMutex.uninit();
	}

	m_edmaUtils.uninit();
	m_vgaDisplay.uninitv4lhandle();

	m_vgaProviderSystem.stop();
	m_vgaProviderSystem.uninit();

	unloadMtgFunc();

	unloadHikFunc();

	if (0 != m_threadId) {
		pthread_join(m_threadId, NULL);
		pthread_attr_destroy(&m_threadAttr);
		m_threadId = 0;
	}
#ifdef CODEC_ENGINE_ENABLE
	if (NULL != m_pAudioCodecEngine) {
		printf("start close codec_engine\n");
		m_pAudioCodecEngine->stop();
		printf("codec_engine::stop done.\n");
		m_pAudioCodecEngine->deinit();
		printf("codec_engine::uninit done.\n");
		delete m_pAudioCodecEngine; 
		m_pAudioCodecEngine = NULL;
		printf("close codec_engine done.\n");
	}
#endif
	return true;
}


bool Kernel::onNotifyVGAFreeze() {
	if (m_vgaFreezeTimerValid) {
		m_vgaFreezeTimer.stop();
		m_vgaFreezeTimer.uninit();

		m_vgaFreezeTimer.init();
		m_vgaFreezeTimer.start(this);
		m_vgaFreezeTimer.setInterval(3000);
		return true;
	}
	m_vgaFreezeTimerValid = true;

	m_vgaFreezeTimer.init();
	m_vgaFreezeTimer.start(this);
	m_vgaFreezeTimer.setInterval(3000);
	if (NULL != m_RecordModeHandle) {
		m_RecordModeHandle->doFreezeVGA(true);
	}
	m_vgaProviderSystem.doFreezeVGA(true);

	return true;
}

bool Kernel::onNotifyVGAStateChanged(int reason) {
	m_vgaProviderSystem.vgaStatusChanged(reason);
	if (m_setting.getNotifyVGAChangedEnable()) {
		NotifyAssistMessage(gVGAStateChanged);
	}

	return true;
}

void Kernel::onTimeout(Timer *timer) {
	if (timer == &m_diskCheckTimer) {
		nDiskCheckMinute++;
		if (nDiskCheckMinute == 60 * 8) {
			int nDiskSpace = CommonUtils::getDiskFreeSpace(m_setting.getSavePath().c_str());
			if (-1 != nDiskSpace) {
				printf("disk space is %d G\n", nDiskSpace);
				if (nDiskSpace <= m_setting.getDiskSpaceAlertSize()) {
					NotifyAssistMessage(gStrDiskfull);

					if (m_setting.getAutoDeleteFiles()) {
						time_t currentTime = time(NULL);
						time_t preTime = currentTime -  m_setting.getDaysBeforeCanDelete() *24 * 60 *60;
						//tm* t = localtime(&preTime);
						//CommonUtils::changeUtcToBeijing(t);
						//preTime = mktime(t);
					    //m_recordList.deleteItemsBeforeTime(preTime);
						printf("before delete dir\n");
					    CommonUtils::delete_dir(m_setting.getSavePath().c_str(), preTime);
					}
				}
			}
			nDiskCheckMinute = 0;
		}
	} else if (timer == &m_commandTimer) {
		SempUtils::semp_pend(&m_commandSem);
		int nCount = m_commandVector.size();
		if (nCount > 0) {
			if (NULL != m_RecordModeHandle && m_RecordModeHandle->isComponentCreated()) {
				int nCommand = m_commandVector.at(0);
				m_commandVector.erase(m_commandVector.begin());
				switch (nCommand) {
				case FLAG_TOSTREAM_PAUSE:
					pauseRecord();
					break;

				case FLAG_TOSTREAM_START:
					startRecord();
					break;

				case FLAG_TOSTREAM_STOP:
					stopRecord();
					break;

				case FLAG_TOSTREAM_SCENE:
					execSceneSwitch();
					break;

				case FLAG_TOSTREAM_TAIL:
					execMovieTail();
					break;
				}
			}
		}
		SempUtils::semp_post(&m_commandSem);
	}
	else if (timer == &m_vgaFreezeTimer) {
		m_vgaFreezeTimer.stop();
		m_vgaFreezeTimer.uninit();
		m_vgaFreezeTimerValid = false;
		if (NULL != m_RecordModeHandle) {
			m_RecordModeHandle->doFreezeVGA(false);
		}
		m_vgaProviderSystem.doFreezeVGA(false);
	}
}

bool Kernel::onProviderNV12(unsigned char* pNV12Buffer, int width, int height) {
	return true;
}

bool Kernel::onProviderYUYV(unsigned char* pYUYVBuffer, int width, int height) {
	m_vgaDisplay.doV4LDisplay((const char*)pYUYVBuffer);
	m_hdmiDisplayManager.doDisplay(HDMIDisplayManager::eDisplayVGA, pYUYVBuffer);
	return true;
}

bool Kernel::onVgaStatus(EVGAStatus status) {
	return true;
}

bool Kernel::onProviderPcm(const char* pBuffer, int len) {
	//m_audioPlayback.playback((char*)pBuffer, len);
	return true;
}

bool Kernel::replay(char* pFileName) {
	if (m_stateHandle.getStreamMediaState()->CurState != SMStopped) {
		stopRecord();
	}


	//ExecReplay(m_pSetting->GetSaveFile());
	return true;
}

bool Kernel::previewRecord() {
	BOOL bRet = TRUE;
	DWORD curState = m_stateHandle.getStreamMediaState()->CurState;
	if (SMRecording == curState) {
		return FALSE;
	} else if (SMReplaying == curState) {
		//bRet = execReplay("");
	} else if (SMStopped == curState) {
		bRet = execCreate(TRUE);
		if (!bRet) {
			return bRet;
		}
	}

	if (startRecord()) {
		m_function.updateStreamMediaState(SMPreview);
	} else {
		m_assistcommandHandle.sendStreamNotify("record failed\n！");
		return FALSE;
	}
	return TRUE;
}

bool Kernel::doStartLC(ELiveCourseRole role) {
	if (role == eLCRoleAudence) {
		stopRecord();
		NotifyAssistMessage(gStrAssitCannotRecord);
	} else if (role == eLCRoleLive) {
		if (NULL != m_RecordModeHandle) {
			if (m_setting.getSaveWidth() == HD_WIDTH && m_setting.getSaveHeight() == HD_HEIGHT) {
				SempUtils::semp_pend(&m_commandSem);
				m_commandVector.push_back(FLAG_TOSTREAM_STOP);
				SempUtils::semp_post(&m_commandSem);
				NotifyAssistMessage(gStrLCRecordNotSupport);
			}
		}
	}
	return true;
}

bool Kernel::startRecord() {
	bool bRet = true;
	WORD curState = m_stateHandle.getStreamMediaState()->CurState;
	printf("cur media state %d\n", curState);
	if (curState == SMPreview) {
		printf("stop record**\n");
		bRet = stopRecord();
		m_setting.setEventId(eLCStopRecord);
	} else if (curState == SMReplaying) {
		printf("replay***\n");
		bRet = replay("");
	} else if (curState == SMStopped ) {
		printf("before exec create\n");
		if (m_liveCourseManager.getCourseWxManager()->getRole() == eLCRoleAudence) {
			NotifyAssistMessage(gStrAssitCannotRecord);
			return false;
		} else if (m_liveCourseManager.getCourseWxManager()->getRole() == eLCRoleLive) {
			if (m_setting.getSaveWidth() == HD_WIDTH && m_setting.getSaveHeight() == HD_HEIGHT) {
				NotifyAssistMessage(gStrLCRecordNotSupport);
				return false;
			}
		}

		bRet = execCreate();
		m_setting.setEventId(eLCStartRecord);
	} else if (curState == SMPaused) {
		if (NULL != m_RecordModeHandle) {
			printf("recv resume\n");
			m_RecordModeHandle->resume();
			m_function.updateStreamMediaState(SMRecording);
		}
	}

	return bRet;
}

bool Kernel::stopRecord() {
	BOOL bReplay = FALSE;
	printf("kernel ::stop record**\n");
	m_setting.writeSetting();
	sync();
	if (NULL != m_RecordModeHandle) {
		if (m_commandOrder != eCommandTail) {
			execMovieTail();
		}
		if (!m_RecordModeHandle->stop()) {
			return false;
		}
		delete m_RecordModeHandle;
		m_RecordModeHandle = NULL;

		m_commandOrder = eCommandNone;

		m_setting.resetIPCInfos();
		m_setting.resetSceneInfos();

		time_t currentTime = time(NULL);
		sync();
		if (m_setting.getRecordMode() == eResourceMode || m_setting.getRecordMode() == eThirdScreenMode) {
			char bufName[255] = {0};
			for (int i = 0; i < 3; ++i) {
				sprintf(bufName, "%s_%d.mp4", m_setting.getSaveFileName().c_str(), i);
				m_recordList.addItem(bufName, m_setting.getRecordMode(),
						m_setting.getVideoBitrate(), m_setting.getBaseRecordParams()->dwWidth,
						m_setting.getBaseRecordParams()->dwHeight, CommonUtils::getFileSize(bufName),
						currentTime);
			}

		} else {
			m_recordList.addItem(m_setting.getSaveFileName().c_str(), m_setting.getRecordMode(),
					m_setting.getVideoBitrate(), m_setting.getBaseRecordParams()->dwWidth,
					m_setting.getBaseRecordParams()->dwHeight, CommonUtils::getFileSize(m_setting.getSaveFileName().c_str()),
					currentTime);
		}

		m_liveCourseManager.getCourseWxManager()->sendClassStatus(LIVEMTG_CLASSSTATUS_END);
	}

	//if (m_bReadyCourseOver) {
	//	m_function.updateStreamMediaState(SMExited);
	//	return true;
	//}
	printf("after delete record mode handle\n");
	if (m_stateHandle.getStreamMediaState()->CurState == SMPreview)
	{
		//replay(m_setting.getSaveFileName().c_str());

		m_assistcommandHandle.recvLive("", 0, false);
	} else {

		if (m_setting.getDestPath().length() > 0) {
			m_moveFileMutex.lock();
			string strSrc = m_setting.getSaveFileName();
			string strDst = m_setting.getDestPath() + m_setting.getUtf8RelateFileName();
			m_vecSrcName.push_back(strSrc);
			m_vecDestName.push_back(strDst);
			printf("the source file is %s\n", strSrc.c_str());
			printf("the dest file is %s\n", strDst.c_str());
			m_moveFileMutex.unlock();
		}

		if (m_setting.getAutoUploadFile() && m_setting.getUtf8RelateFileName().length() != 0) {
			pthread_attr_init(&m_threadAttr);
			if (0 != pthread_create(&m_threadId,
					&m_threadAttr,
					uploadFileThread,
					this)) {
				printf("create upload file thread failed\n");
			}
			m_setting.setAutoUploadFile(false);
		}
	}
	printf("before updateStreamMediaState\n");
	m_function.updateStreamMediaState(SMStopped);

	return true;
}

bool Kernel::pauseRecord() {
	printf("kernel :: pause record**\n");
	BOOL bRet = TRUE;
	if (m_stateHandle.getStreamMediaState()->CurState != SMRecording){
		return false;
	}

	if (NULL == m_RecordModeHandle) {
		return false;
	}
	if (!m_RecordModeHandle->pause()) {
		NotifyAssistMessage(gStrRecordFailed);
		//SendNotify(_T("暂停录制失败！"));
		//SendActionResult(PR_ACTIONRESULT_PAUSE);
		return false;
	}
	m_function.updateStreamMediaState(SMPaused);
	m_liveCourseManager.getCourseWxManager()->sendClassStatus(LIVEMTG_CLASSSTATUS_BREAK);
	return true;
}

bool Kernel::execMovieHead() {
	DWORD curState = m_stateHandle.getStreamMediaState()->CurState;
	if (SMRecording == curState || SMPreview == curState) {
		return false;
	} else {
		m_setting.setHeadEnable(true);
	}

	return true;
}

bool Kernel::execMovieTail() {
	if (NULL != m_RecordModeHandle) {
		if (eCommandStart == m_commandOrder && !m_setting.getHeaderEnable()){
			m_RecordModeHandle->doMovieTail();
			m_commandOrder = eCommandTail;
		} else {
			//there must be some error
			//NotifyAssistMessage(gStrPressFast);
		}
	}

	return true;
}

bool Kernel::execCreate(bool bPreview) {
	m_setting.updateSaveFileName(bPreview);
	if (NULL == m_RecordModeHandle) {
		m_RecordModeHandle = RecordModeFactory::createInstance(m_setting.getRecordMode(), this);
		if (NULL == m_RecordModeHandle) {
			return false;
		}

		printf("exec create fun\n");
		m_RecordModeHandle->init(&m_setting);
	} else {
		return false;
	}

	if (m_RecordModeHandle->start()) {
		m_function.updateStreamMediaState(SMRecording);
		m_commandOrder = eCommandStart;
		m_liveCourseManager.getCourseWxManager()->sendClassStatus(LIVEMTG_CLASSSTATUS_BEGIN);
	} else {
		NotifyAssistMessage(gStrRecordFailed);
		return false;
	}
	m_assistcommandHandle.setCourseInfo(m_setting.getSaveFileName(), "");
	return true;
}

bool Kernel::execCourseOver() {
	/*m_bReadyCourseOver = TRUE;
	WORD curState = m_stateHandle.getStreamMediaState()->CurState;
	 if (curState == SMPausing || curState == SMRecording) {
	 } else {
		 m_function.updateStreamMediaState(SMExited);
	 }*/
	return true;
}

bool Kernel::execSceneSwitch() {
	WORD curState = m_stateHandle.getStreamMediaState()->CurState;
	if (SMRecording != curState && SMPreview != curState) {
		return false;
	}
	if (NULL == m_RecordModeHandle) {
		return false;
	}
	if (eCommandStart != m_commandOrder) {
		return false;
	}
	if (!m_RecordModeHandle->switchScene()) {
		NotifyAssistMessage(gStrRecordFailed);
	}
	return true;
}


bool Kernel::execReplay(string strFileName) {
	if (strFileName.length() == 0) {
		m_assistcommandHandle.playMedia("");
		m_function.updateStreamMediaState(SMStopped);
	} else {
		int pos = strFileName.find_last_of('\\');
		string strFile;
		for (int i = pos + 1; i < strFileName.length(); ++i) {
			strFile.push_back(strFileName.at(i));
		}
		m_assistcommandHandle.playMedia(strFile);
		m_function.updateStreamMediaState(SMReplaying);
	}
	return true;
}

void Kernel::NotifyAssistMessage(const char* strMessage) {
	printf("send assist message %s\n\n\n", strMessage);
	m_assistcommandHandle.sendStreamNotify(strMessage);
	printf("***********\n");
}

void Kernel::NotifyVGAResolution720P() {
	printf("notify VGA Resolution 720P\n");
	m_vgaProviderSystem.setResolution720P(true);
}

void Kernel::moveFile() {
	while (m_bInited) {
		m_moveFileMutex.lock();
		if (m_vecSrcName.size() > 0) {
			printf("start to move file\n");
			sync();
			string strSrcName = m_vecSrcName.at(0);
			printf("source file is %s\n", strSrcName.c_str());
			FILE* pFileSrc = fopen(strSrcName.c_str(), "rb");
			string strDestName = m_vecDestName.at(0);
			printf("dest file is %s\n", strDestName.c_str());
			m_vecSrcName.erase(m_vecSrcName.begin());
			m_vecDestName.erase(m_vecDestName.begin());
			m_moveFileMutex.unlock();
			if (NULL == pFileSrc) {
				printf("open the src file failed\n");
				continue;
			}
			printf("open src file ok\n");
			FILE* pFileDst = fopen(strDestName.c_str(), "wb+");
			if (NULL == pFileDst) {
				fclose(pFileSrc);
				continue;
			}
			printf("open dest file ok\n");
			char pBuffer[1024];
			int nRead = 0;
			while (!feof(pFileSrc)) {
				nRead = fread(pBuffer, 1, 1024, pFileSrc);
				if (nRead > 0) {
					fwrite(pBuffer, 1, nRead, pFileDst);
				}
			}
			fclose(pFileSrc);
			fclose(pFileDst);
			printf("after move a file to remote\n");
		} else {
			m_moveFileMutex.unlock();
			sleep(6);
		}
	}
	pthread_exit(NULL);
	return;
}

void Kernel::uploadFile() {
	ACCWebService webService;
	if (!webService.setWebServiceURL(m_setting.getWebServiceParam()->ACCUrl)) {
		printf("acc service not ready\n");
		pthread_exit(NULL);
		return ;
	}
	sync();
	bool bLoginOK = webService.login(m_setting.getACCLoginName().c_str(), m_setting.getACCLoginPassword().c_str());
	if (!bLoginOK) {
		printf("login failed\n");
		pthread_exit(NULL);
		return ;
	}
	printf("login ok\n");
	printf("unicode name is %s\n", m_setting.getUnicodeRelateFileName().c_str());
	bool bUploaded = webService.uploadMediaFile(m_setting.getSaveFileName(), m_setting.getUnicodeRelateFileName(), m_setting.getUnicodeRelateFileName());
	if (bUploaded) {
		printf("upload file ok\n");
		webService.setUpLoadStrInfo(m_setting.getACCProductID(), m_setting.getACCCourseID(), m_setting.getACCServerID(),
			m_setting.getACCFolderID(), m_setting.getACCFolderTreeID(), m_setting.getACCFolderName(),
			m_setting.getACCFolderType());
			webService.accCreateIndex();
			printf("create index ok\n");
	} else {
		printf("uploaded file failed\n");
	}
	pthread_exit(NULL);
}


void Kernel::NotifyIPCChannelChanged() {
	if (NULL != m_RecordModeHandle) {
		m_RecordModeHandle->IPCChannelChanged();

	}
}

void Kernel::NotifySwitchVGAChannel(int aVGAChannel) {
	if (!m_setting.getVGASwitchOldPanel()) {
		if (m_nlastVGAChannel != aVGAChannel) {
			m_nlastVGAChannel = aVGAChannel;
			printf("notify switch vga channel %d\n", aVGAChannel);
			onNotifyVGAFreeze();
			if (aVGAChannel == 0) {
				//first vga channel
				system("i2cset -f -y 1 0x5d 0x19 0x15");
				system("i2cset -f -y 1 0x5d 0x1a 0x62");
			} else if (aVGAChannel == 1) {
				//second vga channel
				system("i2cset -f -y 1 0x5d 0x19 0x2a");
				system("i2cset -f -y 1 0x5d 0x1a 0x67");
			}
			m_vgaProviderSystem.vgaStatusChanged(VGAStatusChannelChange);
		}
	}
}

void Kernel::NotifyGetCommand(int command) {
	switch (command) {
	case FLAG_TOSTREAM_UTF8_RecordParam:
		loadHeadPic(m_outWidth, m_outHeight);
		loadVideoCourseInfo(m_outWidth, m_outHeight);
		strcpy(m_cUtf8CourseName, m_setting.getUtf8RecordParam()->cUtf8CourseName);
		strcpy(m_cUtf8TeacherName, m_setting.getUtf8RecordParam()->cUtf8TeacherName);
		strcpy(m_cUtf8TitleName, m_setting.getUtf8RecordParam()->cUtf8TitleName);
		break;

	case FLAG_TOSTREAM_PRPARAM:
		if (m_outWidth != m_setting.getBaseRecordParams()->dwWidth ||
				m_outHeight != m_setting.getBaseRecordParams()->dwHeight) {
			m_outWidth = m_setting.getBaseRecordParams()->dwWidth;
			m_outHeight = m_setting.getBaseRecordParams()->dwHeight;
			loadBackImage(m_outWidth, m_outHeight);
			loadTailPic(m_outWidth, m_outHeight);
		}
		loadHeadPic(m_outWidth, m_outHeight);
		loadVideoCourseInfo(m_outWidth, m_outHeight);
		strcpy(m_cUtf8CourseName, m_setting.getUtf8RecordParam()->cUtf8CourseName);
		strcpy(m_cUtf8TeacherName, m_setting.getUtf8RecordParam()->cUtf8TeacherName);
		strcpy(m_cUtf8TitleName, m_setting.getUtf8RecordParam()->cUtf8TitleName);
		break;

	case FLAG_TOSTREAM_START:
		SempUtils::semp_pend(&m_commandSem);
		if (m_commandVector.size() != 0 ) {
			if (m_stateHandle.getStreamMediaState()->CurState != SMPaused) {
				printf("reconnect\n");
				m_kernelboardcommndHandle.sendStopCmd();
			} else {
				m_commandVector.push_back(FLAG_TOSTREAM_START);
				NotifyAssistMessage(gStrPressFast);
				printf("notify assist press too fast\n");
			}
		} else {
			printf("before start record\n");
			startRecord();
			printf("after start record\n");
		}
		SempUtils::semp_post(&m_commandSem);
		break;

	case FLAG_TOSTREAM_PAUSE:
		SempUtils::semp_pend(&m_commandSem);
		if (m_commandVector.size() != 0 || (NULL != m_RecordModeHandle && !m_RecordModeHandle->isComponentCreated())) {
			m_commandVector.push_back(FLAG_TOSTREAM_PAUSE);
			printf("push back pause command**\n");
		} else {
			pauseRecord();
		}
		SempUtils::semp_post(&m_commandSem);
		break;

	case FLAG_TOSTREAM_STOP:
		SempUtils::semp_pend(&m_commandSem);
		if (m_commandVector.size() != 0 || (NULL != m_RecordModeHandle && !m_RecordModeHandle->isComponentCreated())) {
			m_commandVector.push_back(FLAG_TOSTREAM_STOP);
			printf("push back stop command** \n");
		} else {
			stopRecord();
		}
		SempUtils::semp_post(&m_commandSem);
		break;

	case FLAG_TOSTREAM_SCENE:
		if (m_setting.getHeaderEnable()) {
			execSceneSwitch();

			printf("exec scene switch header\n");
		} else {
			SempUtils::semp_pend(&m_commandSem);
			if (m_commandVector.size() != 0 || (NULL != m_RecordModeHandle && !m_RecordModeHandle->isComponentCreated())) {
				m_commandVector.push_back(FLAG_TOSTREAM_SCENE);
				printf("push scene switch command**\n");
				//printf("in header state ignore the switch command\n");
			} else {
				execSceneSwitch();
				printf("exec scene switch\n");
			}
			SempUtils::semp_post(&m_commandSem);
		}
		break;

	case FLAG_TOSTREAM_HEAD:
		execMovieHead();
		break;

	case FLAG_TOSTREAM_TAIL:
		SempUtils::semp_pend(&m_commandSem);
		if (m_commandVector.size() != 0 || (NULL != m_RecordModeHandle && !m_RecordModeHandle->isComponentCreated())) {
			m_commandVector.push_back(FLAG_TOSTREAM_TAIL);
			printf("push tail command**\n");
		} else {
			execMovieTail();
		}
		SempUtils::semp_post(&m_commandSem);
		break;

	case FLAG_TOSTREAM_PREVIEW:
		if (m_commandVector.size() != 0 || (NULL != m_RecordModeHandle && !m_RecordModeHandle->isComponentCreated())) {
			m_commandVector.push_back(FLAG_TOSTREAM_PREVIEW);
		} else {
			previewRecord();
		}
		break;

	case FLAG_TOSTREAM_REPLAY:
		replay("");
		break;

	case FLAG_TOSTREAM_COURSEOVER:
		execCourseOver();
		break;
	}
}

bool Kernel::loadBackImage(int videoWidth, int videoHeight) {
	printf("before load back image\n");
	tagRecordTemplate* pTemplate = m_setting.getRecordTemplate(videoWidth, videoHeight);
	if (pTemplate->bVideoLogoShow) {
		OSDManager logo;
		logo.init(pTemplate->VideoLogo.cMixPic);
	    printf("the logo name is %s\n", pTemplate->VideoLogo.cMixPic);
	    glogoWidth = logo.getImageWidth();
	    glogoHeight = logo.getImageHeight();
	    if (glogoWidth == 0 || glogoHeight == 0) {
	    	return false;
	    }

	    ColorConvertUtils::rgb32_to_yuyv(buffer_addr[0] + glogoImagePhyAddr - BASE_ADDR,
	    		(unsigned char*)logo.getImage(), glogoWidth, glogoHeight);
	     printf("after load video logo\n");

	     logo.uninit();
	}

	OSDManager osd;
	char bufFileName[255];
	string tempTemplatePath = m_setting.getRecordTemplatePath();
	if (videoWidth != 1536 || videoHeight != 768) {
		sprintf(bufFileName, "T_%d_%d.png", videoWidth, videoHeight);
		tempTemplatePath += bufFileName;
		osd.init(tempTemplatePath.c_str());

		printf("single back image name %s\n", tempTemplatePath.c_str());
		if (osd.getImageWidth() == 0 || osd.getImageHeight() == 0) {
			return false;
		}

		ColorConvertUtils::rgb32_to_yuyv(
				buffer_addr[0] + gbackTImagePhyAddr - BASE_ADDR,
				(unsigned char*) osd.getImage(), videoWidth, videoHeight);
		osd.uninit();

		sprintf(bufFileName, "TS_%d_%d.png", videoWidth, videoHeight);
		tempTemplatePath = m_setting.getRecordTemplatePath() + bufFileName;
		osd.init(tempTemplatePath.c_str());
		printf("TS back image name %s\n", tempTemplatePath.c_str());
		if (osd.getImageWidth() == 0 || osd.getImageHeight() == 0) {
			return false;
		}

		ColorConvertUtils::rgb32_to_yuyv(
				buffer_addr[0] + gbackTSImagePhyAddr - BASE_ADDR,
				(unsigned char*) osd.getImage(), videoWidth, videoHeight);

		osd.uninit();

		sprintf(bufFileName, "TV_%d_%d.png", videoWidth, videoHeight);
		tempTemplatePath = m_setting.getRecordTemplatePath() + bufFileName;
		osd.init(tempTemplatePath.c_str());
		printf("TV back image name %s\n", tempTemplatePath.c_str());
		if (osd.getImageWidth() == 0 || osd.getImageHeight() == 0) {
			return false;
		}

		ColorConvertUtils::rgb32_to_yuyv(
				buffer_addr[0] + gbackTVImagePhyAddr - BASE_ADDR,
				(unsigned char*) osd.getImage(), videoWidth, videoHeight);

		osd.uninit();
	} else {
		sprintf(bufFileName, "TSV_%d_%d.png", videoWidth, videoHeight);
		tempTemplatePath = m_setting.getRecordTemplatePath() + bufFileName;
		osd.init(tempTemplatePath.c_str());
		printf("TSV back image name %s\n", tempTemplatePath.c_str());
		if (osd.getImageWidth() == 0 || osd.getImageHeight() == 0) {
			return false;
		}

		printf("before load TSVImage\n");

		printf("the tsv addr is %x\n", gbackTSVImagePhyAddr);
		ColorConvertUtils::rgb32_to_yuyv(
				buffer_addr[0] + gbackTSVImagePhyAddr - BASE_ADDR,
				(unsigned char*) osd.getImage(), videoWidth, videoHeight);

		printf("after load TSVImage\n");
		osd.uninit();
	}
	return true;
}

bool Kernel::loadHeadPic(int outWidth, int outHeight) {
	OSDManager osd;
	tagRecordTemplate *pTemplate = m_setting.getRecordTemplate(outWidth, outHeight);
	if (NULL == pTemplate) {
		return false;
	}
	osd.init(pTemplate->HeadBackPic.cBackPic);
	printf("the head back pic is %s\n", pTemplate->HeadBackPic.cBackPic);
	if (osd.getImageWidth() == 0 || osd.getImageHeight() == 0) {
		return false;
	}

	if ( pTemplate->bHeadCourseNameShow || pTemplate->bHeadTeacherNameShow || pTemplate->bHeadTitleShow) {
		if (pTemplate->bHeadCourseNameShow) {
			osd.setInputText(m_setting.getUtf8RecordParam()->cUtf8CourseName,
					pTemplate->HeadCourseName.dwPosX,
					pTemplate->HeadCourseName.dwPosY,
					((pTemplate->HeadCourseName.dwColor >> 16) & 0xFF) / 255.0,
					((pTemplate->HeadCourseName.dwColor >> 8) & 0xFF) / 255.0,
					(pTemplate->HeadCourseName.dwColor & 0xFF) / 255.0,
					pTemplate->HeadCourseName.dwSize);
		}
		if (pTemplate->bHeadTeacherNameShow) {
			osd.setInputText(m_setting.getUtf8RecordParam()->cUtf8TeacherName,
					pTemplate->HeadTeacherName.dwPosX,
					pTemplate->HeadTeacherName.dwPosY,
					((pTemplate->HeadTeacherName.dwColor >> 16) & 0xFF) / 255.0,
					((pTemplate->HeadTeacherName.dwColor >> 8) & 0xFF) / 255.0,
					(pTemplate->HeadTeacherName.dwColor & 0xFF) / 255.0,
					pTemplate->HeadTeacherName.dwSize);
		}
		if (pTemplate->bHeadTitleShow) {
			osd.setInputText(m_setting.getUtf8RecordParam()->cUtf8TitleName,
					pTemplate->HeadTitleName.dwPosX,
					pTemplate->HeadTitleName.dwPosY,
					((pTemplate->HeadTitleName.dwColor >> 16) & 0xFF) / 255.0,
					((pTemplate->HeadTitleName.dwColor >> 8) & 0xFF) / 255.0,
					(pTemplate->HeadTitleName.dwColor & 0xFF) / 255.0,
					pTemplate->HeadTitleName.dwSize);
		}
	}

	unsigned char* pYUV420 = (unsigned char*)malloc(KHeadPicWidth * KHeadPicHeight * 3 /2);
	ColorConvertUtils::rgb32_to_yuv420(pYUV420, (unsigned char*)osd.getImage(),
			KHeadPicWidth, KHeadPicHeight);

	ColorConvertUtils::yuv420_to_nv12(buffer_addr[0] + gheadImagePhyAddr - BASE_ADDR,
			pYUV420, KHeadPicWidth, KHeadPicHeight);

	free(pYUV420);


	printf("after update head pic\n");

	osd.uninit();
	return true;
}

bool Kernel::loadTailPic(int outWidth, int outHeight) {
	OSDManager osd;
	tagRecordTemplate *pTemplate = m_setting.getRecordTemplate(outWidth, outHeight);
	if (NULL == pTemplate) {
		printf("load tail pic failed*******\n");
		return false;
	}
	osd.init(pTemplate->TailBackPic.cBackPic);
	printf("tail template ******* %d\n", pTemplate->bTailTitleShow);
	if (pTemplate->bTailTitleShow) {
		printf("tail set input text %s, pos %d, %d, size %d\n",
				pTemplate->TailTitle.cString,
				pTemplate->TailTitle.dwPosX,
				pTemplate->TailTitle.dwPosY,
				pTemplate->TailTitle.dwSize);

		osd.setInputText(pTemplate->TailTitle.cString,
				pTemplate->TailTitle.dwPosX,
				pTemplate->TailTitle.dwPosY,
				((pTemplate->TailTitle.dwColor >> 16) & 0xFF) / 255.0,
				((pTemplate->TailTitle.dwColor >> 8) & 0xFF) / 255.0,
				(pTemplate->TailTitle.dwColor & 0xFF) / 255.0,
				pTemplate->TailTitle.dwSize);
	}
	if (pTemplate->bTailTimeShow) {
		osd.setInputText(pTemplate->TailTime.cString,
				pTemplate->TailTime.dwPosX,
				pTemplate->TailTime.dwPosY,
				((pTemplate->TailTime.dwColor >> 16) & 0xFF) / 255.0,
				((pTemplate->TailTime.dwColor >> 8) & 0xFF) / 255.0,
				(pTemplate->TailTime.dwColor & 0xFF) / 255.0,
				pTemplate->TailTime.dwSize);
	}

	unsigned char* pYUV420 = (unsigned char*)malloc(KHeadPicWidth * KHeadPicHeight * 3 /2);
	ColorConvertUtils::rgb32_to_yuv420(pYUV420, (unsigned char*)osd.getImage(),
			KHeadPicWidth, KHeadPicHeight);


	ColorConvertUtils::yuv420_to_nv12(buffer_addr[0] + gtailImagePhyAddr - BASE_ADDR,
			pYUV420, KHeadPicWidth, KHeadPicHeight);

	return true;
}

bool Kernel::loadVideoCourseInfo(int outWidth, int outHeight) {
	tagRecordTemplate* pTemplate = m_setting.getRecordTemplate(outWidth, outHeight);
	bool bUpdated = false;
	if (pTemplate->bVideoCoursePicShow) {
		OSDManager videoCourseOSD;

		videoCourseOSD.init(pTemplate->VideoCoursePic.cMixPic);
		printf("the course frame name is %s\n", pTemplate->VideoCoursePic.cMixPic);
		gcourseImageWidth = videoCourseOSD.getImageWidth();
		gcourseImageHeight = videoCourseOSD.getImageHeight();
		if (0 == gcourseImageWidth || 0 == gcourseImageHeight) {
			return false;
		}

		if ( pTemplate->bVideoCourseNameShow || pTemplate->bVideoTeacherNameShow ) {
		    if (pTemplate->bVideoCourseNameShow) {
		       videoCourseOSD.setInputText(m_setting.getUtf8RecordParam()->cUtf8CourseName,
		    		pTemplate->VideoCourseName.dwPosX,
		    		pTemplate->VideoCourseName.dwPosY,
		    		((pTemplate->VideoCourseName.dwColor >> 16) & 0xFF) / 255.0,
		    		((pTemplate->VideoCourseName.dwColor >> 8) & 0xFF) / 255.0,
		    		(pTemplate->VideoCourseName.dwColor & 0xFF) / 255.0,
		    		pTemplate->VideoCourseName.dwSize);
		    }
		    if (pTemplate->bVideoTeacherNameShow) {
		       videoCourseOSD.setInputText(m_setting.getUtf8RecordParam()->cUtf8TeacherName,
		    		pTemplate->VideoTeacherName.dwPosX,
		    		pTemplate->VideoTeacherName.dwPosY,
		    		((pTemplate->VideoTeacherName.dwColor >> 16) & 0xFF) / 255.0,
		    		((pTemplate->VideoTeacherName.dwColor >> 8) & 0xFF) / 255.0,
		    		(pTemplate->VideoTeacherName.dwColor & 0xFF) / 255.0,
		    		pTemplate->VideoTeacherName.dwSize);
		    }
		}


		ColorConvertUtils::rgb32_to_yuyv(buffer_addr[0] + gcourseImagePhyAddr - BASE_ADDR,
		     (unsigned char*)videoCourseOSD.getImage(), gcourseImageWidth, gcourseImageHeight);

		videoCourseOSD.uninit();
		printf("after load video course pic\n");
	}
	return true;
}


bool Kernel::checkSystemStatus() {
	printf("check system status\n");
	if (NULL != m_pModelStatus) {
		delete m_pModelStatus;
	}
	m_pModelStatus = new ModelStatus();
	m_pModelStatus->init(this);
	printf("model status manager init\n");
	m_pModelStatus->queryStatus();
	delete m_pModelStatus;
	m_pModelStatus = NULL;
	return true;
}
