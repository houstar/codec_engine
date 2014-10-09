#pragma once

#include "mixtureMode_IPC.h"

class InteractionMode_IPC : public MixtureMode_IPC
{
public:
	InteractionMode_IPC(Kernel* pKernel);
	~InteractionMode_IPC();

public:
	virtual bool calcScalerWidthHeight();

protected:

	virtual void onWriteTask(ComponentWrapper *aComponent);
};
