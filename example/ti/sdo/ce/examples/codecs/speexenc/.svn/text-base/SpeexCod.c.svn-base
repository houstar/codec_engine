
#define CONFIG_TI_C6X 
#define HAVE_CONFIG_H 
#define USE_SMALLFT  

#include "speex/speex.h"
#include "avdaec.h"
#include "faac.h"
#include <stdio.h>
#include <stdlib.h>
#include "mixer.h"
#include "SpeexCod.h"
#include <xdc/std.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>


#define FRAME_SIZE       320

/* aec */
T_avdAECParam   ec_param;
short           ec_buffer[FRAME_SIZE];

/* aac encode */
#define  AAC_DATA_LENGTH  124      /* save aac data length */
#define  AAC_DATA_OFFSET  128		 /* output aac data */
#define  PCM_SINGLE_FRAME 2048	 /* faac input 1024 samples * 2 bytes */
#define  PCM_BUFFER_SIZE  (PCM_SINGLE_FRAME << 2)    /* input pcm data, mono */
faacEncHandle aac;
unsigned long inputsamples = 0, maxoutputbytes = 0;
int           pcm_buffer_length = 0;		/* frames = Bytes * 2 */
unsigned char pcm_buffer[PCM_BUFFER_SIZE] = {0};
int           read_pos = 0, write_pos = 0;
unsigned char encode_buffer_mono[PCM_SINGLE_FRAME] = {0};

/* mixer */
unsigned short mix_in_buffer[FRAME_SIZE] = {0};
unsigned short mix_out_buffer[FRAME_SIZE] = {0};

#define MAX_CHANNEL 8
void*  _enc_handle = NULL;
void*  _dec_handle[MAX_CHANNEL] = {NULL};
int    _start_run = 0;

struct enc_info* _enc    = 0;
struct dec_info* _dec[MAX_CHANNEL] = 0;

char enc_pcmbuf[640] = {0};
char enc_spxbuf[640] = {0};
char dec_pcmbuf[640*MAX_CHANNEL] = {0};
char dec_spxbuf[42*MAX_CHANNEL]  = {0};

Clock_Handle myClock1, myClock2, myClock3, myClock4;

void interleave(const unsigned short* in_l,  unsigned short* out, int samples)
{
	int i = 0;
	for (; i < samples; ++ i) {
		out[i*2]   = in_l[i];
		out[i*2+1] = in_l[i]; 
	}
}

void start_ec(void)
{
    memset(&ec_param, 0, sizeof(ec_param));
    ec_param.aec_param   = 1;
    ec_param.ns_param    = 1;
    ec_param.sample_rate = 1;
    ec_param.buffercount = 10;

    ec_param.avdAgcParam.hagc_max_vol = 65535;
    ec_param.avdAgcParam.hagc_min_vol = 0;
    ec_param.avdAgcParam.hagc_param   = 0;
    ec_param.avdAgcParam.sagc_param   = 0;
    ec_param.avdAgcParam.hagc_in_vol  = 0;

    avd_AEC_Init(&ec_param);
}

void stop_ec(void)
{
	avd_AEC_UnInit();
}

void start_aac(void)
{	
	aac = faacEncOpen(16000, 1, &inputsamples, &maxoutputbytes);

	faacEncConfigurationPtr aacconfig = faacEncGetCurrentConfiguration(aac);

	aacconfig->inputFormat   = FAAC_INPUT_16BIT;
	aacconfig->outputFormat  = 1;
	aacconfig->bitRate       = 0;
	aacconfig->bandWidth     = 0;	
	aacconfig->quantqual     = 100;
	aacconfig->allowMidside  = 1;
	aacconfig->useLfe        = 1; 
	aacconfig->shortctl      = SHORTCTL_NORMAL;
	aacconfig->useTns        = 0;
	aacconfig->mpegVersion   = MPEG4;
	aacconfig->aacObjectType = LOW;
	
	faacEncSetConfiguration(aac, aacconfig);
} 

void stop_aac(void)
{
	faacEncClose(aac);
}

