#pragma once
#include "v4lDisplay.h"
#include "../setting.h"
#include "Mutex.h"
#include "../liveCourse/liveCourseManager.h"
#include "../timer.h"


class HDMIDisplayManager : public ITimerListener {
public:
	HDMIDisplayManager();
	virtual ~HDMIDisplayManager();

	enum EDisplayContext {
		eDisplayNone,
		eDisplayVGA,
		eDisplayRecord
	};

public:
	virtual bool setLiveCourseRole(ELiveCourseRole role);

	virtual void onTimeout(Timer* pTimer);

public:
	bool init(Setting* pSetting);

	bool setDisplayContext(EDisplayContext context);

	bool setDisplayResolution(int width, int height);

	bool uninit(EDisplayContext context);

	bool doDisplay(EDisplayContext context, const unsigned char* pBuffer);

	bool changeDisplayResution(int width, int height);

private:

	Setting* m_pSetting;
	DisplayHandle m_displayHandle;
	EDisplayContext m_displayContext;
	int	m_displayWidth;
	int m_displayHeight;
	bool m_bInited;
	Mutex m_mutex;

	int m_lastDisplayWidth;
	int m_lastDisplayHeight;

	Timer m_timer;
	bool m_bTimerValid;
};
