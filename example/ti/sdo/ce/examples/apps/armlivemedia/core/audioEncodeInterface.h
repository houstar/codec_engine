#pragma once
#include <string>
#include "../config.h"
#include "../KVideo/audioCapture.h"

using namespace std;

class IAudioEncodeListener {
public:
	virtual ~IAudioEncodeListener() {

	}

	virtual void onAACEncoded(unsigned char* pAACBuf, int len) {

	}
};

class IAudioEncodeInterface : public IAudioCaptureObserver {
public:
	virtual ~IAudioEncodeInterface() {

	}

public:
	virtual bool init(int aAudioChannel, int aSampleRate,
			int aSampleBit, int aacBitRate, bool bPCMNonBlock, string strDeviceName) = 0;

	virtual bool uninit() = 0;

	virtual bool start() = 0;

	virtual bool stop() = 0;

	virtual bool addListener(IAudioEncodeListener* pListener) = 0;

	virtual bool removeListener(IAudioEncodeListener* pListener) = 0;

	virtual bool onProviderPcm(const char* pBuffer, int len) {
		return true;
	}
};
