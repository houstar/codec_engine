#include "HikVideo.h"
#include <dlfcn.h>
#include <iostream>
#include <string.h>

#define HikSoFile "libhcnetsdk.so"
#define HikStreamSoFile "libStreamTransClient.so"
#define HikAnalyzeSoFile "libanalyzedata.so"

using namespace KVideo;
using namespace std;


extern BOOL (*pfInit)();
extern BOOL (*pfSetConnectTime)(DWORD, DWORD);
extern BOOL (*pfSetReconnect)(DWORD, BOOL);
extern LONG (*pfLogin)(char* pIP, WORD port, char* pUsername, char* pPassword, NET_DVR_DEVICEINFO_V30* pNetDeviceInfo);
extern LONG (*pfRealPlay)(LONG, NET_DVR_CLIENTINFO*, void (CALLBACK *)(LONG, DWORD, BYTE*, DWORD, void*), void*, BOOL);
extern BOOL (*pfSetRealDataCallBack)(LONG, void (CALLBACK*)(LONG, DWORD, BYTE*, DWORD, DWORD), DWORD);
extern BOOL (*pfMakeKeyFrame)(LONG, LONG);
extern BOOL (*pfStopRealPlay)(LONG);
extern void (*pfAnalyzeDestroy)(void*);
extern BOOL (*pfNet_DVR_Logout) (LONG);
extern BOOL (*pfNet_DVR_Cleanup)();
extern int (*pfInputData)(void *, unsigned char * , unsigned long);
extern int (*pfGetOnePacket)(void *, PACKET_INFO_EX*);
extern void * (*pfCreateStreamEx)(unsigned long, unsigned char *);

static void CALLBACK g_STDDataCallBack_V30(LONG lRealHandle, DWORD dwDataType,
		BYTE *pBuffer, DWORD dwBufSize, DWORD dwUser) {
	HikVideo *pSelf = (HikVideo *) dwUser;
	if (NULL != pSelf) {
		pSelf->DataCallback(dwDataType, pBuffer, dwBufSize);
	}
}

HikVideo::HikVideo() :
		m_channelid(0), m_isopen(false), m_isSoundopen(false),
		m_timerCount(0), m_hAnalyze(NULL) {
	m_cardinfo.setPort(8000);
	m_cardinfo.setUsername("admin");
	m_cardinfo.setPassword("12345");
	/*m_pdHikHandle = dlopen(HikSoFile, RTLD_LAZY);
	if (NULL == m_pdHikHandle) {
		cout << "can not find hik lib" << endl;
	}
	m_pdAnalyHandle = dlopen(HikAnalyzeSoFile, RTLD_LAZY);
	if (NULL == m_pdAnalyHandle) {
		cout << "can not find analyze lib" <<endl;
	}*/

	//loadFunc();
}

HikVideo::~HikVideo() {
	close();
	//dlclose(m_pdHikHandle);
	//dlclose(m_pdAnalyHandle);
}

