#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "KernelBoardCommandHandle.h"
#include "../netprotocal/ICIICommandDef.h"
#include "../netprotocal/ICIICommandStruct.h"
#include "../netprotocal/SCSMCommandStruct.h"


KernelBoardCommandHandle::KernelBoardCommandHandle() :
		m_wOwnPort(IC2_COMMAND_PORT)
		, m_byteFrameIndex(0), m_bIsInit(FALSE),  m_wUnitPort(
				0) {
}

KernelBoardCommandHandle::~KernelBoardCommandHandle() {
	uninit();
}

BOOL KernelBoardCommandHandle::init(Setting* pSetting, IUDPCommandListener * pUdpCommandListener) {
	m_pSetting = pSetting;
	m_pUdpCommandListener = pUdpCommandListener;
	m_strUnitIP = pSetting->getUnitIP();
	m_wUnitPort = pSetting->getUnitPort();
	m_strOwnIP = pSetting->getOwnIP();
	m_wOwnPort = pSetting->getOwnPort();

	m_wOwnPort = (m_wOwnPort == 0) ? IC2_COMMAND_PORT : m_wOwnPort;
	m_Udp.setPort(m_wOwnPort);
	m_Udp.addListener(this);
	m_Udp.init();

	m_bIsInit = TRUE;

	memset(&m_CmdHeader, 0x00, sizeof(IC2CMD_Header));
	m_CmdHeader.dwSyncHead = CMD_ABLE_ICII;
	m_CmdHeader.dwFromIP = ntohl(inet_addr(m_strOwnIP.c_str()));
	m_CmdHeader.dwToIP = ntohl(inet_addr(m_strUnitIP.c_str()));
	m_CmdHeader.byteFrameIndex = 0;
	m_CmdHeader.byteProtocolType = CMD_TYPE_COMMAND | CMD_SENDER_ICII;
	m_CmdHeader.wDataLen = 0;

	//sendStopCmd();
	return TRUE;
}

BOOL KernelBoardCommandHandle::uninit() {
	if (!m_bIsInit) {
		return FALSE;
	}
	m_Udp.uninit();
	m_bIsInit = FALSE;
	return TRUE;
}

void KernelBoardCommandHandle::onRecv(const char *ip, unsigned int port, const char *data,
		int len) {
	if (len < sizeof(IC2CMD_Header) || len > IC2_COMMAND_MAXLEN) {
		return;
	}

	IC2CMD_Header Header;
	memcpy(&Header, data, sizeof(IC2CMD_Header));
	if (Header.dwSyncHead != CMD_ABLE_ICII) {
		return;
	}

	if ((Header.byteProtocolType & CMD_TYPE_ACK) == CMD_TYPE_ACK) {
		return;
	} else {
		IC2CMD_CmdID CmdID;
		if (len < sizeof(IC2CMD_CmdID) + sizeof(IC2CMD_Header)) {
			return;
		}
		memcpy(&CmdID, data + sizeof(IC2CMD_Header), sizeof(IC2CMD_CmdID));
		sendAck(htonl(Header.dwFromIP), Header.byteFrameIndex, CmdID);
		if (m_pUdpCommandListener){
			m_pUdpCommandListener->execCmd(ip, &CmdID,
			(unsigned char*)data + sizeof(IC2CMD_Header) + sizeof(IC2CMD_CmdID), len - sizeof(IC2CMD_Header) - sizeof(IC2CMD_CmdID) );
		}
	}

}

BOOL KernelBoardCommandHandle::sendData(string strDestIP, WORD wDestPort, BYTE* pBuffer, DWORD dwLen) {
	if (!m_bIsInit) {
		return FALSE;
	}

	return m_Udp.send(strDestIP.c_str(), wDestPort, (const char*)pBuffer, dwLen);
}

