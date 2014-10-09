#include "VideoFactory.h"
#include "HikVideo.h"

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

#define HIKVISION "HikVision"

int KVideoCreatorFactory::m_nInstanceCount = 0;

KVideoInterface* KVideoCreatorFactory::createInstance(
		  const string& description){
	if (description == HIKVISION)
	{
		if (m_nInstanceCount == 0) {
			(*pfInit)();
			(*pfSetConnectTime)(2000, 1);
			(*pfSetReconnect) (10000, true);
		}
		m_nInstanceCount++;
		HikVideo* pHikVideo = new HikVideo();
		pHikVideo->getCardInfo()->setCardDescription(description);
		return pHikVideo;
	}

	return NULL;
}

void KVideoCreatorFactory::destroyInstance(KVideoInterface*& pVideo) {
	if (NULL != pVideo) {
		printf("close step 1\n");
		pVideo->close();
		printf("close step 2\n");
		delete pVideo;
		printf("step 3\n");
		pVideo = NULL;

		m_nInstanceCount--;
		if (m_nInstanceCount == 0) {
			(*pfNet_DVR_Cleanup)();
		}
	}
}
