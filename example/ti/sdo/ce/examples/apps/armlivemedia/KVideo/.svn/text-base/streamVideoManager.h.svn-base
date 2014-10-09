#pragma once

#include <vector>
#include "KVideoInterface.h"
#include "../setting.h"


using namespace std;
using namespace KVideo;


class Kernel;

class IStreamVideoManagerListener {
public:
	virtual ~IStreamVideoManagerListener() {}

	virtual bool onNotifyStreamVideo(const char* pData, int len, bool bIFrame, int index, int channel, bool bChannelChanged = false) {
		return true;
	}
};


class StreamVideoManager : public IStreamVideoListener {
public:
	StreamVideoManager();
	virtual ~StreamVideoManager();

public:
	virtual void onH264(KVideoInterface* video, int channel, int frameType,
			void* data, int len);

public:
	bool init(Kernel * pKernel);

	bool uninit();

	bool openVideos();

	bool closeVideos();

	bool addlistener(IStreamVideoManagerListener* plistener);

	bool removelistener(IStreamVideoManagerListener* plistener);

	bool IPCChannelChanged();

	bool getTeacherChannelChanged();

	bool getStudentChannelChanged();

private:
	bool sendListenerStreamVideo(const char* pData, int len, bool bIFrame, int index, int channel, bool bChannelChanged = false);

private:

	Kernel* m_pKernel;
	KVideo::KVideoInterface*	m_pVideoInterface[MAX_IPC];
	vector<IStreamVideoManagerListener*> m_listeners;

	Mutex		 m_IPCChannelMutex;

	PRChannel	 m_IPCChannel;
	PRChannel	 m_OldIPCChannel;

	bool		 m_bTeacherChannelChanged;
	bool		 m_bStudentChannelChanged;
};
