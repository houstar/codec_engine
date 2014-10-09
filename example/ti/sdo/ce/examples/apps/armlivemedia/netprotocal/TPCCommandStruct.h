#pragma once

#include "ICIICommandStruct.h"
#include "SCSMCommandStruct.h"

typedef struct tagPRCourseParam {
	char CourseName[50];
	char Title[50];
	char TeacherName[50];
} PRCourseParam;

typedef struct tagPRACCParam {
	char AutoSavePath[512];
} PRACCParam;

typedef struct tagPRRecordParam {
	char PRTemplate[50];
	char StrategyTemplate[50];
	ePRMode PRMode;
	eRecordSize ScreenMode;
	eRecordRate RateMode;
	eRecordPos ShenGao;
	eUnitCtrl UnitCtrl;
	eEncodeType EncodeType;
	int sceneSecond;
	eRecordRate RateLiveMode;
	eRecordSize ScreenLiveMode;
} PRRecordParam;

typedef struct tagWebServiceParam {
	char ACCUrl[512];
	char PPTUrl[512];
	char WMSUrl[512];
} WebServiceParam;

typedef struct tagStrategyName {
	char cName[20];
	tagStrategyName() {
		memset(cName, 0x00, 20);
	}
} StrategyName;

typedef struct tagStrategyList {
	StrategyName Stretagy[10];
} StrategyList;

typedef struct tagTemplateName {
	char cName[20];
	tagTemplateName() {
		memset(cName, 0x00, 20);
	}
} TemplateName;

typedef struct tagTemplateList {
	TemplateName Template[10];
} TemplateList;

typedef struct tagASCommonInfo {
	DWORD dwVersion;
	DWORD dwIP;
} ASCommonInfo;

typedef struct tagAVInfo {
	DWORD dwAVIP;
	WORD wAVPort;
	WORD wAVChannel;
	WORD wUnitType;
} AS_AVInfo;

