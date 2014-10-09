#pragma once

#include "../setting.h"
#include "udp.h"
#include "../netprotocal/ICIICommandDef.h"
#include "udpCommandListener.h"

typedef struct tagPPTConfig
{
	string		strOwnIP;
	WORD		wOwnPort;
	WORD		wDestPort;
	string		strSaveMediaFile;
	string		strTeacher;
	string		strCourse;
	string		strSharePath;
	string		strMServiceURL;
}PPTConfig;

typedef enum tagePPTRecordAction
{
	aLiveStart = 10
	, aLivePause
	, aLiveStop
}PPTRecordAction;

typedef enum tagLiveState
{
	Live_Playing
	, Live_Paused
	, Live_Stopped
}LiveState;


class AssistCommandHandle : public IUdpListener {
public:
	AssistCommandHandle();
	~AssistCommandHandle();

public:
	virtual void onRecv(const char *ip, unsigned int port, const char *data,
			int len);

public:
	bool init(Setting* pSetting);

	bool uninit();

	bool notifyConnect();

	bool setCourseAndTeacherName(string strTeacherName, string strCourseName);

	bool setSharePath(string strSharePath);

	bool setCourseInfo(string strRecordMediaFile, string strMediaServiceURL);

	bool doAction(PPTRecordAction eAction);

	void updateCourseInfo();

	bool uploadSaveMediaFile();

	bool playMedia(string strMediaFile);

	bool recvLive(string strServerIP, WORD port, bool bStart);

	bool sendSaveFile(string strMediaFile);

	bool sendStreamNotify(const char* strNotify);

protected:

	bool sendCurState();

	bool sendConnect(DWORD dwDestIP);

	bool sendDisconnect();

	bool sendSharePath(string strPath);

	/**
	 * \brief 发送录制的媒体文件名
	 */
	bool sendMediaFile(string strFileName);

	/**
	 * \brief 发送教师信息
	 */
	bool sendTeacherInfo(string strTeacherInfo);

	bool sendCourseInfo(string strCourseInfo);

	bool sendCurTime();

	/**
	 * \brief 发送片头片尾模板列表
	 */
	bool sendRecordTemplate();

	BYTE GetFrameIndex();

protected:
	Udp 	m_Udp;
	Setting* m_pSetting;
	PPTConfig		m_PPTConfig;
	string 	 m_strUploadCourseId;
	string   m_strTeacherName;
	string 	 m_strCourseName;
	bool 	 m_bIsInit;
	DWORD			m_dwPPTDestIP;
	DWORD			m_dwEncodeServiceIP;
	WORD			m_wEncodeServicePort;
	LiveState		m_LiveState;
	BYTE			m_byteFrameIndex;

	DWORD			m_dwRecordTime;
	DWORD			m_dwRefTime;

	DWORD			m_dwSendLen;			/// 要发送的数据长度
	DWORD			m_dwRecvLen;			/// 接收到的数据长度
	IUDPCommandListener * m_pUdpCommandListener;

	IC2CMD_Header m_CmdHeader;
	BYTE m_pSendBuffer[IC2_COMMAND_MAXLEN];

	bool	m_bMountSharePath;
};
