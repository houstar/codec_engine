#pragma once
#include <string.h>

#include "ICIIUnitCellDef.h"


//#define VIDEOHEAD_LEN		4
//#define VIDEOHEAD_VS		_T("[VS]")
//#define VIDEOHEAD_WM		_T("[WM]")
//#define VIDEOHEAD_PR		_T("[PR]")

typedef enum tagCommonState
{
	State_Off			=0
	, State_On			
}eCommonState;


typedef enum tagUnitState
{
	UState_Off			=0x00
	, State_On_Key
	, State_On_RFCard
	, State_On_Time
	, State_On_Cmd
}eUnitState;

typedef enum tagPRMode
{
	FilmMode = 0
	, ResourceMode
	, PPTMode
	, HudongMode
	, WeigeMode
}ePRMode;

typedef enum tagUnitCtrl
{
	UCtrl_Manual		=0x00   /**< 手动		*/
	, UCtrl_Auto				/**< 自动		*/
	, UCtrl_TDev				/**< 接管设备		*/
	, UCtrl_TRec				/**< 接管录制		*/
	, UCtrl_TDevRec				/**< 接管设备+录制*/
}eUnitCtrl;


typedef enum tagRecordState
{
	RState_Stop			=0x00
	, RState_Pause
	, RState_Start
	, RState_Null
}eRecordState;


typedef enum tagRecordRate
{
	RRate_500K			=0x00
	, RRate_1024K
	, RRate_Other		=0x02
	, RRate_NULL		=0x04
	, RRate_HD			=0x08
}eRecordRate;


typedef enum tagRecordSize
{
	RSize_VGA			=0x00
	, RSize_D1
	, RSize_Other
	, RSize_NULL
	, RSize_HD
	, RSize_D2
	, RSize_D3
}eRecordSize;

typedef enum tagRecordMode
{
	RMode_Normal		=0x00
	, RMode_PinP
	, RMode_Other
	, RMode_NULL
}eRecordMode;

typedef enum tagRecordPos
{
	RPos_160			=0x00
	, RPos_170
	, RPos_180
	, RPos_NULL
}eRecordPos;

typedef enum tagEncodeType
{
	EncodeType_WMV		=0x00
	, EncodeType_FLV
	, EncodeType_MP4	=0x03
}eEncodeType;

struct tagPR2COM_UnitState
{
	eUnitState		UnitState;
	eUnitCtrl		UnitCtrl;

	eCommonState	LessonState;
	eCommonState	FollowState;
	eCommonState	StrategyState;
	eCommonState	ExchangeState;	/// ���߻������?���߻�����ֹ
	eRecordState	RecordState;

	BYTE			UsingDevice;
	BYTE			SourceDevice;
	BYTE			SceneNum;

	DWORD			dwSMSpace;
	eRecordRate		RecordRate;
	eRecordSize		RecordSize;
	eRecordMode		RecordMode;
	eRecordPos		RecordPos;

	BYTE			WarningIn;
	BYTE			WarningOut;	

	eCommonState	TPCState;
	eCommonState	TPCConnect;
	eCommonState	TPCSoft;
	eCommonState	SMPCState;
	eCommonState	SMPCConnect;
	eCommonState	SMPCCSoft;
	eCommonState	LightState;

	eCommonState	KeyboardConnect;
	eCommonState	FollowCardConnect;
	eCommonState	VideoSyncConnect;
	eCommonState	AudioCardConnect;
	eCommonState	VideoEncCard1Connect;
	eCommonState	VideoEncCard2Connect;
	eCommonState	AmplifierState;
	eCommonState	ProjectorState;

	

	eCommonState	DebugPortState;

	BYTE			CameraID;
	
	WORD			SelDeviceType;

	//BYTE			RecState;


	BOOL			OnLine;
	int				iUpdateIndex;
	BOOL			bMute;
	WORD			wVolume;

	BOOL			bIsAssisted;

