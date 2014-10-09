#pragma once

#include <string>
#include "typedef.h"
#include "netprotocal/SCSMCommandStruct.h"
#include "netprotocal/ICIICommandDef.h"
#include "netprotocal/TPCCommandStruct.h"
#include "tinyxml.h"
#include "./core/Mutex.h"
#include "config.h"

using namespace std;
#ifdef VERSION_MUL_CAMERA
#define MAX_IPC 	9
#else
#define MAX_IPC 	5
#endif
#define MAX_TEMPLATE_COUNT 5

enum ERecordMode{
	eMovieMode = 0, /**< 电影	*/
	eResourceMode,
	eThirdScreenMode,	/**< */
	eInterMode, 	/**< 互动	*/
	eMicroMode,		/**< 微格  	*/
	eDualScreenMode /**< 双画面*/
};

typedef struct tagRecordTemplate
{
	SceneBackPicInfo	HeadBackPic;
	BOOL				bHeadCourseNameShow;
	SceneStringInfo		HeadCourseName;
	BOOL				bHeadTitleShow;
	SceneStringInfo		HeadTitleName;
	BOOL				bHeadTeacherNameShow;
	SceneStringInfo		HeadTeacherName;
	BOOL				bHeadTimeShow;
	SceneStringInfo		HeadTime;

	BOOL				bVideoLogoShow;
	SceneMixPicInfo		VideoLogo;
	BOOL				bVideoCoursePicShow;
	SceneMixPicInfo		VideoCoursePic;
	BOOL				bVideoCourseNameShow;
	SceneStringInfo		VideoCourseName;
	BOOL				bVideoTeacherNameShow;
	SceneStringInfo		VideoTeacherName;

	BOOL				bTailTitleShow;
	SceneStringInfo		TailTitle;
	BOOL				bTailTimeShow;
	SceneStringInfo		TailTime;
	SceneBackPicInfo	TailBackPic;
};

enum ERecordResolution{
	e1920_1080 = 0,
	e1536_768,
	e1280_720,
	e1024_768,
	e1280_1024
};

#define MAX_RESOLUTION_COUNT 5

struct stVideoDelayParam {
	int				nVGASlowFrameCount;
	int 			nCameraRepeatFrame;
};

enum EDeviceChannel {
	eChannelDevice,
	eChannelStudent1,
	eChannelStudent2,
	eChannelTeacher2,
	eChannelTeacher1,
	eChannelTeacher3,
	eChannelTeacher4,
	eChannelStudent3,
	eChannelStudent4
};



class Setting{
public:
	Setting();
	~Setting();

public:
	bool readSetting();

	bool writeSetting();

public:
	string 		getWebID();

	string 		getOwnIP();

	WORD		getOwnPort();

	string 		getUnitIP();

	WORD		getUnitPort();

	void 		setRecordMode(int mode);

	int			getRecordMode();

	bool 		setIPCChannel(PRChannel channel);

	bool		setOldIPCTeacherChannel(int dwTeacherChannel);

	bool		setOldIPCStudentChannel(int dwStudentChannel);

	PRChannel* 	getIPCChannel();

	PRChannel*	getOldIPCChannel();

	void 		setIPCInfo(IPCInfo* info, int index);

	IPCInfo*	getIPCInfo(int index);

	int 		getVideoFrameRate();

	string 		updateSaveFileName(bool bPreview);

	string 		getSaveFileName();

	string      getUtf8RelateFileName();

	string 		getUnicodeRelateFileName();

	string 	    getSavePath();

	int 		getSaveWidth();

	int 		getSaveHeight();

	int 		getAudioSampleRate();

	int			getAACBitRate();

	bool 		getPCMNonBlock();

	string 		getAudioDeviceName();

	int 		getVideoBitrate();

	void 		setHeadEnable(bool bEnable);

	bool 		getHeaderEnable();

	bool		getRtmpEnable();

	void 		setRtmpEnable(bool bRtmpEnable);

	string 		getInterRtmpUrl();

	string		getLCRtmpUrl();

	void 		setLCRtmpUrl(const char* pLCRtmpUrl);

	bool		getMtgEnable();

	void 		setMtgEnable(bool bMtgEnable);

	bool 		getLCSendResourceEnable();

	void 		setLCSendResourceEnable(bool bMtgSendResourceEnable);

	string 		getMtgIp();

	string 	    getLCRecordPath();

