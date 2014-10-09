#pragma once

#include "../KVideo/KVideoInterface.h"
#include "../setting.h"

using namespace KVideo;

class Kernel;

class SubVideoManager : public IStreamVideoListener{
public:
	SubVideoManager();
	virtual ~SubVideoManager();

public:

	virtual void onH264(KVideoInterface* video, int channel, int frameType,
			void* data, int len);
public:
	bool init(Kernel * pKernel);

	bool uninit();

	bool openSubVideos();

	bool closeSubVideos();

private:
	Kernel* m_pKernel;
	KVideoInterface* m_pTeacherSubInterface;
	KVideoInterface* m_pStudentSubInterface;

	PRChannel	 m_IPCChannel;
};
