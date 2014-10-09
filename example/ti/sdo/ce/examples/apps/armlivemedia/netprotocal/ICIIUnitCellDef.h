#pragma once
#include "../typedef.h"

#define		UnitAddressPort						7101

#define		BaseInfoIndex						0
#define		SyllabusInfoStartIndex				3
#define		SyllabusInfoEndIndex				9
#define		DevInfoStartIndex					16
#define		DevStatisticsIndex					0xFA
#define		InfoEndIndex						0xFFFF

typedef struct tagUnitBaseInfo
{
	BYTE				bUnitDeviceID;
	BYTE				bUnitDeviceType;

	char				szSchool[32];
	char				szBuilding[32];
	char				szClassrooom[32];
	WORD				wClassroomType;
	BYTE				bUnitID[8];
	BYTE				bIPAddress[8];
	BYTE				bIPGeteway[8];
	BYTE				bSvrIPAddress[8];

	BYTE				bDefenceHour;
	BYTE				bDefenceMinute;
	BYTE				bUnDefenceHour;
	BYTE				bUnDefenceMinute;

	DWORD				dwVersionNum;
	BYTE				bMonitorDevice;

	BYTE				bBootType;

	//2009-05-05
	BYTE				bEncodedCard1IP[6];
	BYTE				bEncodedCard2IP[6];

	BYTE				bTeacherPCIP[6];
	BYTE				bStreamMediaIP[6];

	BYTE				bWatchDeviceTypeID;
//	BYTE				StreamMedia2IP[8];

	BYTE				bIsDefOpen;

	char				szExtend[88];
	//////////////////////////////////////////////////////////////////////////

	tagUnitBaseInfo()
	{
		bUnitDeviceID = bUnitDeviceType = 0;
		//wClassroomType = Compact;
	}
}UnitBaseInfo;


#define		UnitSingleEncodeChannelCount			4
#define		UnitMaxEncoderCount						2
struct IOConnectParam
{
	//BYTE bTargetConnectDeviceID;
	BYTE bChannelNumber;
	BYTE bProperty;
	BYTE bAudioChannelNumber;
	//WORD wProperty;
	BYTE Extra;
};

struct ConnectParam
{
	BYTE bIN;
	BYTE bOUT;
	BYTE bLink;
	BYTE bProperty;
};

typedef struct tagUnitDevInfo
{

	BYTE				bUnitDeviceID;
	BYTE				bUnitDeviceType;
	char				szName[32];
	char				szDescription[32];

	//////////////////////////////////////////////////////////////////////////
	ConnectParam		stuVGAConnect;
	ConnectParam		stuAVConnect;
	ConnectParam		stuAUDIOConnect;
	IOConnectParam		stuIOInputConnect;
	IOConnectParam		stuIOOutnputConnect;

	BYTE				PowerConnectDeviceID;
	BYTE				PowerConnectChannelNumber;

	WORD				CommuniteConnectType;

	BYTE				CommuniteConnectParam[8];

	WORD				wDeviceSelectKey;
	WORD				wPowerOn;
	WORD				wPowerOff;
	WORD				wPowerDelaySeconds;


	//////////////////////////////////////////////////////////////////////////
	BYTE				szPrivate[148];

	UINT				usedTime;
	UINT				usedCount;
	
	tagUnitDevInfo(){

		bUnitDeviceType = true;//EmptyDevice;
	}

}UnitDevInfo;

typedef struct tagUnitSyllabus
{
	BYTE bStartTimeHour;
	BYTE bStartTimeMinute;
	BYTE bEndTimeHour;
	BYTE bEndTimeMinute;

	char szName[16];
	char szTeacher[10];

}UnitSyllabus;

typedef struct tagSyllabusTable
{
	BYTE bDeviceID;
	BYTE bDeviceType;
	BYTE bWeekNumber;
	UnitSyllabus syllabus[8];
	BYTE szPrivate[13];

}SyllabusTable;


