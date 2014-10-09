#pragma once
#include "../typedef.h"
#include "AssistFlagDef.h"
#include "CmdSenderRoleDef.h"
#include "CTHeartbeatFlagDef.h"
#include "ICCmdFlagDef.h"
#include "LanKeyValueDef.h"
#include "TVWallCmdFlagDef.h"
#include "UnitActionDef.h"
#include "UnitDeviceDef.h"
#include "SCSMCommandDef.h"

#include <string.h>

#define CMD_ABLE_ICII			0x3230424C
//#define CMD_ABLE_ICII			0x4c423032
#define	IC2_COMMAND_PORT		7101
#define	IC2_COMMAND_MAXLEN		2048
#define	IC2_COMMAND_BUF_COUNT	100
#define	IC2_COMMAND_HEADLEN		16

#define IC2_PR1_CMDPORT			7199

#define IC2_PPT_PORT			(IC2_COMMAND_PORT+10)
#define IC2_PPTCLIENT_PORT		(IC2_COMMAND_PORT+11)
#define	PRASSIST_PORT			(IC2_COMMAND_PORT+12)
#define	CONFIGCLIENT_PORT		(IC2_COMMAND_PORT+13)

#define CMD_TYPE_ACK			0x80
#define CMD_TYPE_COMMAND		0x00

typedef struct tagIC2CMD_Header {
	DWORD dwSyncHead;
	DWORD dwFromIP;
	DWORD dwToIP;
	BYTE byteFrameIndex;
	BYTE byteProtocolType;
	WORD wDataLen;
} IC2CMD_Header;

#define ID_GUID_LEN		10
#define DATA1_C_LEN		260

typedef struct tagIC2CMD_CmdID {
	DWORD dwCommand;
	DWORD dwFlag;
	char cUserID[ID_GUID_LEN];
	char cGUID[ID_GUID_LEN];

	tagIC2CMD_CmdID() {
		dwCommand = 0;
		dwFlag = 0;
		memset(cUserID, 0x00, ID_GUID_LEN);
		memset(cGUID, 0x00, ID_GUID_LEN);
	}
} IC2CMD_CmdID;

typedef struct tagIC2CMD_AD_Msg {
	BYTE byteDeviceID;
	BYTE byteActionID;
} IC2CMD_AD_Msg;

typedef struct tagIC2CMD_Data1 {
	DWORD dwValue;
	char cValue2[DATA1_C_LEN];
	tagIC2CMD_Data1() {
		dwValue = 0;
		memset(cValue2, 0x00, DATA1_C_LEN);
	}
} IC2CMD_Data1;

typedef struct tagIC2CMD_Data2 {
	char cValue[IC2_COMMAND_MAXLEN - 300];
	tagIC2CMD_Data2() {
		memset(cValue, 0x00, IC2_COMMAND_MAXLEN - 300);
	}
} IC2CMD_Data2;

#define LANKEY_ID		0x04
#define VOLUME_ID		0x03

typedef struct tagIC2CMD_AD_LanKey {
	BYTE LanKeyID;
	WORD KeyValue;
	tagIC2CMD_AD_LanKey() {
		LanKeyID = LANKEY_ID;
		KeyValue = 0;
	}
} IC2CMD_AD_LanKey;

#define CMD_TEST	0x00000000
#define FLAG_NULL	0x00000000

#define CMD_AD_SENDMSG     0x00010000
#define CMD_AD_RECVMSG     0x00010001
#define CMD_SVR_RECVMSG    CMD_AD_SENDMSG
#define CMD_SVR_SENDMSG    CMD_AD_RECVMSG

#define CMD_CT_BEGIN		0x00020000
#define CMD_CT_END			0x0002ffff

#define CMD_CT_LOGIN		0x00020010

#define FLAG_LOGIN_IC       0x00020011
#define FLAG_LOGIN_TVWALL   0x00020012
#define FLAG_LOGIN_ASS      0x00020013
#define FLAG_LOGIN_REC      0x00020014

#define CMD_CT_LOGINRESULT		0x00020020

#define FLAG_LOGINRESULT_SUC    0x00020021
#define FLAG_LOGINRESULT_FAIL   0x00020022
#define FLAG_LOGINRESULT_ASSFULL	0x00020023
#define FLAG_LOGINRESULT_DOGFAIL	0x00020024

#define CMD_CT_UNITSTATE   0x00020030

#define CMD_CT_RECSTATE   0x0002003a

#define FLAG_RECSTATE_START		0x0002003b
#define FLAG_RECSTATE_STOP		0x0002003c

#define CMD_CT_TVWALLCTRL   0x00020040

#define CMD_CT_TVWALLSYNC   0x00020050

#define CMD_CT_TVWALLMAIN   0x0002005a

#define CMD_CT_ICCONTROL      0x00020060

#define CMD_CT_LOGOUT      0x00020070

#define CMD_CT_QUERYUNITSTATE   0x00020090

#define CMD_CT_QUERYRECSTATE	0x0002009a

#define CMD_CT_HEART			 0x000200a0

#define CMD_CT_FROMUNITMESSAGE   0x00020100

#define CMD_CT_TOUNITMESSAGE     0x00020110

#define CMD_CT_SVW2MVW   0x00020220
#define FLAG_SVW2MVW_CONNECT    0x00020221
#define FLAG_SVW2MVW_DISCONNECT 0x00020222

#define CMD_CT_MVW2SVWSYNC		0x00020230

#define CMD_CT_TVREC_CHECK		0x00020300
#define CMD_CT_TVREC_REC		0x00020310
#define CMD_CT_TVREC_STOP		0x00020320

#define CMD_RP_TVREC_STATE		0x00020330
#define CMD_RP_TVREC_REC		0x00020340
#define CMD_RP_TVREC_STOP		0x00020350

#define FLAG_CT_TVREC_SUCCEES   0x00020301
#define FLAG_CT_TVREC_FAIL		0x00020302

#define CMD_CT_UPDATEUNITRESULT		0x00020400
#define FLAG_UPDATEUNIT_SUC		0x00020401
#define FLAG_UPDATEUNIT_FAIL	0x00020402

#define CMD_CT_ASSISUSER			0x00020500
#define FLAG_ASSISENTER				0x00020501
#define FLAG_ASSISLEAVE				0x00020502

#define CMD_AP_QUERYTPC			0x00030110	
#define CMD_AP_TPCRESPONSE      0x00030120	
#define CMD_AP_TPCENTER			0x00030140	

#define CMD_AP_VIDEOBROADCAST		0x00030130

#define FLAG_VIDEO_WM				0x00030231
#define FLAG_VIDEO_TOPVS			0x00030232
#define FLAG_VIDEO_PR				0x00030233
#define FLAG_VIDEO_TITLE			0x0003023e
#define FLAG_VIDEO_STOP				0x0003023f

#define CMD_AS_CONTROL			0x00040100

#define CMD_AS_RESPONSE			0x00040200

#define	CMD_EVA_LOGIN			0x00090100
#define	FLAG_LOGIN_EVA			0x00090101

#define	CMD_EVA_LOGINRESULT		0x00090110

