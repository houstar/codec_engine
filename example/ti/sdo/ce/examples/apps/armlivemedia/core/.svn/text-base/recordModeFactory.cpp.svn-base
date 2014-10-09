#include "recordModeFactory.h"
#include "../setting.h"
#include "microMode_IPC.h"
#include "microMode_SDI.h"
#include "movieMode_IPC.h"
#include "movieMode_SDI.h"
#include "dualScreenMode_IPC.h"
#include "dualScreenMode_SDI.h"
#include "interactionMode_IPC.h"
#include "interactionMode_SDI.h"
#include "resourceMode_IPC.h"
#include "resourceMode_SDI.h"

BaseMode* RecordModeFactory::createInstance(int mode, Kernel* pKernel, bool bSDIVideo) {
	BaseMode* pMode = NULL;
	if (eMicroMode == mode) {
		if (bSDIVideo) {
			pMode = new MicroMode_SDI(pKernel);
		} else {
			pMode = new MicroMode_IPC(pKernel);
		}
	} else if (eMovieMode == mode) {
		if (bSDIVideo) {
			pMode = new MovieMode_SDI(pKernel);
		} else {
			pMode = new MovieMode_IPC(pKernel);
		}
	} else if (eInterMode == mode) {
		if (bSDIVideo) {
			pMode = new InteractionMode_SDI(pKernel);
		} else {
			pMode = new InteractionMode_IPC(pKernel);
		}
	} else if (eDualScreenMode == mode) {
		if (bSDIVideo) {
			pMode = new DualScreenMode_SDI(pKernel);
		} else {
			pMode = new DualScreenMode_IPC(pKernel);
		}
	} else if (eResourceMode == mode || eThirdScreenMode == mode) {
		if (bSDIVideo) {
			pMode = new ResourceModeHandle_SDI(pKernel);
		} else {
			pMode = new ResourceModeHandle_IPC(pKernel);
		}
	}
	return pMode;
}
