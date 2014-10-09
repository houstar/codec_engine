#include "AssistCommandHandle.h"
#include "AssistCommandDefines.h"

#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../utils/commonUtils.h"


AssistCommandHandle::AssistCommandHandle() : m_bIsInit(false)  {
	m_byteFrameIndex = 0;
	m_dwPPTDestIP = 0;
	m_bMountSharePath = false;
}

AssistCommandHandle::~AssistCommandHandle() {
	uninit();
}


bool AssistCommandHandle::init(Setting* pSetting) {
	if (m_bIsInit) {
		return true;
	}

	if (NULL == pSetting) {
		return false;
	}
	m_pSetting = pSetting;

	m_PPTConfig.strOwnIP = m_pSetting->getOwnIP();
	m_PPTConfig.wDestPort = IC2_PPTCLIENT_PORT;
	m_PPTConfig.wOwnPort = IC2_PPT_PORT;
	m_wEncodeServicePort = 0;
	m_Udp.setPort(m_PPTConfig.wOwnPort);
	m_Udp.addListener(this);
	m_Udp.init();

	m_dwRefTime = 0;
	m_dwRecordTime = 0;

	m_bIsInit = TRUE;

	memset(&m_CmdHeader, 0x00, sizeof(IC2CMD_Header));
	m_CmdHeader.dwSyncHead = CMD_ABLE_ICII;
	m_CmdHeader.dwFromIP = inet_addr(m_PPTConfig.strOwnIP.c_str()) ;    // *****(100)*****
	m_CmdHeader.dwToIP = 0;
	m_CmdHeader.byteFrameIndex = 0;
	m_CmdHeader.byteProtocolType = 0;
	m_CmdHeader.wDataLen = 0;

	m_LiveState = Live_Stopped;
	return true;
}

bool AssistCommandHandle::uninit() {
	if (!m_bIsInit) {
		return false;
	}
	m_Udp.uninit();
	m_bIsInit = FALSE;
	return true;
}

void AssistCommandHandle::onRecv(const char *ip, unsigned int port, const char *data, int len) {
	static int s_iRecvIndex = 0;
	DWORD dwReadPos = 0;

	if (len < sizeof(IC2CMD_Header) || len > IC2_COMMAND_MAXLEN) {
			return;
	}

	IC2CMD_Header Header;
	memcpy(&Header, data, sizeof(IC2CMD_Header));
	if (Header.dwSyncHead != CMD_ABLE_ICII) {
		return;
	}

	if ((Header.byteProtocolType & NET_ACK_CMD) == NET_ACK_CMD) {
		return;
	}

	IC2CMD_CmdID CmdID;

	IC2CMD_CmdID CmdID_tmp;       // *****(100)*****  
	memcpy(&CmdID_tmp, data + sizeof(IC2CMD_Header), sizeof(IC2CMD_CmdID));     // *****(100)*****

	printf("AssistCommandHandle onRecv len=%d   head=%d  cmd=%d  \n\n", len, sizeof(IC2CMD_Header), sizeof(IC2CMD_CmdID) );

	CmdID.dwCommand = ntohl(CmdID_tmp.dwCommand);       // *****(100)*****

	switch(CmdID.dwCommand ) {
	case NB2SE_CONNECT:
		printf("recv assist connect\n");
		if (m_dwPPTDestIP != 0 && m_dwPPTDestIP != Header.dwFromIP) {
			sendDisconnect();
			printf("send connect to assist\n");
		}
		m_dwPPTDestIP = Header.dwFromIP;
		printf("the assist dest ip %x", m_dwPPTDestIP);

		sendSharePath(m_pSetting->getSharePath());
		sendCurState();
		sendRecordTemplate();
		if (!m_bMountSharePath && m_pSetting->getDestPath().length() > 0) {
			char buf[256]={0};
			sprintf(buf, "mount -t cifs -o username=%s %s %s", m_pSetting->getSharePathUsername().c_str(), m_pSetting->getSharePath().c_str(), m_pSetting->getDestPath().c_str());
			printf("%s\n", buf);
			system(buf);
			m_bMountSharePath = true;
		}
		break;

	case NB2SE_DISCONNECT:
		if (Header.dwFromIP == m_dwPPTDestIP) {
			m_dwPPTDestIP = 0;
		}
		printf("recv assist disconnect\n");
		break;

	case MC2SE_CONNECT:
		if (m_wEncodeServicePort == 0) {
			break;
		}
		m_dwEncodeServiceIP = Header.dwFromIP;
		break;

	case MC2SE_DISCONNECT:
		m_dwEncodeServiceIP = 0;
		break;

	case NB2SE_COURSEID:
		{
			PR2COM_Data2 ComData2;
			memcpy(&ComData2, data + sizeof(IC2CMD_Header) + sizeof(IC2CMD_CmdID),
					len - sizeof(IC2CMD_Header) - sizeof(IC2CMD_CmdID) );
			m_strUploadCourseId = ComData2.cValue;
		}
		break;

	case ALL2SE_QUERY_STATE:
		sendCurState();
		break;

	case ALL2SE_QUERY_TIME:
		{
			/*DWORD dwMediaTime = m_dwRecordTime + (GetTickCount() - m_dwRefTime);
			if( m_LiveState == Live_Playing ) {
				sendCurTime(dwMediaTime);
			}*/
			break;
		}

	case NB2SE_PPT_PAGE:
		{
			/*PR2COM_Data1 ComData1;
			m_CommandAdapter.get_COMData1( pCmdData, ComData1, dwReadPos );
			DWORD dwPageIndex = ComData1.dwValue;

			PR2COM_Data2 ComData2;
			m_CommandAdapter.get_COMData2( pCmdData, ComData2, dwReadPos );
			CString strDesc = CString(ComData2.cValue);*/
		}
		break;

	case NB2SE_UPLOAD_FINISH:
		{
		}
		break;

	}
	return;
}