/*bool HikVideo::loadFunc() {
	pfInit = (BOOL (*)() ) dlsym(m_pdHikHandle, "NET_DVR_Init");
	if (NULL == pfInit) {
		return false;
	}

	pfSetConnectTime = (BOOL (*)(DWORD, DWORD) ) dlsym(m_pdHikHandle, "NET_DVR_SetConnectTime");
	if (NULL == pfSetConnectTime) {
		return false;
	}

	pfSetReconnect = (BOOL (*)(DWORD, BOOL) )dlsym(m_pdHikHandle, "NET_DVR_SetReconnect");
	if (NULL == pfSetReconnect) {
		return false;
	}

	pfLogin = (LONG (*)(char*, WORD, char*, char*,
			NET_DVR_DEVICEINFO_V30*)) dlsym(m_pdHikHandle, "NET_DVR_Login_V30");
	if (NULL == pfLogin) {
		return false;
	}

	pfRealPlay = (LONG (*)(LONG, NET_DVR_CLIENTINFO*,
			void (CALLBACK *)(LONG, DWORD, BYTE*, DWORD, void*), void*,
			BOOL))dlsym(m_pdHikHandle, "NET_DVR_RealPlay_V30");
	if (NULL == pfRealPlay) {
		return false;
	}

	pfNet_DVR_Cleanup = (BOOL (*) ()) dlsym(m_pdHikHandle, "NET_DVR_Cleanup");
	if (NULL == pfNet_DVR_Cleanup) {
		return false;
	}

	pfSetRealDataCallBack = (BOOL (*)(LONG,
			void (CALLBACK*)(LONG, DWORD, BYTE*, DWORD, DWORD),
			DWORD))dlsym(m_pdHikHandle, "NET_DVR_SetRealDataCallBack");
	if (NULL == pfSetRealDataCallBack) {
		return false;
	}

	pfMakeKeyFrame = (BOOL (*)(LONG, LONG)) dlsym(m_pdHikHandle, "NET_DVR_MakeKeyFrame");
	if (NULL == pfMakeKeyFrame) {
		return false;
	}

	pfStopRealPlay = (BOOL (*)( LONG) )dlsym(m_pdHikHandle, "NET_DVR_StopRealPlay");
	if (NULL == pfStopRealPlay) {
		return false;
	}

	pfAnalyzeDestroy = (void (*) (void*)) dlsym(m_pdAnalyHandle, "HIKANA_Destroy");
	if (NULL == pfAnalyzeDestroy) {
		return false;
	}

	pfNet_DVR_Logout = (BOOL (*) (LONG)) dlsym(m_pdHikHandle, "NET_DVR_Logout");
	if (NULL == pfNet_DVR_Logout) {
		return false;
	}

	pfCreateStreamEx = (void* (*)(unsigned long, unsigned char*) ) dlsym(m_pdAnalyHandle, "HIKANA_CreateStreamEx");
	if (NULL == pfCreateStreamEx) {
		return false;
	}

	pfInputData = (int (*)(void*, unsigned char*, unsigned long) ) dlsym(m_pdAnalyHandle, "HIKANA_InputData");
	if (NULL == pfInputData) {
		return false;
	}

	pfGetOnePacket = (int (*)(void*, PACKET_INFO_EX*) )dlsym(m_pdAnalyHandle, "HIKANA_GetOnePacketEx");
	if (NULL == pfGetOnePacket) {
		return false;
	}
	return true;
}*/

CardInfo* HikVideo::getCardInfo() {
	return &m_cardinfo;
}

int HikVideo::getChannelID() {
	return m_channelid;
}

bool HikVideo::setChannelID(int channelID) {
	m_channelid = channelID;
	return true;
}

bool HikVideo::addListener(IStreamVideoListener* listener) {
	for (vector<IStreamVideoListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
		if ((*it) == listener) {
			return false;
		}
	}

	m_listeners.push_back(listener);
	return true;
}

bool HikVideo::open(bool aSubStream) {
	if (m_isopen) {
		return false;
	}

	printf("before login\n");
	NET_DVR_DEVICEINFO_V30 struDeviceInfo;
	memset(&struDeviceInfo, 0, sizeof(NET_DVR_DEVICEINFO_V30));

	m_userId = (*pfLogin)((char *) m_cardinfo.getIP().c_str(), m_cardinfo.getPort(),
			(char *) m_cardinfo.getUsername().c_str(),
			(char *) m_cardinfo.getPassword().c_str(), &struDeviceInfo);

	if (m_userId == -1) {
		printf("Net_DVR_Login_V30 failed \n");
		//(*pfNet_DVR_Cleanup)();

		return false;
	}
	printf("Net_DVR_Login_V30 sucess \n");

	NET_DVR_CLIENTINFO ClientInfo = { 0 };
	ClientInfo.hPlayWnd = NULL;
	ClientInfo.lChannel = m_channelid + 1; //预览通道号

	//判断是否启用副码流
	if (aSubStream) {
		//最高位(31)为0表示主码流，为1表示子码流0～30位表示连接方式：0－TCP方式；1－UDP方式；2－多播方式；3－RTP方式;
		ClientInfo.lLinkMode = 0 | 0x80000000;
	} else {
		ClientInfo.lLinkMode = 0; //最高位(31)为0表示主码流，为1表示子码流0～30位表示连接方式：0－TCP方式；1－UDP方式；2－多播方式；3－RTP方式;
	}

	ClientInfo.sMultiCastIP = NULL; //多播地址，需要多播预览时配置

	m_handle = (*pfRealPlay)(m_userId, &ClientInfo, NULL, NULL, 0);

	if (m_handle >= 0) {
		printf("NET_DVR_RealPlay_V30 ok \n");
		(*pfSetRealDataCallBack)(m_handle, g_STDDataCallBack_V30,
				(DWORD) this);

		printf("NET_DVR_SetRealDataCallBack ok \n");
		//设置播放,arm 版不支持
		/*BOOL (*pfRigisterDrawFun)(LONG, void (CALLBACK*)(LONG, HDC, DWORD), DWORD);
		pfRigisterDrawFun = (BOOL (*)(LONG, void (CALLBACK*)(LONG, HDC, DWORD),
				DWORD) )dlsym(m_pdHikHandle, "NET_DVR_RigisterDrawFun");
		(*pfRigisterDrawFun)(m_handle, NULL, NULL);
		printf("NET_DVR_RigisterDrawFun ok \n");*/
		(*pfMakeKeyFrame)(m_userId, 1);
		printf("NET_DVR_MakeKeyFrame ok \n");

		//arm版不支持
		/*BOOL (*pfSetPlayerBufNumber)(LONG, DWORD);
		pfSetPlayerBufNumber = (BOOL (*)(LONG,
				DWORD) ) dlsym(m_pdHikHandle, "NET_DVR_SetPlayerBufNumber");
		(*pfSetPlayerBufNumber)(m_handle, 15);
		printf("NET_DVR_SetPlayerBufNumber ok \n");*/

		m_isopen = true;
		return true;
	}
	printf("NET_DVR_RealPlay_V30 failed \n");
	close();
	return false;
}

