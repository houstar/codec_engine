#include <dlfcn.h>
#include <stdio.h>
#include <signal.h>
#include "rtmpHandle.h"
#include "../utils/commonUtils.h"




#define KFlvPushLibName "libflvpulish.so"


void* RtmpHandle::sendRtmpBuffer(void* pParam) {
	RtmpHandle* pSelf = (RtmpHandle*) pParam;
	if (NULL != pSelf) {
		pSelf->procRtmpThread();
	}
	return NULL;
}

void* RtmpHandle::openURLThreadProc(void* pParam) {
	RtmpHandle* pSelf = (RtmpHandle*) pParam;
	if (NULL != pSelf) {
		pSelf->onOpenUrl();
	}
	return NULL;
}

void RtmpHandle::onTimeout(Timer* pTimer) {
	if (pTimer == &m_reconnectTimer) {
		if (m_bNeedReconnect) {
			reConnect();
		}
	}
}

RtmpHandle::RtmpHandle() :  m_handle(NULL), m_bTimerValid(false), m_threadId(0) {
	loadFunc();
	m_videoFramerate = 0;
	m_videoCounts = 0;
	m_audioCounts = 0;
	m_bNextVideo = true;

	m_videoFrameCount = 0;
	m_audioFrameCount = 0;

	m_videoWidth = 0;
	m_videoHeight = 0;

	m_openUrlThreadId = 0;

#ifdef SAVE_LIVE_FILE
	m_mp4File = NULL;
#endif

	m_bNeedReconnect = false;
	m_nResendCount = 0;
}

RtmpHandle::~RtmpHandle() {
	uninit();
	if (NULL != m_handle) {
		(*pfun_RECORD_RecordDestory)(m_handle);
		m_handle = NULL;
	}

	dlclose(m_plibHandle);
	m_plibHandle = NULL;
}

bool RtmpHandle::onOpenUrl() {
	printf("on open url start\n");
	fflush(stdout);
	m_handle = (*pfun_RECORD_Open)(m_strUrl.c_str());
	if (NULL == m_handle) {
		printf("init rtmp handle failed\n");
		sem_post(&m_openUrlSem);
		for (vector<IRtmpListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
			(*it)->notifyRtmpConnect(this, false);
		}
		m_bNeedReconnect = true;
		return false;
	}
	V_int32 ret = (*pfun_RECORD_SetAudioInfo) (m_handle, m_audioSamplerate, m_AudioChannels,
			m_AudioSampleBit, 1000, NULL, 0);
	if (0 != ret) {
		printf("set audio info failed\n");
		sem_post(&m_openUrlSem);
		for (vector<IRtmpListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
			(*it)->notifyRtmpConnect(this, false);
		}
		m_bNeedReconnect = true;
		return false;
	}
	ret = (*pfun_RECORD_SetVideoInfo) (m_handle, m_videoWidth, m_videoHeight,
			m_videoFramerate, 1000, NULL, 0);
	if (0 != ret) {
		printf("set video info failed\n");
		sem_post(&m_openUrlSem);
		for (vector<IRtmpListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
			(*it)->notifyRtmpConnect(this, false);
		}
		m_bNeedReconnect = true;
		return false;
	}

	m_bNeedReconnect = false;

	sem_post(&m_openUrlSem);
	for (vector<IRtmpListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
		(*it)->notifyRtmpConnect(this, true);
	}
	printf("open url ok\n");

	return true;
}

bool RtmpHandle::init(const char* strUrl, int aVideoWidth, int aVideoHeight,
		int aFramerate, int aAudioChannels,  int aAudioSampleRate,
		int aAudioSampleBit) {
	m_videoFramerate = aFramerate;
	m_audioSamplerate = aAudioSampleRate;
	m_strUrl = strUrl;
	m_AudioChannels = aAudioChannels;
	m_AudioSampleBit = aAudioSampleBit;

	m_videoWidth = aVideoWidth;
	m_videoHeight = aVideoHeight;

	m_bTimerValid = true;
	m_reconnectTimer.init();
	m_reconnectTimer.setInterval(30000);
	m_reconnectTimer.start(this);

	sem_init(&m_openUrlSem, 0, 0);
	pthread_attr_init(&m_openUrlThreadAttr);
	if (0 != pthread_create(&m_openUrlThreadId,
		&m_openUrlThreadAttr,
		openURLThreadProc,
		this)) {
		printf("create open Url thread failed\n");
		m_bInited = false;

		return false;
	}

	m_bInited = true;

	m_AudioframePool.init(80);

	SempUtils::semp_init(&m_AudioSem, 1);
	m_VideoframePool.init(60);

	SempUtils::semp_init(&m_VideoSem, 1);

	pthread_attr_init(&m_threadAttr);
	if (0 != pthread_create(&m_threadId,
				&m_threadAttr,
				sendRtmpBuffer,
				this)) {
			printf("create rtmp thread failed\n");
			m_bInited = false;
	}
	printf("create rtmp thread ok\n");
	sem_init(&m_exitSem, 0, 0);

#ifdef SAVE_LIVE_FILE
	if (NULL == m_mp4File) {
		m_mp4File = new Mp4Output();
		m_mp4File->init("/media/sda/testlive.mp4", m_videoWidth, m_videoHeight,
				m_videoFramerate, m_AudioChannels, m_audioSamplerate, m_AudioSampleBit);
	}
#endif
	return true;
}

