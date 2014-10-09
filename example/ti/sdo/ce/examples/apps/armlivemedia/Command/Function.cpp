#include "Function.h"
#include "../netprotocal/ICIICommandDef.h"
#include "../netprotocal/TPCCommandStruct.h"
#include "../kernel.h"
#include <netinet/in.h>
#include <arpa/inet.h>

CFunction::CFunction(): m_pKernel(NULL),
m_pSetting(NULL) {

}

CFunction::~CFunction() {
	unregisterToKernelBoard();
/*	if (m_pVolume) {
		delete m_pVolume;
		m_pVolume = NULL;
	}*/
}

BOOL CFunction::init(Kernel* pKernel){
	if (NULL == pKernel) {
		return FALSE;
	}

	m_pKernel = pKernel;
	m_pSetting = pKernel->getSetting();

	sendHeartBeat();

	m_heartbeatTimer.init();
	m_heartbeatTimer.start(this);
	m_heartbeatTimer.setInterval(1000);


	m_bregisterToKernelBoard = false;
	return TRUE;
}

BOOL CFunction::uninit(){
	return TRUE;
}

void CFunction::onTimeout(Timer *timer) {
	if (timer == &m_heartbeatTimer) {
		sendHeartBeat();
		m_heartbeatTimer.stop();
		m_heartbeatTimer.start(this);
	}
}

