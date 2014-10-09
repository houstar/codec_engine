#include "OMXComponentUtils.h"
#include "timm_osal_trace.h"
#include "timm_osal_interfaces.h"
#include <string.h>

#include "../kernel.h"
#include "../constStringDefine.h"
extern Kernel kernel;

OMX_ERRORTYPE OMXComponentUtils::connectComponents(
		ComponentWrapper *handleCompPrivA, unsigned int compAPortOut,
		ComponentWrapper *handleCompPrivB, unsigned int compBPortIn) {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OutportParams *outPortParamPtr = NULL;
	InportParams *inPortParamPtr = NULL;

	/* update the input port connect structure */
	outPortParamPtr = handleCompPrivA->outPortParams + compAPortOut
			- handleCompPrivA->startOutportIndex;

	inPortParamPtr = handleCompPrivB->inPortParams + compBPortIn;

	/* update input port component pipe info with connected port */
	inPortParamPtr->connInfo.remoteClient = handleCompPrivA;
	inPortParamPtr->connInfo.remotePort = compAPortOut;
	inPortParamPtr->connInfo.remotePipe[0] = handleCompPrivA->localPipe[0];
	inPortParamPtr->connInfo.remotePipe[1] = handleCompPrivA->localPipe[1];

	/* update output port component pipe info with connected port */
	outPortParamPtr->connInfo.remoteClient = handleCompPrivB;
	outPortParamPtr->connInfo.remotePort = compBPortIn;
	outPortParamPtr->connInfo.remotePipe[0] = handleCompPrivB->localPipe[0];
	outPortParamPtr->connInfo.remotePipe[1] = handleCompPrivB->localPipe[1];

	return eError;
}

OMX_ERRORTYPE OMXComponentUtils::getSelfBufHeader(ComponentWrapper *thisComp,
		OMX_U8 *pBuffer, EPortType type, OMX_U32 portIndex,
		OMX_BUFFERHEADERTYPE **pBufferOut) {
	OMX_U32 i;
	InportParams *inPortParamsPtr;
	OutportParams *outPortParamsPtr;
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	/* Check for input port buffer header queue */
	if (type == eInputPort) {
		inPortParamsPtr = thisComp->inPortParams + portIndex;
		for (i = 0; i < inPortParamsPtr->nBufferCountActual; i++) {
			if (pBuffer == inPortParamsPtr->pInBuff[i]->pBuffer) {
				*pBufferOut = inPortParamsPtr->pInBuff[i];
				break;
			}
		}
	}
	/* Check for output port buffer header queue */
	else {
		outPortParamsPtr = thisComp->outPortParams + portIndex
				- thisComp->startOutportIndex;
		for (i = 0; i < outPortParamsPtr->nBufferCountActual; i++) {
			if (pBuffer == outPortParamsPtr->pOutBuff[i]->pBuffer) {
				*pBufferOut = outPortParamsPtr->pOutBuff[i];
				break;
			}
		}
	}

	return (eError);
}

void OMXComponentUtils::initComponent(ComponentWrapper* &aComponent) {
	aComponent = (ComponentWrapper *) malloc(sizeof(ComponentWrapper));
	memset(aComponent, 0x0, sizeof(ComponentWrapper));

	sem_init(&aComponent->state_sem, 0, 0);

	sem_init(&aComponent->port_sem, 0, 0);

	aComponent->eos_sem = (semp_t*)malloc(sizeof(semp_t));
	SempUtils::semp_init(aComponent->eos_sem, 0);
}

void OMXComponentUtils::uninitComponent(ComponentWrapper* &aComponent) {
	sem_destroy(&aComponent->state_sem);

	sem_destroy(&aComponent->port_sem);

	SempUtils::semp_deinit(aComponent->eos_sem);

	pthread_attr_destroy(&aComponent->ThreadAttr);
	free(aComponent->eos_sem);

	free(aComponent);
	aComponent = NULL;
}

void OMXComponentUtils::allocInOutParams(ComponentWrapper* &aComponent) {
	if (aComponent->numInport > 0) {
		aComponent->inPortParams = (InportParams*) malloc(
				sizeof(InportParams) * aComponent->numInport);
		memset(aComponent->inPortParams, 0x0, sizeof(InportParams));
	}

	if (aComponent->numOutport) {
		aComponent->outPortParams = (OutportParams*) malloc(
				sizeof(OutportParams) * aComponent->numOutport);
		memset(aComponent->outPortParams, 0x0, sizeof(OutportParams));
	}
}

