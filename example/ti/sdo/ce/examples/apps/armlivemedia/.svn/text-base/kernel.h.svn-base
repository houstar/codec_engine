#pragma once

#include "timer.h"
#include "KVideo/vgaProviderSystem.h"
#include "Command/KernelBoardCommandHandle.h"
#include "Command/AssistCommandHandle.h"
#include "setting.h"
#include "Command/Function.h"
#include "KVideo/VideoFactory.h"
#include "liveMediaState.h"
#include "core/baseMode.h"
#include "utils/vgaState.h"
#include "liveCourse/liveCourseManager.h"
#include "web/modelStatus.h"
#include "web/recordList.h"
#include "core/v4lDisplay.h"
#include "core/HDMIDisplayManager.h"

#ifndef CODEC_ENGINE_ENABLE
#include "KVideo/audioCapture.h"
#include "KVideo/audio/audio_playback.h"
#endif


#include "core/rtmpManager.h"
#include "monitor/monitorSender.h"

#include "../config.h"
#include "KVideo/sdiVideoProviderSystem.h"

enum eCommandOrder {
	eCommandNone,
	eCommandStart,
	eCommandTail,
	eCommandStop
};


class codec_engine;

class Kernel : public ITimerListener, public IVGAProviderObserver, public IAudioCaptureObserver {
public:
	Kernel();

	virtual ~Kernel();
public:

	virtual void onTimeout(Timer *timer);

	virtual bool onNotifyVGAStateChanged(int reason);

	virtual bool onNotifyVGAFreeze();

	virtual bool onProviderNV12(unsigned char* pNV12Buffer, int width, int height);

	virtual bool onProviderYUYV(unsigned char* pYUYVBuffer, int width, int height);

	virtual bool onVgaStatus(EVGAStatus status);

	virtual	bool onProviderPcm(const char* pBuffer, int len);
public:

	virtual bool init();

	virtual bool uninit();

	inline Setting *getSetting() {
		return &m_setting;
	}

	KernelBoardCommandHandle* getKernelBoardCommandHandle() {
		return &m_kernelboardcommndHandle;
	}

	AssistCommandHandle* getAssistCommandHandle() {
		return &m_assistcommandHandle;
	}

	CFunction *getFunction() {
		return &m_function;
	}

	StateHandle* getStateHandle() {
		return &m_stateHandle;
	}

	inline LiveCourseManager* getLiveCourseManager() {
		return &m_liveCourseManager;
	}

	inline EdmaUtils* getEdmaInstance() {
		return &m_edmaUtils;
	}

	inline VGAProviderSystem* getVGAProviderSystemInstance() {
		return &m_vgaProviderSystem;
	}

	inline HDMIDisplayManager* getHDMIDisplayManagerInstance() {
		return &m_hdmiDisplayManager;
	}

#ifndef CODEC_ENGINE_ENABLE
	inline AudioCapture* getAudioCaptureInstance() {
		return &m_audioCapture;
	}

	inline audio_playback* getAudioPlaybackInstance() {
		return &m_audioPlayback;
	}
	
#else
	inline codec_engine* getCodecEngineInstance() {
		return m_pAudioCodecEngine;
	}	
#endif
	inline RtmpManager* getRtmpManager() {
		return &m_rtmpManager;
	}

	inline IHeap_Object* getHeapObjectInstance() {
		return m_heap;
	}

	SDIVideoProviderSystem* getSDIProviderSystemInstance() {
		return &m_sdiVideoProvider;
	}

	bool replay(char* pFileName);



	bool execCourseOver();

	bool execReplay(string strFileName);

	void uploadFile();

	void NotifyAssistMessage(const char* strMessage);

	void NotifyVGAResolution720P();

	void NotifyGetCommand(int command);

	void NotifyIPCChannelChanged();

	void NotifySwitchVGAChannel(int nVGAChannel);

	void moveFile();

	bool loadBackImage(int outWidth, int outHeight);

	bool loadHeadPic(int outWidth, int outHeight);

	bool loadTailPic(int outWidth, int outHeight);

	bool loadVideoCourseInfo(int outWidth, int outHeight);

	bool checkSystemStatus();

	bool doStartLC(ELiveCourseRole role);

private:
	bool previewRecord();

	bool startRecord();

	bool stopRecord();

	bool pauseRecord();

	bool execSceneSwitch();

	bool execMovieHead();

	bool execMovieTail();

	bool execCreate(bool bPreview = false);


public:
	Setting 		m_setting;
	KernelBoardCommandHandle 	m_kernelboardcommndHandle;
	AssistCommandHandle   m_assistcommandHandle;
	CFunction		m_function;
	StateHandle		m_stateHandle;

	MemBufUtils m_memBufUtils;

	EdmaUtils m_edmaUtils;

	VGAProviderSystem m_vgaProviderSystem;

//#ifdef CAMERA_SDI
	SDIVideoProviderSystem m_sdiVideoProvider;
//#endif
	HDMIDisplayManager m_hdmiDisplayManager;

#ifndef CODEC_ENGINE_ENABLE
	AudioCapture m_audioCapture;
	audio_playback m_audioPlayback;
#else
	codec_engine* m_pAudioCodecEngine;
#endif
	RtmpManager m_rtmpManager;
	//VGAStateHandle	m_vgaStateHandle;

	BaseMode*		m_RecordModeHandle;

	IHeap_Object* m_heap;

	bool 	m_bReadyCourseOver;

	Timer	m_diskCheckTimer;

	eCommandOrder m_commandOrder; /**< */

	pthread_t m_threadId;
	pthread_attr_t m_threadAttr;

	vector<int>	m_commandVector;
	semp_t		m_commandSem;
	Timer		m_commandTimer;

	Timer		m_vgaFreezeTimer;
	bool		m_vgaFreezeTimerValid;

	pthread_attr_t m_moveFileThreadAttr;
	pthread_t		m_moveFileThreadId;
	Mutex			m_moveFileMutex;


	bool m_bInited;
	vector<string> m_vecSrcName;
	vector<string> m_vecDestName;

	int		m_outWidth;
	int		m_outHeight;
	char 	m_cUtf8CourseName[50];
	char	m_cUtf8TeacherName[50];
	char	m_cUtf8TitleName[50];

	LiveCourseManager m_liveCourseManager;

	ModelStatus*		m_pModelStatus;
	RecordList		m_recordList;

	int		m_nlastVGAChannel;

	DisplayHandle m_vgaDisplay;

	MonitorSender m_monitorSender;
};
