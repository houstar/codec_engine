#include "vgaState.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>


int inline getcontent(char *path)
{
	int fd;
	int ret;
	char buf[32];

	fd = open(path,O_RDONLY);
	if (fd < 0) {
		perror("open");
		return -1;
	}
	ret = read(fd, buf, sizeof(buf));
	if (ret <= 0) {
		perror("read");
		return -1;
	}
	close(fd);

	ret = strtol(buf, 0, 16);

	return  ret;
}

void* VGAStateHandle::CheckVGAStateTask(void* pParam) {
	VGAStateHandle* pSelf = (VGAStateHandle*) pParam;
	if (NULL != pSelf) {
		pSelf->checkState();
	}
	return NULL;
}


VGAStateHandle::VGAStateHandle() {
	m_laststate = -1;
	m_nowstate = 0;
	m_lastconnect = -1;
	m_nowconnect = 0;

	m_pListener = NULL;

	m_bInited = false;
}


VGAStateHandle::~VGAStateHandle() {
	uninit();
}



bool VGAStateHandle::init(IVGAStateListener* pListener, int millSecond) {
	m_pListener = pListener;
	m_millSecond = millSecond;

	m_bInited = true;
	pthread_attr_init(&m_checkThreadAttr);
	if (0 != pthread_create(&m_checkThreadId, &m_checkThreadAttr, CheckVGAStateTask, this)) {
		m_bInited = false;
		return false;
	}
	sem_init(&m_exitSem, 0, 0);

	return true;
}

bool VGAStateHandle::uninit() {
	if (!m_bInited) {
		return true;
	}

	m_bInited = false;
	sem_wait(&m_exitSem);
	pthread_join(m_checkThreadId, NULL);

	sem_destroy(&m_exitSem);

	return true;
}


bool VGAStateHandle::checkState() {

	system("touch /tmp/r14");
	system("touch /tmp/r37");
	system("touch /tmp/r38");
	system("touch /tmp/r39");
	system("touch /tmp/r3a");

	system("chmod 666 /tmp/r14");
	system("chmod 666 /tmp/r37");
	system("chmod 666 /tmp/r38");
	system("chmod 666 /tmp/r39");
	system("chmod 666 /tmp/r3a");

	while (m_bInited) {
		system("i2cget -f -y 1 0x5d 0x14 > /tmp/r14");
		m_r14 = (unsigned char )getcontent("/tmp/r14");
		if ((char)m_r14 == -1){
			sleep(1);
			continue;
		}
		m_nowconnect = !!(m_r14 & 0x80);
		if (m_lastconnect == -1)
			m_lastconnect = m_nowconnect;
		if (m_lastconnect != m_nowconnect){
			printf("%d => %d\n", m_lastconnect, m_nowconnect);
			m_lastconnect = m_nowconnect;
			if (NULL != m_pListener) {
				m_pListener->onNotifyVGAStateChanged(0);
			}
			if (m_nowconnect){
				//update laststate
				system("i2cget -f -y 1 0x5d 0x37 > /tmp/r37");
				system("i2cget -f -y 1 0x5d 0x38 > /tmp/r38");
				system("i2cget -f -y 1 0x5d 0x39 > /tmp/r39");
				system("i2cget -f -y 1 0x5d 0x3a > /tmp/r3a");

				m_r37 = (unsigned char )getcontent("/tmp/r37");
				m_r38 = (unsigned char )getcontent("/tmp/r38");
				m_r39 = (unsigned char )getcontent("/tmp/r39");
				m_r3a = (unsigned char )getcontent("/tmp/r3a");

				if(((char)m_r37 == -1)||((char)m_r38 == -1)||((char)m_r39 == -1)||((char)m_r3a == -1)){
					sleep(1);
					continue;
				}

				m_laststate = (m_r3a<<24)|(m_r39<<16)|(m_r38<<8)|m_r37;
			}

			goto wait;
		}
		if (!m_nowconnect) {
			goto wait;
		}
		//be sure vga connct
		system("i2cget -f -y 1 0x5d 0x37 > /tmp/r37");
		system("i2cget -f -y 1 0x5d 0x38 > /tmp/r38");
		system("i2cget -f -y 1 0x5d 0x39 > /tmp/r39");
		system("i2cget -f -y 1 0x5d 0x3a > /tmp/r3a");

		m_r37 = (unsigned char )getcontent("/tmp/r37");
		m_r38 = (unsigned char )getcontent("/tmp/r38");
		m_r39 = (unsigned char )getcontent("/tmp/r39");
		m_r3a = (unsigned char )getcontent("/tmp/r3a");

		if (((char)m_r37 == -1)||((char)m_r38 == -1)||((char)m_r39 == -1)||((char)m_r3a == -1)){
			sleep(1);
			continue;
		}

		m_nowstate = (m_r3a<<24)|(m_r39<<16)|(m_r38<<8)|m_r37;

		if (m_laststate == -1)
			m_laststate = m_nowstate;

		if (abs(m_nowstate - m_laststate) > 1){
			printf("%d => %d\n", m_laststate, m_nowstate);
			m_laststate = m_nowstate;
			if (NULL != m_pListener) {
				m_pListener->onNotifyVGAStateChanged(0);
			}
		}
		wait:
				//wait some times
			if (m_millSecond < 1000)
				usleep(m_millSecond * 1000);
			else{
				sleep(m_millSecond/1000);
				if ((m_millSecond%1000))
					usleep((m_millSecond%1000)*1000);
			}
	}
	sem_post(&m_exitSem);
	pthread_exit(NULL);
	return true;
}





