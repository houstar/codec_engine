#pragma once

#include "baseMode.h"

class RecordModeFactory {
public:

	static BaseMode* createInstance(int mode, Kernel* pKernel, bool bSDIVideo = false);
};
