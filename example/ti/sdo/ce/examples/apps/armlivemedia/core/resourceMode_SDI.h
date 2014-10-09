#pragma once
#include "baseMode.h"
#include "../KVideo/sdiVideoProviderSystem.h"
#include "../KVideo/vgaProviderSystem.h"
#include "audioEncodeInterface.h"
#include "../KVideo/audio/iaudio.h"

#define MAX_CAMERA 2

#define KMaxSDICount 2

using namespace KVideo;


class ResourceModeHandle_SDI : public BaseMode, public IAudioEncodeListener,
		public ITimerListener, public ISDIVideoProviderObserver, public IVGAProviderObserver,
		public iaudio {
public:
	ResourceModeHandle_SDI(Kernel* pKernel);

	~ResourceModeHandle_SDI();
public:
	static void* OutputBitStreamWriteTask(void *threadsArg);

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

	virtual void onTimeout(Timer* pTimer);

	virtual void OnAAC(char* aac, int len);

	virtual void OnSpeex(char* spx, int len);

	virtual void onAACEncoded(unsigned char* pAACBuf, int len);

	virtual bool onProviderNV12(unsigned char* pNV12Buffer, int width, int height);

	virtual bool onProviderYUYV(unsigned char* pYUYVBuffer, int width, int height);

	virtual bool onVgaStatus(EVGAStatus status);

	virtual void notifySDIProviderBuffer(int aIndex, unsigned char* pYUYVBuffer, int width, int height);
public:

	virtual BOOL init(Setting* pSetting);

	virtual BOOL uninit();

	virtual BOOL start();

	virtual BOOL stop();

	virtual BOOL pause();

	virtual BOOL resume();

	virtual BOOL doMovieHead();

	virtual BOOL doMovieTail();

	virtual bool switchScene();

	virtual bool isComponentCreated();

protected:
	virtual void setSDIData(unsigned char* pPhyAddr, int size, int aSDIIndex, int channel);

	virtual void setVGAEncodeData(unsigned char* pPhyAddr, int size);

	virtual bool createComponents();

	virtual bool destroyComponents();

	virtual void allocInOutParams();

	virtual void freeInOutParams();

	virtual BOOL initOMXHandle();

	virtual bool startOMXThread();

	virtual bool stopOMXThread();

	virtual bool establistConnections();

	virtual bool setSDIDEIParams(int nSdiDEIIndex);

	virtual bool setSDIEncParams(int aSdiEncodeIndex);

	virtual bool setEncodeParams();

	BOOL initEncodeOMXHandle();

	BOOL initSDIDEIOMXHandle();

	BOOL initSDIEncOMXHandle();

	bool freeEncBuffers();

	virtual bool freeSDIDEIBuffers(int aIndex);

	virtual bool freeSDIEncBuffers(int aIndex);

	void openFileHandles();

	void closeFileHandles();

	BOOL initDeiOMXHandle();

	bool freeOMXHandles();

protected:
	virtual void onWriteTask(ComponentWrapper* comp);

	virtual void onConnectTask(ComponentWrapper* comp);


protected:

	Mp4Output* m_pResourceMP4File[KResourceNum];
	semp_t		m_resourceMP4Sem[KResourceNum];

	OMX_HANDLETYPE m_pSDIDEIHandle[KMaxSDICount];
	OMX_HANDLETYPE m_pSDIEncHandle[KMaxSDICount];
	OMX_HANDLETYPE m_pVGAEncHandle;
	OMX_CALLBACKTYPE m_pCb;
	string m_strMp4FileName;

	OMX_U32 m_inVGAWidth;
	OMX_U32 m_inVGAHeight;

	int		m_inWidth[KResourceNum];
	int 	m_inHeight[KResourceNum];

	bool m_bInited;

	OMX_U32 m_nFrameRate;
	OMX_U32 m_nEncBitRate;
	ComponentWrapper *m_VGAEncComponent;

	ComponentWrapper *m_SDIDEIComponent[KMaxSDICount];
	ComponentWrapper *m_SDIEncComponent[KMaxSDICount];
	int		m_nSDIInputIndex[KMaxSDICount];

	bool 	m_bExit;

	Setting* m_pSetting;

	IHeap_Handle    m_heap;
	OMX_PTR m_pHeapBuffer;

#ifndef AUDIO_CODECENGINE
	IAudioEncodeInterface*	m_pAudioEncode;
#else
	codec_engine* m_pAudioEncode;
	stRTPPacket m_stRTPPack;
#endif
	RtmpHandle*	m_rtmpHandle;

	bool 		m_bPaused;

	Mp4Output* m_mp4OutputHandle;
	semp_t 	m_mp4Sem;

	FILE* m_pResourceH264File[KResourceNum];
	FILE* m_pAACFile;
	FILE* m_pRecordH264File;

	bool m_bThreadsStart;

	long long 	m_nTotalEncCount;

	int			m_nVGAEncodeInputIndex;

	stConnInConnOutThreadArgs m_SDIDEIParams[2];
	stConnInConnOutThreadArgs m_SDIEncParams[2];
	stConnInConnOutThreadArgs m_vgaEncParams;
	bool		m_bTailState;
	int			m_liveOutWidth;
	int			m_liveOutHeight;

	SDIVideoProviderSystem m_sdiProvider;
};
