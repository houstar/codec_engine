#include "sdiVideoProviderSystem.h"

void SDIVideoProviderSystem::onTimeout(Timer *timer) {
	if (timer == &m_captureSDITimer[0]) {
		if (m_pCaptureSDI[0]->init((const char*)HD_SDI0)) {
			m_pCaptureSDI[0]->start();
		} else {
			m_captureSDITimer[0].stop();
			m_captureSDITimer[0].uninit();
			m_captureSDITimerValid[0] = false;
		}
	} else if (timer == &m_captureSDITimer[1]) {
		if (m_pCaptureSDI[1]->init((const char*) HD_SDI1)) {
			m_pCaptureSDI[1]->start();
		} else {
			m_captureSDITimer[1].stop();
			m_captureSDITimer[1].uninit();
			m_captureSDITimerValid[1] = false;
		}
	}
}


SDIVideoProviderSystem::SDIVideoProviderSystem() {
	m_pCaptureSDI[0] = NULL;
	m_pCaptureSDI[1] = NULL;
	m_captureSDITimerValid[0] = false;
	m_captureSDITimerValid[1] = false;
}

SDIVideoProviderSystem::~SDIVideoProviderSystem() {

}

bool SDIVideoProviderSystem::init() {
	if (NULL == m_pCaptureSDI[0]) {
		m_pCaptureSDI[0] = new CaptureSDI(0);
		m_pCaptureSDI[0]->addListener(this);
	}

	if (NULL == m_pCaptureSDI[1]) {
		m_pCaptureSDI[1] = new CaptureSDI(1);
		m_pCaptureSDI[1]->addListener(this);
	}

	return true;
}

bool SDIVideoProviderSystem::uninit() {
	if (NULL != m_pCaptureSDI[0]) {
		delete m_pCaptureSDI[0];
		m_pCaptureSDI[0] = NULL;
	}
	if (NULL != m_pCaptureSDI[1]) {
		delete m_pCaptureSDI[1];
		m_pCaptureSDI[1] = NULL;
	}

	if (m_captureSDITimerValid[0]) {
		m_captureSDITimer[0].stop();
		m_captureSDITimer[0].uninit();
		m_captureSDITimerValid[0] = false;
	}

	if (m_captureSDITimerValid[1]) {
		m_captureSDITimer[1].stop();
		m_captureSDITimer[1].uninit();
		m_captureSDITimerValid[1] = false;
	}
	return true;
}

bool SDIVideoProviderSystem::open() {
	if (NULL != m_pCaptureSDI[0]) {
		if (m_pCaptureSDI[0]->init((const char*)HD_SDI0)) {
			m_pCaptureSDI[0]->start();
		} else {
			if (m_captureSDITimerValid[0]) {
				m_captureSDITimer[0].stop();
				m_captureSDITimer[0].uninit();
			}
			m_captureSDITimer[0].init();
			m_captureSDITimer[0].setInterval(2000);
			m_captureSDITimer[0].start(this);
			m_captureSDITimerValid[0] = true;
		}
	}

	if (NULL != m_pCaptureSDI[1]) {
		if (m_pCaptureSDI[1]->init((const char*) HD_SDI1)) {
			m_pCaptureSDI[1]->start();
		} else {
			if (m_captureSDITimerValid[1]) {
				m_captureSDITimer[1].stop();
				m_captureSDITimer[1].uninit();
			}
			m_captureSDITimer[1].init();
			m_captureSDITimer[1].setInterval(2000);
			m_captureSDITimer[1].start(this);
			m_captureSDITimerValid[1] = true;
		}
	}

	if (m_captureSDITimerValid[0] || m_captureSDITimerValid[1]) {
		return false;
	}

	return true;
}

bool SDIVideoProviderSystem::close() {
	if (m_captureSDITimerValid[0]) {
		m_captureSDITimer[0].stop();
		m_captureSDITimer[0].uninit();
		m_captureSDITimerValid[0] = false;
	}
	if (NULL != m_pCaptureSDI[0]) {
		m_pCaptureSDI[0]->stop();
	}
	if (m_captureSDITimerValid[1]) {
		m_captureSDITimer[1].stop();
		m_captureSDITimer[1].uninit();
		m_captureSDITimerValid[1] = false;
	}
	if (NULL != m_pCaptureSDI[1]) {
		m_pCaptureSDI[1]->stop();
	}
	return true;
}

void SDIVideoProviderSystem::onCaptureSDIBuffer(CaptureSDI* pCaptureSDI,
		unsigned char* pYUYVBuffer, int width, int height) {
	int nIndex = -1;
	if (pCaptureSDI == m_pCaptureSDI[0]) {
		nIndex = 0;
	} else if (pCaptureSDI == m_pCaptureSDI[1]) {
		nIndex = 1;
	}
	if (nIndex < 0) {
		return;
	}

	for (vector<ISDIVideoProviderObserver*>::iterator it = m_vecListeners.begin();
					it != m_vecListeners.end(); ++it) {
		(*it)->notifySDIProviderBuffer(nIndex, pYUYVBuffer, width, height);
	}
}

void SDIVideoProviderSystem::onNotifySDIFMT(CaptureSDI* pCaptureSDI, int width, int height) {

}

bool SDIVideoProviderSystem::addListener(ISDIVideoProviderObserver* pListener) {
	for (vector<ISDIVideoProviderObserver*>::iterator it = m_vecListeners.begin();
				it != m_vecListeners.end(); ++it) {
		if ( *it == pListener) {
			return false;
		}
	}
	m_vecListeners.push_back(pListener);
	return true;
}

bool SDIVideoProviderSystem::removeListener(ISDIVideoProviderObserver* pListener) {
	for (vector<ISDIVideoProviderObserver*>::iterator it = m_vecListeners.begin();
			it != m_vecListeners.end(); ++it) {
		if ( *it == pListener) {
			m_vecListeners.erase(it);
			return true;
		}
	}
	return true;
}
