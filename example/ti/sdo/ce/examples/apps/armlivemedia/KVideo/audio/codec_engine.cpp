#include "codec_engine.h"

#include <ti/sdo/ce/osal/Memory.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>

#include <string.h>  /* for memset */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <signal.h>
#include "audio_capture.h"
#include "audio_playback.h"
#include "audio_mixer.h"
#include "rtpstruct.h"

#define AAC_DATA_LENGTH  124
#define AAC_DATA_OFFSET  128

/*
 * If an XDAIS algorithm _may_ use DMA, buffers provided to it need to be
 * aligned on a cache boundary.
 */

#ifdef CACHE_ENABLED
#ifndef BUFALIGN
#define BUFALIGN 128
#endif
#else
/* Not a cached system, no buffer alignment constraints */
#define BUFALIGN 0
#endif


namespace {

	const int kMapSize = 0x1000000;		         // 16M
	
	const int SPEEX_SETENC     = 10;
	const int SPEEX_SETDEC_0   = 11;
	const int SPEEX_SETDEC_1   = 12;
	const int SPEEX_SETDEC_2   = 13;
	const int SPEEX_SETDEC_3   = 14;
	const int SPEEX_RUN_THREAD = 15;

	const String ENCODE_NAME   = "speexenc";

	// echo cancellation parameter
	const int EC_DELAY_100 = 5;	 // 100ms
	const int EC_DELAY_200 = 10; // 200ms
	const int EC_DELAY_300 = 15; // 300ms
	const int EC_DELAY_400 = 20; // 400ms
	const int EC_DELAY_500 = 25; // 500ms
	const int EC_DELAY_600 = 30; // 600ms

	const int kBytesPerFrame = 640;	
	const int kRTPPacketLen  = 54;
	const int kSPXBufferLen  = 100; // 2s
	
	int NSAMPLES   = 0;
	int IFRAMESIZE = 0;
	int EFRAMESIZE = 0;
	int DFRAMESIZE = 0;
	int OFRAMESIZE = 0;

	// save aac
	FILE* fpcm[6] = {NULL};

	void* handle_local     = NULL;

	// speex out
	char speexdata[256] = {0};
	char pcm[640] = {0};

	const int kMaxChannel = 8;

	stRTPPacket rtp_send;
	stRTPPacket rtp_recv;

	Memory_AllocParams          _allocParams;
	
} // namespace end
unsigned long GetTickCount()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec/1000000);
}

void* EncodeThread(void* param)
{
	int spx_len = 0, len = 0;
	char pcm[640] = {0};
	codec_engine* pself = (codec_engine*)param;
	while (pself->_initialed) {
		while (pself->_started) {
			if (pself->_enc->pcm_valid) {
				usleep(1000);
				continue;
			}
			if (kTrue == pself->_pcm_fifo.pop(pcm, &len, 0)) {
				pself->encode(pcm, kBytesPerFrame, speexdata, &spx_len);
				
				// rtp pack
				if (pself->m_bMixRemoteEnable) {
					memcpy(rtp_send.data, speexdata, spx_len);
					pself->_sequence_number ++;
					pself->_timestamp += 320;
					rtp_send.header.sequencenumber = htons(pself->_sequence_number);
					rtp_send.header.timestamp = htonl(pself->_timestamp);

					if (NULL != pself->_spx_listener) {
						pself->_spx_listener->OnSpeex((char*)&rtp_send, kRTPPacketLen);
					}
				}
			} else {
				usleep(5000);
			}
			
		}
		sleep(1);
	}
	return NULL;
}

void* DecodeThread(void* param)
{
	channelinfo* chinfo = (channelinfo* )param;

	char spxdata[SPX_LEN] = {0};
	int len = 0;
	while (chinfo->working) {
		if (kTrue == chinfo->spxfifo.pop(spxdata, &len, 0)) {
			if (!chinfo->ce->decode(chinfo->path, (unsigned char*)spxdata, SPX_LEN)) {
				// wait 1ms, decode again
				usleep(1000);
				printf("decode again...\n");
				chinfo->ce->decode(chinfo->path, (unsigned char*)spxdata, SPX_LEN, true);
			}
		}
		usleep(5000);
	}
	return NULL;
}