void CFunction::execCmd(const char* ip, IC2CMD_CmdID* pCmdID, unsigned char* pData, int len) {
	if (pCmdID->dwCommand == CMD_PR_TOSTREAM) {
		switch (pCmdID->dwFlag) {
		case FLAG_TOSTREAM_HEARTBEAT: {
			if (!m_bregisterToKernelBoard) {
				registerToKernelBoard();
				printf("register to kernel board\n");
				m_bregisterToKernelBoard = true;
			}
		}
		break;

		case FLAG_TOSTREAM_START:
			printf("exec start\n");
			m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_START);
			//m_pKernel->startRecord();
			break;
		case FLAG_TOSTREAM_PAUSE:
			printf("exec pause\n");
			//m_pKernel->pauseRecord();
			m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_PAUSE);
			break;

		case FLAG_TOSTREAM_STOP:
			printf("exec stop\n");
			m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_STOP);
			//m_pKernel->stopRecord();
			//printf("after exec stop record\n");
			break;

		case FLAG_TOSTREAM_PREVIEW:
			printf("exec preview\n");
			//m_pKernel->previewRecord();
			m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_PREVIEW);
			break;

		case FLAG_TOSTREAM_REPLAY:
			printf("replay\n");
			m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_REPLAY);
			//m_pKernel->replay("");
			break;

		case FLAG_TOSTREAM_COURSEOVER:
			printf("exec course over\n");
			//m_pKernel->execCourseOver();
			m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_COURSEOVER);
			break;

		case FLAG_TOSTREAM_PRSTATE: {
			printf("recv PR setate \n");
			PRState stPRState;
			memcpy(&stPRState, pData, sizeof(PRState));
			m_pKernel->getStateHandle()->setPRState(stPRState);
		}
		break;

		case FLAG_TOSTREAM_RECORD_MODE:
			stStragyMode modeParams;
			memcpy(&modeParams, pData, sizeof(stStragyMode));
			printf("recv record mode %d\n", modeParams.mode);
			m_pSetting->setRecordMode(modeParams.mode);
			break;
		case FLAG_TOSTREAM_PRPARAM: {
			printf("recv record param\n");
			BaseRecordParam recordParams;
			memcpy(&recordParams, pData, sizeof(BaseRecordParam) );
			m_pSetting->setBaseRecordParams(recordParams);

			m_pKernel->getAssistCommandHandle()->setCourseAndTeacherName(recordParams.cTeacherName,
					recordParams.cCourseName);

			m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_PRPARAM);
		}
		break;

		case FLAG_TOSTREAM_UTF8_RecordParam: {
			printf("recv UTF8 name\n");
			stUtf8RecordParam utf8Name;
			memcpy(&utf8Name, pData, sizeof(stUtf8RecordParam));
			m_pSetting->setUtf8RecordParam(&utf8Name);

			m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_UTF8_RecordParam);
		}
		break;

		case FLAG_TOSTREAM_ACCPARAM: {
			printf("recv acc param\n");
			PRACCParam accParam;
			memcpy(&accParam, pData, sizeof(PRACCParam));
			m_pSetting->setACCParam(accParam);
		}
		break;

		case FLAG_TOSTREAM_WEBSERVICEPARAM: {
			printf("recv web service param\n");
			WebServiceParam webServiceParam;
			memcpy(&webServiceParam,  pData, sizeof(WebServiceParam));
			m_pSetting->setWebServiceParam(webServiceParam);
		}
		break;

		case FLAG_TOSTREAM_SCENE: {
			printf("recv scene\n");
			SceneCommonInfo CommonInfo;
			int nOffset = 0;
			if (len < sizeof(CommonInfo)) {
				return;
			}
			memcpy(&CommonInfo, pData, sizeof(CommonInfo));
			nOffset += sizeof(CommonInfo);
			len -= nOffset;

			m_pSetting->resetSceneInfos();
			eSceneElementType eElementType;
			do {
				if (len < sizeof(eSceneElementType)) {
					break;
				}
				memcpy(&eElementType, pData + nOffset, sizeof(eSceneElementType));
				nOffset += sizeof(eSceneElementType);
				len -= sizeof(eSceneElementType);
				switch (eElementType) {
					case SceneVideo: {
						SceneVideoInfo stVideoInfo;
						if (len < sizeof(stVideoInfo)) {
							break;
						}
						memcpy(&stVideoInfo, pData + nOffset, sizeof(stVideoInfo));
						nOffset += sizeof(stVideoInfo);
						len -= sizeof(stVideoInfo);

						if (eMicroMode == m_pSetting->getRecordMode()) {
							printf("ori left right top bottom %d, %d,%d,%d\n", stVideoInfo.dwLeft, stVideoInfo.dwRight,
									stVideoInfo.dwTop, stVideoInfo.dwBottom);
							stVideoInfo.dwRight = UTIL_ALIGN(stVideoInfo.dwRight - stVideoInfo.dwLeft, 32) + stVideoInfo.dwLeft;
						} else if (eMovieMode == m_pSetting->getRecordMode() ) {
							//stVideoInfo.dwLeft = 0;
							//stVideoInfo.dwRight = UTIL_ALIGN(m_pSetting->getBaseRecordParams()->dwWidth, 32);
							//stVideoInfo.dwTop = 0;
							//stVideoInfo.dwBottom = m_pSetting->getBaseRecordParams()->dwHeight;

							printf("ori left right top bottom %d, %d,%d,%d\n", stVideoInfo.dwLeft, stVideoInfo.dwRight,
									stVideoInfo.dwTop, stVideoInfo.dwBottom);
							stVideoInfo.dwRight = UTIL_ALIGN(stVideoInfo.dwRight - stVideoInfo.dwLeft, 32) + stVideoInfo.dwLeft;

						} else if (eInterMode == m_pSetting->getRecordMode() ) {
							printf("ori left right top bottom %d, %d,%d,%d\n", stVideoInfo.dwLeft, stVideoInfo.dwRight,
									stVideoInfo.dwTop, stVideoInfo.dwBottom);
							stVideoInfo.dwRight = UTIL_ALIGN(stVideoInfo.dwRight - stVideoInfo.dwLeft, 32) + stVideoInfo.dwLeft;
						} else if (eDualScreenMode == m_pSetting->getRecordMode() ) {
							stVideoInfo.dwRight = UTIL_ALIGN(stVideoInfo.dwRight - stVideoInfo.dwLeft, 32) + stVideoInfo.dwLeft;
						}

						printf("recv scene video source %d, pos %d, %d, %d, %d\n",
							stVideoInfo.dwVideoSource, stVideoInfo.dwLeft,
							stVideoInfo.dwRight, stVideoInfo.dwTop,
							stVideoInfo.dwBottom);

						m_pSetting->setSceneVideoInfo(&stVideoInfo);
					}
					break;
					case SceneBackPic: {
						SceneBackPicInfo stBackPicInfo;
						if (len < sizeof(stBackPicInfo)) {
							break;
						}
						memcpy(&stBackPicInfo, pData + nOffset, sizeof(stBackPicInfo));
						nOffset += sizeof(stBackPicInfo);
						len -= sizeof(stBackPicInfo);

						printf("recv back pic info %s\n", stBackPicInfo.cBackPic);
						m_pSetting->setSceneBackPicInfo(&stBackPicInfo);
					}
					break;
					case SceneString: {
						SceneStringInfo stStringInfo;
						if (len < sizeof(stStringInfo)) {
							break;
						}
						memcpy(&stStringInfo, pData + nOffset, sizeof(stStringInfo));
						nOffset += sizeof(stStringInfo);
						len -= sizeof(stStringInfo);
						printf("recv scene string info\n");
						m_pSetting->setSceneStringInfo(&stStringInfo);
					}
					break;
					case SceneMixPic: {
						SceneMixPicInfo stMixPicInfo;
						if (len < sizeof(stMixPicInfo)) {
							break;
						}
						memcpy(&stMixPicInfo, pData + nOffset, sizeof(stMixPicInfo));
						nOffset += sizeof(stMixPicInfo);
						len -= sizeof(stMixPicInfo);
						printf("recv mix pic info\n");
						m_pSetting->setSceneMixPicInfo(&stMixPicInfo);
					}
					break;
					case SceneMediaFile: {
						SceneMeidaFileInfo stMediaFileInfo;
						if (len < sizeof(stMediaFileInfo)) {
							break;
						}
						memcpy(&stMediaFileInfo, pData + nOffset, sizeof(stMediaFileInfo));
						nOffset += sizeof(stMediaFileInfo);
						len -= sizeof(stMediaFileInfo);
						printf("recv media file info\n");
						m_pSetting->setSceneMediaFileInfo(&stMediaFileInfo);
					}
					break;
				}
			} while (eElementType != SceneNULL);
			//m_pKernel->execSceneSwitch();
			m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_SCENE);
		}
		break;

		case FLAG_TOSTREAM_HEAD: {
			printf("recv head\n");
			//m_pKernel->execMovieHead();
			m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_HEAD);
		}
		break;

		case FLAG_TOSTREAM_TAIL: {
			printf("recv tail\n");
			//m_pKernel->execMovieTail();
			m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_TAIL);
		}
		break;

		case FLAG_TOSTREAM_IPUPDATE: {
			printf("recv top ip update\n");
			BPIPINFO IPInfo;
			memcpy(&IPInfo, pData, sizeof(BPIPINFO) );
			execPowerOff();
		}
		break;

		case FLAG_TOSTREAM_IPINFO: {
			printf("recv notebook ip\n");
			BPIPINFO IPInfo;
			memcpy(&IPInfo, pData, sizeof(BPIPINFO) );
			m_pSetting->setBPIPInfo(IPInfo);
			m_pKernel->getAssistCommandHandle()->notifyConnect();
			printf("notify connect to assist\n");
		}
		break;

		case FLAG_TOSTREAM_CHANNEL: {
			PRChannel Channel;
			memcpy(&Channel, pData, sizeof(PRChannel));
			printf("recv channel teacher %d, stud %d, device %d\n", Channel.dwTeacherChannel,
					Channel.dwStudentChannel, Channel.dwDeviceChannel);
			if (m_pSetting->setIPCChannel(Channel)) {
				m_pKernel->NotifyIPCChannelChanged();
			}
		}
		break;

		case FLAG_TOSTREAM_FREEZE: {
			PRChannelNo ChannelNo;
			memcpy(&ChannelNo, pData, sizeof(PRChannelNo));
			printf("recv freeze cmd channel %d\n", ChannelNo.dwChannelNo);
			if (ChannelNo.dwChannelNo == m_pSetting->getIPCChannel()->dwDeviceChannel) {
				m_pKernel->onNotifyVGAFreeze();
			}

			//m_pMedia->Freeze(TRUE, ChannelNo.dwChannelNo);
		}
		break;

		case FLAG_TOSTREAM_SWITCH_VGA : {
			unsigned char nVGAChannel = 0;
			memcpy(&nVGAChannel, pData, sizeof(unsigned char));
			//printf("recv switch vga %d\n", nVGAChannel);

			m_pKernel->NotifySwitchVGAChannel(nVGAChannel);
		}
		break;

		case FLAG_TOSTREAM_THAW: {
			printf("recv thaw cmd\n");
			PRChannelNo ChannelNo;
			memcpy(&ChannelNo, pData, sizeof(PRChannelNo));
		}
		break;

		case FLAG_TOSTREAM_QUERY: {
			printf("recv query\n");
			sendQueryResponce((char*)ip);
		}
		break;

		case FLAG_TOSTREAM_QUERYSMSTATE: {
			sendStreamMediaState();
		}
		break;

		case FLAG_TOSTREAM_VOLADD: {
		//	m_pVolume->AddLineInVol();
		}
		break;

		case FLAG_TOSTREAM_VOLDEC: {
			//m_pVolume->DecLineInVol();
		}
		break;

		case FLAG_TOSTREAM_IPCINFO: {
			printf("recv ipc info\n");
			IPCInfo IPC[MAX_IPC];
			memcpy(&IPC[0], pData, sizeof(IPCInfo) * MAX_IPC);
			for (int i = 0; i < MAX_IPC; ++i) {
				printf("the ipc info name %s, channel %d, height %d, width %d, ip %d.%d.%d.%d\n", IPC[i].cIPCTypeName,
						IPC[i].dwIPCChannel, IPC[i].dwIPCHeight, IPC[i].dwIPCWidth,
						(IPC[i].dwIPCIP >> 24) & 0xFF,
						(IPC[i].dwIPCIP >> 16) & 0xFF,
						(IPC[i].dwIPCIP >> 8) & 0xFF,
						(IPC[i].dwIPCIP) & 0xFF);
				m_pSetting->setIPCInfo(&IPC[i], i);
			}
		}
		break;

		case FLAG_TOSTREAM_ENCCARDINFO: {
			printf("recv enc card info\n");
			//device, student 1, student 2, teacher 2, teacher 1
			IPCInfo IPC[MAX_IPC];
			memcpy(&IPC[0], pData, sizeof(IPCInfo) * MAX_IPC);
			for (int i = 0; i < MAX_IPC; ++i) {
				printf("the ipc info name %s, channel %d, height %d, width %d, ip %d.%d.%d.%d\n", IPC[i].cIPCTypeName,
						IPC[i].dwIPCChannel, IPC[i].dwIPCHeight, IPC[i].dwIPCWidth,
						(IPC[i].dwIPCIP >> 24) & 0xFF,
						(IPC[i].dwIPCIP >> 16) & 0xFF,
						(IPC[i].dwIPCIP >> 8) & 0xFF,
						(IPC[i].dwIPCIP) & 0xFF);
			}
		}
		break;

		case FLAG_TOSTREAM_SWITCHEFFECT: {
			DWORD dwEffect = 0;
			memcpy(&dwEffect, pData, sizeof(DWORD));
			//m_pSetting->SetEffectParam(dwEffect);
		}
		break;

		case FLAG_TOSTREAM_REBOOT:
		{
			system("reboot -f");
		}
		break;

		case FLAG_TOSTREAM_SYSTEMSTATUS:
		{
			m_pKernel->checkSystemStatus();
			sendSystemStatusResponse();
			printf("send system status response\n");
		}
		break;

		default:
			break;
		}
	}
	return;
}

