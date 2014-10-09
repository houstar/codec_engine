#include "audioEncodeFactory.h"
#include "audioEncodeOMX.h"


IAudioEncodeInterface* AudioEncodeFactory::createInstance(bool bOMX) {
	if (bOMX) {
		return (new AudioEncodeOMX());
	} else {
		return NULL;
	}
}


