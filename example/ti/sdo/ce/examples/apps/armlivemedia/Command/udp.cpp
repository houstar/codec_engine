#include "udp.h"
#include <list>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <assert.h>
#include "../log.h"


void* Udp::recvDataThread(void* pParam) {
	Udp *pSelf = (Udp*)pParam;
	if (NULL != pSelf) {
		pSelf->recvBuff();
	}
	return NULL;
}

Udp::Udp() :
		_socketfd(-1), _port(-1), m_bExit(false), m_bInited(false) {
}

Udp::~Udp() {
	this->uninit();
}

bool Udp::setPort(short port) {
	this->_port = port;

	return true;
}

short Udp::getPort() {
	return this->_port;
}

bool Udp::init() {
	if (this->_socketfd != -1) {
		LOG_ERROR("this Udp Object is already inited.");
		return false;
	}

	if (this->getPort() == -1) {
		LOG_ERROR("UDP port is not be set.");
		assert(0);
		return false;
	}

	this->_socketfd = ::socket(AF_INET, SOCK_DGRAM, 0);

	if (this->_socketfd == -1) {
		LOG_ERROR("create socket failed.");
		abort();
		return false;
	}

	struct sockaddr_in fromaddr;

	::bzero(&fromaddr, sizeof(fromaddr));
	fromaddr.sin_port = htons(this->getPort());
	fromaddr.sin_family = AF_INET;
	fromaddr.sin_addr.s_addr = INADDR_ANY;

	if (::bind(this->_socketfd, (struct sockaddr *) &fromaddr, sizeof(fromaddr))
			== -1) {
		::close(this->_socketfd);
		this->_socketfd = -1;

		LOG_ERROR("bind socket failed.");

		return false;
	}

	pthread_attr_init(&m_threadAttr);
	if (0 != pthread_create (&m_threadId, &m_threadAttr, recvDataThread, this)) {
		printf("can not create udp thread\n");
	}

	sem_init(&m_exitSem, 0, 0);
	m_bInited = true;
	return true;
}

bool Udp::uninit() {
	if (!m_bInited) {
		return true;
	}
	m_bInited = false;
	m_bExit = true;
	sem_wait(&m_exitSem);
	if (this->_socketfd != -1) {
		::close(this->_socketfd);
		this->_socketfd = -1;
	}
	int ret = 0;
	pthread_join(m_threadId, (void**) &ret);
	sem_destroy(&m_exitSem);

	return true;
}

bool Udp::addListener(IUdpListener *listener) {
	if (listener == NULL) {
		//系统中，不允许存在该现象
		return false;
	}

	if (std::find(_listener.begin(), _listener.end(), listener)
			!= _listener.end()) {
		LOG_MSG("this listener is already added.");
		return false;
	}

	this->_listener.push_back(listener);
	return true;
}

bool Udp::delListener(IUdpListener *listener) {
	IUdpListener *tmp = listener;

	if (std::find(_listener.begin(), _listener.end(), tmp) == _listener.end()) {
		return false;
	}

	this->_listener.remove(listener);

	return true;
}

bool Udp::send(const char *ip, short port, const char *data, int len) {
	struct sockaddr_in toaddr;

	::bzero(&toaddr, sizeof(toaddr));

	toaddr.sin_port = htons(port);
	toaddr.sin_family = AF_INET;
	toaddr.sin_addr.s_addr = inet_addr(ip);

	::sendto(this->_socketfd, data, len, 0, (struct sockaddr*) &toaddr,
			sizeof(toaddr));

	return true;
}

bool Udp::send(DWORD dwDestIP, short port, const char *data, int len){
	struct sockaddr_in toaddr;

	::bzero(&toaddr, sizeof(toaddr));

	toaddr.sin_port = htons(port);
	toaddr.sin_family = AF_INET;
	toaddr.sin_addr.s_addr = dwDestIP;

	::sendto(this->_socketfd, data, len, 0, (struct sockaddr*) &toaddr,
			sizeof(toaddr));

	return true;
}

void Udp::recvBuff() {
	struct sockaddr_in addr;
	socklen_t socklen = sizeof(addr);
	char buf[UDP_MAX_LEN] = { 0 };
	while (!m_bExit) {
		if (_socketfd == -1) {
			return;
		}

		int length = ::recvfrom(this->_socketfd, buf, sizeof(buf), 0,
				(struct sockaddr*) &addr, &socklen);

		if (length <= 0) {
			continue;
		}

		const char *ip = ::inet_ntoa(addr.sin_addr);
		int port = ntohs(addr.sin_port);

		//printf("udp from %s port %d\n", ip, port);

		for (list<IUdpListener *>::iterator it = this->_listener.begin();
				it != this->_listener.end(); it++) {
			(*it)->onRecv(ip, port, buf, length);
		}
	}
	sem_post(&m_exitSem);
}



