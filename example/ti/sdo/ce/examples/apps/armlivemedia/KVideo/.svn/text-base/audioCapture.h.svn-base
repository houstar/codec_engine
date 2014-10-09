#pragma once
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <string>

#include <alsa/asoundlib.h>
#include <pthread.h>
#include <vector>
#include <string>
#include "../core/semp.h"


using namespace std;


class IAudioCaptureObserver {
public:
	virtual ~IAudioCaptureObserver() {

	}

	virtual bool onProviderPcm(const char* pBuffer, int len) {
		return true;
	}
};


class AudioCapture  {
public:
	AudioCapture();
	~AudioCapture();
	
	friend void* readPcmThread(void* param);
	
public:
	bool  init(int aAudioChannel, int aSampleRate,
			int aSampleBit, bool bPCMNonBlock, string strDeviceName);
	bool  uninit(void);
	bool  start(void);
	bool  stop(void);

	bool  addListener(IAudioCaptureObserver* _listener);
	bool  removeListener(IAudioCaptureObserver* _listener);
private:
	bool  setAlsaAudioParam();
	
	void xrun();

	void suspend();


public:
	snd_pcm_t* _caphandle;
	pthread_attr_t _readpcmthrdattr;
	pthread_t _readpcmthrdid;


	int m_nChannels;
	int m_nSampleBit;
	int m_nSampleRate;

	bool m_bPCMNonBlock;

	bool m_bExitPcmReadThread;
	semp_t	m_exitSem;

	unsigned char* m_audioBuf;

	int		m_chunk_bytes;
	int		m_bits_per_frame;
	int		m_monotonic;

	unsigned long int m_chunk_size;

	unsigned  int m_periodTime;
	unsigned int m_periodCount;

	string 	m_strDeviceName;

	vector<IAudioCaptureObserver*> _pcm_listener;
	bool m_bInited;
};