bool AssistCommandHandle::notifyConnect() {
	if (!m_bIsInit) {
		return true;
	}

	sendConnect(htonl(m_pSetting->getBPIPInfo()->dwTeacherPCIP));
	sendConnect(htonl(m_pSetting->getBPIPInfo()->dwNoteBookIP));
	return true;
}

bool AssistCommandHandle::setCourseAndTeacherName(string strTeacherName, string strCourseName) {
	if (!m_bIsInit) {
		return false;
	}

	m_strTeacherName = strTeacherName;
	m_strCourseName = strCourseName;

	sendCourseInfo(m_strCourseName);
	sendTeacherInfo(m_strTeacherName);
	return true;
}

bool AssistCommandHandle::setSharePath(string strSharePath) {
	m_PPTConfig.strSharePath = strSharePath;
	return true;
}

bool AssistCommandHandle::setCourseInfo(string strRecordMediaFile, string strMediaServiceURL) {
	m_PPTConfig.strSaveMediaFile = strRecordMediaFile;
	m_PPTConfig.strMServiceURL = strMediaServiceURL;

	sendMediaFile(strRecordMediaFile);
	return true;
}

bool AssistCommandHandle::doAction(PPTRecordAction eAction) {
	if (!m_bIsInit) {
		return false;
	}

	switch(eAction)  {
	case aLiveStart:
		//m_dwRefTime = GetTickCount();
		m_LiveState = Live_Playing;
		sendCurState();
		break;
	case aLivePause:
		//m_dwRecordTime += GetTickCount() - m_dwRefTime;
		m_LiveState = Live_Paused;
		sendCurState();
		break;
	case aLiveStop:
		m_dwRefTime = 0;
		m_dwRecordTime = 0;
		m_LiveState = Live_Stopped;

		//  只有ASF才上传
		/*if(m_pSetting->GetEncodeType() == ENCODE_ID_DSHOW)
		{
			UploadSaveMediaFile();
		}*/
		sendCurState();
		break;
	default:
		return false;
	}
	return true;
}

void AssistCommandHandle::updateCourseInfo() {
	m_strTeacherName = m_pSetting->getBaseRecordParams()->cTeacherName;
	m_strCourseName = m_pSetting->getBaseRecordParams()->cCourseName;
}

bool AssistCommandHandle::uploadSaveMediaFile() {
	if (!m_bIsInit) {
		return false;
	}
	if (m_strUploadCourseId.length() == 0 || m_PPTConfig.strSaveMediaFile.length() == 0) {
		return false;
	}
	m_strUploadCourseId.empty();
	return TRUE;
}

