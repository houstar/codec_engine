#pragma once

#include "mixtureMode_IPC.h"


class MicroMode_IPC : public MixtureMode_IPC
{
public:
	MicroMode_IPC(Kernel *pKernel);
	virtual ~MicroMode_IPC();
};