/* Encoder */
void spx_enc_close(void* data)
{
	struct speex_enc* sp = (struct speex_enc *)data;
	if (sp) {
		speex_bits_destroy(&sp->enc.bits);
		speex_encoder_destroy(sp->enc.st);
		free(sp);
	}
}

/* Decoder */
void spx_dec_close(void* data)
{
	struct speex_dec* sp = (struct speex_dec *)data;
	if (sp) {
		speex_bits_destroy(&sp->dec.bits);
		speex_decoder_destroy(sp->dec.st);
		free(sp);
	}
}


void *spx_enc_open(int clock_rate, int channel, int quality)
{
	if (clock_rate != 16000)
		return 0;
	const SpeexMode* mode = 0;
	mode = speex_lib_get_mode(SPEEX_MODEID_WB);
	struct speex_enc* sp = (struct speex_enc *)malloc(sizeof(struct speex_enc));
	if (!sp)
		return 0;
	memset(sp, 0, sizeof(struct speex_enc));
	
	sp->frame_size = 320;
	sp->channels   = channel;
	sp->clocks     = clock_rate;

	sp->enc.st = speex_encoder_init(mode);
	if (!sp->enc.st)
		return 0;
	speex_bits_init(&sp->enc.bits);
	speex_encoder_ctl(sp->enc.st, SPEEX_SET_SAMPLING_RATE, &clock_rate);	
	speex_encoder_ctl(sp->enc.st, SPEEX_SET_QUALITY, &quality);

	return sp;
}

void *spx_dec_open(int clock_rate, int channel)
{
	int tmp = 1;
	if (clock_rate != 16000)
		return NULL;
	const SpeexMode* mode = 0;
	mode = speex_lib_get_mode(SPEEX_MODEID_WB);
	struct speex_dec* sp = (struct speex_dec *)malloc(sizeof(struct speex_dec));
	if (!sp)
		return 0;
	memset(sp, 0, sizeof(struct speex_dec));

	sp->frame_size = 320;
	sp->channels   = channel;
	sp->clocks     = clock_rate;
	
	sp->dec.st = speex_decoder_init(mode);
	if (!sp->dec.st)
		return 0;
	speex_bits_init(&sp->dec.bits);
	speex_decoder_ctl(sp->dec.st, SPEEX_SET_SAMPLING_RATE, &clock_rate);
	speex_decoder_ctl(sp->dec.st, SPEEX_SET_ENH, &tmp);

	return sp;
}


int aac_encode(unsigned char* inpcm, int len, unsigned char* outaac)
{
	if (*(int* )(MAP_ADDR_BASE + CODEC_PARAM_IN + ENABLE_AAC_OFFSET) == 0) {
		return 0;
	}
	/* write */
	if (write_pos + len <= PCM_BUFFER_SIZE) {
		memcpy(pcm_buffer + write_pos, inpcm, len);
	} else {
		memcpy(pcm_buffer + write_pos, inpcm, PCM_BUFFER_SIZE - write_pos);
		memcpy(pcm_buffer, inpcm + PCM_BUFFER_SIZE - write_pos, len + write_pos - PCM_BUFFER_SIZE);
	}

	write_pos += len;
	write_pos %= PCM_BUFFER_SIZE;
	
	/* update length */
	pcm_buffer_length += len;
	
	if (pcm_buffer_length < (inputsamples << 1))
		return 0;

	pcm_buffer_length -= (inputsamples << 1);
	
	/* read */
	if (read_pos + (inputsamples << 1) <= PCM_BUFFER_SIZE) {
		memcpy(encode_buffer_mono, pcm_buffer + read_pos, (inputsamples << 1));
	} else {
		memcpy(encode_buffer_mono, pcm_buffer + read_pos, PCM_BUFFER_SIZE - read_pos);
		memcpy(encode_buffer_mono + PCM_BUFFER_SIZE - read_pos, pcm_buffer, read_pos + (inputsamples << 1) - PCM_BUFFER_SIZE);
	}
	 
	read_pos += (inputsamples << 1);
	read_pos %= PCM_BUFFER_SIZE;

	return faacEncEncode(aac, (int32_t *)encode_buffer_mono, inputsamples, outaac, maxoutputbytes);
}