codec_engine::codec_engine()
	: _inBuf(NULL)
	, _encodedBuf(NULL)
	, _decodeBuf(NULL)
	, _outBuf(NULL)
	, _encEngineHandle(NULL)
	, _decodeHandle(NULL)
	, _encodeHandle(NULL)
	, _initialed(false)
	, _started(false)
	, _audioCapture(NULL)
	, _audioPlayback(NULL)
	, _aac_listener(NULL)
	, _spx_listener(NULL)
	, _map_base(NULL)
	, _enc(NULL)
	, _timestamp(0)
	, _sequence_number(0)
	, m_bMixRemoteEnable(false)
{
}

codec_engine::~codec_engine()
{
	deinit();
}

int codec_engine::init(char* pBaseAddr)
{

	int fdmem = open("/dev/mem", O_RDWR | O_SYNC);
	if (fdmem < 0) {
		printf("codec_engine: open /dev/mem error.\n");
		return false;
	}

	//if (NULL == pBaseAddr) {
		_map_base = (char* )mmap(0, kMapSize, PROT_READ|PROT_WRITE, MAP_SHARED, fdmem, MAP_ADDR_BASE & ~4096);
		if (_map_base == (void*)-1) {
			printf("codec_engine: mmap error.\n");
			return false;
		}
		printf("codec_engine::use mmap base addr[local]\n");
		//} else {
		//	_map_base = pBaseAddr + MAP_ADDR_BASE;
		//	printf("codec_engine::use mmap base addr[remote]\n");
		//}

	printf("codec_engine: mem map done.\n");
	
	
	
	pthread_mutexattr_t attr;
	int re = pthread_mutexattr_init(&attr);
	if (re != 0)
		return false;
	re = pthread_mutex_init(&_lock, &attr); 
	if (re != 0)
		return false;
	
	
	NSAMPLES  =  (640); 
	IFRAMESIZE = (NSAMPLES*2);  /* raw frame (input) */
	EFRAMESIZE = (NSAMPLES*2);  /* encoded frame (output) */
	DFRAMESIZE = (NSAMPLES*2);  /* decode src frame (input) */
	OFRAMESIZE = (NSAMPLES*2);  /* decoded frame (output) */
	
	/* allocate buffers */
	_allocParams.type = Memory_CONTIGPOOL;
	_allocParams.flags = Memory_NONCACHED;
	_allocParams.align = BUFALIGN;
	_allocParams.seg = 0;

	_inBuf      = (XDAS_Int8 *)Memory_alloc(IFRAMESIZE, &_allocParams);
	_encodedBuf = (XDAS_Int8 *)Memory_alloc(EFRAMESIZE, &_allocParams);
	_decodeBuf  = (XDAS_Int8 *)Memory_alloc(DFRAMESIZE, &_allocParams);
	_outBuf     = (XDAS_Int8 *)Memory_alloc(OFRAMESIZE, &_allocParams);
	
	if ((_inBuf == NULL) ||
		(_encodedBuf == NULL) ||
		(_decodeBuf == NULL) ||
		(_outBuf == NULL)) {
		printf("memory_alloc failed\n");
		return false;
	}
	printf("codec_engine: 4\n");
	/* Initialize attrs fields to default values, set the procId */
	Engine_initAttrs(&_attrs);
	_attrs.procId = "DSP";

	////////////////////////////////////////////////////////////
	
	/* Open the Engine for enc*/
	printf("codec_engine: enc engine open start.\n");
	
	_encEngineHandle = Engine_open("remote_copy_DSP", &_attrs, NULL);
	if (_encEngineHandle == NULL) {
		printf("engine open failed\n");
		return false;
		}
	
	printf("codec_engine: enc engine open done.\n");	
	/* allocate and initialize video encoder on the engine */
	
	_encodeHandle = AUDENC_create(_encEngineHandle, "speexenc", NULL);
	if (_encodeHandle == NULL) {
		printf("AUDENC create failed\n");
		return false;
	}
	
	printf("codec_engine: enc engine create done.\n");
	/////////////////////////////////////////////////////////////

	
	/* initial codec params */  
	_encInArgs.size    = sizeof(_encInArgs);
	_encOutArgs.size   = sizeof(_encOutArgs);
	_encDynParams.size = sizeof(_encDynParams);
	_encStatus.size    = sizeof(_encStatus);
	
	_decOutArgs.size   = sizeof(_decOutArgs);
	_decDynParams.size = sizeof(_decDynParams);
	_decInArgs.size    = sizeof(_decInArgs);
	_decStatus.size    = sizeof(_decStatus);
	
	/* clear and initialize the buffer descriptors */
	memset(_src,     0, sizeof(_src[0])     * XDM_MAX_IO_BUFFERS);
	memset(_encoded, 0, sizeof(_encoded[0]) * XDM_MAX_IO_BUFFERS);
	memset(_decode,  0, sizeof(_decode[0])  * XDM_MAX_IO_BUFFERS);
	memset(_dst,     0, sizeof(_dst[0])     * XDM_MAX_IO_BUFFERS);
	
	_inBufDesc.numBufs      = 1;
	_encodedBufDesc.numBufs = 1;
	_decodeBufDesc.numBufs  = 1;
	_outBufDesc.numBufs     = 1;
	
	_inBufSizes[0]          = IFRAMESIZE;
	_encBufSizes[0]         = EFRAMESIZE;
	_decBufSizes[0]         = DFRAMESIZE;
	_outBufSizes[0]         = OFRAMESIZE;
	
	_inBufDesc.bufSizes      = _inBufSizes;
	_encodedBufDesc.bufSizes = _encBufSizes;
	_decodeBufDesc.bufSizes  = _decBufSizes;
	_outBufDesc.bufSizes     = _outBufSizes;
	
	_src[0]     = _inBuf;
	_encoded[0] = _encodedBuf;
	_decode[0]  = _decodeBuf;
	_dst[0]     = _outBuf;
	
	_inBufDesc.bufs      = _src;
	_encodedBufDesc.bufs = _encoded;
	_decodeBufDesc.bufs  = _decode;
	_outBufDesc.bufs     = _dst;
	
	Int32 status = AUDENC_control(_encodeHandle, XDM_GETSTATUS, &_encDynParams, &_encStatus);
	if (status != AUDENC_EOK) {
		printf("encode handle not ok!!!!\n");
		return false;
	}
	
	printf("encStatus.bufInfo.minInBufSize[0] = %d :%d\n",_encStatus.bufInfo.minInBufSize[0],IFRAMESIZE);
	printf("encStatus.bufInfo.minOutBufSize[0] = %d :%d\n",_encStatus.bufInfo.minOutBufSize[0],EFRAMESIZE);
	
	if ((_inBufDesc.numBufs < _encStatus.bufInfo.minNumInBufs) ||
		(IFRAMESIZE < _encStatus.bufInfo.minInBufSize[0]) ||
		(_encodedBufDesc.numBufs<_encStatus.bufInfo.minNumOutBufs)||
		(EFRAMESIZE < _encStatus.bufInfo.minOutBufSize[0])) {
		printf("size error\n");
		return false;
	}
	
	// save aac
	_enc    = (struct enc_info* )(_map_base + ENC_ADDR_OFFSET);	
	
	printf("codec_engine::init start initial mix\n");
	
	handle_local = audio_mixer::getInstance()->createChannel(true);
	
	// get mix out data
	audio_mixer::getInstance()->addListener(this);
	printf("codec_engine::init mix initial end\n");
	
	
	_fifo.init(20);
	_fifo.enablesize(EC_DELAY_300);


	if (NULL == _audioCapture)
		_audioCapture = new audio_capture();
	if (NULL == _audioPlayback)
		_audioPlayback = new audio_playback();
	
	if (!_audioCapture->init("default")) {
		printf("audio capture init false\n");
		return false;
	}

	if (!_audioPlayback->init("default")) {
		printf("audio playback init false\n");
		return false;
	}
	
	// read pcm
	_audioCapture->addListener(this);

	// rtp 
	timeval tm;
	gettimeofday(&tm, NULL);
	int seed = tm.tv_usec;
	srand(seed);

	memset(&rtp_send, 0, sizeof(rtp_send));

	rtp_send.header.version        = 2;
	rtp_send.header.marker         = 0; // 1
	rtp_send.header.payloadtype    = 0x67;
	rtp_send.header.sequencenumber = 0;
	rtp_send.header.timestamp      = 0;
	rtp_send.header.ssrc           = htonl(rand());
	
	printf("codec_engine::init done.\n");

	_pcm_fifo.init(50);
	
	_initialed = true;
	return true;
}

