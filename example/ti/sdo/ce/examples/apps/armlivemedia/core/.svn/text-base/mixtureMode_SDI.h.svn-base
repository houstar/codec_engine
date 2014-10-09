#pragma once
#include "baseMode.h"
#include "v4lDisplay.h"
#include "Mutex.h"
#include "cameraBufferPool.h"
#include "../KVideo/sdiVideoProviderSystem.h"
#include "../KVideo/vgaProviderSystem.h"
#include "audioEncodeInterface.h"
#include "../KVideo/audio/iaudio.h"

#define KHeadPicWidth 1024
#define KHeadPicHeight 768


//#define ALPHA_TEST


class MixtureMode_SDI : public BaseMode, public IAudioEncodeListener,
		public ITimerListener, public ISDIVideoProviderObserver,
		public IVGAProviderObserver, public iaudio {

public:

	static void* SDIScaleOutputChannel1(void* threadsArg);

	static void* SDIScaleOutputChannel2(void* threadsArg);

	static void* OutputBitStreamWriteTask(void *threadsArg);

	static void* LiveOutputBitStreamWriteTask(void* threadsArg);

	static void* ConnInConnOutTask(void *threadsArg);

	static void* VGAScaleOutWriteTask(void* threadsArg);

	static void* InputMixStreamReadTask(void* threadsArg);

	static void* VGAEncodeOutWriteTask(void* threadsArg);

	static void* HeadTailScaleInputBitStreamReadTask(void* threadsArg);

	static void* HeadTailScaleWriteTask(void* threadsArg);

	static void* SDIEncOutputWriteTask(void* threadsArg);

public:
	MixtureMode_SDI(Kernel* pKernel);
	virtual ~MixtureMode_SDI();

public:

	virtual void onTimeout(Timer* pTimer);

	virtual void OnAAC(char* aac, int len);

	virtual void OnSpeex(char* spx, int len);

	virtual void onAACEncoded(unsigned char* pAACBuf, int len);

	virtual bool onProviderNV12(unsigned char* pNV12Buffer, int width, int height);

	virtual bool onProviderYUYV(unsigned char* pYUYVBuffer, int width, int height);

	virtual bool onVgaStatus(EVGAStatus status);

	virtual void notifySDIProviderBuffer(int nIndex, unsigned char* pYUYVBuffer, int width, int height);

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

	bool setSDIDEIParams();

	bool setSDIScaleParams();

	bool setSDINfParams();

	bool setSDIEncParams();

	bool setVGAScaleParams();

	bool setHeadTailScaleParams();

	bool setVGAEncodeParams();

	virtual bool setEncodeParams();

	bool setLiveEncodeParams();

	bool setLiveScaleParams();

	bool setDeiParams();

	bool setLiveNfParams();

	bool setNfParams();

	BOOL initSDIDEIOMXHandle();

	BOOL initSDIScaleOMXHandle();

	BOOL initSDINfOMXHandle();

	BOOL initSDIEncOMXHandle();

	BOOL initVGAScaleOMXHandle();

	BOOL initHeadTailScaleOMXHandle();

	BOOL initVGAEncOMXHandle();

	BOOL initEncodeOMXHandle();

	BOOL initLiveEncodeOMXHandle();

	BOOL initDeiOMXHandle();

	BOOL initLiveNfOMXHandle();

	BOOL initNfOMXHandle();

	virtual bool stopOMXThread();

	virtual bool freeOMXHandles();
protected:

	virtual bool freeSDIDEIBuffers();

	virtual bool freeSDIScaleBuffers();

	virtual bool freeSDINfBuffers();

	virtual bool freeSDIEncBuffers();

	virtual bool freeVGAScaleBuffers();

	virtual bool freeHeadTailScaleBuffers();

	virtual bool freeNfBuffers();

	virtual bool freeEncBuffers();

	bool freeDeiBuffers();

	bool freeLiveNfBuffers();

	bool freeLiveEncBuffers();

	bool freeVGAEncBuffers();

	virtual bool initVGAComponent();

	virtual bool uninitVGAComponent();

	virtual bool stopVGAComponentThread();

	virtual bool startVGAComponentThread();

protected:

	virtual void setSDIDEIData(unsigned char* pPhyAddr, int size, int aSDIIndex);

	virtual void setVGAScaleData(unsigned char* pPhyAddr, int size);

	virtual void setVGAEncodeData(unsigned char* pPhyAddr, int size);

	virtual bool setMixData(unsigned char* pPhyAddr, int size);

	virtual void onVGAEncodeWriteTask();

	virtual void onVGAScaleWriteTask();

	virtual void onSDIScaleChannel1(ComponentWrapper* aComponent);

	virtual void onSDIScaleChannel2(ComponentWrapper* aComponent);

	virtual void onSDIEncWriteTask(ComponentWrapper* aComponent);

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

	Mutex		 m_IPCChannelMutex;
	bool		 m_bIPCChannelChanged;
	int			 m_nReadSameChannelCount;
	int			 m_nSDIInputIndex[KMaxDecComponentCount];
	PRChannel	 m_IPCChannel;
	PRChannel	 m_OldIPCChannel;
	bool		 m_bTeacherChannelNormal;
	bool		 m_bStudentChannelNormal;

	int			m_nVGAScaleInputIndex;
	int			m_nVGAEncodeInputIndex;
	int			m_nMixInputIndex;

	OMX_HANDLETYPE m_pSDIDEIHandle;
	OMX_HANDLETYPE m_pSDIScaleHandle;
	OMX_HANDLETYPE m_pSDINfHandle; /**< for channel 1 to enc */
	OMX_HANDLETYPE m_pSDIEncHandle; /**< for channel 1 to enc*/
	OMX_HANDLETYPE m_pVGAScaleHandle; /**< VGA的处理*/
	OMX_HANDLETYPE m_pNfHandle;
	OMX_HANDLETYPE m_pEncHandle;
	OMX_HANDLETYPE m_pHeadTailScaleHandle;/**< 用作片头片尾处理*/

	OMX_HANDLETYPE m_pVGAEncHandle;

	//when enable double encode
	OMX_HANDLETYPE m_pDeiHandle;
	OMX_HANDLETYPE m_pLiveNfHandle;
	OMX_HANDLETYPE m_pLiveEncHandle; /**< double birate enc */

	OMX_CALLBACKTYPE m_pCb;
	string m_strMp4FileName;
	OMX_U32 m_inWidth[KMaxDecComponentCount];
	OMX_U32 m_inHeight[KMaxDecComponentCount];

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
	ComponentWrapper *m_sdiDEIComponent;
	ComponentWrapper *m_sdiScaleComponent;
	ComponentWrapper *m_sdiNfComponent;
	ComponentWrapper *m_sdiEncComponent;
	ComponentWrapper* m_vgaScaleComponent;	/**< VGA处理*/
	ComponentWrapper *m_encComponent;
	ComponentWrapper *m_nfComponent;

	ComponentWrapper *m_deiComponent;
	ComponentWrapper *m_liveNfComponent;
	ComponentWrapper *m_liveEncComponent;
	ComponentWrapper *m_vgaEncComponent;
	ComponentWrapper *m_headTailScaleComponent; /**< 用作片头片尾处理*/

	int m_maxDecode;

	stConnInConnOutThreadArgs m_sdiDEIParams;
	stConnInConnOutThreadArgs m_sdiScaleParams;
	stConnInConnOutThreadArgs m_sdiNfParams;
	stConnInConnOutThreadArgs m_sdiEncParams;

	stConnInConnOutThreadArgs m_encParams;
	stConnInConnOutThreadArgs m_nfParams;

	stConnInConnOutThreadArgs m_deiParams;
	stConnInConnOutThreadArgs m_liveNfParams;
	stConnInConnOutThreadArgs m_liveEncParams;

	bool 	m_bExit;
	bool 	m_bExitVGAScaleThread;
	bool	m_bExitVGAEncodeThread;
	bool	m_bExitMixtureThread;
	bool	m_bExitHeadTailScaleThread;

	Timer m_ExitTimer;
	bool  m_ExitTimerValid;
	Timer m_headDelayTimer;
	bool  m_headDelayTimerValid;

	Timer m_checkAudioSyncTimer;
	bool  m_bcheckAudioSyncEnable;
	bool  m_checkAudioSyncTimerValid;

	Setting* m_pSetting;

#ifndef AUDIO_CODECENGINE
	IAudioEncodeInterface*	m_pAudioEncode;
#else
	codec_engine* m_pAudioEncode;
	stRTPPacket m_stRTPPack;
#endif
	RtmpHandle*	m_rtmpHandle;

	bool 		m_bPaused;

	long long m_mixafttime;
	long long m_mixbeftime;

	long long m_headTailScaleReadExpectTime;
	long long m_headTailScaleReadbefTime;
	long long m_headTailScaleReadaftTime;

	Mp4Output* m_mp4OutputHandle;
	Mutex 	m_mp4Mutex;


	IHeap_Handle    m_heap;
	OMX_PTR m_pHeapBuffer;

	unsigned int m_nAudioFrames;
	int   m_resourceH264fd[KResourceNum];
	int	  m_liveH264fd;
	int	  m_aacfd;
	int	  m_recordH264fd;
	int	  m_VGAH264fd;

	bool m_bThreadsStart;

	bool		m_binputMixInited;
	sem_t		m_inputMixWaitSem;

#ifdef ADDR_ALLOC_IN_HEAP
	unsigned char*  m_backTImage; /**< only teacher or student,or vga*/
	OMX_PTR			m_backTImagePhyAddr;

	unsigned char*	m_backTSImage; /**< only teacher and student*/
	OMX_PTR			m_backTSImagePhyAddr;

	unsigned char*	m_backTVImage; /**< only teacher and vga or only student and vga */
	OMX_PTR			m_backTVImagePhyAddr;

	unsigned char*	m_backTSVImage; /**< both teacher and student and vga */
	OMX_PTR		m_backTSVImagePhyAddr;

	unsigned char*	m_logoImage;
	OMX_PTR		m_logoImagePhyAddr;
#else
#endif
	Mutex		m_exchangeMutex;

#ifdef ADDR_ALLOC_IN_HEAP
	unsigned char*		m_pExchangeImage;
	OMX_PTR		m_exchangeImagePhyAddr;
#else
	long long m_exchangeImagePhyAddr;
	long long m_preExchangeImagePhyAddr;
	bool	  m_bPreExchangeImageValid;
#endif


	long long 	m_nTotalEncCount;

	long long   m_nLiveTotalEncCount;
	unsigned int m_livebeftime;
	unsigned int m_liveafttime;

	long long m_mixTotalCount;

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

	DisplayHandle m_displayHandle;

	SceneVideoInfo 	m_teacherSceneInfo;
	SceneVideoInfo	m_studentSceneInfo;
	SceneVideoInfo	m_deviceSceneInfo;

	int m_enc_frames;

	Timer			m_ResumeTimer; /**< when state from pause to resume, it delay sometime to copy frame*/
	bool			m_ResumeTimerValid;/**< when it is true, then do not copy */

	int				m_nAudioCountInMP4File;
	int				m_nVideoCountInMP4File;

	unsigned long m_nInitTime;
	unsigned long m_nInitEndTime;

	bool			m_bDirectCopyChannel1;
	bool			m_bDirectCopyChannel2;
};


