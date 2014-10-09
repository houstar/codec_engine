#pragma once
#include "captureVga.h"
#include "../core/OMXCommonDef.h"
#include "../timer.h"
#include <vector>
#include "../core/Mutex.h"

using namespace std;

class Kernel;

enum EVGAStatus {
	eVGAStatusNULL = 0,
	eVGAStatusOpenOK,
	eVGAStatusOpenFailed,
	eVGAStatusClosed,
	eVGAStatusChange  /**< resolution change or vga change */
};

#define VGAStatusChangeNormal 0
#define VGAStatusException 1
#define VGAStatusChannelChange 2

class IVGAProviderObserver {
public:
	virtual ~IVGAProviderObserver() {

	}

	virtual bool onProviderNV12(unsigned char* pNV12Buffer, int width, int height) {
		return true;
	}

	virtual bool onProviderYUYV(unsigned char* pYUYVBuffer, int width, int height) {
		return true;
	}

	virtual bool onProviderH264(unsigned char* pH264Buffer, int len) {
		return true;
	}

	virtual bool onVgaStatus(EVGAStatus status) {
		return true;
	}
};


//provider the vga width = 1024, height = 768 ,the format has yuyv and nv12
class VGAProviderSystem : public ICaptureVGAListener, public ITimerListener {
public:

	VGAProviderSystem();

	virtual ~VGAProviderSystem();

public:

	virtual void onNotifyVGAFMT(CaptureVGA* pCaptureHandle, int width, int height);

	virtual void onCaptureVGABuffer(CaptureVGA* pCaptureHandle, unsigned char* pNV12Buffer, int width, int height);

	virtual void onTimeout(Timer *timer);

public:
	static void* scaleOutWriteTask(void* pParam);

	static void* VGAEncodeOutWriteTask(void* pParam);

public:
	static OMX_ERRORTYPE procCallbackEventHandler(OMX_HANDLETYPE hComponent,
			OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1,
			OMX_U32 nData2, OMX_PTR pEventData);

	static OMX_ERRORTYPE procCallbackEmptyBufferDone(OMX_HANDLETYPE hComponent,
			OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer);

	static OMX_ERRORTYPE procCallbackFillBufferDone(OMX_HANDLETYPE hComponent,
			OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer);

public:

	bool init(Kernel* pKernel);

	bool uninit();

	bool start();

	bool stop();

	bool addListener(IVGAProviderObserver* pObserver);

	bool removeListener(IVGAProviderObserver* pObserver);

	bool vgaStatusChanged(int reason);

	void setResolution720P(bool bUnder720P);

	bool doFreezeVGA(bool bFreezeVGA);

protected:
	bool createComponents();

	bool destroyComponents();

	bool allocInOutParams();

	bool freeInOutParams();

	bool initOMXHandle();

	bool initScaleOMXHandle();

	bool initEncodeOMXHandle();

	bool startOMXThread();

	bool stopOMXThread();

	bool setScaleParams();

	bool setEncodeParams();

	bool freeOMXHandles();

protected:

	bool freeScaleBuffers();

	bool freeEncodeBuffers();

protected:

	void setVGAScaleData(unsigned char* pPhyAddr, int size);

	void setVGAEncodeData(unsigned char* pPhyAddr, int size);

	void onVGAScaleWriteTask();

	void onVGAEncodeWriteTask();

private:

	Kernel* m_pKernel;

	OMX_HANDLETYPE m_pScaleHandle;
	OMX_HANDLETYPE m_pEncHandle;
	OMX_CALLBACKTYPE m_pCb;
	ComponentWrapper *m_scaleComponent;
	ComponentWrapper *m_encComponent;
	bool 	m_bExitThread;
	bool 	m_bExitVGAEncodeThread;


	CaptureVGA* m_pCaptureVGAHandle;
	Mutex		m_vgaMutex;
	unsigned long long m_captureChangeTime;

	vector<IVGAProviderObserver*> m_listeners;

	int m_inVGAWidth;
	int m_inVGAHeight;

	Timer m_captureVGATimer;
	bool m_captureVGATimerValid;
	bool m_bgetVGABuffer;

	bool	m_bStartCaptureVGASucess;

	bool m_bInited;
	int  m_nVGAScaleInputIndex;
	int	 m_nVGAEncodeInputIndex;

	IHeap_Object* m_heap;
	OMX_PTR m_pHeapBuffer;

	long long m_vgaCapturebefTime;
	long long m_vgaCaptureaftTime;
	long long m_nVgaCaptureMicroSecond;
	bool 	 m_bVGAUnder768;

	bool m_bFreezeVGA;
};