int codec_engine::deinit()
{	
	// save aac
	/* close capture and playback */
	if (_audioCapture) {
		delete _audioCapture;
		_audioCapture = NULL;
	}

	if (_audioPlayback) {
		delete _audioPlayback;
		_audioPlayback = NULL;
	}

	if (NULL != _map_base) {
		munmap(_map_base, kMapSize);
	}
	
	_fifo.uninit();
	pthread_mutex_destroy(&_lock);

	
	usleep(100000);
	
	/* tear down codecs*/
	if (_decodeHandle) {
		printf("delete decode handle\n");
		AUDDEC_delete(_decodeHandle);
		_decodeHandle = NULL;
	}
	if (_encodeHandle) {
		printf("delete encode handle\n");
		AUDENC_delete(_encodeHandle);
		_encodeHandle = NULL;
	}
  
	/* close engine */
	if (_encEngineHandle) {
		printf("delete enc engine handle\n");
		Engine_close(_encEngineHandle);
		_encEngineHandle = NULL;
	}
	
	/* free buffers */
	if (_inBuf) {
		Memory_free(_inBuf,      IFRAMESIZE, &_allocParams);
		_inBuf = NULL;
	}
	if (_encodedBuf) {
		Memory_free(_encodedBuf, EFRAMESIZE, &_allocParams);
		_encodedBuf = NULL;
	}
	if (_decodeBuf) {
		Memory_free(_decodeBuf,  DFRAMESIZE, &_allocParams);
		_decodeBuf = NULL;
	}
	if (_outBuf) {
		Memory_free(_outBuf,     OFRAMESIZE, &_allocParams);
		_outBuf = NULL;
	}

	printf("codec_engine: destroy amixer handle.\n");
	audio_mixer::getInstance()->destroy(handle_local);
	
	for (spx_map::iterator it = _recv_spx_map.begin(); it != _recv_spx_map.end(); ++ it) {
		printf("stop %s...\n", it->first);
		audio_mixer::getInstance()->destroy(it->second.handle);
		it->second.spxfifo.uninit();
		it->second.working = false;
		pthread_attr_destroy(&it->second.attr);
		pthread_join(it->second.id, 0);		
	}

	_recv_spx_map.clear();
	
	audio_mixer::getInstance()->uninit();
	printf("codec_engine: destroy amixer done.\n");

	
	_initialed = false;

	pthread_join(_dsp_thread_id, NULL);
	
	return true;
}