/* 
   in_len: byte count;
   out_len: byte count;
*/ 
int spx_codec_encode(void *s, short *input,int in_len ,short *output,int *out_len)
{
	struct speex_enc *sp = (struct speex_enc *)s;
	
	int aac_len = 0, len;
    // echo cancellation based on remote pcm
	if (*(int* )(MAP_ADDR_BASE + CODEC_PARAM_IN + ENABLE_ECHO_CANCAL_OFFSET) == 0) {
		memcpy(ec_buffer, input, in_len);
	} else {
		avd_AEC_Proc(input, output, ec_buffer, 0);
   		avd_AEC_Proc(input + 160, output + 160, ec_buffer + 160, 0);
	}
    // mix local and remote
	if (*(int*) (MAP_ADDR_BASE + CODEC_PARAM_IN + ENABLE_MIX_REMOTE_OFFSET) == 0) {
		if ((aac_len = aac_encode((unsigned char*)ec_buffer, PCM_LEN, _enc->aac_data)) > 0) {
			_enc->aac_len = aac_len;
		} else {
			_enc->aac_len = 0;
		}
	} else {
		mixprocess(ec_buffer, output, mix_out_buffer);
			// aac encode
		if ((aac_len = aac_encode((unsigned char*)mix_out_buffer, PCM_LEN, _enc->aac_data)) > 0) {
			_enc->aac_len = aac_len;
		} else {
			_enc->aac_len = 0;
		}
	}
	
	if (sp) {
		/* Flush all the bits in the struct so we can encode a new frame */
		speex_bits_reset(&sp->enc.bits);
        
		len = speex_encode_int(sp->enc.st, (spx_int16_t *)ec_buffer, &sp->enc.bits);
		if(len  > 0)
			len = speex_bits_write(&sp->enc.bits, (char *)output, sp->frame_size);
	}
	
	*out_len = len; 
	
	return len;
} 


int spx_codec_decode(void *s,  short *input,int in_len ,short *output,int *out_len)
{
	struct speex_dec *sp = (struct speex_dec *)s;

	if (sp) {		
		speex_bits_read_from(&sp->dec.bits, (char *)input, in_len);
		speex_decode_int(sp->dec.st, &sp->dec.bits, output);
		*out_len = sp->frame_size * sizeof(short);		
	}		
	return *out_len;
}

int  spx_codec_recover(void *s,spx_int16_t *output,  int *out_len)
{
	struct speex_dec *sp = (struct speex_dec *)s;
	if (!sp) 
		return 0;
	int framesize = sp->frame_size;
	speex_decode_int(sp->dec.st, 0, output);
	*out_len = sp->channels * framesize *sizeof(short);
	return *out_len;
}

void codec_run_enc()
{
	// encode
	int ret = 0;
	if (_enc->pcm_valid) {
		memcpy(enc_pcmbuf, _enc->pcm_data, PCM_LEN);
		memcpy(enc_spxbuf, _enc->spx_data, PCM_LEN);
		spx_codec_encode(_enc_handle, (short* )enc_pcmbuf, PCM_LEN, (short* )enc_spxbuf, &ret);
		memcpy(_enc->spx_data, enc_spxbuf, SPX_LEN);
		_enc->pcm_valid = 0; 
	}	
}

void codec_run_dec0()
{
	// decode
	int ret = 0;
	
	if (_dec[0]->spx_valid) {

		char *spx = dec_spxbuf;
		char *pcm = dec_pcmbuf;
			
		memcpy(spx, _dec[0]->spx_data, SPX_LEN);
		spx_codec_decode(_dec_handle[0], (short* )spx, SPX_LEN, (short* )pcm, &ret);
		memcpy(_dec[0]->pcm_data, pcm, PCM_LEN);
		_dec[0]->spx_valid = 0;
	}

}