	string		getClassRoomID();

	string 		getLCVGAPath();

	string		getLCTeacherPath();

	string  	getLCStudentPath();

	string 		getLCSpeexPath();

	string		getLCAACPath();

	bool		getVGAEncodeEnable();

	void 		setBaseRecordParams(BaseRecordParam param);

	BaseRecordParam* getBaseRecordParams();

	void		setUtf8RecordParam(stUtf8RecordParam* pParam);

	stUtf8RecordParam* getUtf8RecordParam();

	void 		setACCParam(PRACCParam param);

	PRACCParam* getACCParam();

	string  getACCLoginName();

	string  getACCLoginPassword();

	string  getACCProductID();

	string  getACCCourseID();

	string  getACCServerID();

	string  getACCFolderID();

	string  getACCFolderTreeID();

	string  getACCFolderName();

	string  getACCFolderType();

	void setWebServiceParam(WebServiceParam param);

	WebServiceParam* getWebServiceParam();

	tagRecordTemplate* getRecordTemplate(int width, int height);

	void resetIPCInfos();
	void resetSceneInfos();

	void setSceneVideoInfo(const SceneVideoInfo* info);
	SceneVideoInfo*	getTeacherSceneVideoInfo();
	SceneVideoInfo* getStudentSceneVideoInfo();
	SceneVideoInfo* getDeviceSceneVideoInfo();

	void setSceneBackPicInfo(const SceneBackPicInfo* info);
	SceneBackPicInfo* getSceneBackPicInfo();

	void setSceneStringInfo(const SceneStringInfo* info);
	SceneStringInfo* getSceneStringInfo();

	void setSceneMixPicInfo(const SceneMixPicInfo* info);
	SceneMixPicInfo* getSceneMixPicInfo();

	void setSceneMediaFileInfo(const SceneMeidaFileInfo* info);
	SceneMeidaFileInfo* getSceneMediaFileInfo();

	bool 	getLiveEncEnable();
	int		getLiveEncBitrate();
	int 	getVGAEncBitrate();

	bool    getAutoUploadFile();

	void 	setAutoUploadFile(bool bAutoUploadFile);

	int 	getAudioDelayFrames();

	void 	setBPIPInfo(BPIPINFO bpIPInfo);

	BPIPINFO *getBPIPInfo();

	string   getSharePath();

	string 	getSharePathUsername();

	string 	getRecordTemplateName();

	string getRecordTemplatePath();

	bool 	getHDMIDisplayEnable();

	bool	getVGADisplayEnable();

	string	getHDMIDisplayDeviceName();

	string  getVGADisplayDeviceName();

	int 	getLiveOutWidth();

	int 	getLiveOutHeight();

	int		getVGASlowFrameCount(int width, int height);

	int 	getCameraRepeatFrameCount(int width, int height);

	int		getLiveEncVOPInter();

	bool 	getLiveEncHighProfile();

	int 	getDiskSpaceAlertSize();

	int		getDeleteFrameCount();

	int		getLiveDeleteFrameCount();

	bool	getAutoDeleteFiles();

	int		getDaysBeforeCanDelete();

	string	getDestPath();

	bool	getEncodeQualityEnable();

	bool    getNotifyVGAChangedEnable();

	bool	getDualScreenCrop();

	bool	getVGASwitchOldPanel();

	char*  getSchool() {
		return m_strSchool;
	}

	char* getCampus() {
		return m_strCampus;
	}

	char* getBuilding() {
		return m_strBuilding;
	}

	char* getRoomName() {
		return m_strRoomName;
	}

	int getEventId() {
		return m_eventId;
	}

	void setEventId(int eventId) {
		m_eventId = eventId;
	}

private:

	bool 	readTemplate();

	void 	readHeaderTemplate(TiXmlHandle* pHeaderInfo, int index);

	void	readTailTemplate(TiXmlHandle* pTailInfo, int index);

	void	readLogoTemplate(TiXmlHandle* pLogoInfo, int index);

	void	readCourseTemplate(TiXmlHandle* pCourseInfo, int index);

	bool 	getSceneStringInfo(TiXmlAttribute* pAttr, SceneStringInfo* info, BOOL &bShow);

	bool 	getRgbValue(string strColor, int& r, int& g, int& b);

public:

	IPCInfo		 m_IPCInfo[MAX_IPC];
	PRChannel	 m_IPCChannel; /**< this index is for m_IPCINfo*/
	PRChannel 	 m_oldIPCChannel;


