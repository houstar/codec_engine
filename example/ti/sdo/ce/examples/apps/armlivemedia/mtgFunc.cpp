#include "mtgFunc.h"
#include <dlfcn.h>
#include "mtg.h"

int (*pfun_wx_mtg_init)(char*,char*,unsigned short,unsigned short);
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
int (*pfun_wx_mtg_rejoin)(s_handle_t);


#define MtgSoFile "mtgsdk.so"

void* m_pMtglibHandle = NULL;

bool loadMtgFunc() {
	m_pMtglibHandle = dlopen(MtgSoFile, RTLD_LAZY); //RTLD_LAZY
	if (NULL == m_pMtglibHandle) {
		return false;
	}
	pfun_wx_mtg_init = (int (*)(char*, char*, unsigned short, unsigned short))
			dlsym(m_pMtglibHandle, "wx_mtg_init");
	if (NULL == pfun_wx_mtg_init) {
		return false;
	}
	pfun_wx_mtg_uninit = (int (*)(DWORD))
			dlsym(m_pMtglibHandle, "wx_mtg_uninit");
	if (NULL == pfun_wx_mtg_uninit) {
		return false;
	}
	pfun_wx_mtg_get_setting = (int (*)(s_handle_t, int, DWORD*, void*))
			dlsym(m_pMtglibHandle, "wx_mtg_get_setting");
	if (NULL == pfun_wx_mtg_get_setting) {
		return false;
	}
	pfun_wx_mtg_set_setting = (int (*)(s_handle_t, int, DWORD*, void*))
			dlsym(m_pMtglibHandle, "wx_mtg_set_setting");
	if (NULL == pfun_wx_mtg_set_setting) {
		return false;
	}
	pfun_wx_mtg_join = (s_handle_t (*)(char*, WxFunCallBack, void*, DWORD,
			DWORD, WORD, char*, char*, StMbyteData*))
			dlsym(m_pMtglibHandle, "wx_mtg_join");
	if (NULL == pfun_wx_mtg_join) {
		return false;
	}
	pfun_wx_mtg_modify = (int (*)(s_handle_t, WORD, WxDataUpdate*))
			dlsym( m_pMtglibHandle, "wx_mtg_modify");
	if (NULL == pfun_wx_mtg_modify) {
		return false;
	}
	pfun_wx_mtg_register = (int (*)(s_handle_t, char*, WxFunCallBack, void*))
			dlsym( m_pMtglibHandle, "wx_mtg_register");
	if (NULL == pfun_wx_mtg_register) {
		return false;
	}
	pfun_wx_mtg_get_buffer_size = (int (*)(s_handle_t, BYTE, DWORD*))
			dlsym( m_pMtglibHandle, "wx_mtg_get_buffer_size");
	if (NULL == pfun_wx_mtg_get_buffer_size) {
		return false;
	}
	pfun_wx_mtg_set_buffer_size = (int (*)(s_handle_t, BYTE, char*, DWORD))
			dlsym( m_pMtglibHandle, "wx_mtg_set_buffer_size");
	if (NULL == pfun_wx_mtg_set_buffer_size) {
		return false;
	}
	pfun_wx_mtg_subscribe = (int (*)(s_handle_t, char*, BYTE))
			dlsym( m_pMtglibHandle, "wx_mtg_subscribe");
	if (NULL == pfun_wx_mtg_subscribe) {
		return false;
	}
	pfun_wx_mtg_leave = (int (*)(s_handle_t, short))
			dlsym( m_pMtglibHandle, "wx_mtg_leave");
	if (NULL == pfun_wx_mtg_leave) {
		return false;
	}
	pfun_wx_mtg_timer = (void (*)())
			dlsym( m_pMtglibHandle, "wx_mtg_timer");
	if (NULL == pfun_wx_mtg_timer) {
		return false;
	}
	pfun_wx_mtg_detect_stream = (int (*)(s_handle_t, BYTE))
			dlsym( m_pMtglibHandle, "wx_mtg_detect_stream");
	if (NULL == pfun_wx_mtg_detect_stream) {
		return false;
	}

	pfun_wx_mtg_rejoin = (int (*) (s_handle_t))
			dlsym(m_pMtglibHandle, "wx_mtg_rejoin");
	if (NULL == pfun_wx_mtg_rejoin) {
		return false;
	}
	return true;
}

bool unloadMtgFunc() {
	if (NULL != m_pMtglibHandle) {
		dlclose(m_pMtglibHandle);
		m_pMtglibHandle = NULL;
	}
	return true;
}
