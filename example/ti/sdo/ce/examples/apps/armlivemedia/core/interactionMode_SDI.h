#pragma once

#include "mixtureMode_SDI.h"

class InteractionMode_SDI : public MixtureMode_SDI {
public:

	InteractionMode_SDI(Kernel* pKernel);
	~InteractionMode_SDI();

public:
	virtual bool calcScalerWidthHeight();

protected:

	virtual void onSDIScaleChannel1(ComponentWrapper* aComponent);

	virtual void onSDIScaleChannel2(ComponentWrapper* aComponent);
};
