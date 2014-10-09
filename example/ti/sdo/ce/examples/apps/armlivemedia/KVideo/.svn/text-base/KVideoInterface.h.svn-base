#ifndef KVIDEO_INTERFACE_H
#define KVIDEO_INTERFACE_H

#include "CardInfo.h"

namespace KVideo {
class KVideoInterface;

class IStreamVideoListener {
public:
	virtual ~IStreamVideoListener() {

	}

	virtual void onH264(KVideoInterface* video, int channel, int frameType,
			void* data, int len) {
	}

	virtual void onAAC(KVideoInterface* video, void* data, int len) {
	}

	virtual void notifyHeartbeatTimerout(KVideoInterface* video) {
	}
};

class KVideoInterface {
public:
	virtual ~KVideoInterface() {
	}

	virtual KVideo::CardInfo* getCardInfo() = 0;

	virtual int getChannelID() = 0;

	virtual bool setChannelID(int channelID) = 0;

	virtual bool addListener(KVideo::IStreamVideoListener* listener) = 0;

	virtual bool open(bool aSubStream = false) = 0;

	virtual bool close() = 0;

	virtual bool isOpen() = 0;

};
}

#endif
