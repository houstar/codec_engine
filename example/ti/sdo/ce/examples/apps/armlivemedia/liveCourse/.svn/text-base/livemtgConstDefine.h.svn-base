#pragma once
#include "prePathMtgConstDefine.h"
/*客户端路径规则：前缀 +/uid
教室端路径规则：前缀+/roomID
*/

#define MAX_PATH 260

enum ELiveCourseCmd {
	eLiveCourseCmdNULL = 0,
	eLiveCourseCmdEnter,
	eLiveCourseCmdLeave,
	eLiveCourseCmdCourseStatus,
	eLiveCourseCmdClassStatus,
	eLiveCourseCmdVideoList,
	eLiveCourseCmdVGAList,
	eLiveCourseCmdMonitorList,
	eLiveCourseCmdInteractiveList,
	eLiveCourseCmdRecordModeList,
	eLiveCourseCmdSubscribeList
};

// 命令路径+/roomID+/uid
#define LIVEMTG_PATH_CMD											"/cmd"
#define LIVEMTG_PATH_CMD_ENTER										"/cmd/enter"
#define LIVEMTG_PATH_CMD_LEAVE										"/cmd/leave"
#define LIVEMTG_PATH_CMD_COURSESTATUS								"/cmd/coursestatus"
#define LIVEMTG_PATH_CMD_CLASSSTATUS								"/cmd/classstatus"
#define LIVEMTG_PATH_CMD_VIDEOLIST									"/cmd/videolist"
#define LIVEMTG_PATH_CMD_VGALIST									"/cmd/vgalist"
#define LIVEMTG_PATH_CMD_MONITORLIST								"/cmd/monitorlist"
#define LIVEMTG_PATH_CMD_INTERACTIVELIST							"/cmd/interactivelist"
#define LIVEMTG_PATH_CMD_RECORDERMODELIST							"/cmd/recordermodelist"
#define LIVEMTG_PATH_CMD_SUBSCRIBELIST								"/cmd/subscribelist"
#define LIVEMTG_PATH_CMD_RTMP										"/cmd/rtmp"
#define LIVEMTG_PATH_CMD_WEBMESSAGE									"/cmd/webmessage"

#define KLiveMtgPathCmdLength (strlen(LIVEMTG_PATH_CMD))
#define KLiveMtgPathCmdEnterLength (strlen(LIVEMTG_PATH_CMD_ENTER))
#define KLiveMtgPathCmdLeaveLength (strlen(LIVEMTG_PATH_CMD_LEAVE))


#define LIVEMTG_PATH_VIDEO_TEACHER_HIGH_1							LIVECOURSE_PATH_VIDEO_TEACHER_HIGH_1
#define LIVEMTG_PATH_VIDEO_TEACHER_HIGH_2							LIVECOURSE_PATH_VIDEO_TEACHER_HIGH_2
#define LIVEMTG_PATH_VIDEO_TEACHER_HIGH_3							LIVECOURSE_PATH_VIDEO_TEACHER_HIGH_3
#define LIVEMTG_PATH_VIDEO_STUDENT_HIGH_1							LIVECOURSE_PATH_VIDEO_STUDENT_HIGH_1
#define LIVEMTG_PATH_VIDEO_STUDENT_HIGH_2							LIVECOURSE_PATH_VIDEO_STUDENT_HIGH_2
#define LIVEMTG_PATH_VIDEO_STUDENT_HIGH_3							LIVECOURSE_PATH_VIDEO_STUDENT_HIGH_3

#define LIVEMTG_PATH_VIDEO_TEACHER_MID_1							LIVECOURSE_PATH_VIDEO_TEACHER_MID_1
#define LIVEMTG_PATH_VIDEO_TEACHER_MID_2							LIVECOURSE_PATH_VIDEO_TEACHER_MID_2
#define LIVEMTG_PATH_VIDEO_TEACHER_MID_3							LIVECOURSE_PATH_VIDEO_TEACHER_MID_3
#define LIVEMTG_PATH_VIDEO_STUDENT_MID_1							LIVECOURSE_PATH_VIDEO_STUDENT_MID_1
#define LIVEMTG_PATH_VIDEO_STUDENT_MID_2							LIVECOURSE_PATH_VIDEO_STUDENT_MID_2
#define LIVEMTG_PATH_VIDEO_STUDENT_MID_3							LIVECOURSE_PATH_VIDEO_STUDENT_MID_3

