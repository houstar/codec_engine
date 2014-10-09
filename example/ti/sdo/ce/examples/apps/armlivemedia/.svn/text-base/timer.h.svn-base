#pragma once


#include <sys/types.h>

#include <event2/event-config.h>

#include <sys/stat.h>
#ifndef WIN32
#include <sys/queue.h>
#include <unistd.h>
#endif
#include <time.h>
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>


class Timer;

class ITimerListener {
public:
	virtual ~ITimerListener() {

	}

	virtual void onTimeout(Timer* pTimer) {

	}
};


class Timer {
public:
	Timer();
	virtual ~Timer();

public:
	bool init();

	bool uninit();

	bool setInterval(int mill);

	bool start(ITimerListener* pListener);

	bool stop();

	bool ProcTimeOut();

private:
	bool m_bPersistent;

	struct event m_timeout;
	struct timeval m_tv;
	int m_flags;
	ITimerListener* m_pListener;
	int m_millSecond;
	bool	m_bInited;
};
