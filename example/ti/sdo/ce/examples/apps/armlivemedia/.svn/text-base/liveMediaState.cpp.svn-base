#include "liveMediaState.h"

StateHandle::StateHandle() {
	m_bMediaInitialize = FALSE;

	m_dwSendBeatCount = 0;
	m_dwRecvBeatCount = 0;

	m_bHeadEnable = FALSE;
	m_bPowerOff = FALSE;

	m_bReadyCourseOver = FALSE;

	m_state.CurState = SMStopped;
	m_state.dwDiskSpace = 1000;
}

StateHandle::~StateHandle() {

}



BOOL StateHandle::getMediaInitialize() {
	return m_bMediaInitialize;
}

void StateHandle::setMediaInitialize(BOOL bMediaInitialize) {
	m_bMediaInitialize = bMediaInitialize;
}

DWORD StateHandle::getRefTime() {
	return m_dwRefTime;
}

void StateHandle::setRefTime(DWORD aRefTime) {
	m_dwRefTime = aRefTime;
}

DWORD StateHandle::getSaveTime() {
	return m_dwSaveTime;
}

void StateHandle::setSaveTime(DWORD aSaveTime) {
	m_dwSaveTime = aSaveTime;
}

BOOL StateHandle::getHeadEnable() {
	return m_bHeadEnable;
}

void StateHandle::setHeadEnable(BOOL bHeadEnable) {
	m_bHeadEnable = bHeadEnable;
}

BOOL StateHandle::getPowerOff() {
	return m_bPowerOff;
}

void StateHandle::setPowerOff(BOOL bPowerOff) {
	m_bPowerOff = bPowerOff;
}

StreamMediaState* StateHandle::getStreamMediaState() {
	return &m_state;
}

void StateHandle::setStreamMediaState(StreamMediaState state) {
	m_state = state;
}

PRState*	StateHandle::getPRState() {
	return &m_PRstate;
}

void StateHandle::setPRState(PRState aPRState) {
	m_PRstate = aPRState;
}


