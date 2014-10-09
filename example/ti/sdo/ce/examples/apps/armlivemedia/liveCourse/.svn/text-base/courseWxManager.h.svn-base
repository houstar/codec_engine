#pragma once

#include "../core/mtgClient.h"
#include "../timer.h"
#include "livemtgConstDefine.h"
#include "../setting.h"
#include "../core/Mutex.h"


struct stSubscribeInfo {
	string path;
	bool   bSubscibeLCMediaType[eLCMediaTypeCount];
};

#define VideoListItemCount 4

class ICourseWxObserver {
public:
	virtual ~ICourseWxObserver() {

	}

	virtual bool onCourseWxLeave() {
		return true;
	}
};

class ILiveCourseManagerObserver;

class CourseWxManager : public IMtgListener, public ITimerListener {
public:
	CourseWxManager();
	virtual ~CourseWxManager();

public:
	virtual void onRecvMtgData(MtgClient* pMtg, const char* path,
				const char*data, int dataLen, int datatype, int action);

	virtual void onJoinCallback(MtgClient* pMtg, bool bJoinOK);

	virtual void onTimeout(Timer* pTimer);


public:
	bool setSpxListener(ILiveCourseManagerObserver* pObserver);

	bool removeSpxListener(void);
	
	bool addListener(ICourseWxObserver* pObserver);

	bool removeListener(ICourseWxObserver* pObserver);

	bool init(Setting* pSetting);

	bool setRole(ELiveCourseRole role);

	ELiveCourseRole getRole() {
		return m_role;
	}

	bool setCourseInfo(int courseId, string strMtgIp);

	bool uninit();

	bool createPaths();

	bool destroyPaths();

	bool sendClassStatus(int classStatus);

	bool sendEnter();

	bool sendVideoList();

	bool sendData(const char* pPath, const char* pData, int dataLen, int dataType, ELiveCourseMediaType lcType);

	tagLiveMtgWebMessage* getWebMessage() {
		return &m_lcWebMessage;
	}

	vector<tagStreamInfo> getStreaInfos() {
		return m_vecStreamInfos;
	}

	int getCourseId() {
		return m_courseID;
	}

	string getPlatformIP() {
		return m_strMtgIp;
	}

	unsigned long getSendCount();

	unsigned long getRecvCount();

	unsigned long getSendLossCount();

	unsigned long getRecvLossCount();

private:
	bool procRecvEnterCmd(const char* pData, int len);

	bool procRecvLeaveCmd(const char* pData, int len);

	bool procRecvCourseStatusCmd(const char* pData, int len);

	bool procRecvClassStatusCmd(const char* pData, int len);

	bool procRecvSubscribeListCmd(const char* pPath, const char* pData, int len);

	bool procRecvWebMonitorCmd(const char* pData, int len);

private:
	Setting* m_pSetting;
	MtgClient* m_pCourseClient;
	bool	m_bSendLCMediaType[eLCMediaTypeCount];
	Timer	m_timer;
	bool	m_bTimerValid;

	ELiveCourseRole m_role;
	int		m_userId;
	string  m_strMtgIp;
	int		m_courseID;

	string	m_strEnterPath;
	string  m_strLeavePath;
	string  m_strClassStatusPath;
	string  m_strSubscribeListPath;
	string	m_strCourseStatusPath;
	string  m_strVideoListPath;
	string  m_strWebMonitorPath;

	vector<stSubscribeInfo> m_vecSubscribeInfos;

	vector<ICourseWxObserver*> m_vecObservers;

	ILiveCourseManagerObserver* m_pSpxListener;
	
	Mutex	m_mutex;

	tagLiveMtgWebMessage m_lcWebMessage;
	vector<tagStreamInfo> m_vecStreamInfos;
	bool m_bExitCourse;
};