bool AssistCommandHandle::playMedia(string strMediaFile) {
	if (!m_bIsInit || m_dwPPTDestIP == 0) {
		return false;
	}

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = GetFrameIndex();
	m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
	m_CmdHeader.dwToIP = m_dwPPTDestIP ;											// 回复到的IP											// 自己的IP
	m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID)+sizeof(PR2COM_Data1) ) ;    // *****(100)*****
	m_dwSendLen += sizeof (IC2CMD_Header);
	memcpy(m_pSendBuffer, &m_CmdHeader, m_dwSendLen);

	PR2COM_ID ComID;
	ComID.dwCommand = htonl( (strMediaFile.length() == 0) ? BP_SM_STOPMEDIA : BP_SM_PLAYMEDIA ) ;    // *****(100)*****

	memcpy( m_pSendBuffer + m_dwSendLen, &ComID, sizeof(PR2COM_ID));
	m_dwSendLen += sizeof(PR2COM_ID);
	if (strMediaFile.length() != 0) {
		PR2COM_Data1 Data1;
		memset(Data1.cValue,0x00,PR2COM_DATA1_CVALUE1_LEN);
		strcpy(Data1.cValue, strMediaFile.c_str());

		memcpy(m_pSendBuffer + m_dwSendLen, &Data1, sizeof(PR2COM_Data1));
		m_dwSendLen += sizeof(PR2COM_Data1);
	}

	m_Udp.send(m_dwPPTDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);
	return true;
}

bool AssistCommandHandle::recvLive(string strServerIP, WORD port, bool bStart) {
	if (!m_bIsInit || m_dwPPTDestIP == 0) {
		return false;
	}

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = GetFrameIndex();
	m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
	m_CmdHeader.dwToIP =  m_dwPPTDestIP  ;												// 回复到的IP											// 自己的IP
	m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID)+sizeof(PR2COM_Data1) );     // *****(100)*****

	memcpy(m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
	m_dwSendLen += sizeof(IC2CMD_Header);

	PR2COM_ID ComID;
	ComID.dwCommand = htonl( bStart ? 	BP_SM_LIVEPLAY : BP_SM_LIVESTOP );     // *****(100)*****  
	memcpy( m_pSendBuffer + m_dwSendLen, &ComID, sizeof(PR2COM_ID));
	m_dwSendLen += sizeof(PR2COM_ID);

	if (bStart) {
		PR2COM_Data1 Data1;
		memset(Data1.cValue, 0x00, PR2COM_DATA1_CVALUE1_LEN);

		strcpy(Data1.cValue, strServerIP.c_str() );
		Data1.dwValue = port;
		memcpy(m_pSendBuffer + m_dwSendLen, &Data1, sizeof(PR2COM_Data1));
		m_dwSendLen += sizeof(PR2COM_Data1);
	}

	m_Udp.send(m_dwPPTDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);
	return TRUE;
}

bool AssistCommandHandle::sendSaveFile(string strMediaFile) {
	if (!m_bIsInit || m_dwPPTDestIP == 0 ) {
		return FALSE;
	}

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = GetFrameIndex();
	m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
	m_CmdHeader.dwToIP = m_dwPPTDestIP ;												// 回复到的IP											// 自己的IP
	m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID)+sizeof(PR2COM_Data1) ) ;       // *****(100)***** 

	memcpy(m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
	m_dwSendLen += sizeof(IC2CMD_Header);

	PR2COM_ID ComID;
	ComID.dwCommand = htonl( BP_SM_SAVEMEDIA );      // *****(100)*****

	memcpy( m_pSendBuffer+m_dwSendLen, &ComID, sizeof(PR2COM_ID));
	m_dwSendLen += sizeof(PR2COM_ID);

	if (strMediaFile.length() != 0)
	{
		PR2COM_Data1 Data1;
		memset(Data1.cValue,0x00,PR2COM_DATA1_CVALUE1_LEN);
		strcpy(Data1.cValue, strMediaFile.c_str());
		memcpy(m_pSendBuffer + m_dwSendLen, &Data1, sizeof(PR2COM_Data1));
		m_dwSendLen += sizeof(PR2COM_Data1);
	}
	m_Udp.send(m_dwPPTDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);
	return TRUE;
}

