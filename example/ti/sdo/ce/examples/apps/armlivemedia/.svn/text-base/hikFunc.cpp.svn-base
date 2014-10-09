#include "hikFunc.h"
#include "HCNetSDK.h"
#include "AnalyzeDataNewInterface.h"
#include <dlfcn.h>
#include <iostream>
using namespace std;

void   *m_pdHikHandle = NULL; /**< 动态库指针 */
void   *m_pdAnalyHandle = NULL;

#define HikSoFile "libhcnetsdk.so"
#define HikStreamSoFile "libStreamTransClient.so"
#define HikAnalyzeSoFile "libanalyzedata.so"


BOOL (*pfInit)();
BOOL (*pfSetConnectTime)(DWORD, DWORD);
BOOL (*pfSetReconnect)(DWORD, BOOL);
LONG (*pfLogin)(char* pIP, WORD port, char* pUsername, char* pPassword, NET_DVR_DEVICEINFO_V30* pNetDeviceInfo);
LONG (*pfRealPlay)(LONG, NET_DVR_CLIENTINFO*, void (CALLBACK *)(LONG, DWORD, BYTE*, DWORD, void*), void*, BOOL);
BOOL (*pfSetRealDataCallBack)(LONG, void (CALLBACK*)(LONG, DWORD, BYTE*, DWORD, DWORD), DWORD);
BOOL (*pfMakeKeyFrame)(LONG, LONG);
BOOL (*pfStopRealPlay)(LONG);
void (*pfAnalyzeDestroy)(void*);
BOOL (*pfNet_DVR_Logout) (LONG);
BOOL (*pfNet_DVR_Cleanup)();
int (*pfInputData)(void *, unsigned char * , unsigned long);
int (*pfGetOnePacket)(void *, PACKET_INFO_EX*);
void * (*pfCreateStreamEx)(unsigned long, unsigned char *);



bool loadHikFunc() {
	m_pdHikHandle = dlopen(HikSoFile, RTLD_LAZY);
	if (NULL == m_pdHikHandle) {
		cout << "can not find hik lib" << endl;
	}
	m_pdAnalyHandle = dlopen(HikAnalyzeSoFile, RTLD_LAZY);
	if (NULL == m_pdAnalyHandle) {
		cout << "can not find analyze lib" <<endl;
	}
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
}

bool unloadHikFunc() {
	if (NULL != m_pdHikHandle) {
		dlclose(m_pdHikHandle);
		m_pdHikHandle = NULL;
	}
	if (NULL != m_pdAnalyHandle) {
		dlclose(m_pdAnalyHandle);
		m_pdAnalyHandle = NULL;
	}
	return true;
}
