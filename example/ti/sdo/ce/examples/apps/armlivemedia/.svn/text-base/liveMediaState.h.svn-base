#pragma once
#include "netprotocal/SCSMCommandStruct.h"
#include "netprotocal/ICIICommandDef.h"
#include <string>
using namespace std;

class StateHandle {
public:

	StateHandle();
	~StateHandle();

public:

	BOOL 	getMediaInitialize();
	void 	setMediaInitialize(BOOL bMediaInited);

	DWORD	getRefTime();
	void 	setRefTime(DWORD aRefTime);

	DWORD	getSaveTime();
	void 	setSaveTime(DWORD aSaveTime);

	BOOL	getHeadEnable();
	void 	setHeadEnable(BOOL bEnable);

	BOOL	getPowerOff();
	void 	setPowerOff(BOOL bPowerOff);

	StreamMediaState*	getStreamMediaState();
	void 	setStreamMediaState(StreamMediaState state);

	PRState*	getPRState();
	void 	setPRState(PRState aPRState);



private:

	BOOL 			m_bMediaInitialize;

	DWORD			m_dwRefTime;
	DWORD 			m_dwSaveTime;

	BOOL 			m_bHeadEnable;
	BOOL 			m_bPowerOff;

	DWORD 			m_dwRecvBeatCount;
	DWORD 			m_dwSendBeatCount;
	BOOL 			m_bReadyCourseOver;

	StreamMediaState 		m_state;

	PRState			m_PRstate;
};