	tagPR2COM_UnitState()
	{

		UnitState = UState_Off;
		UnitCtrl = UCtrl_Manual;

		LessonState = State_Off;
		FollowState = State_Off;
		StrategyState = State_Off;
		ExchangeState = State_Off;	/// ���߻������?���߻�����ֹ
		RecordState = RState_Stop;

		UsingDevice = 0;
		SourceDevice = 0;
		SceneNum = 0;

		dwSMSpace = 0;
		RecordRate = RRate_1024K;
		RecordSize = RSize_D1;
		RecordMode = RMode_Normal;
		RecordPos = RPos_170;

		WarningIn = 0;
		WarningOut = 0;	

		TPCState = State_Off;
		TPCConnect = State_Off;
		TPCSoft = State_Off;
		SMPCState = State_Off;
		SMPCConnect = State_Off;
		SMPCCSoft = State_Off;
		LightState = State_Off;

		KeyboardConnect = State_Off;
		FollowCardConnect = State_Off;
		VideoSyncConnect = State_Off;
		AudioCardConnect = State_Off;
		VideoEncCard1Connect = State_Off;
		VideoEncCard2Connect = State_Off;
		AmplifierState = State_Off;
		ProjectorState = State_Off;

		DebugPortState = State_Off;

		CameraID = State_Off;

		SelDeviceType = 26;
		//RecState = 0x00;

		OnLine	= FALSE;
		iUpdateIndex	= -1;

		bMute = FALSE;
		wVolume = 50;

		bIsAssisted = FALSE;
	}
};

typedef tagPR2COM_UnitState PR2COM_UnitState;
typedef tagPR2COM_UnitState ClassState;


#define	CustomerMaxCount			80

typedef struct tagRECCtrl{
	DWORD		dwChannel;
	DWORD		dwRecLen;
	char		cRecParam[300];

public:
	tagRECCtrl(){

		dwChannel = 0;
		dwRecLen = 0;
		memset(cRecParam,0,300);
	};

}RECCtrl;


typedef struct tagRECHead{

	BYTE		bVolume;
	BYTE		bCurRecCount;

}RECHead;

typedef struct tagRECRecord{

	char		szClassGuid[10];
	BYTE		bChannel;

}RECRecord;

struct tagPR2COM_RECState{

	DWORD		dwVolume;
	BYTE		bCurRecCount;
	RECRecord	recCur[CustomerMaxCount];

public:
	tagPR2COM_RECState(){

		dwVolume = 0;
		bCurRecCount = 0;
		memset(recCur,0,sizeof(RECRecord)*CustomerMaxCount);

	}
};

typedef tagPR2COM_RECState PR2COM_RECState;
typedef tagPR2COM_RECState RECState;
typedef tagRECCtrl PR2COM_RECCtrl;


struct tagPR2COM_HeartState{

	BOOL	bTVWallExist;
	BOOL	bRECExist;
	DWORD	dwRECVolFactor;
public:
	tagPR2COM_HeartState(){

		bTVWallExist = FALSE;
		bRECExist = FALSE;
		dwRECVolFactor = 0;
	}
};

typedef tagPR2COM_HeartState PR2COM_HeartState;


typedef struct tagUpdateUnitHead{
	WORD		fileType;	
	WORD		fileIndex;	/// 0x00
}UpdateUnitHead;

typedef struct tagPR2COM_UnitBaseInfo{
	UpdateUnitHead		Head;		// fileType = 21
	UnitBaseInfo		BaseInfo;
}PR2COM_UnitBaseInfo;

typedef struct tagPR2COM_UnitSyllabus{
	UpdateUnitHead		Head;		// fileType = 22
	SyllabusTable		Syllabuses;
}PR2COM_UnitSyllabus;

struct tagDogInfo{
	BOOL				bHasDog;
	BOOL				bServer;
	BOOL				bIC;
	BOOL				bVW;
	BOOL				bDemo;
	BOOL				bEvaluator;
	DWORD				dwAssisCount;
	DWORD				dwSVWCount;
	DWORD				dwClassCount;

	tagDogInfo()
	{
		bHasDog = FALSE;
		bServer = FALSE;
		bIC		= FALSE;
		bVW		= FALSE;
		bDemo	= FALSE;
		bEvaluator	 = FALSE;
		dwAssisCount = 0;
		dwSVWCount	 = 0;
		dwClassCount = 0;
	}
};
typedef tagDogInfo PR2COM_DogInfo;



