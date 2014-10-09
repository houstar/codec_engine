#pragma once
#include "../typedef.h"
#include "KernelBoardCommandHandle.h"
#include "../netprotocal/SCSMCommandStruct.h"
#include "../timer.h"


#define MEDIA_EVENT_FREEZED		0x00020001

class Kernel;

class CFunction : public IUDPCommandListener , public ITimerListener {
public:
	CFunction();
	~CFunction();

public:
	void execCmd(const char* ip, IC2CMD_CmdID* pCmdID, unsigned char* pData, int len);

	void onTimeout(Timer *timer);
public:

	BOOL init(Kernel* pKernel);

	BOOL uninit();

	 BOOL MediaStateEvent(DWORD dwEvent, DWORD dwValue);

	 BOOL RemoteVideoDataLost(string strMsg);

	 BOOL updateStreamMediaState(eSMState CurState);

	BOOL sendStreamMediaState();

	BOOL SendNotify(string strNotify, BOOL bError = FALSE);


private:

	BOOL execPowerOff();

	BOOL UpdateIPConfig();

	bool registerToKernelBoard();

	bool unregisterToKernelBoard();

	BOOL sendHeartBeat();

	BOOL sendQueryResponce(char* strFromIp);

	BOOL sendSystemStatusResponse();


private:

	Kernel			*m_pKernel;
	Setting			*m_pSetting;
	Timer			m_heartbeatTimer;
	bool 			m_bregisterToKernelBoard;
};

