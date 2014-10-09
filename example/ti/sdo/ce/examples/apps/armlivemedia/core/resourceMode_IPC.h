#pragma once

#include "baseMode.h"
#include "../KVideo/vgaProviderSystem.h"

#include "../KVideo/audio/iaudio.h"
#include "../KVideo/audio/codec_engine.h"
#include "../KVideo/audio/rtpstruct.h"

#include "../KVideo/streamVideoManager.h"

#define MAX_CAMERA 2

using namespace KVideo;


class ResourceModeHandle_IPC : public BaseMode, public IAudioEncodeListener,
		public IStreamVideoManagerListener, public ITimerListener,
		public iaudio,
		public IVGAProviderObserver {
public:
	ResourceModeHandle_IPC(Kernel* pKernel);

	~ResourceModeHandle_IPC();

public:

	virtual void onTimeout(Timer* pTimer);

	virtual void OnAAC(char* aac, int len);

	virtual void OnSpeex(char* spx, int len);

	virtual void onAACEncoded(unsigned char* pAACBuf, int len);

	virtual bool onNotifyStreamVideo(const char* pData, int len, bool bIFrame, int index, int channel);

	virtual bool onProviderNV12(unsigned char* pNV12Buffer, int width, int height);

	virtual bool onProviderYUYV(unsigned char* pYUYVBuffer, int width, int height);

	virtual bool onProviderH264(unsigned char* pH264Buffer, int size);

	virtual bool onVgaStatus(EVGAStatus status);
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

	void openFileHandles();

	void closeFileHandles();


protected:

	Mp4Output* m_pResourceMP4File[KResourceNum];

	string m_strMp4FileName;

	OMX_U32 m_inVGAWidth;
	OMX_U32 m_inVGAHeight;

	int		m_inWidth[KResourceNum];
	int 	m_inHeight[KResourceNum];

	bool m_bInited;

	bool 	m_bExit;

	StreamVideoManager m_streamVideoManager;

	Setting* m_pSetting;

#ifndef AUDIO_CODECENGINE
	IAudioEncodeInterface*	m_pAudioEncode;
#else
	codec_engine* m_pAudioEncode;
	stRTPPacket m_stRTPPack;
#endif

	RtmpHandle*	m_rtmpHandle;

	bool 		m_bPaused;

	int	 m_aacfd;
};