#define LIVEMTG_PATH_VIDEO_TEACHER_LOW_1							LIVECOURSE_PATH_VIDEO_TEACHER_LOW_1
#define LIVEMTG_PATH_VIDEO_TEACHER_LOW_2							LIVECOURSE_PATH_VIDEO_TEACHER_LOW_2
#define LIVEMTG_PATH_VIDEO_TEACHER_LOW_3							LIVECOURSE_PATH_VIDEO_TEACHER_LOW_3
#define LIVEMTG_PATH_VIDEO_STUDENT_LOW_1							LIVECOURSE_PATH_VIDEO_STUDENT_LOW_1
#define LIVEMTG_PATH_VIDEO_STUDENT_LOW_2							LIVECOURSE_PATH_VIDEO_STUDENT_LOW_2
#define LIVEMTG_PATH_VIDEO_STUDENT_LOW_3							LIVECOURSE_PATH_VIDEO_STUDENT_LOW_3

#define LIVEMTG_PATH_VIDEO_VGA_HIGH									LIVECOURSE_PATH_VIDEO_VGA_HIGH
#define LIVEMTG_PATH_VIDEO_VGA_MID									LIVECOURSE_PATH_VIDEO_VGA_MID
#define LIVEMTG_PATH_VIDEO_VGA_LOW									LIVECOURSE_PATH_VIDEO_VGA_LOW

#define LIVEMTG_PATH_VIDEO_RECODER_HIGH								LIVECOURSE_PATH_VIDEO_RECODER_HIGH
#define LIVEMTG_PATH_VIDEO_RECODER_MID								LIVECOURSE_PATH_VIDEO_RECODER_MID
#define LIVEMTG_PATH_VIDEO_RECODER_LOW								LIVECOURSE_PATH_VIDEO_RECODER_LOW

//音频 PCM格式 48K 单声道 16位
#define LIVEMTG_PATH_AUDIO_PCM_48000_1_16							"/audio/pcm/48000/1/16"
#define LIVEMTG_PATH_AUDIO_AAC										"/audio/aac"
#define LIVEMTG_PATH_AUDIO_SPEEX									"/audio/speex"

#define LIVEMTG_USER_TYPE_NULL										0			//没有类型
#define LIVEMTG_USER_TYPE_ASSISTANT_LIVE							1			//ֱ直播助教
#define LIVEMTG_USER_TYPE_ASSISTANT_AUDENCE							2			//收视助教
#define LIVEMTG_USER_TYPE_VIDEO_LIVE								3			//ֱ直播主机
#define LIVEMTG_USER_TYPE_VIDEO_AUDENCE								4			//收视主机
#define LIVEMTG_USER_TYPE_STREAMMEDIA								5			//Web收视，流媒体
#define LIVEMTG_USER_TYPE_CONTROL									6			//控制室
#define LIVEMTG_USER_TYPE_WEB_LIVECOURSE_PROXY						7			//LiveCourse代理者
#define LIVEMTG_USER_TYPE_VIDEOSERVER								8			//视频服务器

#define LIVEMTG_USER_TYPE_IC_PROVIDER								9


#define LIVEMTG_HEADER_MAGIC										0x20131203		//命令头魔数

