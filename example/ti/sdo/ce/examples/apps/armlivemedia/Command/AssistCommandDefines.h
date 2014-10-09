#pragma once

#define CHECK_SUM_LEN					0x01					///串口数据校验位长度

//=================================================================================
/**
@brief 完整的数据结构，网络数据完整的结构
*/
#define ALL_DATA_LENGTH					0x2000	//0x2000//即8192 0x0200					///一帧完整数据的长度
#define NET_HEAD_LEN					0x10					///网络协议的帧头长度

#define SYNC_HEAD						0x4C423032				///录播2的标示符
#define SOURCE_POS						0x04					///发送方IP的位置
#define DESTINATION_POS					0x08					///接受方IP的位置
#define FRAME_NO_POS					0x0C					///帧序列号的位置
#define CMD_TYPE_POS					0x0D					///命令类型位置


///协议类型开始=================================
/**
@brief 协议类型 Cmd_Type ，比特是 D0 D1 D2 D3 D4 D5 D6 D7
	   D6 D7 ：最后2个比特表示协议类型；
	   D4 D5 ：表示命令类型
	   D0 D1 D2 D3：保留
*/
//保留位D0 D1 D2 D3
#define NET_RESERVE_VALUDE				0xF0

//发送的命令类型，是发送的数据命令还是响应命令 D4 D5
#define NET_SEND_CMD					0x00			///给对方主动发出命令
#define NET_ACK_CMD						0x04			///响应对方信息，在接收完对方信息之后发出
#define NET_CMD_TYPE					0x0C            ///0000 1100

//命令中的协议类型，是网络协议，还是设备协议D6 D7
#define NET_DATA_PROTOCOL				0x00			///网络协议
#define DEVICE_DATA_PROTOCOL			0x01			///设备协议
#define PROTOCOL_TYPE					0x03			///0000 0011
///协议类型结束==================================

//=================================================================================

/**
@brief 设备子地址的数据结构，发送目的地的子地址
*/
#define DEVICE_SUB_ADDR_HEAD_LEN		0x02					///设备子地址协议头长度
#define DEVICE_ADDR_ONE_FRAME_LEN		\
	(ALL_DATA_LENGTH - NET_HEAD_LEN)							///设备子地址一帧数据最长的长度

/**
@brief 设备的类型，对应于网络头后面的子设备类型
*/
#define ALL_DEVICE_TYPE					0x00				///所有设备，带该ID发出的命令肯定是集控发出的
#define	MCU1_TYPE						0x01				///辅助MCU1
#define	SMALL_SPACE_POSITION_TYPE		0x02				///小空间定位模块
#define	REGION_SPACE_POSITION_TYPE		0x03				///区域定位模块
#define	WIRELESS_SPACE_POSITION_TYPE	0x04				///无线定位模块
#define AUDIO_CHECK_TYPE				0x05				///音频检测模块
#define	CONTROL_PANEL_TYPE				0x06				///控制面板
#define	CENTRE_CONTROL_TYPE				0x07				///中控
#define	CAMERA_TYPE						0x08				///摄像头
#define	INTELLIGENT_PLATFORM_TYPE		0x09				///智能中央平台
#define STREAM_DEVICE_TYPE				0x0A				///流媒体设备
#define FOUR_ENCODE_DEVICE_TYPE			0x0B				///四路编码设备
#define MCU2_TYPE						0x0F				///辅助MCU2

#define INTEGRATE_CONTROL_TYPE			0x01				///集控

//================================================================================
/**
@brief 网络数据的数据结构	，有可能会取消掉
*/
#define NET_DATA_HEAD_LEN				0x04//0x00//0x02					///网络数据帧头长度
#define NET_DATA_LEN					\
	(ALL_DATA_LENGTH - NET_HEAD_LEN - DEVICE_SUB_ADDR_HEAD_LEN - NET_DATA_HEAD_LEN)	///网络的有效数据
#define NET_ONE_FRAME_LEN				\
	(ALL_DATA_LENGTH - NET_HEAD_LEN - DEVICE_SUB_ADDR_HEAD_LEN )///网络一帧数据的长度

