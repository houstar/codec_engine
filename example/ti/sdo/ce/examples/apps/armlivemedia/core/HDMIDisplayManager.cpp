#include "HDMIDisplayManager.h"


void HDMIDisplayManager::onTimeout(Timer* pTimer) {
	if (pTimer == &m_timer) {
		m_displayHandle.initv4lhandle(1024, 768, m_pSetting->getHDMIDisplayDeviceName().c_str(), 1);
		m_bInited = true;
		m_timer.stop();
		m_timer.uninit();
		m_bTimerValid = false;
	}
}

HDMIDisplayManager::HDMIDisplayManager()
: m_pSetting(NULL)
, m_displayContext(eDisplayNone)
, m_displayWidth(0)
, m_displayHeight(0)
, m_bInited(false)
, m_lastDisplayWidth(1920)
, m_lastDisplayHeight(1080)
, m_bTimerValid(false)
{
	m_mutex.init();
}

HDMIDisplayManager::~HDMIDisplayManager() {
	m_mutex.uninit();
}

bool HDMIDisplayManager::init(Setting* pSetting) {
	m_pSetting = pSetting;
	return true;
}


bool HDMIDisplayManager::uninit(EDisplayContext context) {
	m_mutex.lock();
	if (m_bInited && m_displayContext == context) {
		m_displayHandle.uninitv4lhandle();
		m_bInited = false;
		m_displayWidth = 0;
		m_displayHeight = 0;
		m_displayContext = eDisplayNone;
	}
	m_mutex.unlock();
	return true;
}

bool HDMIDisplayManager::setDisplayContext(EDisplayContext context) {
	m_mutex.lock();
	if (context == eDisplayRecord) {
		if (!m_pSetting->getHDMIDisplayEnable()) {
			m_mutex.unlock();
			return false;
		}
	}
	if (m_displayContext == eDisplayVGA && context == eDisplayRecord) {
		m_mutex.unlock();
		return false;
	}
	m_displayContext = context;
	m_mutex.unlock();
	return true;
}

bool HDMIDisplayManager::setDisplayResolution(int width, int height) {
	m_mutex.lock();
	m_displayWidth = width;
	m_displayHeight = height;
	changeDisplayResution(HD_WIDTH, HD_HEIGHT);
	m_displayHandle.initv4lhandle(width, height, m_pSetting->getHDMIDisplayDeviceName().c_str(), 1);

	m_bInited = true;
	m_mutex.unlock();
	return true;
}

bool HDMIDisplayManager::doDisplay(EDisplayContext context, const unsigned char* pBuffer) {
	m_mutex.lock();
	if (context == eDisplayRecord && !m_pSetting->getHDMIDisplayEnable()) {
		m_mutex.unlock();
		return false;
	}
	if (m_bInited && context == m_displayContext) {
		m_displayHandle.doV4LDisplay((const char*)pBuffer);
	}
	m_mutex.unlock();
	return true;
}

bool HDMIDisplayManager::setLiveCourseRole(ELiveCourseRole role) {
	m_mutex.lock();
	if (role == eLCRoleAudence) {
		m_displayHandle.uninitv4lhandle();
		m_displayContext = HDMIDisplayManager::eDisplayVGA;
		m_displayWidth = 1024;
		m_displayHeight = 768;
		changeDisplayResution(1024, 768);

		if (m_bTimerValid) {
			m_timer.stop();
			m_timer.uninit();
		}
		m_timer.init();
		m_timer.setInterval(5000);
		m_timer.start(this);
		m_bTimerValid = true;
	} else {
		if (m_bTimerValid) {
			m_timer.stop();
			m_timer.uninit();
			m_bTimerValid = false;
		}
		if (m_displayContext != HDMIDisplayManager::eDisplayRecord) {
			m_displayHandle.uninitv4lhandle();
			m_bInited = false;
			m_displayWidth = 0;
			m_displayHeight = 0;
			m_displayContext = eDisplayNone;
		}
		changeDisplayResution(1920, 1080);
		m_displayWidth = 1920;
		m_displayHeight = 1080;
	}
	m_mutex.unlock();
	return true;
}

bool HDMIDisplayManager::changeDisplayResution(int width, int height) {
	if (width == 0 || height == 0) {
		return false;
	}
	if (m_lastDisplayWidth == width && m_lastDisplayHeight == height) {
		return true;
	}
	system("echo 0 > /sys/devices/platform/vpss/display0/enabled");

	char buf[128] = {0};
	sprintf(buf, "echo %dx%d@60 > /sys/devices/platform/vpss/display0/mode", width, height);
	system(buf);
	system("echo 1 > /sys/devices/platform/vpss/display0/enabled");
	m_lastDisplayWidth = width;
	m_lastDisplayHeight = height;
	return true;
}