typedef struct tagUnitDevStat{

	BYTE	bDeviceType;
	BYTE	bAlterCount;
	BYTE	bYearHigh;
	BYTE	bYearLow;
	BYTE	bMonth;
	BYTE	bDay;
	BYTE	bHour;
	BYTE	bMinute;
	DWORD	dwUseMinute;
	DWORD	dwUseCount;

}UnitDevStat;


typedef struct tagDevStatTable
{
	BYTE		bDeviceID;
	BYTE		bDeviceType;
	BYTE		bBlank[2];

	UnitDevStat devStat[15];
	BYTE		bPrivate[12];

}DevStatTable;


#define		UnitStateInfoBlock						58
//////////////////////////////////////////////////////////////////////////
#define		UnitConfVer_Offset						48
#define		UnitConfVer_Len							4
//////////////////////////////////////////////////////////////////////////

#define		DeviceCurCtr_Offset						52
#define		DeviceCurCtr_Len						2
//////////////////////////////////////////////////////////////////////////
#define		DeviceCurCtr_CurDev_Offset				0

#define		VolumeCurDevice_Offset					56
#define		VolumeCurDevice_Len						2
//////////////////////////////////////////////////////////////////////////
#define		VolumeCurDevice_Mute_Offset				1
#define		VolumeCurDevice_Mute_BitOffset			7
#define		VolumeCurDevice_Mute_BitMask			0x1

#define		VolumeCurDevice_Value_BitMask			0x7FFF


#define		RunState_Offset							14
#define		RunState_Len							1
//////////////////////////////////////////////////////////////////////////
#define		RunState_Power_Offset					0
#define		RunState_Power_BitOffset				0
#define		RunState_Power_BitMask					0x0F

#define		RunState_Mode_Offset					0
#define		RunState_Mode_BitOffset					4
#define		RunState_Mode_BitMask					0x0F


#define		ClassState_Offset						15
#define		ClassState_Len							1
//////////////////////////////////////////////////////////////////////////
#define		ClassState_State_Offset					0
#define		ClassState_State_BitOffset				0
#define		ClassState_State_BitMask				0x1

#define		ClassState_AutoTrack_Offset				0
#define		ClassState_AutoTrack_BitOffset			1
#define		ClassState_AutoTrack_BitMask			0x1

#define		ClassState_Policy_Offset				0
#define		ClassState_Policy_BitOffset				2
#define		ClassState_Policy_BitMask				0x1

#define		ClassState_Interaction_Offset			0
#define		ClassState_Interaction_BitOffset		3
#define		ClassState_Interaction_BitMask			0x1

#define		ClassState_Record_Offset				0
#define		ClassState_Record_BitOffset				4
#define		ClassState_Record_BitMask				0x0F


//////////////////////////////////////////////////////////////////////////
#define		DeviceCurUse_Offset						16
#define		DeviceCurUse_Len						2
//////////////////////////////////////////////////////////////////////////
#define		DeviceCurUse_CurDev_Offset				0

#define		SceneCurRecord_Offset					18
#define		SceneCurRecord_Len						2
//////////////////////////////////////////////////////////////////////////
#define		SceneCurRecord_CurShootDev_Offset		0
#define		SceneCurRecord_CurShootDev_Len			1

#define		SceneCurRecord_CurShootScene_Offset		1
#define		SceneCurRecord_CurShootScene_Len		1

#define		RecordCurStorage_Offset					20
#define		RecordCurStorage_Len					4

#define		RecordConfig_Offset						24
#define		RecordConfig_Len						2
//////////////////////////////////////////////////////////////////////////
#define		RecordConfig_Rate_Offset				0
#define		RecordConfig_Rate_BitOffset				0
#define		RecordConfig_Rate_BitMask				0x0F

#define		RecordConfig_Size_Offset				0
#define		RecordConfig_Size_BitOffset				4
#define		RecordConfig_Size_BitMask				0x0F