bool AssistCommandHandle::sendStreamNotify(const char* strNotify) {
	if (!m_bIsInit || m_dwPPTDestIP == 0) {
		return false;
	}

	if (strNotify == NULL) {
		return false;
	}

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = GetFrameIndex();
	m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
	m_CmdHeader.dwToIP = m_dwPPTDestIP ;												// 回复到的IP											// 自己的IP
	m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID)+sizeof(PR2COM_Data1) );      // *****(100)*****

	memcpy(m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
	m_dwSendLen += sizeof(IC2CMD_Header);

	PR2COM_ID ComID;
	ComID.dwCommand = htonl( BP_SM_STREAMNOTIFY ) ;      // *****(100)*****

	memcpy( m_pSendBuffer+m_dwSendLen, &ComID, sizeof(PR2COM_ID));
	m_dwSendLen += sizeof(PR2COM_ID);

	PR2COM_Data1 Data1;
	Data1.dwValue = 1;
	memset(Data1.cValue, 0x00, PR2COM_DATA1_CVALUE1_LEN);
	strcpy(Data1.cValue,strNotify);
	memcpy(m_pSendBuffer + m_dwSendLen, &Data1, sizeof(PR2COM_Data1));
	m_dwSendLen += sizeof(PR2COM_Data1);

	m_Udp.send(m_dwPPTDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);
	return true;
}

bool AssistCommandHandle::sendCurState() {
	if (!m_bIsInit) {
		return false;
	}

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = GetFrameIndex();
	m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
	m_CmdHeader.dwToIP = m_dwPPTDestIP ;												// 回复到的IP
	m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID) );                // *****(100)***** 
	memcpy( m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
	m_dwSendLen = sizeof(IC2CMD_Header);
	PR2COM_ID ComID;
	if( m_LiveState == Live_Playing ) {
		ComID.dwCommand = htonl( SE2ALL_RESPONSE_STATE_PLAY );       // *****(100)*****
		printf("current state is live playing\n");
	} else if( m_LiveState == Live_Paused ) {
		ComID.dwCommand = htonl( SE2ALL_RESPONSE_STATE_PAUSE );      // *****(100)*****
		printf("current state is live pause\n");
	} else if( m_LiveState == Live_Stopped ) {
		ComID.dwCommand = htonl( SE2ALL_RESPONSE_STATE_STOP );       // *****(100)*****  
		printf("current state is live stop\n");
	} else {
		ComID.dwCommand = htonl( SE2ALL_RESPONSE_STATE_STOP );       // *****(100)*****
	}
	memcpy( m_pSendBuffer+m_dwSendLen, &ComID, sizeof(PR2COM_ID));
	m_dwSendLen += sizeof(PR2COM_ID);

	if (m_dwPPTDestIP != 0) {
		m_Udp.send(m_dwPPTDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);
	} else {
		m_Udp.send(m_dwEncodeServiceIP, m_wEncodeServicePort, (const char*)m_pSendBuffer, m_dwSendLen);
	}
	return true;
}

bool AssistCommandHandle::sendConnect(DWORD dwDestIP) {
	if (!m_bIsInit) {
		return false;
	}
	if (dwDestIP != 0) {
		m_dwSendLen = 0;
		m_CmdHeader.byteFrameIndex = GetFrameIndex();
		m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
		m_CmdHeader.dwToIP = dwDestIP ;												// 回复到的IP
		m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID) );      // *****(100)***** 

		memcpy( m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
		m_dwSendLen = sizeof(IC2CMD_Header);

		PR2COM_ID ComID;
		ComID.dwCommand = htonl( BP_SM_CONNECT );       // *****(100)*****

		memcpy( m_pSendBuffer+m_dwSendLen, &ComID, sizeof(PR2COM_ID));
		m_dwSendLen += sizeof(PR2COM_ID);

		m_Udp.send(dwDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);
		printf("the assist dest ip %x", dwDestIP);
	}

	return true;
}

bool AssistCommandHandle::sendDisconnect() {
	if (!m_bIsInit) {
		return false;
	}
	if (m_dwPPTDestIP != 0) {
		m_dwSendLen = 0;
		m_CmdHeader.byteFrameIndex = GetFrameIndex();
		m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
		m_CmdHeader.dwToIP =  m_dwPPTDestIP ;												// 回复到的IP
		m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID) ) ;                // *****(100)***** 

		memcpy( m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
		m_dwSendLen = sizeof(IC2CMD_Header);

		PR2COM_ID ComID;
		ComID.dwCommand = htonl( BP_SM_DISCONNECT );      // *****(100)*****  

		memcpy( m_pSendBuffer+m_dwSendLen, &ComID, sizeof(PR2COM_ID));
		m_dwSendLen += sizeof(PR2COM_ID);

		m_Udp.send(m_dwPPTDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);
	}

	return true;
}