BOOL KernelBoardCommandHandle::sendAck(DWORD dwFromIP, BYTE byteFrameIndex, IC2CMD_CmdID CmdID) {
	m_CmdHeader.byteFrameIndex = byteFrameIndex;
	m_CmdHeader.byteProtocolType = CMD_SENDER_ICII | CMD_TYPE_ACK;
	m_CmdHeader.dwToIP = dwFromIP; //
	m_CmdHeader.wDataLen = sizeof(IC2CMD_CmdID);

	memcpy(m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
	memcpy(m_pSendBuffer + sizeof(IC2CMD_Header), &CmdID, sizeof(IC2CMD_CmdID));

	return m_Udp.send(dwFromIP, (short)m_wUnitPort, (const char*)m_pSendBuffer,
			sizeof(IC2CMD_Header) + sizeof(IC2CMD_CmdID));
}

BOOL KernelBoardCommandHandle::sendMsg(IC2CMD_CmdID CmdID, IC2CMD_Data1* pData1,
		DWORD dwExtendLen, BYTE* pExtend, DWORD dwDestIP, WORD wDestPort) {
	WORD wDataLen = 0;

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = getFrameIndex();
	m_CmdHeader.byteProtocolType = CMD_SENDER_ICII | CMD_TYPE_COMMAND;

	DWORD dwSendIP = ntohl(inet_addr(m_strUnitIP.c_str() ));
	WORD wSendPort = m_wUnitPort;

	if (dwDestIP != 0) {
		dwSendIP = dwDestIP;
	}
	if (wDestPort != 0) {
		wSendPort = wDestPort;
	}

	m_CmdHeader.dwToIP = dwSendIP;

	wDataLen = sizeof(IC2CMD_CmdID);
	if (pData1 != NULL) {
		wDataLen += sizeof(IC2CMD_Data1);
	}
	if (dwExtendLen != 0) {
		wDataLen += dwExtendLen;
	}
	m_CmdHeader.wDataLen = wDataLen;

	memcpy(m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
	memcpy(m_pSendBuffer + sizeof(IC2CMD_Header), &CmdID, sizeof(IC2CMD_CmdID));

	m_dwSendLen = sizeof(IC2CMD_Header) + sizeof(IC2CMD_CmdID);
	if (pData1) {
		memcpy(m_pSendBuffer + m_dwSendLen, pData1, sizeof(IC2CMD_Data1));
		m_dwSendLen += sizeof(IC2CMD_Data1);
	}

	if (dwExtendLen != 0) {
		memcpy(m_pSendBuffer + m_dwSendLen, pExtend, dwExtendLen);
		m_dwSendLen += dwExtendLen;
	}

	return m_Udp.send(htonl(dwSendIP), (short)wSendPort, (const char*)m_pSendBuffer, m_dwSendLen);
}

BOOL KernelBoardCommandHandle::sendMsg(IC2CMD_CmdID CmdID, IC2CMD_AD_Msg Msg,
		IC2CMD_Data1* pData1, DWORD dwExtendLen, BYTE* pExtend) {
	WORD wDataLen = 0;

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = getFrameIndex();
	m_CmdHeader.byteProtocolType = CMD_SENDER_ICII | CMD_TYPE_COMMAND;
	m_CmdHeader.dwToIP = ntohl(inet_addr(m_strUnitIP.c_str()));

	wDataLen = sizeof(IC2CMD_CmdID) + sizeof(IC2CMD_AD_Msg);
	if (pData1 != NULL) {
		wDataLen += sizeof(IC2CMD_Data1);
	}
	if (dwExtendLen != 0) {
		wDataLen += dwExtendLen;
	}
	m_CmdHeader.wDataLen = wDataLen;

	memcpy(m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
	memcpy(m_pSendBuffer + sizeof(IC2CMD_Header), &CmdID, sizeof(IC2CMD_CmdID));
	m_dwSendLen = sizeof(IC2CMD_Header) + sizeof(IC2CMD_CmdID);
	if (pData1) {
		memcpy(m_pSendBuffer + m_dwSendLen, pData1, sizeof(IC2CMD_Data1));
		m_dwSendLen += sizeof(IC2CMD_Data1);
	}

	if (dwExtendLen != 0) {
		memcpy(m_pSendBuffer + m_dwSendLen, pExtend, dwExtendLen);
		m_dwSendLen += dwExtendLen;
	}

	return m_Udp.send(m_strUnitIP.c_str(), (short)m_wUnitPort, (const char*)m_pSendBuffer, m_dwSendLen);
}

BOOL KernelBoardCommandHandle::sendMsg(IC2CMD_CmdID CmdID, IC2CMD_AD_Msg Msg,
		IC2CMD_AD_LanKey LanKey) {
	WORD wDataLen = 0;

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = getFrameIndex();
	m_CmdHeader.byteProtocolType = CMD_SENDER_ICII | CMD_TYPE_COMMAND;
	m_CmdHeader.dwToIP = ntohl(inet_addr(m_strUnitIP.c_str() ));

	wDataLen = sizeof(IC2CMD_CmdID) + sizeof(IC2CMD_AD_Msg)
			+ sizeof(IC2CMD_AD_LanKey);
	m_CmdHeader.wDataLen = wDataLen;

	memcpy(m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
	memcpy(m_pSendBuffer + sizeof(IC2CMD_Header), &CmdID, sizeof(IC2CMD_CmdID));
	m_dwSendLen = sizeof(IC2CMD_Header) + sizeof(IC2CMD_CmdID);

	memcpy(m_pSendBuffer + m_dwSendLen, &Msg, sizeof(IC2CMD_AD_Msg));
	m_dwSendLen += sizeof(IC2CMD_AD_Msg);

	memcpy(m_pSendBuffer + m_dwSendLen, &LanKey, sizeof(IC2CMD_AD_LanKey));
	m_dwSendLen += sizeof(IC2CMD_AD_LanKey);

	return m_Udp.send(m_strUnitIP.c_str(), m_wUnitPort, (const char*)m_pSendBuffer,
			m_dwSendLen);
}

BYTE KernelBoardCommandHandle::getFrameIndex() {
	if (m_byteFrameIndex == 0xff) {
		m_byteFrameIndex = 0;
	} else {
		m_byteFrameIndex++;
	}
	return m_byteFrameIndex;
}

BOOL KernelBoardCommandHandle::sendFreezed(DWORD dwChannel) {
	IC2CMD_CmdID CmdID;
	CmdID.dwCommand = CMD_PR_FROMSTREAM;
	CmdID.dwFlag = FLAG_FROMSTREAM_FREEZED;

	PRChannelNo ChannelNo;
	ChannelNo.dwChannelNo = dwChannel;

	return sendMsg(CmdID, NULL, sizeof(PRChannelNo),
			(BYTE*) &ChannelNo);
}

BOOL KernelBoardCommandHandle::sendStopCmd() {
	IC2CMD_CmdID CmdID;
	CmdID.dwCommand = CMD_PR_FROMSTREAM;
	CmdID.dwFlag = FLAG_FROMSTREAM_CONNECT;

	return sendMsg(CmdID, NULL, 0, NULL);

	/*IC2CMD_CmdID CmdID;
	CmdID.dwCommand = CMD_AS_CONTROL;
	CmdID.dwFlag = FLAG_AS_LIVESTOP;

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = getFrameIndex();
	m_CmdHeader.byteProtocolType = CMD_SENDER_TC | CMD_TYPE_COMMAND;

	WORD wDataLen = sizeof(IC2CMD_CmdID);

	m_CmdHeader.wDataLen = wDataLen;

	memcpy(m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
	memcpy(m_pSendBuffer + sizeof(IC2CMD_Header), &CmdID, sizeof(IC2CMD_CmdID));
	m_dwSendLen = sizeof(IC2CMD_Header) + sizeof(IC2CMD_CmdID);

	return m_Udp.send(m_strUnitIP.c_str(), m_wUnitPort, (const char*)m_pSendBuffer,
			m_dwSendLen);*/
}

BOOL KernelBoardCommandHandle::sendStateCmd(int state) {
	IC2CMD_CmdID CmdID;
	m_CmdID.dwCommand = CMD_AS_CONTROL;
	m_CmdID.dwFlag = state;

	return sendMsg(CmdID, NULL, 0, NULL);
}

BOOL KernelBoardCommandHandle::enterAssistContol() {
	m_CmdID.dwCommand = CMD_AS_CONTROL;
	m_CmdID.dwFlag = FLAG_AS_ENTER;

	m_CmdData1.dwValue = m_pSetting->getOwnPort();

	return sendMsg(m_CmdID, &m_CmdData1, 0, NULL);
}

BOOL KernelBoardCommandHandle::leaveAssistContol() {
	m_CmdID.dwCommand =CMD_AS_CONTROL;
	m_CmdID.dwFlag = FLAG_AS_LEAVE;

	return sendMsg(m_CmdID, NULL, 0, NULL);
}

BOOL KernelBoardCommandHandle::takeOver(BOOL bTake)
{
	m_CmdID.dwCommand =CMD_AS_CONTROL;
	m_CmdID.dwFlag = bTake ? FLAG_AS_TAKEOVERSTART : FLAG_AS_TAKEOVEREND;
	return sendMsg(m_CmdID, NULL, 0, NULL);
}