BOOL CFunction::MediaStateEvent(DWORD dwEvent, DWORD dwValue) {
	if (dwEvent == MEDIA_EVENT_FREEZED) {
		//SendFreezed(dwValue);
	}
	return TRUE;
}

BOOL CFunction::RemoteVideoDataLost(string strMsg) {
	SendNotify(strMsg, TRUE);
	return TRUE;
}

BOOL CFunction::execPowerOff() {
	m_pKernel->NotifyGetCommand(FLAG_TOSTREAM_STOP);
	unregisterToKernelBoard();
	return true;
}

BOOL CFunction::updateStreamMediaState(eSMState CurState) {
	IC2CMD_CmdID CmdID;
	StreamMediaState* state = m_pKernel->getStateHandle()->getStreamMediaState();
	DWORD dwState = state->CurState;
	if (CurState == dwState) {
		return TRUE;
	}
	state->CurState = CurState;
	switch(CurState) {
	case SMStopped:
		m_pKernel->getAssistCommandHandle()->doAction(aLiveStop);
		break;

	case SMRecording:
		//m_bReadyCourseOver = FALSE;
		m_pKernel->getAssistCommandHandle()->doAction(aLiveStart);
		break;

	case SMPaused:
		m_pKernel->getAssistCommandHandle()->doAction(aLivePause);
		break;

	case SMPreview:
		m_pKernel->getAssistCommandHandle()->doAction(aLiveStart);
		break;

	case SMReplaying:
		break;

	case SMExited:
		break;

	default:
		break;
	}
	m_pKernel->getStateHandle()->setStreamMediaState(*state);

	sendStreamMediaState();
	return TRUE;
}

