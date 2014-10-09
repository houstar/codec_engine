#pragma once

#include "OMXCommonDef.h"
#include "../typedef.h"
#include "audioEncodeInterface.h"
#include "../KVideo/audioCapture.h"

#include <alsa/asoundlib.h>
#include <vector>
#include <string>
using namespace std;


#define NUM_OF_IN_BUFFERS 1
#define NUM_OF_OUT_BUFFERS 1

#define AENC_ENCODER_INPUT_READY 1
#define AENC_ENCODER_OUTPUT_READY   2
#define AENC_ENCODER_ERROR_EVENT 4
#define AENC_ENCODER_END_OF_STREAM 8
#define INPUT_BUF_SIZE 1024


class AudioEncodeOMX : public IAudioEncodeInterface, public IAudioCaptureObserver {
public:
	AudioEncodeOMX();
	~AudioEncodeOMX();

public:
	virtual bool onProviderPcm(const char* pBuffer, int len);

public:

	bool init(int aAudioChannel, int aSampleRate,
			int aSampleBit, int aacBitRate, bool bPCMNonBlock, string strDeviceName);

	bool uninit();

	bool start();

	bool stop();

	bool addListener(IAudioEncodeListener* pListener);

	bool removeListener(IAudioEncodeListener* pListener);

public:
	bool initialInputResources(int aInputIndex);

	OMX_U32 fillData(OMX_BUFFERHEADERTYPE * pBuf);

public:

	void createComponents();

	void destroyComponents();

	bool allocInOutParams();

	void freeInOutParams();

	bool initOMXHandle();

	OMX_ERRORTYPE setParamPortDefinition();

	OMX_ERRORTYPE changePortSettings();

	bool freeAudioEncodeBuffers();


public:
	OMX_HANDLETYPE m_pHandle;
	OMX_CALLBACKTYPE *m_pCb;
	OMX_STATETYPE m_eState;
	OMX_PARAM_PORTDEFINITIONTYPE *m_pInPortDef;
	OMX_PARAM_PORTDEFINITIONTYPE *m_pOutPortDef;
	OMX_U8 m_eCompressionFormat;
	OMX_BUFFERHEADERTYPE *m_pInBuff[NUM_OF_IN_BUFFERS];
	OMX_BUFFERHEADERTYPE *m_pOutBuff[NUM_OF_OUT_BUFFERS];
	OMX_S32 m_IpBuf_Pipe[2];
	OMX_S32 m_OpBuf_Pipe[2];
	OMX_S32 m_Event_Pipe[2];

	OMX_U32 m_nChannels;
	OMX_U32 m_nSampleBit;
	OMX_U32 m_nSampleRate;
	int		m_aacBitRate;

	OMX_AUDIO_AACSTREAMFORMATTYPE m_outputformat;

	semp_t *m_port_sem;
	semp_t *m_state_sem;
	semp_t *m_eos_sem;

    fd_set m_rd;
    fd_set m_wr;
    fd_set m_er;

	pthread_attr_t m_outConnectAttr;
	pthread_t m_outThreadId;

	pthread_attr_t m_inConnectAttr;
	pthread_t m_inThreadId;

	BOOL m_bExit;

	bool m_bInited;

	vector<IAudioEncodeListener*> m_vecListeners;

	unsigned char* m_pPcmBuffer;
	int   m_nPcmBufferSize;
	int   m_nPcmBufferMaxSize;

	semp_t   m_pcmSem;

	bool		m_bThreadStart;
};




