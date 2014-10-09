#include "mtgClient.h"
#include <dlfcn.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>


//#define MtgSoFile "mtgsdk.so"

extern int (*pfun_wx_mtg_init)(char*,char*,unsigned short,unsigned short);
extern	int (*pfun_wx_mtg_uninit)(DWORD );
extern	int (*pfun_wx_mtg_get_setting)(s_handle_t,int,DWORD*,void*);
extern	int (*pfun_wx_mtg_set_setting)(s_handle_t,int,DWORD*,void*);
extern	s_handle_t (*pfun_wx_mtg_join)(char*,WxFunCallBack,void*,DWORD,DWORD,WORD,char*,char*,StMbyteData*);
extern	int (*pfun_wx_mtg_modify)(s_handle_t,WORD,WxDataUpdate*);
extern	int (*pfun_wx_mtg_register)(s_handle_t,char*,WxFunCallBack,void*);
extern	int (*pfun_wx_mtg_get_buffer_size)(s_handle_t,BYTE,DWORD*);
extern	int (*pfun_wx_mtg_set_buffer_size)(s_handle_t,BYTE,char*,DWORD);
extern	int (*pfun_wx_mtg_subscribe)(s_handle_t,char*,BYTE);
extern	int (*pfun_wx_mtg_leave)(s_handle_t,short);
extern	void (*pfun_wx_mtg_timer)();
extern	int (*pfun_wx_mtg_detect_stream)(s_handle_t,BYTE);
extern	int (*pfun_wx_mtg_rejoin)(s_handle_t);


int MtgClient::mtgjoin_callback(void* user_data, DWORD conf_id, short msg,
		DWORD uParam1, DWORD uParam2, void* pParam) {
	MtgClient* pSelf = (MtgClient*) user_data;
	if (NULL == pSelf) {
		return 0;
	}
	if (MTG_MSG_TYPE_LEFT == msg) {
		printf("conf id %d\n", conf_id);
	}
	pSelf->procJoinCallback(msg, uParam1, uParam2, pParam);
	return 1;
}

MtgClient::MtgClient() :
		m_bInited(false) {

	//m_plibHandle = NULL;
	sem_init(&m_joinSem, 0, 0);
	m_joinThreadId = 0;
	m_audioThreadId = 0;
}

MtgClient::~MtgClient() {
	uninit();
	sem_destroy(&m_joinSem);
	//if (NULL != m_plibHandle) {
		//dlclose(m_plibHandle);
	//}
}

bool MtgClient::init(const char* pMtgIP, int confId, int userId) {
	//if (!loadMtgFunc()) {
	//	return false;
	//}
	m_strMtgIP = pMtgIP;
	m_confId = confId;
	m_userId = userId;

	m_bInited = false;
	return true;
}

bool MtgClient::uninit() {
	if (0 != m_joinThreadId) {
		pthread_cancel(m_joinThreadId);
		pthread_join(m_joinThreadId, NULL);
		m_joinThreadId = 0;
	}

	if (m_bInited) {
		m_bInited = false;
		if (0 != m_audioThreadId) {
			sem_wait(&m_audioThreadExitSem);
			pthread_cancel(m_audioThreadId);
			pthread_join(m_audioThreadId, NULL);
			pthread_attr_destroy(&m_audioThreadAttr);
			sem_destroy(&m_audioThreadExitSem);
			m_audioThreadId = 0;
		}
	}
	printf("mtg client after exit audio thread\n");

	leaveConf();
	return true;
}

bool MtgClient::addListener(IMtgListener * listener) {
	for (vector<IMtgListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
		if ( *it == listener) {
			return false;
		}
	}
	m_vecListeners.push_back(listener);
	return true;
}

bool MtgClient::removeListener(IMtgListener* listener) {
	for (vector<IMtgListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
		if ( *it == listener) {
			m_vecListeners.erase(it);
			return true;
		}
	}
	return true;
}

bool MtgClient::procJoin() {
	printf("user id is %d\n", m_userId);
	sprintf(m_userSeq, "%d", m_userId);
	//m_userSeq[2] = 0;
	printf("seq %s\n", m_userSeq);
	printf("the mtg ip is %s\n", m_strMtgIP.c_str());

	/*serverip,callback,this,siteid,confid,uid,seqid,confkey,null*/
	m_conf_handle = (*pfun_wx_mtg_join)((char*)m_strMtgIP.c_str(), mtgjoin_callback, this, 0,
			m_confId, m_userId, m_userSeq, "wxplanform", NULL);
	if (NULL == m_conf_handle) {
		printf("conf_handle=NULL\n");
		return false;
	}

	timespec tv;
	clock_gettime(CLOCK_REALTIME, &tv);
	tv.tv_sec += 60;
	tv.tv_nsec = 0;

	sem_timedwait(&m_joinSem, &tv);

	if (!m_bInited) {
		printf("leave conf\n");
		leaveConf();
		for (vector<IMtgListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
			(*it)->onJoinCallback(this, false);
		}
		return false;
	}
	for (vector<IMtgListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
		(*it)->onJoinCallback(this, true);
	}


	return true;
}