bool CFunction::registerToKernelBoard() {
	if (!m_pKernel->getKernelBoardCommandHandle()) {
		return false;
	}
	IC2CMD_CmdID CmdID;
	CmdID.dwCommand = CMD_PR_FROMSTREAM;
	CmdID.dwFlag = FLAG_FROMSTREAM_CONNECT;

	m_pKernel->getKernelBoardCommandHandle()->sendMsg(CmdID, NULL, 0, NULL);

	usleep(1000);
	sendStreamMediaState();
	return true;
}

bool CFunction::unregisterToKernelBoard() {
	if (!m_pKernel->getKernelBoardCommandHandle()) {
			return FALSE;
	}
	IC2CMD_CmdID CmdID;
	CmdID.dwCommand = CMD_PR_FROMSTREAM;
	CmdID.dwFlag = FLAG_FROMSTREAM_DISCONNECT;

	m_pKernel->getKernelBoardCommandHandle()->sendMsg(CmdID, NULL, 0, NULL);

	m_pKernel->getStateHandle()->setPowerOff(TRUE);
	return true;
}

BOOL CFunction::sendHeartBeat() {
	IC2CMD_CmdID CmdID;
	CmdID.dwCommand = CMD_PR_FROMSTREAM;
	CmdID.dwFlag = FLAG_FROMSTREAM_HEARTBEAT;

	m_pKernel->getKernelBoardCommandHandle()->sendMsg(CmdID, NULL, 0, NULL);
	return TRUE;
}