void codec_run_dec1()
{
	// decode
	int ret = 0;
	
	if (_dec[1]->spx_valid) {

		char *spx = dec_spxbuf + SPX_LEN;
		char *pcm = dec_pcmbuf + PCM_LEN;
			
		memcpy(spx, _dec[1]->spx_data, SPX_LEN);
		spx_codec_decode(_dec_handle[1], (short* )spx, SPX_LEN, (short* )pcm, &ret);
		memcpy(_dec[1]->pcm_data, pcm, PCM_LEN);
		_dec[1]->spx_valid = 0;
	}

}

void codec_run_dec2()
{
	// decode
	int ret = 0;

	if (_dec[2]->spx_valid) {
		
		char *spx = dec_spxbuf + (SPX_LEN << 1);
		char *pcm = dec_pcmbuf + (PCM_LEN << 1);
			
		memcpy(spx, _dec[2]->spx_data, SPX_LEN);
		spx_codec_decode(_dec_handle[2], (short* )spx, SPX_LEN, (short* )pcm, &ret);
		memcpy(_dec[2]->pcm_data, pcm, PCM_LEN);
		_dec[2]->spx_valid = 0;
	}
 
}

void codec_run_dec3()
{
	// decode
	int ret = 0;

	if (_dec[3]->spx_valid) {
		
		char *spx = dec_spxbuf + (SPX_LEN << 1) + SPX_LEN;
		char *pcm = dec_pcmbuf + (PCM_LEN << 1) + PCM_LEN;
			
		memcpy(spx, _dec[3]->spx_data, SPX_LEN);
		spx_codec_decode(_dec_handle[3], (short* )spx, SPX_LEN, (short* )pcm, &ret);
		memcpy(_dec[3]->pcm_data, pcm, PCM_LEN);
		_dec[3]->spx_valid = 0;
	}

}

void codec_run_dec4()
{
	// decode
	int ret = 0;

	if (_dec[4]->spx_valid) {
		
		char *spx = dec_spxbuf + (SPX_LEN << 2);
		char *pcm = dec_pcmbuf + (PCM_LEN << 2);
			
		memcpy(spx, _dec[4]->spx_data, SPX_LEN);
		spx_codec_decode(_dec_handle[4], (short* )spx, SPX_LEN, (short* )pcm, &ret);
		memcpy(_dec[4]->pcm_data, pcm, PCM_LEN);
		_dec[4]->spx_valid = 0;
	}
}

void codec_run_dec5()
{
	// decode
	int ret = 0;

	if (_dec[5]->spx_valid) {
		
		char *spx = dec_spxbuf + (SPX_LEN << 2) + SPX_LEN;
		char *pcm = dec_pcmbuf + (PCM_LEN << 2) + PCM_LEN;
			
		memcpy(spx, _dec[5]->spx_data, SPX_LEN);
		spx_codec_decode(_dec_handle[5], (short* )spx, SPX_LEN, (short* )pcm, &ret);
		memcpy(_dec[5]->pcm_data, pcm, PCM_LEN);
		_dec[5]->spx_valid = 0;
	}
}

void codec_run_dec6()
{
	// decode
	int ret = 0;

	if (_dec[6]->spx_valid) {
		
		char *spx = dec_spxbuf + (SPX_LEN << 2) + (SPX_LEN << 1);
		char *pcm = dec_pcmbuf + (PCM_LEN << 2) + (PCM_LEN << 1);
			
		memcpy(spx, _dec[6]->spx_data, SPX_LEN);
		spx_codec_decode(_dec_handle[6], (short* )spx, SPX_LEN, (short* )pcm, &ret);
		memcpy(_dec[6]->pcm_data, pcm, PCM_LEN);
		_dec[6]->spx_valid = 0;
	}
}

void codec_run_dec7()
{
	// decode
	int ret = 0;

	if (_dec[7]->spx_valid) {
		
		char *spx = dec_spxbuf + (SPX_LEN << 3) - SPX_LEN;
		char *pcm = dec_pcmbuf + (PCM_LEN << 3) - PCM_LEN;
			
		memcpy(spx, _dec[7]->spx_data, SPX_LEN);
		spx_codec_decode(_dec_handle[7], (short* )spx, SPX_LEN, (short* )pcm, &ret);
		memcpy(_dec[7]->pcm_data, pcm, PCM_LEN);
		_dec[7]->spx_valid = 0;
	}
}