void OMXComponentUtils::freeInOutParams(ComponentWrapper* &aComponent) {
	InportParams *inPortParamsPtr;
	OutportParams *outPortParamsPtr;

	for (OMX_U32 i = 0; i < aComponent->numInport; ++i) {
		inPortParamsPtr = aComponent->inPortParams + i;
		close((int) inPortParamsPtr->ipBufPipe[0]);
		close((int) inPortParamsPtr->ipBufPipe[1]);

	}
	for (OMX_U32 i = 0; i < aComponent->numOutport; ++i) {
		outPortParamsPtr = aComponent->outPortParams + i;
		close((int) outPortParamsPtr->opBufPipe[0]);
		close((int) outPortParamsPtr->opBufPipe[1]);
	}
	close((int) aComponent->localPipe[0]);
	close((int) aComponent->localPipe[1]);

	if (aComponent->inPortParams != NULL) {
		free(aComponent->inPortParams);
		aComponent->inPortParams = NULL;
	}
	if (aComponent->outPortParams != NULL) {
		free(aComponent->outPortParams);
		aComponent->outPortParams = NULL;
	}
}

OMX_ERRORTYPE OMXComponentUtils::initialOutputResources(
		ComponentWrapper* &pComponent) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	unsigned int i = 0, j;
	OutportParams *outPortParamPtr = NULL;
	OMX_PARAM_PORTDEFINITIONTYPE param;

	memset(&param, 0, sizeof(param));

	OMX_INIT_PARAM(&param);

	/* Give output buffers to component which is limited by no of output buffers
	 available. Rest of the data will be written on the callback from output
	 data write thread */
	for (j = 0; j < pComponent->numOutport; j++) {
		param.nPortIndex = j + pComponent->startOutportIndex;

		OMX_GetParameter(pComponent->handle, OMX_IndexParamPortDefinition,
				&param);

		outPortParamPtr = pComponent->outPortParams + j;
		if (OMX_TRUE == param.bEnabled) {
			if (outPortParamPtr->connInfo.remotePipe != NULL) {

				for (i = 0; i < pComponent->outPortParams->nBufferCountActual;
						i++) {
					/* Pass the output buffer to the component */
					err =
							OMX_FillThisBuffer (pComponent->handle, outPortParamPtr->pOutBuff[i]);

				} /* for (i) */
			} /* if (outPortParamPtr->...) */
		} /* if (OMX_TRUE...) */
	} /* for (j) */

	return err;
}

OMX_ERRORTYPE OMXComponentUtils::procPipeCmdEmptyThisBuffer(ComponentWrapper *thisComp,
		PipeMsg *pipeMsg) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn;

	/* search its own buffer header based on submitted by connected comp */
	if (OMX_ErrorNone != getSelfBufHeader(thisComp, pipeMsg->bufHeader.pBuffer,
			eInputPort, pipeMsg->bufHeader.nInputPortIndex, &pBufferIn) ) {
		return OMX_ErrorUndefined;
	}

	/* populate buffer header */
	pBufferIn->nFilledLen = pipeMsg->bufHeader.nFilledLen;
	pBufferIn->nOffset = pipeMsg->bufHeader.nOffset;
	pBufferIn->nTimeStamp = pipeMsg->bufHeader.nTimeStamp;
	pBufferIn->nFlags = pipeMsg->bufHeader.nFlags;
	pBufferIn->hMarkTargetComponent = pipeMsg->bufHeader.hMarkTargetComponent;
	pBufferIn->pMarkData = pipeMsg->bufHeader.pMarkData;
	pBufferIn->nTickCount = 0;

	/* call etb to the component */
	err = OMX_EmptyThisBuffer (thisComp->handle, pBufferIn);
	return (err);
}

OMX_ERRORTYPE OMXComponentUtils::procPipeCmdFillThisBuffer(ComponentWrapper *thisComp,
		PipeMsg *pipeMsg) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut;

	/* search its own buffer header based on submitted by connected comp */
	if (OMX_ErrorNone != getSelfBufHeader(thisComp, pipeMsg->bufHeader.pBuffer,
			eOutputPort, pipeMsg->bufHeader.nOutputPortIndex, &pBufferOut) ) {
		return OMX_ErrorUndefined;
	}

	/* call etb to the component */
	err = OMX_FillThisBuffer (thisComp->handle, pBufferOut);

	return (err);
}