BOOL CFunction::SendNotify(string strNotify, BOOL bError) {
	if (bError) {
		IC2CMD_CmdID CmdID;
		CmdID.dwCommand = CMD_PR_FROMSTREAM;
		CmdID.dwFlag = FLAG_FROMSTREAM_NOTIFYERROR;

		if (strNotify.size() >= 255) {
			return FALSE;
		}
		char cNotify[256] = { 0 };
		strcpy(cNotify, strNotify.c_str());

		m_pKernel->getKernelBoardCommandHandle()->sendMsg(CmdID, NULL, 256, (BYTE*) cNotify);
	}

	return TRUE;
}

BOOL CFunction::sendStreamMediaState() {
	IC2CMD_CmdID CmdID;
	CmdID.dwCommand = CMD_PR_FROMSTREAM;
	CmdID.dwFlag = FLAG_FROMSTREAM_STATE;

	StreamMediaState* state = m_pKernel->getStateHandle()->getStreamMediaState();
	return m_pKernel->getKernelBoardCommandHandle()->sendMsg(CmdID, NULL, sizeof(*state), (BYTE*)state);
}

BOOL CFunction::sendQueryResponce(char* strFromIp) {
	IC2CMD_CmdID CmdID;
	CmdID.dwCommand = CMD_PR_FROMSTREAM;
	CmdID.dwFlag = FLAG_FROMSTREAM_RESPONSE;

	StreamMediaCommonInfo	info;
	info.dwIP = ntohl(inet_addr(m_pSetting->getOwnIP().c_str()));
	info.dwVersion = 7;

	return m_pKernel->getKernelBoardCommandHandle()->sendMsg(CmdID, NULL, sizeof(info),
			(BYTE*)&info, ntohl(inet_addr(strFromIp)));
}

BOOL CFunction::sendSystemStatusResponse() {
	IC2CMD_CmdID CmdID;
	CmdID.dwCommand = CMD_PR_FROMSTREAM;
	CmdID.dwFlag = FLAG_FROMSTREAM_REPONSESYSTEMSTATUS;


	return m_pKernel->getKernelBoardCommandHandle()->sendMsg(CmdID, NULL, 0, NULL);
}