void RunThreadEnc()
{
	codec_run_enc();
}

Void FxClock1(UArg arg)
{
	codec_run_dec0();
	codec_run_dec4();
	//static int i = 0;
	//*(int *)(0x89f00000) = i ++;
}

Void FxClock2(UArg arg)
{
	codec_run_dec1();
	codec_run_dec5();
}

void FxClock3(UArg arg)
{
	codec_run_dec2();
	codec_run_dec6();
}

void FxClock4(UArg arg)
{
	codec_run_dec3();
	codec_run_dec7();
}

void spx_codec_open()
{
	/* start enc & dec handle */
	int i = 0;
	_enc_handle = spx_enc_open(16000, 1, 5);
	for (; i < MAX_CHANNEL; ++ i) {
		_dec_handle[i] = spx_dec_open(16000, 1);
		speex_bits_reset(&(((struct speex_dec* )_dec_handle[i])->dec.bits));
	}

	/* echo cancellation */
    start_ec();

	/* aac */
	start_aac();

	/* mixer */
    mixinit();

	/* set addr */
	memset(MAP_ADDR_BASE, 0, 0x900000); /* 9M */
	_enc    = (struct enc_info* )(MAP_ADDR_BASE + ENC_ADDR_OFFSET);   
	_dec[0] = (struct dec_info* )(MAP_ADDR_BASE + DEC_ADDR_OFFSET_C1);
	_dec[1] = (struct dec_info* )(MAP_ADDR_BASE + DEC_ADDR_OFFSET_C2);
	_dec[2] = (struct dec_info* )(MAP_ADDR_BASE + DEC_ADDR_OFFSET_C3);
	_dec[3] = (struct dec_info* )(MAP_ADDR_BASE + DEC_ADDR_OFFSET_C4);
	_dec[4] = (struct dec_info* )(MAP_ADDR_BASE + DEC_ADDR_OFFSET_C5);
	_dec[5] = (struct dec_info* )(MAP_ADDR_BASE + DEC_ADDR_OFFSET_C6);
	_dec[6] = (struct dec_info* )(MAP_ADDR_BASE + DEC_ADDR_OFFSET_C7);
	_dec[7] = (struct dec_info* )(MAP_ADDR_BASE + DEC_ADDR_OFFSET_C8);
	
	_start_run = 1;

	Clock_Params clockParams;

	Clock_Params_init(&clockParams);
	clockParams.period = 5;
	clockParams.startFlag = 1;
	clockParams.arg = (UArg)0x5555;
	myClock1 = Clock_create(FxClock1, 5, &clockParams, NULL);

	Clock_Params_init(&clockParams);
	clockParams.period = 5;
	clockParams.startFlag = 1;
	clockParams.arg = (UArg)0x5555;
	myClock2 = Clock_create(FxClock2, 5, &clockParams, NULL);

	Clock_Params_init(&clockParams);
	clockParams.period = 5;
	clockParams.startFlag = 1;
	clockParams.arg = (UArg)0x5555;
	myClock3 = Clock_create(FxClock3, 5, &clockParams, NULL);

	Clock_Params_init(&clockParams);
	clockParams.period = 5;
	clockParams.startFlag = 1;
	clockParams.arg = (UArg)0x5555;
	myClock4 = Clock_create(FxClock4, 5, &clockParams, NULL);

	Clock_start(myClock1);
	Clock_start(myClock2);
	Clock_start(myClock3);
	Clock_start(myClock4);
}

void spx_codec_close()
{

	_start_run = 0;
	
	int i = 0;
	/* close ec */
	stop_ec();

	/* close aac */
	stop_aac();

	/* close enc & dec handle */
	for (; i < MAX_CHANNEL; ++ i) {
		spx_dec_close(_dec_handle);
	}
	spx_enc_close(_enc_handle);

	Clock_stop(myClock1);
	Clock_stop(myClock2);
	Clock_stop(myClock3);
	Clock_stop(myClock4);
}