bool HikVideo::close() {
	if (!m_isopen) {
		return true;
	}
	if (m_handle != -1) {
		//(*pfSetRealDataCallBack)(m_handle, NULL, NULL);
		(*pfStopRealPlay)(m_handle);
		m_handle = -1;
	}

	if (m_userId != -1) {
		(*pfNet_DVR_Logout) (m_userId);
		m_userId = -1;
	}

	if (m_hAnalyze != NULL) {
		(*pfAnalyzeDestroy)(m_hAnalyze);
		m_hAnalyze = NULL;
	}
	//(*pfNet_DVR_Cleanup)();
	m_isopen = false;
	return true;
}

bool HikVideo::isOpen() {
	return m_isopen;
}

void HikVideo::DataCallback(DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize) {
	switch (dwDataType) {
	case NET_DVR_SYSHEAD:
		if (m_hAnalyze == NULL) {
			m_hAnalyze = (*pfCreateStreamEx)(dwBufSize, pBuffer);
			if (m_hAnalyze == NULL) {
				printf("HIKANA_CreateStreamEx error\n");
			}
		}
		break;

	case NET_DVR_STD_VIDEODATA:
	case NET_DVR_STD_AUDIODATA:
	case NET_DVR_STREAMDATA:
		if (dwBufSize > 0) {
			if (m_hAnalyze == NULL) {
				break;
			}

			if (!(*pfInputData)(m_hAnalyze, pBuffer, dwBufSize)){
				printf("HIKANA_InputData error\n");
			} else {
				while (true) {
					PACKET_INFO_EX stInfoEx   = {0};
					int hr = (*pfGetOnePacket)(m_hAnalyze, &stInfoEx);

					if (0 != hr) {
						break;
					}

					switch (stInfoEx.nPacketType) {
					case VIDEO_P_FRAME:
					case VIDEO_B_FRAME:
						for (vector<IStreamVideoListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
							(*it)->onH264(this, m_channelid, 1,
									stInfoEx.pPacketBuffer, stInfoEx.dwPacketSize);
						}
						break;
					case VIDEO_I_FRAME:
						for (vector<IStreamVideoListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
							(*it)->onH264(this, m_channelid, 0,
									stInfoEx.pPacketBuffer, stInfoEx.dwPacketSize);
						}
						break;
					case AUDIO_PACKET:
						for (vector<IStreamVideoListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it) {
							(*it)->onAAC(this, stInfoEx.pPacketBuffer, stInfoEx.dwPacketSize);
						}
						break;
					case FILE_HEAD:
						break;
					default:
						break;
					}
				}
			}

		}
		break;
	default:
		break;
	}
}