int codec_engine::start()
{	
	if (_audioCapture && _audioPlayback) {
		if(_audioCapture->start() && _audioPlayback->start())
			_started = true;
	}

	pthread_create(&_dsp_thread_id, NULL, EncodeThread, this);
	
	return _started;
}

int codec_engine::stop()
{
	if (_audioCapture && _audioPlayback) {
		if( _audioCapture->stop() && _audioPlayback->stop())
			_started = false;
	}	
	
	return !_started;	
}
bool codec_engine::onRecvMtgSpx(const char* path, const char* data, int len)
{
	spx_map::iterator it = _recv_spx_map.find(path);
	if (_recv_spx_map.end() == it) {
		if (_recv_spx_map.size() == kMaxChannel) {
			printf("recv spx channels overflow, ignore %s\n", path);
			return false;
		} else {
			initchannelinfo((char*)path, &(_recv_spx_map[path]));			
			printf("recv spx channel[%d], add %s success\n",  _recv_spx_map.size() - 1, path);			
		}
	} 

	// rtp unpack
	if (len != kRTPPacketLen) {
		return false;
	}

	memcpy(&rtp_recv, data, kRTPPacketLen);
	
	// add data
	it->second.spxfifo.push((char*)rtp_recv.data, SPX_LEN, 0);
	return true;
}

