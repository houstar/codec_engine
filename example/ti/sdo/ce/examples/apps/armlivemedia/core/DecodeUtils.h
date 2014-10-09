#pragma once

#include "OMXCommonDef.h"

typedef unsigned int AVC_Err;

/* Common errorcodes */
#define AVC_SUCCESS                 (0)
#define AVC_ERR_NOT_SUPPORTED       (1)
#define AVC_ERR_MALLOC_FAILED       (2)
#define AVC_ERR_INTERNAL_ERROR      (3)
#define AVC_ERR_INVALID_ARGS        (4)
#define AVC_ERR_NO_OUTPUT_CHANNEL   (5)
#define AVC_ERR_WRONG_STATE         (6)
#define AVC_ERR_INSUFFICIENT_INPUT  (7)

/* Various start codes for H264 */
/* Last byte only */
#define H264_NAL_ACCESS_UNIT_CODEDSLICE_CODE_FOR_NONIDR		0x01
#define H264_NAL_ACCESS_UNIT_CODEDSLICE_CODE_FOR_IDR 		0x05
#define H264_PPS_START_CODE                                 0x08
#define H264_SPS_START_CODE                                 0x07
#define H264_WORKING_WORD_INIT  							0xFFFFFFFF

/* Read size is data, which is read by parser in single read, this will be
  chuked by parser untill more data is required to find a frame */

#define READSIZE 	(1*1024*1024)
#define CHUNKSIZE (1*1024*1024)
#define CHUNK_TO_READ (CHUNKSIZE/(64*16))

typedef struct
{
  unsigned char *ptr;
  int bufsize;
  int bufused;
} AVChunk_Buf;

/*!
*******************************************************************************
*  \enum
*  \brief  Parser State
*******************************************************************************
*/
enum
{
  H264_ST_LOOKING_FOR_SPS,     /**< Initial state at start, look for SPS */
  H264_ST_LOOKING_FOR_ZERO_SLICE,    /**< Looking for slice header with zero MB num */
  H264_ST_INSIDE_PICTURE,         /**< Inside a picture, looking for next picure start */
  H264_ST_STREAM_ERR,             /**< When some discontinuity was detected in the stream */
  H264_ST_HOLDING_SC              /**< Intermediate state, when a new frame is detected */
};

/*!
*******************************************************************************
*  \struct H.264_ChunkingCtx
*  \brief  H.264 Chunking Context
*******************************************************************************
*/
/* H264 video chunking */
typedef struct
{
  unsigned int workingWord;
  unsigned char fifthByte;
  unsigned char state;
} H264_ChunkingCtx;

typedef struct
{
  unsigned int frameNo;
  unsigned int frameSize;
  unsigned char *readBuf;
  unsigned char *chunkBuf;
  H264_ChunkingCtx ctx;
  AVChunk_Buf inBuf;
  AVChunk_Buf outBuf;
  unsigned int bytes;
  unsigned int tmp;
  unsigned char firstParse;
  unsigned int chunkCnt;
} H264_ParsingCtx;


class DecodeUtils {

public:
	static unsigned int Decode_GetNextFrameSize (H264_ParsingCtx *pc);

	static void Decode_VDEC_Reset_Parser (void *parserPtr);
	static void Decode_ParserInit (H264_ParsingCtx *pc);

	static void Decode_ChunkingCtxInit (H264_ChunkingCtx *c);
	static AVC_Err Decode_DoChunking (H264_ChunkingCtx *c,
            AVChunk_Buf *opBufs, unsigned int numOutBufs,
            AVChunk_Buf *inBuf, void *attrs);
};
