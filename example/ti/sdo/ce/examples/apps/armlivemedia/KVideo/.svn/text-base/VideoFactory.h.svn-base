#ifndef VIDEO_FACTORY_H
#define VIDEO_FACTORY_H

#include "KVideoInterface.h"
#include <string>

using namespace KVideo;
using namespace std;

class KVideoCreatorFactory{
public:
	static KVideoInterface* createInstance(const string& description);

	static void destroyInstance(KVideoInterface*& pVideo);
private:
	static int m_nInstanceCount;
};

#endif
