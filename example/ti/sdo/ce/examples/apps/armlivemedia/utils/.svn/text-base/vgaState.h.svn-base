#pragma once
#include "../timer.h"
#include <semaphore.h>


class IVGAStateListener {
public:
	virtual ~IVGAStateListener() {

	}

	virtual bool onNotifyVGAStateChanged(int reason) {
		return true;
	}
};

class VGAStateHandle  {
public:
	VGAStateHandle();
	~VGAStateHandle();

public:
	static void* CheckVGAStateTask(void* pParam);

public:

	bool init(IVGAStateListener* pListener, int millSecond);

	bool uninit();

private:
	bool checkState();

private:

	int		m_fd;
	unsigned char m_r37;
	unsigned char m_r38;
	unsigned char m_r39;
	unsigned char m_r3a;
	unsigned char m_r14;

	int m_laststate;
	int m_nowstate;
	int m_lastconnect;
	int m_nowconnect;

	pthread_t	m_checkThreadId;
	pthread_attr_t	m_checkThreadAttr;


	IVGAStateListener* m_pListener;

	int m_millSecond;
	bool	m_bInited;
	sem_t	m_exitSem;

};
