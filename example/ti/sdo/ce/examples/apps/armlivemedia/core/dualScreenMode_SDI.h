#pragma once

#include "mixtureMode_SDI.h"


class DualScreenMode_SDI : public MixtureMode_SDI
{
public:
	DualScreenMode_SDI(Kernel *pKernel);
	virtual ~DualScreenMode_SDI();
};
