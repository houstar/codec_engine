#pragma once
#include "OMXCommonDef.h"
#include "DecodeUtils.h"
#include "../typedef.h"
#include "constDefines.h"
#include "mp4FileReader.h"


//this class only play mp4 video,do not play audio

class Mp4FilePlayer {
public:
	static void* InputBitStreamReadTask(void *threadsArg);

	static void* ConnInConnOutTask(void *threadsArg);

public:
	static OMX_ERRORTYPE procCallbackEventHandler(OMX_HANDLETYPE hComponent,
			OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1,
			OMX_U32 nData2, OMX_PTR pEventData);

	static OMX_ERRORTYPE procCallbackEmptyBufferDone(OMX_HANDLETYPE hComponent,
			OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer);

	static OMX_ERRORTYPE procCallbackFillBufferDone(OMX_HANDLETYPE hComponent,
			OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer);

public:
	Mp4FilePlayer();
	~Mp4FilePlayer();

public:
	bool init(const char* pFileName);

	bool uninit();

	bool play();

	bool stop();

	bool pause();

	bool resume();

protected:

	void createComponents();

	void destroyComponents();

	void allocInOutParams();

	void freeInOutParams();

	BOOL initOMXHandle();

	BOOL establishConnections();

	void onReadTask();

	void onConnectTask(ComponentWrapper *aComponent);

	bool initialInputResources(int aInputIndex);

	OMX_U32 fillData(OMX_BUFFERHEADERTYPE *pBuf);

	OMX_U32 getFrameSize();

	bool startOMXThread();

	BOOL initDecOMXHandle();

	BOOL initScaleOMXHandle();

	BOOL initDisplayOMXHandle();

	bool setDecodeParams();

	bool setScalarParams();

	bool setDisplayParams();

	bool freeDecBuffers();

	bool freeScaleBuffers();

	bool freeDisplayBuffers();

protected:
	OMX_HANDLETYPE m_pDecHandle;
	OMX_HANDLETYPE m_pScaleHandle;
	OMX_HANDLETYPE m_pDisplayHandle;
	OMX_HANDLETYPE m_pctrlHandle;

	OMX_CALLBACKTYPE m_pCb;
	OMX_U32 m_inWidth;
	OMX_U32 m_inHeight;
	OMX_U32 m_nDecStride;

	OMX_U32 m_nFrameRate;
	OMX_U32 m_displayId;
	H264_ParsingCtx m_pc;
	ComponentWrapper *m_decComponent;
	ComponentWrapper *m_scaleComponent;
	ComponentWrapper *m_displayComponent;
	stInputThreadArgs m_inputThreadArgs;

	stConnInConnOutThreadArgs m_displayParams;
	stConnInConnOutThreadArgs m_decParams;
	stConnInConnOutThreadArgs m_scaleParams;
	stConnInConnOutThreadArgs m_vgaScaleParams;
	bool 	m_bExit;

	CMP4FileReader* m_mp4Reader;

	unsigned char* m_pInBuffer;
	int 			m_nInBufferLen;
	int				m_nInBufferMaxLen;

	bool 			m_bGetHeader;
	int				m_videoSampleId;
	int				m_videoSampleCount;
};