int codec_engine::decode(char* path, unsigned char* buf, int length, bool again)
{	
	if (!_started)
		return false;	

	pthread_mutex_lock(&_lock);
	spx_map::iterator it = _recv_spx_map.find(path);
	if (_recv_spx_map.end() == it) {
		printf("codec_engine::decode %s, return\n", path);
		pthread_mutex_unlock(&_lock);
		return false;
	}

	channelinfo* channel = &(it->second);
	
	if (channel->dec->spx_valid && again) {
		printf("codec_engine::decode: _dec[%d]->spx_valid = 1\n", channel->index);
		pthread_mutex_unlock(&_lock);
		return false;
	} 
	if (m_bMixRemoteEnable) {
		audio_mixer::getInstance()->inputData(channel->handle, (char* )channel->dec->pcm_data, PCM_LEN);
	}
	//printf("codec_engine::decode channel = %d\n", channel->index);
	
	memcpy(channel->dec->spx_data, buf, SPX_LEN);	
	channel->dec->spx_valid = 1;
	pthread_mutex_unlock(&_lock);
	
	return true;
}

void codec_engine::changestate()
{
	if (NULL != _audioPlayback)
		_audioPlayback->reopen();
}

void codec_engine::OnPCM(char* pcm, int length)
{
	if (!_started)
		return;

	if (length != kBytesPerFrame)
		return;

	// mix audio
	if (m_bMixRemoteEnable) {
		audio_mixer::getInstance()->inputData(handle_local, pcm, length);
	} else {
		if (NULL != _audioPlayback) {
			_audioPlayback->playback(pcm, length);
		}

	}
	if (kFalse == _pcm_fifo.push(pcm, length, 0, !m_bMixRemoteEnable)) {
		printf("push pcm fifo failed\n");
	}
}

void codec_engine::OnMixOut(char* mix, int length, char* all)
{
	/* playback decoded pcm */
	if (NULL != _audioPlayback)
		_audioPlayback->playback(all, length);
	
	if (length != kBytesPerFrame) {
		printf("OnMixOut: %d\n\n\n", length);
		return;
	}	
	// aec
	_fifo.push((char*)mix, length, 0);		
}

void codec_engine::setSpxListener(iaudio* _listener)
{
	_spx_listener = _listener;
}

void codec_engine::removeSpxListener()
{
	_spx_listener = NULL;
}

void codec_engine::setAACListener(iaudio* _listener)
{
	_aac_listener = _listener;
}

void codec_engine::removeAACListener()
{
	_aac_listener = NULL;
}

void codec_engine::startAACEncode()
{
	changestate();
	*(int *)(_map_base + CODEC_PARAM_IN + ENABLE_AAC_OFFSET) = 1;
}

void codec_engine::stopAACEncode()
{
	changestate();		
	*(int *)(_map_base + CODEC_PARAM_IN + ENABLE_AAC_OFFSET) = 0;
}

void codec_engine::startEchoCancel() {
	*(int*)(_map_base + CODEC_PARAM_IN + ENABLE_ECHO_CANCAL_OFFSET) = 1;
}

void codec_engine::stopEchoCancel() {
	*(int*)(_map_base + CODEC_PARAM_IN + ENABLE_ECHO_CANCAL_OFFSET) = 0;
}

