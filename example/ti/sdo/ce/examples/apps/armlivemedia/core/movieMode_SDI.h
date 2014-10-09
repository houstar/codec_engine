#pragma once

#include "mixtureMode_SDI.h"


class MovieMode_SDI : public MixtureMode_SDI {

public:
	MovieMode_SDI(Kernel* pKernel);
	~MovieMode_SDI();

protected:

	virtual bool calcScalerWidthHeight();
};