//  命令ID [视频与音频没有ID]
#define LIVEMTG_CMD_ID_ENTER										0x00000200
#define LIVEMTG_CMD_ID_LEAVE										0x00000201
#define LIVEMTG_CMD_ID_COURSE										0x00000202		//课堂状态
#define LIVEMTG_CMD_ID_CLASSSTATUS									0x00000203		//教室状态 上课 下课 课间休息
#define LIVEMTG_CMD_ID_VIDEOLIST									0x00000210		//视频列表-该教室有多少视频
#define LIVEMTG_CMD_ID_VGALIST										0x00000211		//VGA列表
#define LIVEMTG_CMD_ID_MONITORLIST									0x00000212		//监控列表
#define LIVEMTG_CMD_ID_INTERAVTIVELIST								0x00000213		//互动列表
#define LIVEMTG_CMD_ID_RECORDERMODELIST								0x00000214		//录播模式列表
#define LIVEMTG_CMD_ID_SUBSCRIBELIST								0x00000215		//订阅列表
#define LIVEMTG_CMD_ID_CAMERACONTROL								0x00000216									//摄像头控制
#define LIVEMTG_CMD_ID_KEYNOTESPEAKER								0x00000217									//主讲人
#define LIVEMTG_CMD_ID_RTMP											0x00000218
#define LIVEMTG_CMD_ID_WEBMESSAGE	 								0x00000219

#define LIVEMTG_COURSESTATUS_ENTER									1				//进入课堂
#define LIVEMTG_COURSESTATUS_LEAVE									0				//退出课堂

#define LIVEMTG_CLASSSTATUS_END										0				//下课
#define LIVEMTG_CLASSSTATUS_BEGIN									1				//上课
#define LIVEMTG_CLASSSTATUS_BREAK									2				//课间休息

#define LIVEMTG_RECORDER_MODE_NULL									0				//录制模式
#define LIVEMTG_RECORDER_MODE_T										1				//录制模式 T
#define LIVEMTG_RECORDER_MODE_VGA									2				//录制模式 VGA
#define LIVEMTG_RECORDER_MODE_S										3				//录制模式 S
#define LIVEMTG_RECORDER_MODE_T_VGA									4				//录制模式 T+VGA
#define LIVEMTG_RECORDER_MODE_T_S									5				//录制模式 T+S
#define LIVEMTG_RECORDER_MODE_T_S_VGA								6				//录制模式 T+S+VGA

// 数据字段长度
#define LIVEMTG_ROOM_ID_LEN											32				//Room ID 最大长度
#define LIVEMTG_UID_LEN												8
#define LIVEMTG_LIST_MAXCOUNT										100			   //最大列表个数 比如tagLiveVideoListItem
#define LIVEMTG_MAX_PATH											64

struct tagLiveMtgHeader
{
	int magic;														//魔数
	int len;														//长度，包括头
	int cmdID;
	int cmdFlag;
};

/**
  *用户进入信息
  */
struct tagLiveMtgEnter
{
	char roomID[LIVEMTG_ROOM_ID_LEN];								//Room ID
	int userType;													//角色，直播主机，收视主机，直播助教，收视助教
	char school[100];
	char campus[100];
	char building[100];
	char roomName[100];
};

/**
  *@brief 课堂状态
  */
struct tagLiveMtgCourse
{
	int status;
	int courseID;
	char liveRoomID[LIVEMTG_ROOM_ID_LEN];
	char teacherName[32];											//老师名称
	char courseName[64];											//课名
	char courseSecondName[64];										//副标题
	char wxIP1[16];													//课堂平台主IP
	char wxIP2[16];													//课堂平台备用IP
};

/**
  * 教室状态
  *     通知状态
  */
struct tagLiveMtgClassStatus
{
	int status;														//状态 上课 下课 休息
	int courseID;													//课堂ID
	char liveRoomID[LIVEMTG_ROOM_ID_LEN];							//直播课堂RoomID
};

/**
  *视频列表
  *    直播 通知外面自己拥有的视频资源
  */
struct tagLiveMtgVideoListItem
{
	char pathH[LIVEMTG_MAX_PATH];											//视频路径(全路径)
	char pathM[LIVEMTG_MAX_PATH];
	char pathL[LIVEMTG_MAX_PATH];
};


/** 
  *VGA列表
  *   助教 通知自己的VGA列表,直播根据VGA列表决定自己的界面表现内容
  */
struct tagLiveMtgVGAListItem
{
	char roomID[LIVEMTG_ROOM_ID_LEN];
	char path[LIVEMTG_MAX_PATH];											//视频路径(全路径)
};

