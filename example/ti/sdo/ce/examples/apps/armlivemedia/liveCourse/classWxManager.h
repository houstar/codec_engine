#pragma once

#include "../core/mtgClient.h"
#include "../timer.h"
#include "../setting.h"
#include "livemtgConstDefine.h"
#include "../core/Mutex.h"

#include <string>
#include <vector>
using namespace std;

class Kernel;

class IClassWxObserver {
public:
	virtual ~IClassWxObserver() {

	}

	virtual bool onRecvLCRole(ELiveCourseRole role) {
		return true;
	}

	virtual bool onRecvCourseStatus(int courseId, string strLCMtgIp) {
		return true;
	}
};

class ClassWxManager : public IMtgListener, public ITimerListener {
public:
	ClassWxManager();
	virtual ~ClassWxManager();

public:
	virtual void onRecvMtgData(MtgClient* pMtg, const char* path,
				const char*data, int dataLen, int datatype, int action);

	virtual void onJoinCallback(MtgClient* pMtg, bool bJoinOK);

	virtual void onTimeout(Timer* pTimer);

public:
	bool init(Kernel* pKernel, string strMtgIp, string strRoomId);

	bool uninit();

	bool createPaths();

	bool destroyPaths();

	bool sendData(const char* pPath, const char* pData, int dataLen, int dataType, ELiveCourseMediaType lcType);

	bool procRecvEnterCmd(const char* pData, int len);

	bool procRecvCourseStatusCmd(const char* pData, int len);

	bool procRecvRtmpPathCmd(const char* pData, int len);

	bool addListener(IClassWxObserver* pObserver);

	bool removeListener(IClassWxObserver* pObserver);

private:
	Kernel* m_pKernel;
	Setting* m_pSetting;
	MtgClient* m_pClassClient; //get the command

	Timer	m_timer;
	bool	m_bTimerValid;

	string m_strMtgIp;
	string m_strRoomId;
	int	   m_userId;

	string	m_strEnterPath;
	string  m_strLeavePath;
	string  m_strResponsePath;

	string  m_strClassStatusPath;
	string  m_strSubscribeListPath;
	string	m_strCourseStatusPath;
	string  m_strVideoListPath;
	string 	m_strRtmpPath;

	vector<IClassWxObserver*> m_vecObservers;
	Mutex	m_mutex;
};
