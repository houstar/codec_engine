#pragma once

#include "OMXCommonDef.h"
#include "DecodeUtils.h"
#include "../typedef.h"
#include "mp4Output.h"
#include "constDefines.h"
#include "../KVideo/VideoFactory.h"
#include "../timer.h"
#include "../setting.h"
#include "audioEncodeInterface.h"
#include "rtmpHandle.h"
#include "mtgClient.h"
#include "../KVideo/captureVga.h"
#include "edma.h"
#include "membufUtils.h"


#define KMaxVGABufferCount 40
#define KMaxDecComponentCount (3)
#define KResourceNum 3


#ifdef BUFFER_ALLOCATE_IN_CLIENT
//#define ADDR_ALLOC_IN_HEAP
//#define VGA_ALLOC_IN_HEAP
#endif


//#define AUDIO_CODECENGINE


class Kernel;

class BaseMode {
public:
	BaseMode(Kernel* pKernel);
	virtual ~BaseMode();

public:
	virtual BOOL init(Setting* pSetting);

	virtual BOOL uninit();

	virtual BOOL start();

	virtual BOOL stop();

	virtual BOOL pause();

	virtual BOOL resume();

	virtual BOOL doMovieHead();

	virtual BOOL doMovieTail();

	virtual bool switchScene();

	virtual bool isComponentCreated();

	virtual bool IPCChannelChanged();

	virtual bool doFreezeVGA(bool bFreeze);

public:
	Kernel* m_pKernel;
	bool m_bVGAFreeze;
};