//=================================================================================
/**
@brief 多串口协议的数据结构，发送给串口管理器的
*/
#define MULTI_SERIES_HEAD_LEN			0x03					///多串口协议头长度
#define MULTI_SERIES_DATA_LEN			0x20					///多串口有效数据的长度
#define MULTI_SERIES_ONE_FRAME_LEN		\
	(MULTI_SERIES_HEAD_LEN + MULTI_SERIES_DATA_LEN + CHECK_SUM_LEN)	///多串口一帧数据的长度

#define MULTI_SEND_FLAG						0xFE					///发送标识符
#define MULTI_ACK_FLAG						0xF0					///响应标识符
#define MULTI_LENGTH_POS					0x02					///多串口数据长度的位置
#define SERIAL_FRAME_NUM					0x10		//16		//多串口的帧序号数目

//=================================================================================
/**
@brief 单设备(串口数据)的数据结构，通过串口发送给硬件的,自己制造的设备用此协议，
	   外购设备不用此协议
*/
#define DEVICE_DATA_HEAD_LEN			0x03					///设备数据帧头长度
#define DEVICE_DATA_LEN					0x0F					///串口设备有效数据的长度
#define DEVICE_ONE_FRAME_LEN			\
	(DEVICE_DATA_HEAD_LEN + DEVICE_DATA_LEN + CHECK_SUM_LEN)	///串口设备一帧数据的长度

#define DEVICE_CONTROL_FLAG				0xF1					///发送给串口数据的设备控制符
//=================================================================================

#define AFAIL					-1					//失败
#define	OUT_DEVICE				0


#define		COMMAND_MAXLEN		ALL_DATA_LENGTH		/// 一条命令的最大数据长度
#define		COMMAND_BUF_COUNT	20					/// 缓存的命令条数

// 流媒体通用查询命令：
typedef enum tagALL2SE
{
	_ALL2SE_START					=	0x00004000	/// 其它系统查询流媒体服务器当前状态

	,NB2SE_CONNECT					=   0x00004100	/// NBAgent连接到流媒体
	,NB2SE_DISCONNECT								/// NBAgent断开到流媒体的连接
	,NB2SE_UPLOAD_FINISH							/// 通知服务器PPT文件传输完成
	,NB2SE_PPT_PAGE									/// 发送PPT的页码信息
	,NB2SE_COURSEID

	,ALL2SE_QUERY_STATE				=	0x00004200	/// 其它系统查询流媒体服务器当前状态
	,ALL2SE_QUERY_TIME								/// 其它系统查询流媒体服务器当前时间
	,ALL2SE_QUERY_FILENAME							/// 其它系统查询流媒体录制的文件名
	,ALL2SE_QUERY_SHAREPATH							/// 其它系统查询流媒体服务器的共享路径信息

	,SE2ALL_RESPONSE_STATE_PLAY		=	0x00004300	/// 通知NBAgent状态为Play
	,SE2ALL_RESPONSE_STATE_PAUSE					/// 通知NBAgent状态为Pause
	,SE2ALL_RESPONSE_STATE_STOP						/// 通知NBAgent状态为Stop
	,SE2ALL_RESPONSE_TIME							/// 当前的时间
	,SE2ALL_RESPONSE_FILENAME						/// 文件名
	,SE2ALL_RESPONSE_SHAREPATH						/// 共享路径(包括登录用户名和密码)
	,SE2ALL_RESPONSE_STATE_COURSEOVER				/// 通知NBAgent本节课结束

	,BP_SM_TEMPLATELIST				= 	0x00004390	/// Template List From BP Stream
	,BP_SM_DISCONNECT								/// 通知小助手断开连接
	,BP_SM_CONNECT									/// 通知小助手连接
	,BP_SM_PLAYMEDIA								/// 通知小助手播放指定媒体文件，后接PR2COM_Data1，cValue指定要播放的文件名
	,BP_SM_STOPMEDIA								/// 通知小助手停止播放
	,BP_SM_SAVEMEDIA								/// 通知小助手保存的文件名，小助手收到此命令后可提示用户是否保存，后接PR2COM_Data1，cValue指定要播放的文件名
	,BP_SM_STREAMNOTIFY								/// 流媒体提示消息，小助手收到此命令后提示用户，后接PR2COM_Data1，cValue指定错误内容
	,BP_SM_LIVEPLAY
	,BP_SM_LIVESTOP

	,SE2ALL_COURSEINFO				=	0x00004400	/// 当前的课程信息
	,SE2ALL_TEACHERINFO								/// 当前的教师信息

	,SES2C_RECEPT_LOGIN				=	0x00004500	/// 流媒体服务器接受客户端的登录
	,SES2C_LIVE_INFO								/// 发送直播信息，客户端收到此命令初始化视频接收端
	,SES2C_PPT_FILE									/// 发送PPT文件名，客户端收到此命令初始化PPT
	,SES2C_PPT_PAGE									/// 发送当前PPT翻到了哪页
	,SES2C_STATE_PLAY								/// 开始播放
	,SES2C_STATE_PAUSE								/// 暂停播放
	,SES2C_STATE_STOP								/// 停止播放

	,SEC2S_LOGIN					=	0x00004600	/// 流媒体客户端登录
	,SEC2S_LOGOUT									/// 流媒体客户端登出

	// Windy Add 四分屏功能实现
	,MC2SE_CONNECT					=	0x00004700	/// 媒体中心连接到录播
	,MC2SE_DISCONNECT

	,_ALL2SE_OVER					=	0x00004FFF	///平台发送给流媒体最大的命令值
}ALL2SE;

