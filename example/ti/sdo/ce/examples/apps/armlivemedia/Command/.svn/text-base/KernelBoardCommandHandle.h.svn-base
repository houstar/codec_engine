#pragma once

#include "../setting.h"
#include "udp.h"
#include "../netprotocal/ICIICommandDef.h"
#include "udpCommandListener.h"


class KernelBoardCommandHandle: public IUdpListener {
public:
	KernelBoardCommandHandle();
	~KernelBoardCommandHandle();

public:
	BOOL init(Setting* pSetting, IUDPCommandListener * pUdpCommandListener);

	BOOL uninit();

public:
	virtual void onRecv(const char *ip, unsigned int port, const char *data,
			int len);

public:

	BOOL sendMsg(IC2CMD_CmdID CmdID, IC2CMD_AD_Msg Msg, IC2CMD_Data1* pData1 =
			NULL, DWORD dwExtendLen = 0, BYTE* pExtend = NULL);

	BOOL sendMsg(IC2CMD_CmdID CmdID, IC2CMD_Data1* pData1,
			DWORD dwExtendLen, BYTE* pExtend, DWORD dwDestIP = 0, WORD wDestPort = 0);

	BOOL sendMsg(IC2CMD_CmdID CmdID, IC2CMD_AD_Msg Msg,
			IC2CMD_AD_LanKey LanKey);

	BOOL sendFreezed(DWORD dwChannel);

	BOOL sendStopCmd();


	BOOL sendStateCmd(int state);

	BOOL enterAssistContol();

	BOOL leaveAssistContol();

	BOOL takeOver(BOOL bTake);

private:

	BOOL sendData(string strDestIP, WORD wDestPort, BYTE* pBuffer, DWORD dwLen);

	BOOL sendAck(DWORD dwFromIP, BYTE byteFrameIndex, IC2CMD_CmdID CmdID);

	BYTE getFrameIndex();

private:
	Udp 	m_Udp;
	Setting *m_pSetting;
	IUDPCommandListener * m_pUdpCommandListener;

	IC2CMD_Header m_CmdHeader;
	IC2CMD_CmdID m_CmdID;
	IC2CMD_Data1 m_CmdData1;
	IC2CMD_Data2 m_CmdData2;

	string m_strOwnIP;
	WORD m_wOwnPort;

	DWORD m_dwSendLen;

	BYTE m_byteFrameIndex;
	BOOL m_bIsInit;

	BOOL m_bExit;

	BYTE m_pSendBuffer[IC2_COMMAND_MAXLEN];

	WORD 	m_wUnitPort;
	string 	m_strUnitIP;;
};

