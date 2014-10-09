#pragma once
#include "../setting.h"
#include "rtmpHandle.h"

class RtmpManager {
public:
	RtmpManager();

	virtual ~RtmpManager();

public:
	bool init(Setting* pSetting);

	bool setLCLive(bool bLCLive);

	bool uninit();

	bool startRtmp();

	bool stopRtmp();

	bool pushAudio(const char* pAACBuf, int len);

	bool pushVideo(const char* pH264Buf, int len);

private:
	Setting* m_pSetting;
	RtmpHandle* m_rtmpHandle;
	bool m_bLCLive;
};