/**
@brief 命令字定义

,IC2AD_RECORD_START	= 0x00000710				///录制开始；面板显示，平台记录状态，发送命令给流媒体；
,IC2AD_RECORD_PAUSE								///录制暂停；面板显示，平台记录状态，发送命令给流媒体；
,IC2AD_RECORD_STOP								///录制结束；面板显示，平台记录状态，发送命令给流媒体；
,IC2AD_RECORD_RESUME							///录制恢复；

*/

typedef struct tagPR2COM_ID
{
	DWORD	dwCommand;			/// 命令字
}PR2COM_ID;


/**
@brief NB2SE_UPLOAD_FINISH 上传完成相应的标志
*/
#define FLAG_NB2SE_UPLOAD_FINISH_PPT	0x01
#define FLAG_NB2SE_UPLOAD_FINISH_XML	0x02

#define PR2COM_DATA1_CVALUE1_LEN		260

/**
@brief 命令数据结构1定义
*本结构适合以下命令：
*IC2SP_SWITCH_LIVE	切换视频源
*						dwValue 为要切换到的视频源(0-4)
*SE2ALL_RESPONSE_SHAREPATH	共享路径(包括登录用户名和密码)
*						cValue 为共享路径
*NB2SE_UPLOAD_FINISH	通知服务器PPT文件或XML文件传输完成，cValue为PPT文件名
*						dwValue = FLAG_NB2SE_UPLOAD_FINISH_PPT 表示PPT文件传输完成
*						dwValue = FLAG_NB2SE_UPLOAD_FINISH_XML 表示XML文件传输完成	(PPT描述文件)
*
*SES2C_LIVE_INFO		直播信息
*						dwValue直播端口
*SES2C_PPT_FILE			流媒体服务器发送PPT文件路径
*						cValue 为PPT文件路径
*SES2C_PPT_PAGE			流媒体服务器发送当前PPT翻到了哪页
*						dwValue 为翻到的页数
*/
typedef struct tagPR2COM_Data1
{
	DWORD	dwValue;								/// 数字型值
	char	cValue[PR2COM_DATA1_CVALUE1_LEN];		/// 字符型值
}PR2COM_Data1;


/**
@brief 命令数据结构2定义
*本结构适合以下命令：
*SP2SE_RECORD_PARAM			传输XML格式的参数
*/
typedef struct tagPR2COM_Data2
{
	char	cValue[ALL_DATA_LENGTH-1024];		/// 字符型数据Buffer
}PR2COM_Data2;
