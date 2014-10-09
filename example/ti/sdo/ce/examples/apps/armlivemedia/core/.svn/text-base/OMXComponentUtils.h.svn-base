#pragma once


#include "OMXCommonDef.h"


class OMXComponentUtils {
public:

	/**
	 * \brief : This util function is to update the pipe
	 *        information of other connected comonnet, so that
	 *         buffers can be passed to connected component.
	 *
	 * @param handleCompPrivA   : application component data structure for producer
	 * @param compAPortOut      : port of producer comp
	 * @param handleCompPrivB   : application component data structure for consumer
	 * @param compBPortIn       : port number of the consumer component
	 *
	 *  @return
	 *  String conversion of the OMX_ERRORTYPE
	 *
	 */
	static OMX_ERRORTYPE connectComponents(
			ComponentWrapper *handleCompPrivA, unsigned int compAPortOut,
			ComponentWrapper *handleCompPrivB, unsigned int compBPortIn);


	/**
	 * \brief : This util function is to get buffer header
	 *                                   specific to one component, from the buffer
	 *                                   received from other component  .
	 *
	 * @param thisComp   : application component data structure
	 * @param pBuffer    : OMX buffer pointer
	 * @param type       : it is to identfy teh port type
	 * @param portIndex  : port number of the component
	 * @param pBufferOut : components buffer header correponding to pBuffer
	 *
	 *  @return
	 *  String conversion of the OMX_ERRORTYPE
	 *
	 */
	static OMX_ERRORTYPE getSelfBufHeader(ComponentWrapper *thisComp,
			OMX_U8 *pBuffer, EPortType type, OMX_U32 portIndex,
			OMX_BUFFERHEADERTYPE **pBufferOut);

	static void initComponent(ComponentWrapper* &aComponent);

	static void uninitComponent(ComponentWrapper* &aComponent);

	static void allocInOutParams(ComponentWrapper* &aComponent);

	static void freeInOutParams(ComponentWrapper* &aComponent);

	static OMX_ERRORTYPE initialOutputResources(ComponentWrapper* &aComponent);

	static OMX_ERRORTYPE procPipeCmdEmptyThisBuffer(ComponentWrapper *thisComp,
			PipeMsg *pipeMsg);

	static OMX_ERRORTYPE procPipeCmdFillThisBuffer(ComponentWrapper *thisComp,
			PipeMsg *pipeMsg);

	static OMX_ERRORTYPE procPipeCmdEmptyBufferDone(ComponentWrapper *thisComp,
			PipeMsg *pipeMsg);

	static OMX_ERRORTYPE procPipeCmdFillBufferDone(ComponentWrapper *thisComp,
			PipeMsg *pipeMsg);

	static bool isGetStateSem(ComponentWrapper* &aComponent);

	static void postStateSem(ComponentWrapper* &aComponent);

	static void waitStateSem(ComponentWrapper* &aComponent);

	static bool isGetPortSem(ComponentWrapper* &aComponent);

	static void postPortSem(ComponentWrapper* &aComponent);

	static void waitPortSem(ComponentWrapper* &aComponent);

	static bool changeComponentState(ComponentWrapper* &aComponent, OMX_STATETYPE newState);
};
