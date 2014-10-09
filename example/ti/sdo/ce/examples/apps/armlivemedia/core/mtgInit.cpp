#include "mtgInit.h"
#include "../../ThirdParty/mtg/mtg.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

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


void* MtgHeartbeat::mtgHeartbeatCallback(void* pParam) {
	MtgHeartbeat* pSelf = (MtgHeartbeat*) pParam;
	pSelf->procHeartbeat();
	pthread_exit(NULL);
	return NULL;
}

void MtgHeartbeat::procHeartbeat() {
	while (m_bHeartbeat) {
		(*pfun_wx_mtg_timer)();
		usleep(10000);
	}
	printf("exit heart beat thread\n");
	sem_post(&m_heartbeatExitSem);
}

MtgHeartbeat::MtgHeartbeat()
: m_bHeartbeat(false)
, m_heartbeatThreadId(0) {
	sem_init(&m_heartbeatExitSem, 0, 0);
}


MtgHeartbeat::~MtgHeartbeat() {
	uninit();
	sem_destroy(&m_heartbeatExitSem);
	(*pfun_wx_mtg_uninit)(0);
}

bool MtgHeartbeat::init() {
	int ret = -1;

	ret = (*pfun_wx_mtg_init)(NULL, NULL, 0, 0);
	printf("mtgclient init  ret %d",  ret);

	int http_port = 0;
	int udp_port = 0;
	int tcp_port = 443;
	(*pfun_wx_mtg_set_setting)(NULL, MTG_SET_OPT_HTTP_PORT, NULL, (void*)&http_port);
	(*pfun_wx_mtg_set_setting)(NULL, MTG_SET_OPT_TCP_PORT, NULL, (void*)&tcp_port);
	(*pfun_wx_mtg_set_setting)(NULL, MTG_SET_OPT_UDP_PORT, NULL, (void*)&udp_port);

	return true;
}

bool MtgHeartbeat::uninit() {
	stopThread();
	return true;
}

bool MtgHeartbeat::startThread() {
	m_bHeartbeat = true;
	pthread_attr_init(&m_heartbeatThreadAttr);
	if (0 != pthread_create(&m_heartbeatThreadId, &m_heartbeatThreadAttr,
			mtgHeartbeatCallback, this)) {
		printf("create thread failed\n");
		return false;
	}

	return true;
}

bool MtgHeartbeat::stopThread() {
	if (0 != m_heartbeatThreadId) {
		m_bHeartbeat = false;
		printf("before mtg client get exit sem\n");
		sem_wait(&m_heartbeatExitSem);
		printf("mtg client get exit sem\n");
		pthread_cancel(m_heartbeatThreadId);
		pthread_join(m_heartbeatThreadId, NULL);
		pthread_attr_destroy(&m_heartbeatThreadAttr);
		m_heartbeatThreadId = 0;

	}
	return true;
}
