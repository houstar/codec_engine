#pragma once
#include "baseMode.h"
#include "HDMIDisplayManager.h"
#include "Mutex.h"
#include "cameraBufferPool.h"
#include "audioEncodeInterface.h"
#include "../KVideo/vgaProviderSystem.h"
#include "../KVideo/streamVideoManager.h"

#include "../KVideo/audio/iaudio.h"
#include "../KVideo/audio/rtpstruct.h"
#include "../KVideo/audio/codec_engine.h"
#include "../config.h"
#define FRAME_POOL

#define KHeadPicWidth 1024
#define KHeadPicHeight 768


//#define ALPHA_TEST
class MixtureMode_IPC : public BaseMode, public IAudioEncodeListener,
					public IStreamVideoManagerListener,
					public ITimerListener,
					public iaudio,
					public IVGAProviderObserver {

public:
	static void* OutputBitStreamWriteTask(void *threadsArg);

	static void* LiveOutputBitStreamWriteTask(void* threadsArg);

	static void* ConnInConnOutTask(void *threadsArg);

	static void* VGAScaleOutWriteTask(void* threadsArg);

	static void* InputMixStreamReadTask(void* threadsArg);

	static void* HeadTailScaleInputBitStreamReadTask(void* threadsArg);

	static void* HeadTailScaleWriteTask(void* threadsArg);

public:
	MixtureMode_IPC(Kernel* pKernel);
	virtual ~MixtureMode_IPC();

public:

	virtual void onTimeout(Timer* pTimer);

	virtual void OnAAC(char* aac, int len);

	virtual void OnSpeex(char* spx, int len);

	virtual void onAACEncoded(unsigned char* pAACBuf, int len);

	virtual bool onNotifyStreamVideo(const char* pData, int len, bool bIFrame, int index, int channel, bool bChannelChanged = false);

	virtual bool onProviderNV12(unsigned char* pNV12Buffer, int width, int height);

	virtual bool onProviderYUYV(unsigned char* pYUYVBuffer, int width, int height);

	virtual bool onProviderH264(unsigned char* pH264Buffer, int size);

	virtual bool onVgaStatus(EVGAStatus status);

public:
	static OMX_ERRORTYPE procCallbackEventHandler(OMX_HANDLETYPE hComponent,
			OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1,
			OMX_U32 nData2, OMX_PTR pEventData);

	static OMX_ERRORTYPE procCallbackEmptyBufferDone(OMX_HANDLETYPE hComponent,
			OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer);

	static OMX_ERRORTYPE procCallbackFillBufferDone(OMX_HANDLETYPE hComponent,
			OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer);

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

	virtual bool IPCChannelChanged();

	virtual bool doFreezeVGA(bool bFreeze);

	bool componentError(OMX_ERRORTYPE error);

protected:
	BOOL changeToRunning(bool bOpenVideos);

	virtual bool createComponents();

	virtual bool destroyComponents();

	virtual bool allocInOutLowerHalfParams();

	virtual bool allocInOutUpperHalfParams();

	virtual bool freeInOutLowerHalfParams();

	virtual bool freeInOutUpperHalfParams();

	virtual bool initLowerHalf();

	virtual bool initUpperHalf();

	virtual bool initLowerHalfOMXHandle();

	virtual bool initUpperHalfOMXHandle();

	virtual bool startLowerHalfOMXThread();

	virtual bool startUpperHalfOMXThread();

	bool establishLowerHalfConnections();

	bool establistUpperHalfConnections();

	virtual bool pauseLowerHalf();

	virtual bool resumeLowerHalf();

	bool setDecodeParams(int nDecIndex);

	bool setScalarParams(int nScaleIndex);

	bool setVGAScalarParams();

	bool setHeadTailScaleParams();

	virtual bool setEncodeParams();

	bool setLiveEncodeParams();

	bool setLiveScaleParams();

	bool setDeiParams();

	bool setLiveNfParams();

	bool setNfParams();

	BOOL initDecOMXHandle();

	BOOL initScaleOMXHandle();

	BOOL initVGAScaleOMXHandle();

	BOOL initHeadTailScaleOMXHandle();

	BOOL initEncodeOMXHandle();

	BOOL initLiveEncodeOMXHandle();

	BOOL initDeiOMXHandle();

	BOOL initLiveNfOMXHandle();

	BOOL initNfOMXHandle();

	virtual bool stopOMXThread();

	virtual bool freeOMXHandles();
protected:

	virtual bool freeDecBuffers(int aIndex);

	virtual bool freeScaleBuffers(int aIndex);

	virtual bool freeVGAScaleBuffers();

	virtual bool freeHeadTailScaleBuffers();

	virtual bool freeNfBuffers();

	virtual bool freeEncBuffers();

	bool freeDeiBuffers();

	bool freeLiveNfBuffers();

	bool freeLiveEncBuffers();

	virtual bool initVGAComponent();

	virtual bool uninitVGAComponent();

	virtual bool stopVGAComponentThread();

	virtual bool startVGAComponentThread();

protected:

	virtual void setDecodeData(char* pData, int length, int aDecIndex, int channel, bool bChannelChaned);

	virtual void setVGAScaleData(unsigned char* pPhyAddr, int size);

	virtual bool setMixData(unsigned char* pPhyAddr, int size);

	virtual void onVGAScaleWriteTask();

	virtual void onWriteTask(ComponentWrapper *aComponent);

	virtual void onLiveWriteTask(ComponentWrapper *aComponent);

	virtual void onMixReadTask(ComponentWrapper *aComponent);

	virtual void onConnectTask(ComponentWrapper *aComponent);

	virtual void onHeadTailScaleReadTask();

	virtual void onHeadTailScaleWriteTask();

	virtual bool initHeadTailInputResources(int aInputIndex);

	virtual int fillHeadTailScaleData(OMX_BUFFERHEADERTYPE* pBuf);

	virtual bool calcScalerWidthHeight();

	virtual void dynamicSwitchScene();

	virtual void changeBackImage();

	virtual bool dynamicConfigScaleComponent();

	virtual bool dynamicConfigVGAScaleComponent();

	void openFileHandles();

	void closeFileHandles();


protected:

	StreamVideoManager m_streamVideoManager;
	bool		m_bControlSwitchScene;
	int			 m_nDecInputIndex[KMaxDecComponentCount];

	int			m_nVGAScaleInputIndex;
	int			m_nMixInputIndex;

	OMX_HANDLETYPE m_pDecHandle[KMaxDecComponentCount];
	OMX_HANDLETYPE m_pScaleHandle[KMaxDecComponentCount];
	OMX_HANDLETYPE m_pVGAScaleHandle; /**< VGA的处理*/
	OMX_HANDLETYPE m_pNfHandle;
	OMX_HANDLETYPE m_pEncHandle;
	OMX_HANDLETYPE m_pHeadTailScaleHandle;/**< 用作片头片尾处理*/

	//when enable double encode
	OMX_HANDLETYPE m_pDeiHandle;
	OMX_HANDLETYPE m_pLiveNfHandle;
	OMX_HANDLETYPE m_pLiveEncHandle; /**< double birate enc */

	OMX_CALLBACKTYPE m_pCb;
	string m_strMp4FileName;
	OMX_U32 m_inWidth[KMaxDecComponentCount];
	OMX_U32 m_inHeight[KMaxDecComponentCount];
	OMX_U32 m_nDecStride[KMaxDecComponentCount];

	Mutex		m_headTailMutex;
	Mutex		m_vgaChangedMutex;
	OMX_U32 m_inVGAWidth;
	OMX_U32 m_inVGAHeight;
	OMX_U32 m_outVGAWidth;
	OMX_U32 m_outVGAHeight;

	OMX_U32 m_outScaleWidth[KMaxDecComponentCount];	/**< 经过scale组件输出的分辨率*/
	OMX_U32 m_outScaleHeight[KMaxDecComponentCount];

	bool m_bInited;

	int m_outWidth;
	int m_outHeight;

	int m_liveOutWidth;
	int m_liveOutHeight;

	OMX_U32 m_nFrameRate;
	OMX_U32 m_nEncBitRate;
	ComponentWrapper *m_decComponent[KMaxDecComponentCount];
	ComponentWrapper *m_scaleComponent[KMaxDecComponentCount];
	ComponentWrapper* m_vgaScaleComponent;	/**< VGA处理*/
	ComponentWrapper *m_encComponent;
	ComponentWrapper *m_nfComponent;

	ComponentWrapper *m_deiComponent;
	ComponentWrapper *m_liveNfComponent;
	ComponentWrapper *m_liveEncComponent;
	ComponentWrapper *m_headTailScaleComponent; /**< 用作片头片尾处理*/

	int m_maxDecode;

	stConnInConnOutThreadArgs m_encParams;
	stConnInConnOutThreadArgs m_nfParams;
	stConnInConnOutThreadArgs m_decParams[KMaxDecComponentCount];
	stConnInConnOutThreadArgs m_scaleParams[KMaxDecComponentCount];

	stConnInConnOutThreadArgs m_deiParams;
	stConnInConnOutThreadArgs m_liveNfParams;
	stConnInConnOutThreadArgs m_liveEncParams;

	bool 	m_bExit;
	bool 	m_bExitDecodeThread;
	bool 	m_bExitVGAScaleThread;
	bool	m_bExitMixtureThread;
	bool	m_bExitHeadTailScaleThread;

	Timer m_ExitTimer;
	bool  m_ExitTimerValid;
	Timer m_headDelayTimer;
	bool  m_headDelayTimerValid;

	Setting* m_pSetting;

	IAudioEncodeInterface*	m_pAudioEncode;

	bool 		m_bPaused;

	long long m_mixafttime;
	long long m_mixbeftime;

	long long m_nTotalEncCount;
	long long m_nLiveTotalEncCount;
	long long m_mixTotalCount;

	long long m_headTailScaleReadExpectTime;
	long long m_headTailScaleReadbefTime;
	long long m_headTailScaleReadaftTime;

	Mp4Output* m_mp4OutputHandle;

	IHeap_Object*	m_heap;
	OMX_PTR m_pHeapBuffer;

	int m_nAudioFrames;

	//不能产生大2G文件
	int		m_resourceH264fd[KResourceNum];
	int		m_liveH264fd;
	int		m_aacfd;
	int		m_recordH264fd;
	int		m_vgaH264fd;

	bool 	m_bThreadsStart;

	bool	m_binputMixInited;
	sem_t	m_inputMixWaitSem;

	Mutex	m_exchangeMutex;

	long long m_exchangeImagePhyAddr;
	long long m_preExchangeImagePhyAddr;
	bool	  m_bPreExchangeImageValid;


	bool   m_bTeacherContainStud; /**< */
	bool   m_bTeacherContainVGA;

	bool   m_bStudContainTeacher;
	bool   m_bStudContainVGA;

	bool   m_bVGAContainTeacher;
	bool   m_bVGAContainStud;


	unsigned char*  		m_pContainTeacherImage;
	OMX_PTR		m_containTeacherImagePhyAddr;
	int			m_nContainTeacherSize;
	semp_t		m_containTeacherSem;



	unsigned char*   m_pContainStudImage;
	OMX_PTR	m_containStudImagePhyAddr;
	int		m_nContainStudSize;
	semp_t  m_containStudSem;


	unsigned char*   m_pContainVGAImage;
	OMX_PTR m_containVGAImagePhyAddr;
	int		m_nContainVGASize;
	semp_t  m_containVGASem;


	bool 	m_bHeadState;
	bool	m_bTailState;

	bool	m_bUpHalfInited;
	bool	m_bCouldExit;

	SceneVideoInfo 	m_teacherSceneInfo;
	SceneVideoInfo	m_studentSceneInfo;
	SceneVideoInfo	m_deviceSceneInfo;

	int m_enc_frames;
	int m_live_enc_frames;

	FILE*			m_pLogFile;
	char			m_logBuffer[256];

	Timer			m_ResumeTimer; /**< when state from pause to resume, it delay sometime to copy frame*/
	bool			m_ResumeTimerValid;/**< when it is true, then do not copy */

	int				m_nAudioCountInMP4File;
	int				m_nVideoCountInMP4File;

	unsigned long m_nInitTime;
	unsigned long m_nInitEndTime;

	bool 	m_bWaitVGAFreezeSwitchScene;

#ifdef VERSION_MUL_CAMERA
	Timer	m_channelChangedTimer;
	bool	m_bChannelChangedTimerValid;
#endif
};


