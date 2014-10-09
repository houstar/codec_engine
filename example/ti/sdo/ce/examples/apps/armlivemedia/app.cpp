/*
 * ======  app.cpp  ======
 */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>

#include <ti/sdo/ce/CERuntime.h>
#include <ti/sdo/ce/Engine.h>

#include "kernel.h"
#include "log.h"
#include <errno.h>
#include <iostream>


#include <signal.h>

#include "./core/OMXComponentUtils.h"
#include "./core/OMXCommonDef.h"
#include "./core/msgq.h"
#include "./utils/commonUtils.h"
#include "config.h"

#include <pthread.h>

using namespace std;
#include <signal.h>




#include <sys/types.h>

#include <event2/event-config.h>

#include <sys/stat.h>
#ifndef WIN32
#include <sys/queue.h>
#include <unistd.h>
#endif
#include <time.h>
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>


Kernel kernel;
pthread_t	gSinalThreadId;
event_base* gEventBase = NULL;

//#define YANGJI_TEST
#ifdef YANGJI_TEST
int gStep = 0;

void sigroutine(int dunno) { /* 信号处理例程，其中dunno将会得到信号的值 */
	switch (dunno) {
	case 1:
	 printf("Get a signal -- SIGHUP ");
	 break;
	case 2:
	printf("Get a signal -- SIGINT ");
	if (gStep == 0) {
		kernel.NotifyGetCommand(FLAG_TOSTREAM_HEAD);
		gStep = 1;
	} else if (gStep == 1) {
		kernel.NotifyGetCommand(FLAG_TOSTREAM_START);
		gStep = 2;
	} else if (gStep == 2) {
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_TEACHER].dwLeft = 0;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_TEACHER].dwLeft = 0;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_TEACHER].dwRight = 768;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_TEACHER].dwRight = 768;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_TEACHER].dwTop = 0;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_TEACHER].dwBottom =   768;

		kernel.getSetting()->m_sceneVideoInfo[SOURCE_STUDENT].dwLeft = 768;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_STUDENT].dwLeft = 768;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_STUDENT].dwRight = 1536;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_STUDENT].dwRight = 1536;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_STUDENT].dwTop = 384;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_STUDENT].dwBottom = 768;

		kernel.getSetting()->m_sceneVideoInfo[SOURCE_DEVICE].dwLeft = 768;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_DEVICE].dwRight = 1536;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_DEVICE].dwTop = 0;
		kernel.getSetting()->m_sceneVideoInfo[SOURCE_DEVICE].dwBottom = 384;
		kernel.NotifyGetCommand(FLAG_TOSTREAM_SCENE);
		gStep = 3;
	} else if (gStep == 3) {
		kernel.NotifyGetCommand(FLAG_TOSTREAM_TAIL);
		gStep = 4;
	} else if (gStep == 4) {
		kernel.NotifyGetCommand(FLAG_TOSTREAM_STOP);
		gStep = 0;
	}
	break;
	case 3:
		printf("Get a signal -- SIGQUIT ");
		break;
	}
	return;
 }

#endif


volatile int gVGAStatus = -1;

static void *singalThreadProc(void *pParam) {
	Kernel* pKernel = (Kernel*) pParam;
	while (1) {
		usleep(40000);
		if (gVGAStatus == VGAStatusChangeNormal) {
			pKernel->onNotifyVGAStateChanged(VGAStatusChangeNormal);
			gVGAStatus = -1;
		} else if (gVGAStatus == VGAStatusException) {
			pKernel->onNotifyVGAStateChanged(VGAStatusException);
			gVGAStatus = -1;
		}
	}
	pthread_exit(NULL);
	return NULL;
}

static void handlerchange(int dunno)
{
	//printf("vga state changed\n");
	//kernel.onNotifyVGAStateChanged(VGAStatusChangeNormal);
	gVGAStatus = VGAStatusChangeNormal;

}

static void handlerVGAException(int dunno) {
	printf("vga status exception\n");
	//kernel.onNotifyVGAStateChanged(VGAStatusException);
	gVGAStatus = VGAStatusException;
}


/* entry funciton */
Int smain()
{	
#ifdef YANGJI_TEST
	signal(SIGINT, sigroutine);
#endif
	printf("begin main\n");
	printf("version:%d.%d, build date %d\n", KMajorVersion, KMinorVersion, KBuildDate);
	char buf[64] = {0};
	sprintf(buf, "echo  %d.%d.%d.%d > /media/sda/version/StreamVersion.txt", KMajorVersion, KMinorVersion, KFunctionVersion, KBuildTime);
	system(buf);

	signal(SIGUSR1, handlerchange);
	signal(SIGUSR2, handlerVGAException);
	printf("singal handlerchange register\n");

	OMX_Init ();
	//ConfigureUIA uiaCfg;
	//uiaCfg.enableAnalysisEvents = 0;
	//uiaCfg.enableStatusLogger = 1;
	//uiaCfg.debugLevel = OMX_DEBUG_LEVEL1;
	//configureUiaLoggerClient(2, &uiaCfg);
	//configureUiaLoggerClient(1, &uiaCfg);

	pthread_create(&gSinalThreadId, NULL, singalThreadProc, &kernel);

	//if (!epoll_init()) {
	//	printf("epoll init failed\n");
	//	return -1;
	//}

	gEventBase = event_base_new();

	if (!kernel.init()) {
		//LOG_ERROR("kernel init failed!");
		return -1;
	}

/*	while (true) {
		if (!epoll_wait()) {
			if (errno != EINTR) {
				break;
			}
		}

		epoll_dispatch();
	}
	printf("epoll uninit\n");
	epoll_uninit();*/


	event_base_dispatch(gEventBase);

	OMX_Deinit();


	return 0;
}
