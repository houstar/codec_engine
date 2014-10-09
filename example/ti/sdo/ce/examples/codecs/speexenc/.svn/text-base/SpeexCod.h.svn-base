#ifndef SPEEX_COD_H
#define SPEEX_COD_H

#define SPX_LEN 42
#define PCM_LEN 640
#define AAC_LEN 512

// 0x8f900000 ~ 0x8ff00000
#define MAP_ADDR_BASE       0x8f900000   // 0x89500000 + 100M
#define ENC_ADDR_OFFSET     0x000000     // 3M   
#define DEC_ADDR_OFFSET_C1  0x300000     // 1M
#define DEC_ADDR_OFFSET_C2  0x400000     // 1M
#define DEC_ADDR_OFFSET_C3  0x500000     // 1M
#define DEC_ADDR_OFFSET_C4  0x600000     // 1M
#define DEC_ADDR_OFFSET_C5  0x700000     // 1M
#define DEC_ADDR_OFFSET_C6  0x800000     // 1M
#define DEC_ADDR_OFFSET_C7  0x900000     // 1M
#define DEC_ADDR_OFFSET_C8  0xa00000     // 1M

// IN  parameters
#define CODEC_PARAM_IN      0xc00000     // 1M
#define ENABLE_AAC_OFFSET   0x0		//aac enable flag bit
#define ENABLE_ECHO_CANCAL_OFFSET 0x04	//echo cancel flag bit
#define ENABLE_MIX_REMOTE_OFFSET  0x08

// OUT parameters
#define CODEC_PARAM_OUT     0xd00000     // 1M
#define CPU_LOAD            0x0			 // cpu load 

// end 0x89ffffff

struct speex_enc {
	int frame_size;  /* Number of samples */
	int channels;
	int clocks;
	struct 
	{ 
		void *st;
		SpeexBits bits;
		void *st_;
		SpeexBits bits_;
		short *pdata;
		short *pdatares;        
	} enc;
};

struct speex_dec
{
	int frame_size;  /* Number of samples */
	int channels;
	int clocks;
	
	struct 
	{
		void *st;
		SpeexBits bits;
		void *st_;
		SpeexBits bits_;
		short *pdata;
		short *pdatares;
	} dec;
};

struct enc_info
{
	int pcm_valid;
	int  aac_len;	
	unsigned char pcm_data[PCM_LEN];
	unsigned char spx_data[PCM_LEN];
	unsigned char aac_data[AAC_LEN];
	unsigned char mix_data[PCM_LEN];
};


struct dec_info
{
     	int spx_valid;
	unsigned char spx_data[SPX_LEN];
	unsigned char pcm_data[PCM_LEN];
};


#endif	// SPEEX_COD_H