void codec_engine::setMixRemoteEnable(bool bMixRemoteEnable) {
	m_bMixRemoteEnable = bMixRemoteEnable;
	*(int*) (_map_base + CODEC_PARAM_IN + ENABLE_MIX_REMOTE_OFFSET) = m_bMixRemoteEnable ? 1 : 0;
	if (m_bMixRemoteEnable) {
		rtp_send.header.sequencenumber = 0;
		rtp_send.header.timestamp      = 0;
	}
}

void codec_engine::clearmembers()
{
	/*
	for (spx_map::iterator it = _recv_spx_map.begin(); it != _recv_spx_map.end(); ++ it) {
		printf("stop %s...\n", it->first);
		audio_mixer::getInstance()->destroy(it->second.handle);
		it->second.spxfifo.uninit();
		it->second.working = false;
		pthread_attr_destroy(&it->second.attr);
		pthread_join(it->second.id, 0);		
	}

	_recv_spx_map.clear();
	*/
}

int codec_engine::encode(char* buf, int length, char* outspeex, int *outlen)
{
	if (!_started)
		return false;
	
	if (_enc->pcm_valid) {
		//printf("codec_engine::encode pcm valid = 1.\n");
		return false;
	}

	// out
	memcpy(outspeex, _enc->spx_data, SPX_LEN);
	*outlen = SPX_LEN;

	if (_enc->aac_len > 0) {
		//printf("codec_engine::encode aac len = %d\n", aac_len);
		//fwrite(_enc->aac_data, aac_len, 1, fwriteaac);
		if (NULL != _aac_listener)
			_aac_listener->OnAAC((char*)_enc->aac_data, _enc->aac_len);
	}

	// capture pcm
	memcpy(_enc->pcm_data, buf, length);

	// aec
	int rc = 0;
	if (kFalse == _fifo.pop((char *)_enc->spx_data, &rc, 0))
		memset(_enc->spx_data, 0, PCM_LEN);

	_enc->pcm_valid = 1;
	_enc->aac_len = 0;
	
	Int32 status = AUDENC_control(_encodeHandle, SPEEX_SETENC, &_encDynParams, &_encStatus);
	if (status != AUDENC_EOK) {		
		_enc->pcm_valid = 0;
		printf("codec_engine::encode ENC failed!!!!\n");
		return false;
	}
	
	showinfor();
	
	return true;
}

void codec_engine::showinfor()
{
	static timeval last, now;	
	gettimeofday(&now, NULL);
	if (now.tv_sec - last.tv_sec >= 2)  {
		//printf("codec_engine: cpu load %d%\n", *(int *)(_map_base + CODEC_PARAM_OUT + CPU_LOAD));
		int nLoad = Engine_getCpuLoad(_encEngineHandle);
		if (nLoad > 85) {
			printf("codec_engine[%d]: cpu load %d%\n", _recv_spx_map.size(), nLoad);
		}
		memcpy(&last, &now, sizeof(timeval));

		int i = 0;
		for (spx_map::iterator it = _recv_spx_map.begin(); it != _recv_spx_map.end(); ++ it) {
			printf("codec engine[%d] spx fifo length: %d\n", ++ i, it->second.spxfifo.getcurlength());
		}

		// mixer
		audio_mixer::getInstance()->printfifostatus();
	}	
}

void codec_engine::initchannelinfo(char* path, channelinfo* cinfo)
{
	cinfo->dec = (struct dec_info* )(_map_base + DEC_ADDR_OFFSET_C1 + DEC_CHANNEL_SIZE * (_recv_spx_map.size() - 1));
	cinfo->handle = audio_mixer::getInstance()->createChannel();
	cinfo->spxfifo.init(kSPXBufferLen);
	cinfo->ce = this;
	cinfo->index = _recv_spx_map.size() - 1;
	cinfo->working = true;
	pthread_attr_init(&cinfo->attr);
	pthread_create(&cinfo->id, &cinfo->attr, DecodeThread, cinfo);
	strcpy(cinfo->path, path);
}
