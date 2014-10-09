/*
 * CaptureVga.h
 * this file define a class CaptureVGA which use V4L2
 */

#pragma once

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <linux/videodev2.h>
#include <linux/fb.h>
#include <linux/ti81xxfb.h>
#include <linux/ti81xxhdmi.h>
#include <linux/ti81xxvin.h>
#include <vector>
#include <pthread.h>
#include "../core/semp.h"

#include "../core/membufUtils.h"

#include "captureVideoDefines.h"

using namespace std;

class CaptureVGA;

class ICaptureVGAListener {
public:
	virtual ~ICaptureVGAListener() {

	}

	virtual void onCaptureVGABuffer(CaptureVGA* pHandle, unsigned char* pNV12Buffer, int width, int height) = 0;

	virtual void onNotifyVGAFMT(CaptureVGA* pHandle, int width, int height) = 0;
};


class CaptureVGA {
public:

	static void* CaptureVGATask(void *pParam);
public:
	CaptureVGA();
	~CaptureVGA();

public:

	bool addListener(ICaptureVGAListener* pListener);

	bool removeListener(ICaptureVGAListener* pListener);

	bool init();

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

	vector<ICaptureVGAListener*> m_vecListeners;

	bool 	m_bExit;

	semp_t 	m_exitSem;

	bool 	m_bInited;
	struct capt_obj capt;
};


