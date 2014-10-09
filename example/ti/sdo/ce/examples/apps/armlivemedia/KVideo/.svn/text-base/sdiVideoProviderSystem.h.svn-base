#pragma once
#include "captureSDIVideo.h"
#include "../timer.h"
#include <vector>

using namespace std;

class ISDIVideoProviderObserver {
public:
	virtual ~ISDIVideoProviderObserver() {

	}

	virtual void notifySDIProviderBuffer(int aIndex, unsigned char* pYUYVBuffer, int width, int height) = 0;
};

class SDIVideoProviderSystem : public ICaptureSDIListener, public ITimerListener {
public:

	SDIVideoProviderSystem();

	virtual ~SDIVideoProviderSystem();

public:
	virtual void onCaptureSDIBuffer(CaptureSDI* pCaptureSDI, unsigned char* pYUYVBuffer, int width, int height);

	virtual void onNotifySDIFMT(CaptureSDI* pCaptureSDI, int width, int height);

	virtual void onTimeout(Timer *timer);
public:

	bool init();

	bool uninit();

	bool open();

	bool close();

	bool addListener(ISDIVideoProviderObserver* pListener);

	bool removeListener(ISDIVideoProviderObserver* pListener);

private:
	CaptureSDI* m_pCaptureSDI[2];
	Timer 		m_captureSDITimer[2];
	bool		m_captureSDITimerValid[2];

	vector<ISDIVideoProviderObserver*> m_vecListeners;
};
