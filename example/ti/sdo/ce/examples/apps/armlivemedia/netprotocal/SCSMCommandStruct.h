#pragma once
#include "ICIICommandStruct.h"
#include <string.h>

typedef struct tagBPIPINFO
{
	DWORD dwCenterIP; // 编辑卡IP
	DWORD dwUnitIP;  // 一体机底板IP
	DWORD dwStreamIP; // 流媒体IP
	DWORD dwTopVS1IP; // 编码卡1 IP
	DWORD dwTopVS2IP; // 编码卡2 IP
	DWORD dwTeacherPCIP; // 教师PC IP
	DWORD dwNoteBookIP; // 笔记本 IP
	DWORD dwGatewayIP; // 网关 IP
	DWORD dwMaskIP;// 子网掩码 IP
}BPIPINFO;

typedef struct tagPRState
{
	eUnitCtrl  		UnitCtrl;
}PRState;

typedef enum tageSMState
{
	SMNull = 0,
	SMPaused ,
	SMStopped ,
	SMReplaying ,
	SMRecording ,
	SMPausing ,
	SMStopping ,
	SMExiting ,
	SMExited ,
	SMPreview
}eSMState;

typedef struct tagStreamMediaState
{
	WORD CurState;
	DWORD dwDiskSpace;
}StreamMediaState;


//typedef tagPRState PR2COM_PRState;

typedef struct tagPRParam
{
	DWORD dwWidth;
	DWORD dwHeight;

	BOOL bRecord;
	DWORD dwRecordKBitrate;

	BOOL bLive;
	DWORD dwLiveWidth;
	DWORD dwLiveHeight;
	DWORD dwLiveKBitrate;
	char cPublishPoint[260];

	BOOL bResourceMode;

	char cRecordStyleName[50];

	char cCourseName[50];
	char cTitleName[50];
	char cTeacherName[50];

	/*
	@brief  ��������
	#define ENCODE_ID_DSHOW			0x00010000
	#define ENCODE_ID_REAL			0x00020000
	#define ENCODE_ID_FLV			0x00030000
	*/
	DWORD dwEncodeType;
}BaseRecordParam;

typedef struct tagUtf8RecordParam {
	char cUtf8CourseName[50];
	char cUtf8TitleName[50];
	char cUtf8TeacherName[50];

}stUtf8RecordParam;

//typedef tagPRParam PR2COM_PRParam;


typedef struct tagPRChannel
{
	DWORD		dwTeacherChannel;
	DWORD		dwStudentChannel;
	DWORD		dwDeviceChannel;
}PRChannel;

typedef struct tagPRChannelNo
{
	DWORD		dwChannelNo;
}PRChannelNo;


typedef struct tagSMCommonInfo
{
	DWORD		dwVersion;
	DWORD		dwIP;
}StreamMediaCommonInfo;


typedef struct tagSceneCommonInfo
{
	BOOL bShowLogo;
	BOOL bShowCourseInfo;
}SceneCommonInfo;


typedef enum tageSceneElementType
{
	SceneNULL = 0,
	SceneVideo ,
	SceneBackPic ,
	SceneString ,
	SceneMixPic ,
	SceneMediaFile
}eSceneElementType;

#define SOURCE_TEACHER	0x00000001
#define SOURCE_STUDENT	0x00000002
#define SOURCE_DEVICE	0x00000003
#define SOURCE_AVDEVICE	0x00000004
#define SOURCE_SCENE_COUNT 0x05

typedef struct tagSceneVideoInfo
{
	DWORD dwVideoSource;
	DWORD dwTop;
	DWORD dwLeft;
	DWORD dwBottom;
	DWORD dwRight;
}SceneVideoInfo;


typedef struct tagSceneBackPicInfo
{
	char cBackPic[256];
}SceneBackPicInfo;


typedef struct tagSceneStringInfo
{
	char cString[50];
	DWORD dwColor;
	DWORD dwFont;
	DWORD dwSize;
	DWORD dwPosX;
	DWORD dwPosY;
}SceneStringInfo;


typedef struct tagSceneMixPicInfo
{
	char cMixPic[256];
	DWORD dwPosX;
	DWORD dwPosY;
}SceneMixPicInfo;


typedef struct tagSceneMeidaFileInfo
{
	char cMediaFile[256];
}SceneMeidaFileInfo;

typedef struct tagIPCInfo
{
	DWORD dwIPCIP;
	DWORD dwIPCChannel;
	DWORD dwIPCWidth;
	DWORD dwIPCHeight;
	DWORD dwSubCode;
	char  cIPCTypeName[32];
	tagIPCInfo()
	{
		dwIPCIP = 0;
		dwIPCChannel = 0;
		dwIPCWidth = 0;
		dwIPCHeight = 0;
		dwSubCode = 0;
		memset(cIPCTypeName,0x00,32);
	}
}IPCInfo;


typedef struct tagStragyMode {
	int	mode;
}stStragyMode;