#define		RecordConfig_Mode_Offset				1
#define		RecordConfig_Mode_BitOffset				0
#define		RecordConfig_Mode_BitMask				0x0F

#define		RecordConfig_Height_Offset				1
#define		RecordConfig_Height_BitOffset			4
#define		RecordConfig_Height_BitMask				0x0F

#define		WarningCurInput_Offset					26
#define		WarningCurInput_Len						1
//////////////////////////////////////////////////////////////////////////
#define		WarningCurInput_Warning_Offset			0

#define		WarningCurOutput_Offset					27
#define		WarningCurOutput_Len					1
//////////////////////////////////////////////////////////////////////////
#define		WarningCurOutput_Warning_Offset			0

#define		UnitDevState_Offset						28
#define		UnitDevState_Len						2
//////////////////////////////////////////////////////////////////////////
#define		UnitDevState_TPCPower_Offset			0
#define		UnitDevState_TPCPower_BitOffset			0
#define		UnitDevState_TPCPower_BitMask			0x1

#define		UnitDevState_TPCLink_Offset				0
#define		UnitDevState_TPCLink_BitOffset			1
#define		UnitDevState_TPCLink_BitMask			0x1

#define		UnitDevState_TPCSoftCtr_Offset			0
#define		UnitDevState_TPCSoftCtr_BitOffset		2
#define		UnitDevState_TPCSoftCtr_BitMask			0x1

#define		UnitDevState_SMPCPower_Offset			0
#define		UnitDevState_SMPCPower_BitOffset		4
#define		UnitDevState_SMPCPower_BitMask			0x1

#define		UnitDevState_SMPCLink_Offset			0
#define		UnitDevState_SMPCLink_BitOffset			5
#define		UnitDevState_SMPCLink_BitMask			0x1

#define		UnitDevState_SMPCSoftCtr_Offset			0
#define		UnitDevState_SMPCSoftCtr_BitOffset		6
#define		UnitDevState_SMPCSoftCtr_BitMask		0x1

#define		UnitDevState_LightState_Offset			0
#define		UnitDevState_LightState_BitOffset		7
#define		UnitDevState_LightState_BitMask			0x1

#define		UnitDevState_KeyBoardLink_Offset		1
#define		UnitDevState_KeyBoardLink_BitOffset		0
#define		UnitDevState_KeyBoardLink_BitMask		0x1

#define		UnitDevState_TrackCardLink_Offset		1
#define		UnitDevState_TrackCardLink_BitOffset	1
#define		UnitDevState_TrackCardLink_BitMask		0x1

#define		UnitDevState_VideoSyncLink_Offset		1
#define		UnitDevState_VideoSyncLink_BitOffset	2
#define		UnitDevState_VideoSyncLink_BitMask		0x1

#define		UnitDevState_AudioCardLink_Offset		1
#define		UnitDevState_AudioCardLink_BitOffset	3
#define		UnitDevState_AudioCardLink_BitMask		0x1

#define		UnitDevState_VideoCard1Link_Offset		1
#define		UnitDevState_VideoCard1Link_BitOffset	4
#define		UnitDevState_VideoCard1Link_BitMask		0x1

#define		UnitDevState_VideoCard2Link_Offset		1
#define		UnitDevState_VideoCard2Link_BitOffset	5
#define		UnitDevState_VideoCard2Link_BitMask		0x1

#define		UnitDevState_AmplifierState_Offset		1
#define		UnitDevState_AmplifierState_BitOffset	6
#define		UnitDevState_AmplifierState_BitMask		0x1

#define		UnitDevState_ProjectorState_Offset		1
#define		UnitDevState_ProjectorState_BitOffset	7
#define		UnitDevState_ProjectorState_BitMask		0x1

#define		CameraCurControl_Offset					30
#define		CameraCurControl_Len					1
//////////////////////////////////////////////////////////////////////////
#define		CameraCurControl_CurCamera_Offset		0