OMX_ERRORTYPE OMXComponentUtils::procPipeCmdEmptyBufferDone(ComponentWrapper *thisComp,
		PipeMsg *pipeMsg) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferIn;
	PipeMsg remotePipeMsg;
	InportParams *inPortParamsPtr;
	int retVal = 0;

	pBufferIn = pipeMsg->pbufHeader;

	/* find the input port structure (pipe) */
	inPortParamsPtr = thisComp->inPortParams + pBufferIn->nInputPortIndex;

	remotePipeMsg.cmd = ePipeCmdFillThisBuffer;
	remotePipeMsg.bufHeader.pBuffer = pBufferIn->pBuffer;
	remotePipeMsg.bufHeader.nOutputPortIndex =
			inPortParamsPtr->connInfo.remotePort;

	/* write the fill buffer message to remote pipe */
	retVal = write(inPortParamsPtr->connInfo.remotePipe[1], &remotePipeMsg,
			sizeof(PipeMsg));

	if (sizeof(PipeMsg) != retVal) {
		printf("Error writing to remote Pipe!\n");
		err = OMX_ErrorNotReady;
		return err;
	}

	return (err);
}

OMX_ERRORTYPE OMXComponentUtils::procPipeCmdFillBufferDone(ComponentWrapper *thisComp,
		PipeMsg *pipeMsg) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut;
	PipeMsg remotePipeMsg;
	OutportParams *outPortParamsPtr;
	int retVal = 0;
	pBufferOut = pipeMsg->pbufHeader;

	remotePipeMsg.cmd = ePipeCmdEmptyThisBuffer;
	remotePipeMsg.bufHeader.pBuffer = pBufferOut->pBuffer;

	outPortParamsPtr = thisComp->outPortParams
			+ (pBufferOut->nOutputPortIndex - thisComp->startOutportIndex);

	/* populate buffer header */
	remotePipeMsg.bufHeader.nFilledLen = pBufferOut->nFilledLen;
	remotePipeMsg.bufHeader.nOffset = pBufferOut->nOffset;
	remotePipeMsg.bufHeader.nTimeStamp = pBufferOut->nTimeStamp;
	remotePipeMsg.bufHeader.nFlags = pBufferOut->nFlags;
	remotePipeMsg.bufHeader.hMarkTargetComponent =
			pBufferOut->hMarkTargetComponent;
	remotePipeMsg.bufHeader.pMarkData = pBufferOut->pMarkData;
	remotePipeMsg.bufHeader.nTickCount = pBufferOut->nTickCount;
	remotePipeMsg.bufHeader.nInputPortIndex =
			outPortParamsPtr->connInfo.remotePort;

	/* write the fill buffer message to remote pipe */
	retVal = write(outPortParamsPtr->connInfo.remotePipe[1], &remotePipeMsg,
			sizeof(PipeMsg));

	if (sizeof(PipeMsg) != retVal) {
		printf("Error writing to remote Pipe!\n");
		err = OMX_ErrorNotReady;
		return err;
	}

	return (err);
}


bool OMXComponentUtils::isGetStateSem(ComponentWrapper* &aComponent) {
	if (aComponent->bGetStateSem) {
		aComponent->bGetStateSem = false;
		return true;
	}
	return false;
}

void OMXComponentUtils::postStateSem(ComponentWrapper* &aComponent) {
	aComponent->bGetStateSem = true;
	sem_post(&aComponent->state_sem);
}

void OMXComponentUtils::waitStateSem(ComponentWrapper* &aComponent) {
	timespec tv;
	tv.tv_sec = time(NULL)+10;
	tv.tv_nsec = 0;

	sem_timedwait(&aComponent->state_sem, &tv);

	if (!aComponent->bGetStateSem) {
		printf("can not get state sem\n");
		kernel.NotifyAssistMessage(gStrRecordFailed);
		system("reboot -f");
	}
}

bool OMXComponentUtils::isGetPortSem(ComponentWrapper* &aComponent) {
	if (aComponent->bGetPortSem) {
		aComponent->bGetPortSem = false;
		return true;
	}
	return false;
}

void OMXComponentUtils::postPortSem(ComponentWrapper* &aComponent) {
	aComponent->bGetPortSem = true;
	sem_post(&aComponent->port_sem);
}

void OMXComponentUtils::waitPortSem(ComponentWrapper* &aComponent) {
	timespec tv;
	tv.tv_sec = time(NULL)+10;
	tv.tv_nsec = 0;

	sem_timedwait(&aComponent->port_sem, &tv);
	if (!aComponent->bGetPortSem) {
		printf("can not get port sem\n");
		system("reboot -f");
	}
}

bool OMXComponentUtils::changeComponentState(ComponentWrapper* &aComponent, OMX_STATETYPE newState) {
	OMX_ERRORTYPE eError = OMX_SendCommand (aComponent->handle, OMX_CommandStateSet, newState, NULL);
	if (eError != OMX_ErrorNone) {
		printf("change state error name %s, %x\n", aComponent->name, eError);
		return false;
	}
	return true;
}
