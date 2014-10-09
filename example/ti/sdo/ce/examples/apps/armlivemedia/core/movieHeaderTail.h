#pragma once
#include "OMXCommonDef.h"
#include "DecodeUtils.h"
#include "../typedef.h"
#include "constDefines.h"
#include "../setting.h"

class IMovieHeaderTailStreamListener  {
public:
	virtual ~IMovieHeaderTailStreamListener() {

	}
	virtual void onNotifyOutStream(unsigned char* pH264Buf, int size) {

	}
};

class MovieHeaderTail  {
public:
	static void* OutputBitStreamWriteTask(void *threadsArg);

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
	MovieHeaderTail();
	~MovieHeaderTail();

public:

	bool init(int inWidth, int inHeight,
			int frameRate, int videoBitrate,
			int outWidth, int outHeight,
			int displayId);

	bool uninit();

	void setListener(IMovieHeaderTailStreamListener* pListener);

	BOOL start();

	BOOL stop();

	BOOL pause();

	BOOL resume();

	BOOL putRGB32Buf(char* pRGB32Buf);

private:

	void createComponents();

	void destroyComponents();

	void allocInOutParams();

	void freeInOutParams();

	BOOL initOMXHandle();

	BOOL initDeiOMXHandle();

	BOOL initEncodeOMXHandle();

	BOOL initDisplayOMXHandle();

	bool setDeiParams();

	bool setEncodeParams();

	bool setDisplayParams();

	BOOL establishConnections();

	OMX_U32 fillData(OMX_BUFFERHEADERTYPE *pBuf);

	bool initialInputResources(int aInputIndex);

	bool freeDeiBuffers();

	bool freeEncBuffers();

	bool freeDisplayBuffers();




public:

	void onWriteTask();

	void onReadTask();

	void onConnectTask(ComponentWrapper *aComponent);

private:
	IMovieHeaderTailStreamListener* m_pListener;

	OMX_HANDLETYPE m_pDisplayHandle;
	OMX_HANDLETYPE m_pctrlHandle;
	OMX_HANDLETYPE m_pEncHandle;
	OMX_HANDLETYPE m_pDeiHandle;
	OMX_CALLBACKTYPE m_pCb;

	OMX_U32 m_nFrameRate;
	OMX_U32 m_displayId;
	OMX_U32 m_nEncBitRate;

	int		m_inWidth;
	int 	m_inHeight;
	int		m_outWidth;
	int		m_outHeight;

	ComponentWrapper *m_displayComponent;
	ComponentWrapper *m_encComponent;
	ComponentWrapper *m_deiComponent;

	stConnInConnOutThreadArgs m_deiParams;
	stConnInConnOutThreadArgs m_encParams;
	stConnInConnOutThreadArgs m_displayParams;
	BOOL m_bExit;

	unsigned char* m_pInImage;

	struct timeval m_tv;
	long long m_afttime;
	long long m_beftime;
};
