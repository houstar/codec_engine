#pragma once

#include "captureVideoDefines.h"

#include "../core/semp.h"

#include "../core/membufUtils.h"

using namespace std;

//让captureVGA和captureSDI分成两个类是由于SDI的不去考虑缩放的问题了

class CaptureSDI;

class ICaptureSDIListener {
public:
	virtual ~ICaptureSDIListener() {

	}

	virtual void onCaptureSDIBuffer(CaptureSDI* pCaptureSDI, unsigned char* pNV12Buffer, int width, int height) = 0;

	virtual void onNotifySDIFMT(CaptureSDI* pCaptureSDI, int width, int height) = 0;
};


class CaptureSDI {
public:

	static void* CaptureSDITask(void *pParam);
public:
	CaptureSDI(int aIndex);
	~CaptureSDI();

public:

	bool addListener(ICaptureSDIListener* pListener);

	bool removeListener(ICaptureSDIListener* pListener);

	bool init(const char* pDeviceName);

	bool uninit();

	bool start();

	bool stop();

private:

	bool onCaptureTask();

	bool setupCapture();

	bool stopCapture();

	bool startCapture();

	bool queueCaptureBuffers();

private:

	pthread_t	m_threadID;
	pthread_attr_t m_threadAttr;

	vector<ICaptureSDIListener*> m_vecListeners;

	bool 	m_bExit;

	semp_t 	m_exitSem;

	bool 	m_bInited;
	struct capt_obj capt;
	int		m_nSDIIndex;

	static  int m_streamOnCount;
};
