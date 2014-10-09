#pragma once

#include <list>
#include "../typedef.h"
#include <pthread.h>
#include <semaphore.h>

using namespace std;

class IUdpListener {
public:
	virtual ~IUdpListener() {

	}

	virtual void onRecv(const char *ip, unsigned int port, const char *data,
			int len){

	}
};

class Udp {
	static void* recvDataThread(void* pParam);

public:
	Udp();

	virtual ~Udp();

	bool setPort(short port);

	short getPort();

	virtual bool init();

	virtual bool uninit();

public:
	bool addListener(IUdpListener *listener);

	bool delListener(IUdpListener *listener);

	bool send(const char *ip, short port, const char *data, int len);

	bool send(DWORD dwDestIP, short port, const char* data, int len);

private:
	void recvBuff();

private:
	int _socketfd;
	bool m_bExit;
	bool m_bInited;
	short _port;
	list<IUdpListener *> _listener;

	pthread_t m_threadId;
	pthread_attr_t m_threadAttr;
	sem_t		m_exitSem;
};