bool MtgClient::joinConf() {
	pthread_attr_init(&m_joinThreadAttr);
	if (0 != pthread_create(&m_joinThreadId, &m_joinThreadAttr, joinThreadProc, this)) {
		printf("create join thread failed\n");
		return false;
	}

	return true;
}

bool MtgClient::createAudioThread() {
	pthread_attr_init(&m_audioThreadAttr);
	if (0 != pthread_create(&m_audioThreadId,
			&m_audioThreadAttr,
			audioThreadProc,
			this)) {
		printf("mtg create audio thread failed\n");
		return false;
	}
	sem_init(&m_audioThreadExitSem, 0 , 0);
	return true;
}

bool MtgClient::leaveConf() {
	if (NULL != m_conf_handle) {
		(*pfun_wx_mtg_leave)(m_conf_handle, KLeaveNormal);

		m_conf_handle = NULL;
		m_userId = 0;
		memset(m_userSeq, 0, sizeof(m_userSeq));
	}
	m_bInited = false;
	return true;
}

bool MtgClient::getJoinStatus() {
	return m_bInited;
}

bool MtgClient::subscribe(const char* pPath, bool bOff) {
	if (NULL == m_conf_handle) {
		return false;
	}
	int res = (*pfun_wx_mtg_subscribe) (m_conf_handle, (char*)pPath, bOff);
	return res == 0;
}

void MtgClient::procJoinCallback(short msg, DWORD uParam1, DWORD uParam2, void* pParam) {
	switch (msg) {
	case MTG_MSG_TYPE_JOINED: // uParam1=result, uParam2=count, pParam=SmpnUpdate*
		if (!m_bInited) {
			if (uParam1 == 0) {
				printf("\n Join Ok! \n");
				m_bInited = true;
			} else if (MTG_LEFT_REASON_CONNECT_FAILED == uParam1
					|| MTG_LEFT_REASON_WAIT_FOR_HOST == uParam1) {
				printf("join failed result =%d\n", uParam1);
			} else {
			//	printf("Join Failed, result=%d\n", uParam1);
			//	m_bInited = false;
				printf("join failed result = %d\n", uParam1);
			}
			sem_post(&m_joinSem);
		}
		break;

	case MTG_MSG_TYPE_RESOURCE_UPDATE:
		for (int i = 0; i < uParam2; ++ i) {
			WxDataUpdate* pData = &((WxDataUpdate*)pParam)[i];

			if (NULL == pData) {
				continue;
			}
			if (pData->uid.data != NULL) {
				for (vector<IMtgListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
					(*it)->onRecvMtgData(this, (const char*)pData->path,
							(const char*)pData->param_bs.data, pData->param_bs.len, pData->data_type, pData->action);
				}
			}

			//printf("on recv mtg data\n");
		}
		break;
	case MTG_MSG_TYPE_LEFT:
		printf("receive left msg\n");
		if (MTG_LEFT_REASON_CONNECT_FAILED == uParam1
				|| MTG_LEFT_REASON_NETWORK_BROKEN == uParam1) {
			printf("user left MTG_MSG_TYPE_LEFT %d\n", uParam1);
			printf("left param %s\n", m_strMtgIP.c_str());
		//(*pfun_wx_mtg_rejoin)(m_conf_handle);
			if (!m_bInited) {
				//sem_post(&m_joinSem);
			}
			//m_bInited = false;
		}
		m_bInited = false;
		break;

	default:
		printf("Join msg= %d\n", msg);
		break;
	}
}

