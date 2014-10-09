#pragma once
#include <pthread.h>
#include "../../ThirdParty/mtg/mtg.h"
#include <string>
#include <vector>
#include <semaphore.h>
using namespace std;

#define JOIN_UID_RES_MAX 10


class MtgClient;
class IMtgListener {
public:
	virtual ~IMtgListener() {

	}

	virtual void onRecvMtgData(MtgClient* pMtg, const char* path,
			const char*data, int dataLen, int datatype, int action) {

	}

	virtual void onJoinCallback(MtgClient* pMtg, bool bJoinOK) {

	}

};


class MtgClient {
public:
	static void* audioThreadProc(void* pParam);

	static void* joinThreadProc(void* pParam);

	static int mtgjoin_callback(void* user_data, tMtgConfID conf_id,
			short msg, DWORD uParam1, DWORD uParam2, void* pParam);

public:
	MtgClient();
	~MtgClient();

public:
	bool init(const char* pMtgIP, int confId, int userId);

	bool uninit();

	bool addListener(IMtgListener * listener);

	bool removeListener(IMtgListener* listener);

	/**<
	 * \brief 其中type可以是MTG_DATA_TYPE_VIDEO,MTG_DATA_TYPE_BINARY,MTG_DATA_TYPE_AUDIO
	 */
	bool sendData(const string& path, const char* pData, int len, int type);

	bool create(const char* pPath, int type = MTG_DATA_TYPE_VIDEO,
			BYTE* pInitData = NULL, int len = 0);

	bool destory(const char* pPath, int type = MTG_DATA_TYPE_VIDEO);

	bool subscribe(const char* pPath, bool off);

	bool joinConf();

	bool leaveConf();

	bool createAudioThread();

	bool getJoinStatus();

	int UserID() {
		return m_userId;
	}

	string mtgIP() {
		return m_strMtgIP;
	}

	unsigned long getSendCount();

	unsigned long getRecvCount();

	unsigned long getSendLossCount();

	unsigned long getRecvLossCount();

private:

	void detectAudioStream();

	bool procJoin();

	void procJoinCallback(short msg, DWORD uParam1, DWORD uParam2, void* pParam);
	//bool loadMtgFunc();


private:
	//void *m_plibHandle;

	pthread_t m_audioThreadId;
	pthread_attr_t m_audioThreadAttr;

	pthread_t	m_joinThreadId;
	pthread_attr_t	m_joinThreadAttr;

	int m_confId;
	bool m_bInited;

	string m_strMtgIP;

	//sem_t m_heartbeatExitSem;
	sem_t m_joinSem;
	sem_t m_audioThreadExitSem;

	char m_userSeq[8];
	int	 m_userId;
	s_handle_t m_conf_handle;
	vector<IMtgListener*> m_vecListeners;

	static const long KBufferSize = 1024 * 1024 * 4L;
	static const int KVideoPriority = 5;
	static const int KAudioPriority = 2;
	static const int KBinaryPriority = 0;
	static const int KLeaveTerminate = 1;
	static const int KLeaveNormal = 0;
	static const int KUninitFlagNormal = 0;

private:

	/*int (*pfun_wx_mtg_init)(char*,char*,unsigned short,unsigned short);
	int (*pfun_wx_mtg_uninit)(DWORD );
	int (*pfun_wx_mtg_get_setting)(s_handle_t,int,DWORD*,void*);
	int (*pfun_wx_mtg_set_setting)(s_handle_t,int,DWORD*,void*);
	s_handle_t (*pfun_wx_mtg_join)(char*,WxFunCallBack,void*,DWORD,DWORD,WORD,char*,char*,StMbyteData*);
	int (*pfun_wx_mtg_modify)(s_handle_t,WORD,WxDataUpdate*);
	int (*pfun_wx_mtg_register)(s_handle_t,char*,WxFunCallBack,void*);
	int (*pfun_wx_mtg_get_buffer_size)(s_handle_t,BYTE,DWORD*);
	int (*pfun_wx_mtg_set_buffer_size)(s_handle_t,BYTE,char*,DWORD);
	int (*pfun_wx_mtg_subscribe)(s_handle_t,char*,BYTE);
	int (*pfun_wx_mtg_leave)(s_handle_t,short);
	void (*pfun_wx_mtg_timer)();
	int (*pfun_wx_mtg_detect_stream)(s_handle_t,BYTE);
	int (*pfun_wx_mtg_rejoin)(s_handle_t);*/

};

