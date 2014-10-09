#pragma once
#include "OMXCommonDef.h"
#include "../../ThirdParty/rtmp/rtmp.h"
#include "framePool.h"
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include <string>
#include <vector>
#include "../timer.h"

using namespace std;


class RtmpHandle;

//#define SAVE_LIVE_FILE
#ifdef SAVE_LIVE_FILE
#include "mp4Output.h"
#include "Mutex.h"
#endif
class IRtmpListener {
public:
	virtual ~IRtmpListener() {

	}

	virtual bool notifyRtmpConnect(RtmpHandle* pRtmpHandle, bool bConnect) {
		return true;
	}
};


class RtmpHandle : public ITimerListener{
public:
	static void* sendRtmpBuffer(void* pParam);

	static void* openURLThreadProc(void* pParam);

public:
	virtual void onTimeout(Timer* pTimer);

public:
	RtmpHandle();
	~RtmpHandle();

	bool init(const char* strUrl, int aVideoWidth, int aVideoHeight,
			int aFramerate, int aAudioChannels,  int aAudioSampleRate,
			int aAudioSampleBit);

	bool uninit();

	bool pushVideo(char* pH264Buf, int len);

	bool pushAudio(char* pAACBuf, int len);

	bool onOpenUrl();

	bool addListener(IRtmpListener * listener);

	bool removeListener(IRtmpListener* listener);

public:

	bool loadFunc();

public:
	bool procRtmpThread();

	bool reConnect();

protected:


	RecordHandle (*pfun_RECORD_Open)( const V_char *);
	V_uint32 (*pfun_RECORD_SetAudioInfo)(RecordHandle ,V_uint32 ,V_uint32 ,V_uint32 ,V_uint32 ,V_char * const ,V_uint32 );
	V_uint32 (*pfun_RECORD_SetVideoInfo)(RecordHandle ,V_uint32 ,V_uint32 ,V_uint32 ,V_uint32 ,V_char * const ,V_uint32 );
	V_uint32 (*pfun_RECORD_RecordPush)(RecordHandle ,RECORD_PUSH_TYPE ,RECORD_Frame const * );
	void (*pfun_RECORD_RecordDestory)( RecordHandle );
protected:

	RecordHandle m_handle;

	unsigned int m_videoCounts;
	unsigned int m_audioCounts;

	bool m_bNextVideo;

	int m_videoFramerate;

	int m_audioSamplerate;


	FrameBufferPool m_VideoframePool;
	FrameBufferPool m_AudioframePool;
	semp_t 			m_AudioSem;
	semp_t			m_VideoSem;

	int				m_videoFrameCount;
	int				m_audioFrameCount;

	pthread_t m_threadId;
	pthread_attr_t m_threadAttr;

	pthread_t	m_openUrlThreadId;
	pthread_attr_t	m_openUrlThreadAttr;
	sem_t	m_openUrlSem;

	string		m_strUrl;
	int			m_AudioChannels;
	int			m_AudioSampleBit;

	int			m_videoWidth;
	int			m_videoHeight;

	bool	m_bInited;
	sem_t	m_exitSem;

	void *m_plibHandle;


	vector <IRtmpListener *> m_vecListeners;

#ifdef SAVE_LIVE_FILE
	Mp4Output* m_mp4File;
#endif

	Timer m_reconnectTimer;
	bool  m_bTimerValid;
	bool  m_bNeedReconnect;
	int	  m_nResendCount;
};