bool RtmpHandle::uninit() {
	if (m_bTimerValid) {
		m_bTimerValid = false;
		m_reconnectTimer.stop();
		m_reconnectTimer.uninit();
	}

	if (m_openUrlThreadId != 0) {
		pthread_join(m_openUrlThreadId, NULL);
		sem_destroy(&m_openUrlSem);
		m_openUrlThreadId = 0;
	}

	if (!m_bInited) {
		return false;
	}
	m_bInited = false;

	if (m_threadId != 0) {
		sem_wait(&m_exitSem);
		pthread_join(m_threadId, NULL);

		sem_destroy(&m_exitSem);
	}

	SempUtils::semp_deinit(&m_AudioSem);
	SempUtils::semp_deinit(&m_VideoSem);
	m_VideoframePool.uninit();
	m_AudioframePool.uninit();

#ifdef SAVE_LIVE_FILE
	if (NULL != m_mp4File) {
		m_mp4File->uninit();
		delete m_mp4File;
		m_mp4File = NULL;
	}
#endif

	return true;
}

bool RtmpHandle::reConnect() {
	printf("before reconnect rtmp\n");
	fflush(stdout);
	m_bInited = false;

	if (m_threadId != 0) {
		sem_wait(&m_exitSem);
		pthread_cancel(m_threadId);
		pthread_join(m_threadId, NULL);

		sem_destroy(&m_exitSem);
		m_threadId = 0;
	}
	if (m_openUrlThreadId != 0) {
		pthread_cancel(m_openUrlThreadId);
		pthread_join(m_openUrlThreadId, NULL);
		sem_destroy(&m_openUrlSem);
		m_openUrlThreadId = 0;
	}

	m_audioCounts = 0;
	m_videoCounts = 0;
	m_audioFrameCount = 0;
	m_videoFrameCount = 0;

	printf("reconnect rtmp after close thread\n");
	sem_init(&m_openUrlSem, 0, 0);
	pthread_attr_init(&m_openUrlThreadAttr);
	if (0 != pthread_create(&m_openUrlThreadId,
		&m_openUrlThreadAttr,
		openURLThreadProc,
		this)) {
		printf("create open Url thread failed\n");
		m_bInited = false;

		return false;
	}
	m_bInited = true;
	pthread_attr_init(&m_threadAttr);
	if (0 != pthread_create(&m_threadId,
				&m_threadAttr,
				sendRtmpBuffer,
				this)) {
			printf("create rtmp thread failed\n");
			m_bInited = false;
	}
	printf("create rtmp thread ok\n");
	sem_init(&m_exitSem, 0, 0);

	return true;
}

bool RtmpHandle::pushVideo(char* pH264Buf, int len) {
	if (len <= 0 || !m_bInited) {
		return false;
	}

	SempUtils::semp_pend(&m_VideoSem);
	m_VideoframePool.PutFrame(pH264Buf, len);
	m_videoFrameCount++;
	SempUtils::semp_post(&m_VideoSem);

	return true;
}

bool RtmpHandle::pushAudio(char* pAACBuf, int len) {
	if (len <= 0 || !m_bInited) {
		return false;
	}

	SempUtils::semp_pend(&m_AudioSem);
	m_AudioframePool.PutFrame(pAACBuf, len);
	m_audioFrameCount++;
	SempUtils::semp_post(&m_AudioSem);


	return true;
}

