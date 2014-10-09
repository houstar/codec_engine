#ifndef HIK_VIDEO_H
#define HIK_VIDEO_H

#include "KVideoInterface.h"
#include "HCNetSDK.h"
#include "AnalyzeDataNewInterface.h"
#include "../core/Mutex.h"
#include <vector>
using namespace std;

namespace KVideo {
class HikVideo: public KVideo::KVideoInterface {
public:
	HikVideo();
	virtual ~HikVideo();

	virtual KVideo::CardInfo* getCardInfo();

	virtual int getChannelID();

	virtual bool setChannelID(int channelID);

	virtual bool addListener(KVideo::IStreamVideoListener* listener);

	virtual bool open(bool aSubStream = false);

	virtual bool close();

	virtual bool isOpen();

public:
	void DataCallback(DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize);

protected:
	//bool loadFunc();

private:
	int m_channelid;
	CardInfo m_cardinfo;
	bool m_isopen;
	bool m_isSoundopen;
	vector<KVideo::IStreamVideoListener*> m_listeners;
	HANDLE m_hAnalyze;
	long	m_userId;
	long	m_handle;
	int		m_timerCount;
	//void   *m_pdHikHandle; /**< 动态库指针 */
	//void   *m_pdAnalyHandle;

private:

	/*BOOL (*pfInit)();
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
	void * (*pfCreateStreamEx)(unsigned long, unsigned char *);*/
};
}
#endif