bool MtgClient::create(const char* pPath, int type, BYTE* pInitData, int len) {
	if (NULL == pPath || NULL == m_conf_handle) {
		printf("mtg create path length is zero or handle is NULL");
		return false;
	}

	WxDataUpdate update_list;
	memset(&update_list, 0, sizeof(update_list));

	update_list.data_type	= type;
	update_list.action		= MTG_ACTION_VIDEO_CREATE;
	update_list.attrib		= MTG_ATTRIB_OWNED_BY_NODE;			// only used when create
	update_list.priority	= KBinaryPriority;
	if (type == MTG_DATA_TYPE_VIDEO) {
		update_list.priority = KVideoPriority;
	} else if (type == MTG_DATA_TYPE_AUDIO) {
		update_list.priority = KAudioPriority;
	}

	update_list.path		= pPath;


	update_list.uid.data = (BYTE*)m_userSeq;
	update_list.uid.len = strlen(m_userSeq);

	update_list.param_dw	= 0;
	update_list.param_dw2	= 0;
	update_list.param_dw3	= 0;
	update_list.param_bs.len= len;
	update_list.param_bs.data = pInitData;
	update_list.reserved	= 3;
	int res = (*pfun_wx_mtg_modify)(m_conf_handle, 1, &update_list);

	if (res == 0) {
		if (type == MTG_DATA_TYPE_VIDEO) {
			//当数据包到达该内存时，开始丢数据，以便保证内存不会一直增长而导致的程序崩溃
			(*pfun_wx_mtg_set_buffer_size)(m_conf_handle, KVideoPriority, (char*)pPath, KBufferSize);
			//(*pfun_wx_mtg_set_buffer_size)(m_conf_handle, 2, (char*)path.c_str(), (1024 * 1024 * 24L));
		}
	}
	if (res != 0) {
		printf("create path  res %d\n",  res);
		printf("path %s\n", pPath);
	}

	return res == 0;
}


bool MtgClient::destory(const char* pPath, int type) {
	if (NULL == m_conf_handle || NULL == pPath) {
		return false;
	}

	WxDataUpdate update_list;
	memset(&update_list, 0, sizeof(update_list));

	update_list.data_type	= type;
	update_list.action		= MTG_ACTION_VIDEO_DESTROY;
	update_list.attrib		= MTG_ATTRIB_OWNED_BY_NODE;			// only used when create
	update_list.priority	= KBinaryPriority;
	if (type == MTG_DATA_TYPE_VIDEO) {
		update_list.priority = KVideoPriority;
	} else if (type == MTG_DATA_TYPE_AUDIO) {
		update_list.priority = KAudioPriority;
	}

	update_list.path		= pPath;

	update_list.uid.data = (BYTE*)m_userSeq;
	update_list.uid.len = strlen(m_userSeq);

	update_list.param_dw	= 0;
	update_list.param_dw2	= 0;
	update_list.param_dw3	= 0;
	update_list.param_bs.len= 0;
	update_list.param_bs.data = NULL;
	update_list.reserved	= 0;

	int res = (*pfun_wx_mtg_modify)(m_conf_handle, 1, &update_list);

	return res == 0;
}

bool MtgClient::sendData(const string& path, const char* pData, int len, int type) {
	if (path.length() == 0 || NULL == m_conf_handle) {
		return false;
	}

	if (type == MTG_DATA_TYPE_VIDEO) {
		DWORD maxSize = KBufferSize;
		int curSize = (*pfun_wx_mtg_get_buffer_size)(m_conf_handle, KVideoPriority, &maxSize);

		if (curSize + len >= KBufferSize) {
			//缓冲区剩余大小小于一帧数据的长度，则没有必要发送
			return false;
		}
	}


	WxDataUpdate update_item;
	memset(&update_item, 0, sizeof(WxDataUpdate));


	update_item.data_type 	= type;
	update_item.action		= MTG_ACTION_VIDEO_DATA;
	update_item.attrib		= MTG_ATTRIB_OWNED_BY_NODE;			// only used when create
	update_item.priority	= KBinaryPriority;
	if (type == MTG_DATA_TYPE_VIDEO) {
		update_item.priority = KVideoPriority;
	} else if (type == MTG_DATA_TYPE_AUDIO) {
		update_item.priority = KAudioPriority;
	}

	update_item.path = path.c_str();

	update_item.uid.data = (BYTE*)m_userSeq;
	update_item.uid.len = strlen(m_userSeq);

	update_item.param_dw = 0;
	update_item.param_dw2 = -1;
	update_item.param_dw3 = 0;
	update_item.param_bs.data = (BYTE *)pData;
	update_item.param_bs.len = len;
	update_item.reserved = 10;

	int res = (*pfun_wx_mtg_modify)(m_conf_handle, 1, &update_item);
	return res == 0;
}

void* MtgClient::audioThreadProc(void* pParam) {
	MtgClient* pSelf = (MtgClient*) pParam;
	pSelf->detectAudioStream();

	pthread_exit(NULL);
	return NULL;
}

void*  MtgClient::joinThreadProc(void* pParam)
{
	MtgClient* pSelf = (MtgClient*)pParam;
	if (NULL != pSelf) {
		pSelf->procJoin();
	}
	pthread_exit(NULL);
	return NULL;
}

void MtgClient::detectAudioStream() {
	while (m_bInited) {
		(*pfun_wx_mtg_detect_stream)(m_conf_handle, MTG_DATA_TYPE_AUDIO);

		usleep(10000);
	}
	sem_post(&m_audioThreadExitSem);
}


