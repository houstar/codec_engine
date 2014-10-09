#pragma once

#include "../timer.h"
#include <string>
using namespace std;
#include "../setting.h"
#include "../liveCourse/liveCourseManager.h"
#include "../core/Mutex.h"

class MonitorSender : public ITimerListener {
public:
	MonitorSender();
	virtual ~MonitorSender();

public:

public:
	bool init(Setting* pSetting, LiveCourseManager* pLiveCourseManager);

	bool uninit();

	bool sendMessage();

public:
	bool generateMessage();

public:
	Setting* m_pSetting;
	LiveCourseManager* m_pLiveCourseManager;
	bool	m_bInited;

	pthread_t	m_threadId;
	pthread_attr_t m_thread_attr;

	string 	m_strMessage;
	string  m_strMac;

	string 	m_strRouter;
	string	m_strMtgIp;

	Mutex 	m_mutex;

	pthread_t	m_threadSendId;
	pthread_attr_t m_threadSend_attr;
};
