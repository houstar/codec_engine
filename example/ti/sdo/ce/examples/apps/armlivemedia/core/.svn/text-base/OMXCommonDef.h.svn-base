#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <xdc/std.h>
#include <memory.h>
#include <getopt.h>
#include <semaphore.h>

#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/IHeap.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/sdo/codecs/aaclcenc/imp4aacenc.h>

/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "ti/omx/interfaces/openMaxv11/OMX_TI_Common.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Video.h"
#include "ti/omx/interfaces/openMaxv11/OMX_TI_Video.h"
#include "omx_venc.h"
#include "omx_vfpc.h"
#include "omx_vfdc.h"
#include "omx_ctrl.h"
#include "omx_vfcc.h"
#include "omx_vdec.h"
#include "omx_vswmosaic.h"
#include "omx_aenc.h"
#include "ti/omx/interfaces/openMaxv11/OMX_TI_Index.h"
#include "semp.h"
#include "OMX_IVCommon.h"
#include <xdc/runtime/knl/Thread.h>



#include "constDefines.h"

#define OMX_TEST_INIT_STRUCT_PTR(_s_, _name_)                                  \
          memset((_s_), 0x0, sizeof(_name_));                                  \
          (_s_)->nSize = sizeof(_name_);                                       \
          (_s_)->nVersion.s.nVersionMajor = 0x1;                               \
          (_s_)->nVersion.s.nVersionMinor = 0x1;                               \
          (_s_)->nVersion.s.nRevision  = 0x0;                                  \
          (_s_)->nVersion.s.nStep   = 0x0;

#define OMX_INIT_PARAM(param)                                                  \
        {                                                                      \
          memset ((param), 0, sizeof (*(param)));                              \
          (param)->nSize = sizeof (*(param));                                  \
          (param)->nVersion.s.nVersionMajor = 1;                               \
          (param)->nVersion.s.nVersionMinor = 1;                               \
        }

#define UTIL_ALIGN(a,b)  ((((uint32_t)(a)) + (b)-1) & (~((uint32_t)((b)-1))))




typedef struct tagComponentWrapper tagComponentWrapper;

//SNT: OpenMax Standard Non-Tunnel
typedef struct tagSNTConnectInfo
{
  OMX_U32 remotePort;
  OMX_S32 remotePipe[2];    /* for making ETB / FTB calls to connected comp */
  tagComponentWrapper *remoteClient;       /* For allocate / use buffer */
} SNTConnectInfo;

typedef struct tagInportParams
{
  SNTConnectInfo connInfo;
  OMX_S32 ipBufPipe[2];       /* input pipe */
  OMX_BUFFERHEADERTYPE *pInBuff[MAX_NUM_OUT_BUFS];
  OMX_U32 nBufferCountActual;
  OMX_U32 nBufferSize;
} InportParams;

typedef struct tagOutportParams
{
  SNTConnectInfo connInfo;
  OMX_S32 opBufPipe[2];       /* output pipe */
  OMX_BUFFERHEADERTYPE *pOutBuff[MAX_NUM_OUT_BUFS];
  OMX_U32 nBufferCountActual;
  OMX_U32 nBufferSize;
} OutportParams;

typedef struct tagComponentWrapper
{
  InportParams *inPortParams;
  OutportParams *outPortParams;    /* Common o/p port params */
  OMX_U32 numInport;
  OMX_U32 numOutport;
  OMX_U32 startOutportIndex;
  OMX_S32 localPipe[2];
  OMX_HANDLETYPE handle;
  pthread_t inDataStrmThrdId;
  pthread_t outDataStrmThrdId;
  pthread_t connDataStrmThrdId;
  pthread_t outDataThreadId[2];
  sem_t state_sem;
  bool	bGetStateSem;
  sem_t port_sem;
  bool	bGetPortSem;
  semp_t *eos_sem;
  pthread_attr_t ThreadAttr;
  char name[16];
  void*	pParent;
} ComponentWrapper;

typedef struct tagPipeMsg
{
	EPipeCmdType cmd;
	OMX_BUFFERHEADERTYPE *pbufHeader;   /* used for EBD/FBD */
	OMX_BUFFERHEADERTYPE bufHeader;     /* used for ETB/FTB */
} PipeMsg;


typedef struct tagInputThreadArgs {
	void *ptrAppData;
	int instId;
} stInputThreadArgs;

typedef struct tagConnInConnOutThreadArgs {
	void *pParent;
	ComponentWrapper *pComponent;
} stConnInConnOutThreadArgs;
