#include "baseMode.h"


BaseMode::BaseMode(Kernel* pKernel) {
	m_pKernel = pKernel;
	m_bVGAFreeze = false;
}

BaseMode::~BaseMode() {

}

BOOL BaseMode::init(Setting* pSetting) {
	return TRUE;
}

BOOL BaseMode::uninit() {
	return TRUE;
}

BOOL BaseMode::start() {
	return TRUE;
}

BOOL BaseMode::stop() {
	return TRUE;
}

BOOL BaseMode::pause(){
	return TRUE;
}

BOOL BaseMode::resume() {
	return TRUE;
}

BOOL BaseMode::doMovieHead() {
	return TRUE;
}

BOOL BaseMode::doMovieTail() {
	return TRUE;
}

bool BaseMode::switchScene() {
	return true;
}

bool BaseMode::isComponentCreated() {
	return true;
}


bool BaseMode::IPCChannelChanged() {
	return true;
}

bool BaseMode::doFreezeVGA(bool bFreeze) {
	m_bVGAFreeze = bFreeze;
	return true;
}

