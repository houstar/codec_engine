#pragma once


#include <vector>
using namespace std;

#include "../setting.h"
#include "classWxManager.h"
#include "courseWxManager.h"
#include "livemtgConstDefine.h"
#include "../core/mtgInit.h"
#include "subVideoManager.h"

#include "../KVideo/audio/iaudio.h"

class Kernel;


class ILiveCourseManagerObserver {
public:
	virtual ~ILiveCourseManagerObserver() {

	}

	virtual bool NotifyLiveCourseChanged() {
		return true;
	}

	virtual bool onRecvMtgSpx(const char* path, const char* data, int datalen) {
		return true;
	}
};

class LiveCourseManager
	: public IClassWxObserver,
	  public iaudio,
	  public ICourseWxObserver {
public:
	LiveCourseManager();
	virtual ~LiveCourseManager();

public:
	virtual bool onRecvLCRole(ELiveCourseRole role);

	virtual bool onRecvCourseStatus(int courseId, string strLCMtgIp);

	virtual bool onCourseWxLeave();

	virtual void OnSpeex(char* spx, int len);
	
public:
	bool init(Kernel *pKernel, Setting* pSetting);

	bool uninit();

	bool addListener(ILiveCourseManagerObserver* pObserver);

	bool removeListener(ILiveCourseManagerObserver* pObserver);

	bool sendData(const char* pPath, const char* pData, int dataLen,
			int dataType, ELiveCourseMediaType lcType);

	ClassWxManager* getClassWxManager() {
		return &m_classWxManager;
	}

	CourseWxManager* getCourseWxManager() {
		return &m_courseWxManager;
	}

	MtgHeartbeat* getMtgHeartbeat() {
		return &m_mtgHeartbeat;
	}
private:
	Kernel* m_pKernel;
	Setting* m_pSetting;

	ClassWxManager m_classWxManager;
	CourseWxManager m_courseWxManager;

	vector<ILiveCourseManagerObserver*> m_vecObservers;

	MtgHeartbeat m_mtgHeartbeat;

	SubVideoManager	m_subVideoManager;
};