unsigned long MtgClient::getSendCount() {
	unsigned long nSendCount = 0;
	(*pfun_wx_mtg_set_setting)(NULL, MTG_GET_OPT_OUTGOING_SEND, &nSendCount, NULL);
	return nSendCount;
}

unsigned long MtgClient::getRecvCount() {
	unsigned long nRecvCount = 0;
	(*pfun_wx_mtg_set_setting)(NULL, MTG_GET_OPT_INCOMING_RECV, &nRecvCount, NULL);
	return nRecvCount;
}

unsigned long MtgClient::getSendLossCount() {
	unsigned long nSendLossCount = 0;
	(*pfun_wx_mtg_set_setting)(NULL, MTG_GET_OPT_OUTGOING_DELETE, &nSendLossCount, NULL);
	return nSendLossCount;
}

unsigned long MtgClient::getRecvLossCount() {
	unsigned long nRecvLossCount = 0;
	(*pfun_wx_mtg_set_setting)(NULL, MTG_GET_OPT_INCOMING_DELETE, &nRecvLossCount, NULL);
	return nRecvLossCount;
}


/*bool MtgClient::loadMtgFunc() {
	m_plibHandle = dlopen(MtgSoFile, RTLD_LAZY); //RTLD_LAZY
	if (NULL == m_plibHandle) {
		return false;
	}
	pfun_wx_mtg_init = (int (*)(char*, char*, unsigned short, unsigned short))
			dlsym(m_plibHandle, "wx_mtg_init");
	if (NULL == pfun_wx_mtg_init) {
		return false;
	}
	pfun_wx_mtg_uninit = (int (*)(DWORD))
			dlsym(m_plibHandle, "wx_mtg_uninit");
	if (NULL == pfun_wx_mtg_uninit) {
		return false;
	}
	pfun_wx_mtg_get_setting = (int (*)(s_handle_t, int, DWORD*, void*))
			dlsym(m_plibHandle, "wx_mtg_get_setting");
	if (NULL == pfun_wx_mtg_get_setting) {
		return false;
	}
	pfun_wx_mtg_set_setting = (int (*)(s_handle_t, int, DWORD*, void*))
			dlsym(m_plibHandle, "wx_mtg_set_setting");
	if (NULL == pfun_wx_mtg_set_setting) {
		return false;
	}
	pfun_wx_mtg_join = (s_handle_t (*)(char*, WxFunCallBack, void*, DWORD,
			DWORD, WORD, char*, char*, StMbyteData*))
			dlsym(m_plibHandle, "wx_mtg_join");
	if (NULL == pfun_wx_mtg_join) {
		return false;
	}
	pfun_wx_mtg_modify = (int (*)(s_handle_t, WORD, WxDataUpdate*))
			dlsym( m_plibHandle, "wx_mtg_modify");
	if (NULL == pfun_wx_mtg_modify) {
		return false;
	}
	pfun_wx_mtg_register = (int (*)(s_handle_t, char*, WxFunCallBack, void*))
			dlsym( m_plibHandle, "wx_mtg_register");
	if (NULL == pfun_wx_mtg_register) {
		return false;
	}
	pfun_wx_mtg_get_buffer_size = (int (*)(s_handle_t, BYTE, DWORD*))
			dlsym( m_plibHandle, "wx_mtg_get_buffer_size");
	if (NULL == pfun_wx_mtg_get_buffer_size) {
		return false;
	}
	pfun_wx_mtg_set_buffer_size = (int (*)(s_handle_t, BYTE, char*, DWORD))
			dlsym( m_plibHandle, "wx_mtg_set_buffer_size");
	if (NULL == pfun_wx_mtg_set_buffer_size) {
		return false;
	}
	pfun_wx_mtg_subscribe = (int (*)(s_handle_t, char*, BYTE))
			dlsym( m_plibHandle, "wx_mtg_subscribe");
	if (NULL == pfun_wx_mtg_subscribe) {
		return false;
	}
	pfun_wx_mtg_leave = (int (*)(s_handle_t, short))
			dlsym( m_plibHandle, "wx_mtg_leave");
	if (NULL == pfun_wx_mtg_leave) {
		return false;
	}
	pfun_wx_mtg_timer = (void (*)())
			dlsym( m_plibHandle, "wx_mtg_timer");
	if (NULL == pfun_wx_mtg_timer) {
		return false;
	}
	pfun_wx_mtg_detect_stream = (int (*)(s_handle_t, BYTE))
			dlsym( m_plibHandle, "wx_mtg_detect_stream");
	if (NULL == pfun_wx_mtg_detect_stream) {
		return false;
	}

	pfun_wx_mtg_rejoin = (int (*) (s_handle_t))
			dlsym(m_plibHandle, "wx_mtg_rejoin");
	if (NULL == pfun_wx_mtg_rejoin) {
		return false;
	}
	return true;
}*/
