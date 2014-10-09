#ifndef SPEEX_COD_H
#define SPEEX_COD_H

#define SPX_LEN   42
#define PCM_LEN   640
#define AAC_LEN   512



// 0x8f900000 ~ 0x90000000
#define MAP_ADDR_BASE       0x8f900000   // 0x89500000 + 100M
#define ENC_ADDR_OFFSET     0x000000     // 3M
#define DEC_ADDR_OFFSET_C1  0x300000     // 1M

#define DEC_CHANNEL_SIZE    0x100000     // 1M

// IN  parameters
#define CODEC_PARAM_IN      0xc00000     // 1M
#define ENABLE_AAC_OFFSET   0x0			//bit flag to enable aac encode
#define ENABLE_ECHO_CANCAL_OFFSET 0x04	//bit flag to enable echo cancal
#define ENABLE_MIX_REMOTE_OFFSET	0x08 //bit flag to enable mix remote

// OUT parameters
#define CODEC_PARAM_OUT     0xd00000     // 1M
#define CPU_LOAD            0x0			 // cpu load

struct enc_info
{
	int pcm_valid; //four bytes to avoid algin
	int  aac_len;
	unsigned char pcm_data[PCM_LEN];
	unsigned char spx_data[PCM_LEN]; // ec
	unsigned char aac_data[AAC_LEN];
};


struct dec_info
{
    int spx_valid;
	unsigned char spx_data[SPX_LEN];
	unsigned char pcm_data[PCM_LEN];
};


#endif	// SPEEX_COD_H