bool AssistCommandHandle::sendSharePath(string strPath) {
	if (!m_bIsInit || m_dwPPTDestIP == 0 ) {
		return false;
	}

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = GetFrameIndex();
	m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
	m_CmdHeader.dwToIP = m_dwPPTDestIP ;												// 回复到的IP											// 自己的IP
	m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID)+sizeof(PR2COM_Data1) );   // *****(100)*****  

	memcpy( m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
	m_dwSendLen = sizeof(IC2CMD_Header);

	PR2COM_ID ComID;
	ComID.dwCommand = htonl( SE2ALL_RESPONSE_SHAREPATH );     // *****(100)***** 

	memcpy( m_pSendBuffer+m_dwSendLen, &ComID, sizeof(PR2COM_ID));
	m_dwSendLen += sizeof(PR2COM_ID);

	PR2COM_Data1 ComData1;
	strcpy(ComData1.cValue, strPath.c_str());

	memcpy( m_pSendBuffer+m_dwSendLen, &ComData1, sizeof(PR2COM_Data1));
	m_dwSendLen += sizeof(PR2COM_Data1);
	m_Udp.send(m_dwPPTDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);

	return true;
}

bool AssistCommandHandle::sendMediaFile(string strFileName) {
	if (!m_bIsInit || m_dwEncodeServiceIP == 0) {
		return false;
	}

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = GetFrameIndex();
	m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
	m_CmdHeader.dwToIP = m_dwEncodeServiceIP ;												// 回复到的IP												// 自己的IP
	m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID)+sizeof(PR2COM_Data1) );     // *****(100)***** 

	memcpy( m_pSendBuffer, &m_CmdHeader, sizeof(IC2CMD_Header));
	m_dwSendLen = sizeof(IC2CMD_Header);

	PR2COM_ID ComID;
	ComID.dwCommand = htonl( SE2ALL_RESPONSE_FILENAME );     // *****(100)*****  

	memcpy( m_pSendBuffer+m_dwSendLen, &ComID, sizeof(PR2COM_ID));
	m_dwSendLen += sizeof(PR2COM_ID);

	PR2COM_Data1 ComData1;
	strcpy(ComData1.cValue, strFileName.c_str());

	memcpy( m_pSendBuffer+m_dwSendLen, &ComData1, sizeof(PR2COM_Data1));
	m_dwSendLen += sizeof(PR2COM_Data1);

	m_Udp.send(m_dwEncodeServiceIP, m_wEncodeServicePort, (const char*)m_pSendBuffer, m_dwSendLen);
	return true;
}

bool AssistCommandHandle::sendTeacherInfo(string strTeacherInfo) {
	if (!m_bIsInit || m_dwPPTDestIP == 0 || strTeacherInfo.length() == 0) {
		return false;
	}

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = GetFrameIndex();
	m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
	m_CmdHeader.dwToIP = m_dwPPTDestIP ;												// 回复到的IP
	m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID)+sizeof(PR2COM_Data1) );       // *****(100)*****

	memcpy( m_pSendBuffer, &m_CmdHeader, sizeof(m_CmdHeader));
	m_dwSendLen = sizeof(m_CmdHeader);

	PR2COM_ID ComID;
	ComID.dwCommand = htonl( SE2ALL_TEACHERINFO );      // *****(100)*****   

	memcpy( m_pSendBuffer+m_dwSendLen, &ComID, sizeof(PR2COM_ID));
	m_dwSendLen += sizeof(PR2COM_ID);

	PR2COM_Data1 ComData1;
	strcpy(ComData1.cValue, strTeacherInfo.c_str());

	memcpy( m_pSendBuffer+m_dwSendLen, &ComData1, sizeof(PR2COM_Data1));
	m_dwSendLen += sizeof(PR2COM_Data1);

	m_Udp.send(m_dwPPTDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);
	return true;
}