	SceneVideoInfo	m_sceneVideoInfo[SOURCE_SCENE_COUNT];
	SceneBackPicInfo	m_sceneBackPicInfo;
	SceneStringInfo		m_sceneStringInfo;
	SceneMixPicInfo		m_sceneMixPicInfo;
	SceneMeidaFileInfo	m_sceneMediaFileInfo;

	int 		m_videoFramerate;
	int			m_recordMode;

	string 		m_strRecordTemplateXMl;	/**< 模板文件的名字 */
	string		m_strWebID;

	string 		m_strOwnIP;
	WORD		m_ownPort;

	string 		m_strUnitIP;
	WORD		m_unitPort;

	bool 		m_bHeaderEnable;

	string		m_strInterRtmpUrl;
	bool 		m_bRtmpEnable;
	string		m_strLCRtmpUrl;
	string 		m_strMtgIp;
	bool		m_bMtgEnable;
	bool 		m_bVGAEncodeEnable;


	string 		m_strLiveCourseIP;
	bool		m_bLCSendResourceEnable; //LC short for LiveCourse

	string 		m_strLCRecordPath;
	string 		m_strClassRoomID;
	bool		m_bRoomIDValid;

	string 		m_strLCVGAPath;
	string		m_strLCTeacherPath;
	string		m_strLCStudentPath;

	string 		m_strLCSpeexPath;
	string		m_strLCAACPath;

	string		m_strSavePath;
	string		m_strSharePath;
	string 		m_strSharePathUsername;
	string		m_strTempPath;
	string		m_strSaveFile;
	string 	    m_strUnicodeRelateFileName;
	string		m_strUtf8RelateFileName;

	int			m_audioSampleRate;	/**< 音频的采样率 */
	int			m_aacBitRate;
	bool		m_bPCMNonBlock;
	string 		m_strAudioDeviceName;

	BaseRecordParam		m_recordParam;
	stUtf8RecordParam   m_recordUtf8Param;
	PRACCParam	m_accParam;
	string 			m_strACCLoginName;
	string			m_strACCLoginPassword;
	string			m_strACCProductID;
	string			m_strACCCourseID;
	string			m_strACCServerID;
	string			m_strACCFolderID;
	string 			m_strACCFolderTreeID;
	string			m_strACCFolderName;
	string			m_strACCFolderType;
	WebServiceParam	m_webServiceParam;

	tagRecordTemplate m_recordTemplate[MAX_TEMPLATE_COUNT];
	string 			m_strRecordTemplatePath; //in this path, saved png files


	bool 			m_bLiveEncEnable;
	int				m_liveEncBitrate;
	int				m_liveVideoWidth;
	int				m_liveVideoHeight;
	int				m_vgaEncBitrate;

	bool			m_bAutoUploadFile;

	int 			m_audioDelayFrames;

	BPIPINFO		m_BPIPInfo;

	bool			m_bHDMIDisplayEnable;
	string			m_strHDMIDisplayDeviceName;
	bool			m_bVGADisplayEnable;
	string 			m_strVGADisplayDeviceName;

	stVideoDelayParam m_videoDelayParams[MAX_RESOLUTION_COUNT];

	int				m_nLiveEncVOPInter;
	bool			m_bLiveEncHighProfile;

	int				m_nDiskSpaceAlertSize;
	int				m_nDeleteFrameCount;
	int				m_nLiveDeleteFrameCount;

	bool			m_bAutoDeleteFiles;
	int				m_nDaysBeforeCanDelete;


	string			m_strDestPath;

	bool			m_bEncodeQulityEnable;

	bool			m_bNotifyVGAChangeEnable;

	bool			m_bDualScreenCrop; /**< 双画面裁剪 */
	bool			m_bVGASwitchOldPanel; /**< VGA是否老面板切换*/


	char* 			m_strSchool;
	char*			m_strCampus;
	char*			m_strBuilding;
	char* 			m_strRoomName;

	int 			m_eventId;

	// 画面特效参数
	//DWORD m_dwEffectProbability;	/// 特效出现频率，0-10 数字越大，频率越高
	//DWORD m_dwEffectType;			/// 特效类型，0-不使用特效,1-4-只使用指定特效，其它值-随机使用特效

//	DWORD		m_dwAudioDenoiseLevel;	/// 音频去噪级别 0-10 0-不去噪
};

