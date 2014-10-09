#ifndef CODEC_ENGINE_H
#define CODEC_ENGINE_H

#include <xdc/std.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/audio/auddec.h>
#include <ti/sdo/ce/audio/audenc.h>
#include <pthread.h>
#include "fifo.h"
#include "iaudio.h"
#include "SpeexCod.h"
#include <map>
#include <string>
#include "../../liveCourse/liveCourseManager.h"

class audio_capture;
class audio_playback;
class codec_engine;

struct channelinfo
{
	dec_info* dec;
	void* handle;
	fifo spxfifo;
	pthread_t id;	
	pthread_attr_t attr;
	codec_engine* ce;
	int index;
	bool working;
	char path[32];
	channelinfo()
	{
		dec     = NULL;
		handle  = NULL;
		ce      = NULL;
		index   = 0;
		working = false;
	}
};

typedef std::map<std::string, channelinfo> spx_map;

class codec_engine : public iaudio, public ILiveCourseManagerObserver {
public:
	codec_engine(void);
	~codec_engine(void);

	friend void* EncodeThread(void* param);
public:
	int init(char* pBaseAddr);
	int deinit(void);
	int start(void);
	int stop(void);

	void changestate(void);

	virtual void OnPCM(char* pcm, int length);
	virtual void OnMixOut(char* mix, int length, char* all);
	virtual bool onRecvMtgSpx(const char* path, const char* data, int datalen);
	
	void  setSpxListener(iaudio* _listener);
	void  removeSpxListener(void);
	
	void  setAACListener(iaudio* _listener);
	void  removeAACListener(void);
	
	int decode(char* path, unsigned char* buf, int length, bool again = false);
	void recvspx(char* path, unsigned char* data, int len);

	void startAACEncode(void);
	void stopAACEncode(void);

	void startEchoCancel();
	void stopEchoCancel();

	void setMixRemoteEnable(bool bMixRemoteEnable);

	void clearmembers(void);
private:
	int encode(char* buf, int length, char* outdata, int* outlen);
	void showinfor(void);
	void initchannelinfo(char* path, channelinfo* cinfo);
public:
	XDAS_Int8 *                 _inBuf;
	XDAS_Int8 *                 _encodedBuf;
	XDAS_Int8 *                 _decodeBuf;
	XDAS_Int8 *                 _outBuf;
	Engine_Handle               _encEngineHandle;
	AUDDEC_Handle               _decodeHandle;
	Engine_Attrs                _attrs;
	
	AUDENC_Handle               _encodeHandle;
  
	AUDENC_InArgs               _encInArgs;
	AUDENC_OutArgs              _encOutArgs;
	AUDENC_DynamicParams        _encDynParams;
	AUDENC_Status               _encStatus;
	
	AUDENC_InArgs               _decInArgs;
	AUDENC_OutArgs              _decOutArgs;
	AUDENC_DynamicParams        _decDynParams;
	AUDENC_Status               _decStatus;
	
	XDM_BufDesc                 _inBufDesc;
	XDAS_Int8*                  _src[XDM_MAX_IO_BUFFERS];
	XDAS_Int32                  _inBufSizes[XDM_MAX_IO_BUFFERS];
	
	XDM_BufDesc                 _encodedBufDesc;
	XDAS_Int8*                  _encoded[XDM_MAX_IO_BUFFERS];
	XDAS_Int32                  _encBufSizes[XDM_MAX_IO_BUFFERS];

	XDM_BufDesc                 _decodeBufDesc;
	XDAS_Int8*                  _decode[XDM_MAX_IO_BUFFERS];
	XDAS_Int32                  _decBufSizes[XDM_MAX_IO_BUFFERS];
	
	XDM_BufDesc                 _outBufDesc;
	XDAS_Int8*                  _dst[XDM_MAX_IO_BUFFERS];
	XDAS_Int32                  _outBufSizes[XDM_MAX_IO_BUFFERS];
	
	int                         _initialed;
	int                         _started;

	audio_capture*              _audioCapture;
	audio_playback*             _audioPlayback;

	fifo                        _fifo;
	pthread_mutex_t             _lock;

	//listenerList                _spx_listener;

	iaudio*                     _aac_listener;
	iaudio*                     _spx_listener;
	
	char*                       _map_base;
	enc_info*                   _enc;
	channelinfo                 _channel[6];

	pthread_t                   _dsp_thread_id;
	pthread_attr_t              _dsp_thread_attr;

	spx_map                     _recv_spx_map;

	unsigned long               _timestamp;
	unsigned short              _sequence_number;

	fifo                        _pcm_fifo;
	bool						m_bMixRemoteEnable;
	
};

#endif	// CODEC_ENGINE_H
