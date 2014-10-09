#include "mp4Output.h"
#include <string.h>


Mp4Output::Mp4Output() : m_pMp4WriterHandle(NULL) {
	memset(&m_infor, 0, sizeof(stmp4infor) );
	m_mp4Mutex.init();
}

Mp4Output::~Mp4Output() {
	uninit();
	m_mp4Mutex.uninit();
}


bool Mp4Output::init(const string& strFilename, int videoWidth, int videoHeight, int frameRate
		, int audioChannel, int audioSampleRate, int audioSampleBit) {
	strcpy(m_infor.mp4_filename, strFilename.c_str() );
	m_infor.video_width = videoWidth;
	m_infor.video_height = videoHeight;
	m_infor.video_rate = frameRate;
	m_infor.audio_channel = audioChannel;
	m_infor.audio_sample = audioSampleRate;
	m_infor.audio_samplebit = audioSampleBit;

	m_pMp4WriterHandle = Mp4Init(&m_infor);
	return (NULL != m_pMp4WriterHandle);
}

bool Mp4Output::uninit() {
	if (NULL != m_pMp4WriterHandle) {
		printf("mp4Uninit\n");
		Mp4UnInit(m_pMp4WriterHandle);
		m_pMp4WriterHandle = NULL;
	}

	return true;
}


bool Mp4Output::writeH264Video(const char* pData, int size) {
	m_mp4Mutex.lock();
	if (NULL == m_pMp4WriterHandle) {
		m_mp4Mutex.unlock();
		return false;
	}
	bool ret = Mp4WriteData(m_pMp4WriterHandle, (unsigned char*)pData, size, MP4_VIDEO_H264);
	m_mp4Mutex.unlock();
	return ret;
}

bool Mp4Output::writeAACAudio(const char* pData, int size) {
	m_mp4Mutex.lock();
	if (NULL == Mp4WriteData) {
		m_mp4Mutex.unlock();
		return false;
	}
	bool ret = Mp4WriteData(m_pMp4WriterHandle, (unsigned char*)pData, size, MP4_AUDIO_AAC);
	m_mp4Mutex.unlock();
	return ret;
}