bool RtmpHandle::procRtmpThread() {
	RECORD_Frame h264_frame;
	int nVideoLen = 0;
	char* pH264Buf = NULL;
	char* pAACBuf = NULL;
	int nAudioLen = 0;
	unsigned int nAudioTimeStamp = 0;
	unsigned int nVideoTimeStamp = 0;
	V_uint32 ret = 0;

	sem_wait(&m_openUrlSem);
	if (NULL == m_handle) {
		sem_post(&m_exitSem);
		pthread_exit(NULL);
		m_bNeedReconnect = true;
		return true;
	}

	while (m_videoFrameCount < 10 && !m_bInited) {
		usleep(40000);
		continue;
	}
	unsigned char *pAllocBuf = new unsigned char[1024 * 1024];
	while (m_bInited) {
		nAudioTimeStamp = 1000 * 1024.0 / (m_audioSamplerate) * m_audioCounts;
		nVideoTimeStamp = 1000.0 / m_videoFramerate * m_videoCounts;
		m_bNextVideo = (nVideoTimeStamp <= nAudioTimeStamp);
		if (m_nResendCount >= 100) {
			m_nResendCount = 0;
			m_bNeedReconnect = true;
			printf("break the rtmp send thread\n");
			break;
		}
		if (m_bNextVideo) {
			SempUtils::semp_pend(&m_VideoSem);
			pH264Buf = m_VideoframePool.GetFrame(nVideoLen);
			SempUtils::semp_post(&m_VideoSem);

			if (nVideoLen == 0) {
				usleep(5000);
			} else {
				RECORD_Frame Frame;
				Frame.buffer = (V_uchar*) pH264Buf;
				Frame.buffer_length = nVideoLen;
				Frame.cts_delta = 0;
				Frame.start_time = nVideoTimeStamp;
				ret = (*pfun_RECORD_RecordPush)(m_handle, rpt_video, &Frame);
				m_videoCounts++;
				if (ret != 0) {
					printf("rtmp push video failed %d\n", ret);
					usleep(5000);
					m_nResendCount++;
					continue;
				}
#ifdef SAVE_LIVE_FILE
	if (NULL != m_mp4File) {
		m_mp4File->writeH264Video((char*)pH264Buf, nVideoLen);
	}
#endif
			}
		} else {
			SempUtils::semp_pend(&m_AudioSem);
			pAACBuf = m_AudioframePool.GetFrame(nAudioLen);
			SempUtils::semp_post(&m_AudioSem);

			if (nAudioLen == 0) {
				usleep(5000);
			} else {
				RECORD_Frame Frame;
				Frame.buffer = (V_uchar*) pAACBuf;
				Frame.buffer_length = nAudioLen;
				Frame.cts_delta = 0;
				Frame.start_time = nAudioTimeStamp;
				ret = (*pfun_RECORD_RecordPush)(m_handle, rpt_audio, &Frame);
				m_audioCounts++;
				if (ret != 0) {
					printf("rtmp push audio failed %d\n", ret);
					usleep(5000);
					m_nResendCount++;
					continue;
				}
#ifdef SAVE_LIVE_FILE
	if (NULL != m_mp4File) {
		m_mp4File->writeAACAudio((char*)pAACBuf, nAudioLen);
	}
#endif
			}
		}
	}
	delete []pAllocBuf;
	sem_post(&m_exitSem);
	pthread_exit(NULL);
	return true;
}

bool RtmpHandle::loadFunc() {
	m_plibHandle = dlopen(KFlvPushLibName, RTLD_NOW );
	if (NULL == m_plibHandle) {
		printf("Open libflvpulish.so Error:%s.\n",dlerror());
		return false;
	}
	pfun_RECORD_Open   =  (RecordHandle (*)( const V_char *))
			dlsym(m_plibHandle, "RECORD_Open");
	if (NULL == pfun_RECORD_Open) {
		printf("load func error 2\n");
		return false;
	}

	pfun_RECORD_SetAudioInfo   =  (V_uint32 (*)(RecordHandle,
			V_uint32 ,V_uint32 ,V_uint32 ,V_uint32 ,V_char * const ,V_uint32 ))
			dlsym(m_plibHandle, "RECORD_SetAudioInfo");
	if (NULL == pfun_RECORD_SetAudioInfo) {
		printf("load func error 3\n");
		return false;
	}
	pfun_RECORD_SetVideoInfo   =  (V_uint32 (*)(RecordHandle,
			V_uint32 ,V_uint32 ,V_uint32 ,V_uint32 , V_char * const , V_uint32 ))
			dlsym(m_plibHandle, "RECORD_SetVideoInfo");
	if (NULL == pfun_RECORD_SetVideoInfo) {
		printf("load func error 4\n");
		return false;
	}
	pfun_RECORD_RecordPush   =  (V_uint32 (*)(RecordHandle, RECORD_PUSH_TYPE
			, RECORD_Frame const * )) dlsym(m_plibHandle, "RECORD_RecordPush");
	if (NULL == pfun_RECORD_RecordPush) {
		printf("load func error 5\n");
		return false;
	}

	pfun_RECORD_RecordDestory   =  (void (*)( RecordHandle ))
			dlsym(m_plibHandle, "RECORD_RecordDestory");
	if (NULL == pfun_RECORD_RecordDestory) {
		printf("load func error 6\n");
		return false;
	}
	printf("load func ok!\n");
	return true;
}

bool RtmpHandle::addListener(IRtmpListener * listener) {
	for (vector<IRtmpListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
		if ( *it == listener) {
			return false;
		}
	}
	m_vecListeners.push_back(listener);
	return true;
}

bool RtmpHandle::removeListener(IRtmpListener* listener) {
	for (vector<IRtmpListener*>::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it) {
		if ( *it == listener) {
			m_vecListeners.erase(it);
			return true;
		}
	}
	return true;
}
