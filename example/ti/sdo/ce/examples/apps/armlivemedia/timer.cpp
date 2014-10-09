#include "timer.h"

extern event_base* gEventBase;

static void timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	Timer* pSelf = (Timer*) arg;
	if (NULL == pSelf) {
		return;
	}

	pSelf->ProcTimeOut();


}

bool Timer::ProcTimeOut() {
	if (NULL != m_pListener) {
		m_pListener->onTimeout(this);
	}
	if (!m_bPersistent) {
		struct timeval tv;
		evutil_timerclear(&tv);
		tv.tv_sec = m_millSecond/1000;
		tv.tv_usec = (m_millSecond % 1000) * 1000;
		event_add(&m_timeout, &tv);
	}
	return true;
}

Timer::Timer()
: m_bPersistent(false)
, m_flags(0) //EV_PERSIST
, m_pListener(NULL)
, m_millSecond(0)
, m_bInited(false)
{

}

Timer::~Timer() {
	uninit();
}

bool Timer::init() {

	/* Initalize one event */
	if (m_bInited) {
		return true;
	}
	m_bPersistent = false;
	event_assign(&m_timeout, gEventBase, -1, m_flags, timeout_cb, (void*)this);
	m_bInited = true;
	return true;
}

bool Timer::uninit() {
	if (!m_bInited) {
		return false;
	}
	m_bPersistent = true;
	//event_free(&m_timeout);
	event_del(&m_timeout);
	m_bInited = false;
	return true;
}


bool Timer::setInterval(int mill) {
	evutil_timerclear(&m_tv);
	m_tv.tv_sec = mill/1000;
	m_tv.tv_usec = (mill % 1000) * 1000;
	event_add(&m_timeout, &m_tv);



	m_millSecond = mill;
	return true;
}

bool Timer::start(ITimerListener* pListener) {
	m_pListener = pListener;
	return true;
}

bool Timer::stop() {
	m_pListener = NULL;
	return true;
}
