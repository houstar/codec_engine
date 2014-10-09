#pragma once
#include <pthread.h>
#include <semaphore.h>

class MtgHeartbeat {
public:
	MtgHeartbeat();
	virtual ~MtgHeartbeat();

public:
	static  void* mtgHeartbeatCallback(void* pParam);

public:
	bool init();

	bool uninit();

	bool startThread();

	bool stopThread();

	void procHeartbeat();

private:

	pthread_t m_heartbeatThreadId;
	pthread_attr_t m_heartbeatThreadAttr;

public:
	bool m_bHeartbeat;

	sem_t m_heartbeatExitSem;
};
