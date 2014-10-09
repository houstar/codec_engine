#pragma once
#include "audioEncodeInterface.h"

#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>

#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/osal/Memory.h>
#include <ti/sdo/ce/audio/auddec.h>
#include <ti/sdo/ce/audio/audenc.h>


#include <alsa/asoundlib.h>
#include <vector>
#include <string>
using namespace std;

#include "Mutex.h"
#include "../typedef.h"
#include "semp.h"

//将采集和编码放在一起是为了减少一次拷贝

/*
 * If an XDAIS algorithm _may_ use DMA, buffers provided to it need to be
 * aligned on a cache boundary.
 */

#ifdef CACHE_ENABLED

/*
 * If buffer alignment isn't set on the compiler's command line, set it here
 * to a default value.
 */
#ifndef BUFALIGN
#define BUFALIGN 128
#endif
#else

/* Not a cached system, no buffer alignment constraints */
#define BUFALIGN 0

#endif

const String encoderName  = "faacenc";


class AudioEncodeFAAC : public IAudioEncodeInterface {
public:

	AudioEncodeFAAC();
	virtual ~AudioEncodeFAAC();

public:

	bool init(int aAudioChannel, int aSampleRate,
			int aSampleBit, int aacBitRate, bool bPCMNonBlock, string strDeviceName);

	bool uninit();

	bool start();

	bool stop();

	bool addListener(IAudioEncodeListener* pListener);

	bool removeListener(IAudioEncodeListener* pListener);

public:
	bool setAlsaAudioParam();

	void xrun();

	void suspend();

	void onReadPcm();

	void onAudioOut();

private:
	bool initCodecEngine();

	bool uninitCodecEngine();

private:

	XDAS_Int8 *m_pInBuf;
	XDAS_Int8 *m_pEncodedBuf;
	XDAS_Int8 *m_pOutBuf;


	int m_NSAMPLES;
	int m_IFRAMESIZE;
	int m_EFRAMESIZE;
	int m_OFRAMESIZE;

	Engine_Handle m_ce;

    Engine_Attrs  m_EngineAttrs;
    AUDENC_Handle m_audioEncHandle;

    Memory_AllocParams m_allocParams;

    AUDENC_InArgs               m_encInArgs;
    AUDENC_OutArgs              m_encOutArgs;
    AUDENC_DynamicParams        m_encDynParams;
    AUDENC_Status               m_encStatus;

    XDM_BufDesc                 m_inBufDesc;
    XDAS_Int8                  *m_src[XDM_MAX_IO_BUFFERS];
    XDAS_Int32                  m_inBufSizes[XDM_MAX_IO_BUFFERS];

    XDM_BufDesc                 m_encodedBufDesc;
    XDAS_Int8                  *m_encoded[XDM_MAX_IO_BUFFERS];
    XDAS_Int32                  m_encBufSizes[XDM_MAX_IO_BUFFERS];

    XDM_BufDesc                 m_outBufDesc;
    XDAS_Int8                  *m_dst[XDM_MAX_IO_BUFFERS];
    XDAS_Int32                  m_outBufSizes[XDM_MAX_IO_BUFFERS];

private:

	int m_nChannels;
	int m_nSampleBit;
	int m_nSampleRate;
	int		m_aacBitRate;

	bool m_bPCMNonBlock;

	pthread_attr_t m_outThreadAttr;
	pthread_t m_outThreadId;


#ifdef AUDIO_BUFFER_COPY
	pthread_attr_t m_readPcmAttr;
	pthread_t m_readPcmThreadId;
#endif
	BOOL m_bExit;

#ifdef AUDIO_BUFFER_COPY
	BOOL m_bExitPcmReadThread;
	semp_t	m_exitReadSem;
#endif
	semp_t	m_eos_sem;

	bool m_bInited;

	snd_pcm_t* m_recordHandle;

	vector<IAudioEncodeListener*> m_vecListeners;

#ifdef AUDIO_BUFFER_COPY
	unsigned char* m_pPcmBuffer;
	int   m_nPcmBufferSize;
	int   m_nPcmBufferMaxSize;

	Mutex   	m_pcmMutex;
	unsigned char* m_audioBuf;
#endif

	int		m_chunk_bytes;
	int		m_bits_per_frame;
	int		m_monotonic;

	unsigned long int m_chunk_size;

	unsigned  int m_periodTime;
	unsigned int m_periodCount;

	string 	m_strDeviceName;

	bool		m_bThreadStart;
};