bool AssistCommandHandle::sendCourseInfo(string strCourseInfo) {
	if (!m_bIsInit || m_dwPPTDestIP == 0 || strCourseInfo.length() == 0){
		return false;
	}

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = GetFrameIndex();
	m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
	m_CmdHeader.dwToIP = m_dwPPTDestIP ;												// 回复到的IP
	m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID)+sizeof(PR2COM_Data1) );     // *****(100)*****

	memcpy( m_pSendBuffer, &m_CmdHeader, sizeof(m_CmdHeader));
	m_dwSendLen = sizeof(m_CmdHeader);

	PR2COM_ID ComID;
	ComID.dwCommand = htonl( SE2ALL_COURSEINFO );    // *****(100)*****

	memcpy( m_pSendBuffer+m_dwSendLen, &ComID, sizeof(PR2COM_ID));
	m_dwSendLen += sizeof(PR2COM_ID);

	PR2COM_Data1 ComData1;
	strcpy(ComData1.cValue, strCourseInfo.c_str());

	memcpy( m_pSendBuffer+m_dwSendLen, &ComData1, sizeof(PR2COM_Data1));
	m_dwSendLen += sizeof(PR2COM_Data1);

	m_Udp.send(m_dwPPTDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);
	return true;
}

bool AssistCommandHandle::sendCurTime() {
	if (!m_bIsInit || m_dwPPTDestIP == 0) {
		return false;
	}

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = GetFrameIndex();
	m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
	m_CmdHeader.dwToIP = m_dwPPTDestIP ;												// 回复到的IP
	m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID) );             // *****(100)*****  

	memcpy( m_pSendBuffer, &m_CmdHeader, sizeof(m_CmdHeader));
	m_dwSendLen = sizeof(m_CmdHeader);

	PR2COM_ID ComID;
	ComID.dwCommand = htonl( SE2ALL_RESPONSE_TIME );      // *****(100)*****
	memcpy( m_pSendBuffer+m_dwSendLen, &ComID, sizeof(PR2COM_ID));
	m_dwSendLen += sizeof(PR2COM_ID);

	PR2COM_Data1 ComData1;
	//ComData1.dwValue = dwTime/1000;

	memcpy( m_pSendBuffer+m_dwSendLen, &ComData1, sizeof(PR2COM_Data1));
	m_dwSendLen += sizeof(PR2COM_Data1);


	m_Udp.send(m_dwPPTDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);

	return true;
}

bool AssistCommandHandle::sendRecordTemplate() {
	if (!m_bIsInit || m_dwPPTDestIP == 0) {
		return false;
	}

	m_dwSendLen = 0;
	m_CmdHeader.byteFrameIndex = GetFrameIndex();
	m_CmdHeader.byteProtocolType = NET_DATA_PROTOCOL | NET_SEND_CMD;			// 回复的协议类型定义
	m_CmdHeader.dwToIP = m_dwPPTDestIP ;												// 回复到的IP
	m_CmdHeader.wDataLen = htonl( sizeof(PR2COM_ID)+sizeof(PR2COM_Data1) );      // *****(100)*****  

	memcpy( m_pSendBuffer, &m_CmdHeader, sizeof(m_CmdHeader));
	m_dwSendLen = sizeof(m_CmdHeader);

	PR2COM_ID ComID;
	ComID.dwCommand = htonl( BP_SM_TEMPLATELIST );      // *****(100)*****

	memcpy( m_pSendBuffer+m_dwSendLen, &ComID, sizeof(PR2COM_ID));
	m_dwSendLen += sizeof(PR2COM_ID);

	PR2COM_Data2 ComData2;


	memcpy(ComData2.cValue, m_pSetting->getRecordTemplateName().c_str(), m_pSetting->getRecordTemplateName().length());

	memcpy( m_pSendBuffer + m_dwSendLen, &ComData2, sizeof(PR2COM_Data1));
	m_dwSendLen += sizeof(PR2COM_Data1);

	m_Udp.send(m_dwPPTDestIP, m_PPTConfig.wDestPort, (const char*)m_pSendBuffer, m_dwSendLen);
	return TRUE;
}

BYTE AssistCommandHandle::GetFrameIndex() {
	if (m_byteFrameIndex == 0xff) {
		m_byteFrameIndex = 0;
	} else {
		m_byteFrameIndex++;
	}
	return m_byteFrameIndex;
}