/** 
  * 监控视频列表
  *     助教 通知自己的监控列表
  */
struct tagLiveMtgMonitorListItem
{
	char roomID[LIVEMTG_ROOM_ID_LEN];
	char path[LIVEMTG_MAX_PATH];											//视频路径(全路径)
};

/** 
  *Interactive 互动列表
  *  助教 通知自己的互动列表, 直播根据互动列表决定自己的界面表现内容
  */
struct tagLiveMtgInteractiveListItem
{
	char roomID[LIVEMTG_ROOM_ID_LEN];
	char path[LIVEMTG_MAX_PATH];											//视频路径(全路径)
};

/** 
  *Subscribe 订阅列表
  *  各端通知自己的订阅列表, 直播根据当前会议中的所有用户的订阅列表，决定自己发布发送视频
  */
struct tagLiveMtgSubscribeListItem
{
	char path[LIVEMTG_MAX_PATH];											//视频路径(全路径)
};

/** 
*Recorder 录制模式列表
*  助教 通知自己的录制模式列表, 录制根据录制列表决定自己的视频内容
*/
struct tagLiveMtgRecorderModeList
{
	int curMode;
	int modeID[LIVEMTG_LIST_MAXCOUNT];
};

/**
  *@brief 摄像头控制
  */
struct tagLiveMtgCameraControl
{
	char roomID[LIVEMTG_ROOM_ID_LEN];
	char path[LIVEMTG_MAX_PATH];
	int flag;
	int value;
};

/**
  *@brief 主讲人
  */
struct tagLiveMtgKeynoteSpeaker
{
	char roomID[LIVEMTG_ROOM_ID_LEN];
	char path[LIVEMTG_MAX_PATH];
};

/**
  *@brief RTMP
  */
struct tagLiveMtgRtmp
{
	char url[MAX_PATH];
};

enum EWebEventType {
	eLCCreateLCGroup = 0x1, //互动服务建立直播组
	eLCCancelLCGroup, //互动服务撤销直播组
	eLCMainJoinGroup, //主讲教室加入直播组
	eLCMainLeftGroup, //主讲教室退出直播组
	eLCAssistJoinGroup, //互动教室加入直播组
	eLCAssistLeftGroup, //互动教室退出直播组
	eLCCreateLiveSource, //直播服务建立直播源
	eLCCanelLiveSource, //直播服务撤销直播源
	eLCStartRecord, //录制开始
	eLCStopRecord,  //录制结束
	eLCServerPowerOn, //服务器开机
	eLCServerPowerOff, //服务器关机
	eLCServerReboot, //服务器重启
	eLCClassPowerOn, //教室开机
	eLCClassPowerOff, //教室关机
	eLCClassReboot, //教室重启
	eLCLiveServerInValid, //直播服务器不可用
	eLCAssistServerInValid, //互动服务器不可用
	eLCClassDeviceError //教室设备自检错误
};

enum EWebMessageType {
	eWebK8 = 1,
	eWebL5000,
	eWebPlatServer,
	eWebPlayerControl
};

struct tagLiveMtgWebMessage{
	int messageType;
	int EventID; //事件id参见对应关系Excel
	int cpu;	//使用百分比
	int memUsed;	//使用百分比
	int upNetWork;
	int downNetWork;
	int upPacketLost;
	int downPacketLost;
	int status;
};

struct tagStreamInfo {
	char streamContent[16]; //流的含义
	int streamfps;	//流的帧率
};

enum ELiveCourseMediaType {
	eLCRecordVideo = 0,
	eLCVGAVideo,
	eLCTeacherVideo,
	eLCStudentVideo,
	eLCSpeexAudio,
	eLCAACAudio,
	eLCMediaTypeCount
};

enum ELiveCourseRole {
	eLCRoleNone = 0,
	eLCRoleAssistLive,
	eLCRoleAssistAudence,
	eLCRoleLive,
	eLCRoleAudence,
	eLCRoleStreamMedia,
	eLCRoleControl,
	eLCRoleWebProxy,
	eLCRoleVideoServer
};
