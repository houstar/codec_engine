#include "DecodeUtils.h"
#include "timm_osal_trace.h"
#include "timm_osal_interfaces.h"


unsigned int DecodeUtils::Decode_GetNextFrameSize(H264_ParsingCtx *pc) {
	H264_ChunkingCtx *ctx = &pc->ctx;
	AVChunk_Buf *inBuf = &pc->inBuf;
	AVChunk_Buf *outBuf = &pc->outBuf;

	while (pc->bytes > pc->tmp) {
		inBuf->bufsize =
				((pc->bytes - pc->tmp) > 184) ? 184 : (pc->bytes - pc->tmp);
		inBuf->bufused = 0;

		while (inBuf->bufsize != inBuf->bufused) {
			if (AVC_SUCCESS
					== Decode_DoChunking(ctx, outBuf, 1, inBuf, NULL)) {
				pc->frameNo = pc->chunkCnt++;
				pc->frameSize = outBuf->bufused;
				pc->tmp += inBuf->bufused;
				inBuf->ptr += inBuf->bufused;
				return pc->frameSize;
			}
		}
		pc->tmp += inBuf->bufused;
		inBuf->ptr += inBuf->bufused;
	}

	return 0;
}

void DecodeUtils::Decode_VDEC_Reset_Parser(void *parserPtr) {
	H264_ParsingCtx *pc = (H264_ParsingCtx *) parserPtr;
	Decode_ChunkingCtxInit(&(pc->ctx));
	pc->frameNo = 0;
	pc->frameSize = 0;
	pc->bytes = 0;
	pc->tmp = 0;
	pc->firstParse = 1;
	pc->chunkCnt = 1;
	pc->outBuf.ptr = NULL;
	pc->outBuf.bufsize = 0;
	pc->outBuf.bufused = 0;
}

void DecodeUtils::Decode_ParserInit(H264_ParsingCtx *pc) {
	/* Initialize H.264 Parsing Context */
	pc->readBuf = (unsigned char *) TIMM_OSAL_Malloc (READSIZE, TIMM_OSAL_TRUE,
			0, TIMMOSAL_MEM_SEGMENT_EXT);
	if (!pc->readBuf) {
		printf("h264 Parser read buf memory allocation failed\n");
	}

	DecodeUtils::Decode_ChunkingCtxInit(&pc->ctx);
	pc->frameNo = 0;
	pc->frameSize = 0;
	pc->bytes = 0;
	pc->tmp = 0;
	pc->firstParse = 1;
	pc->chunkCnt = 1;
	pc->outBuf.ptr = NULL;
	pc->outBuf.bufsize = 0;
	pc->outBuf.bufused = 0;
}

void DecodeUtils::Decode_ChunkingCtxInit(H264_ChunkingCtx *c) {
	c->workingWord = H264_WORKING_WORD_INIT;
	c->fifthByte = 0xFF;
	c->state = H264_ST_LOOKING_FOR_SPS;
}

AVC_Err DecodeUtils::Decode_DoChunking(H264_ChunkingCtx *c, AVChunk_Buf *opBufs,
		unsigned int numOutBufs, AVChunk_Buf *inBuf, void *attrs) {
	int i = 0, j,  sc_found, frame_end, tmp, bytes;
	unsigned int w, z;
	unsigned char *inp;

	inp = &inBuf->ptr[inBuf->bufused];
	bytes = inBuf->bufsize - inBuf->bufused;

BACK:
	if (H264_ST_INSIDE_PICTURE == c->state) {
		tmp = i;
		sc_found = frame_end = 0;
		while (i < bytes) {
			z = c->workingWord << 8;
			w = c->fifthByte;
			c->workingWord = z | w;
			c->fifthByte = inp[i++];

			if (z == 0x100) {

				w &= 0x1f;
				if (w == H264_NAL_ACCESS_UNIT_CODEDSLICE_CODE_FOR_NONIDR
						|| w == H264_NAL_ACCESS_UNIT_CODEDSLICE_CODE_FOR_IDR) {
					/* check for MB number in slice header */
					if (c->fifthByte & 0x80) {
						sc_found = frame_end = 1;
						break;
					}
				}
				if (w == H264_PPS_START_CODE || w == H264_SPS_START_CODE) {
					sc_found = frame_end = 1;
					break;
				}
			}
		}
		j = i - tmp;

		/* minus 5 to remove next header that is already copied */
		if (frame_end) {
			j -= 5;
		}

		if (j > (int32_t) (opBufs->bufsize - opBufs->bufused)) {
			memcpy(&opBufs->ptr[opBufs->bufused], &inp[tmp], opBufs->bufsize - opBufs->bufused);
			opBufs->bufused = opBufs->bufsize;
			c->state = H264_ST_LOOKING_FOR_ZERO_SLICE;
			frame_end = 1;
		} else if (j > 0) {
			memcpy(&opBufs->ptr[opBufs->bufused], &inp[tmp], j);
			opBufs->bufused += j;
		} else {
			opBufs->bufused += j;
		}

		if (frame_end) {
			if (sc_found) {
				c->state = H264_ST_HOLDING_SC;
				//printf("FrameSize = %d\n", i);
			}
			inBuf->bufused += i;
			return AVC_SUCCESS;
		}
	}

	if (H264_ST_LOOKING_FOR_ZERO_SLICE == c->state) {
		tmp = i;
		sc_found = 0;
		while (i < bytes) {
			z = c->workingWord << 8;
			w = c->fifthByte;
			c->workingWord = z | w;
			c->fifthByte = inp[i++];

			if (z == 0x100) {
				w &= 0x1f;
				if (w == H264_NAL_ACCESS_UNIT_CODEDSLICE_CODE_FOR_NONIDR
						|| w == H264_NAL_ACCESS_UNIT_CODEDSLICE_CODE_FOR_IDR) {
					/* check for MB number in slice header */
					if (c->fifthByte & 0x80) {
						sc_found = 1;
						break;
					}
				}
			}
		}
		j = i - tmp;

		if (j > (opBufs->bufsize - opBufs->bufused)) {
			/* output buffer is full, discard this data, go back to looking for seq
			 start code */
			opBufs->bufused = 0;
			c->state = H264_ST_LOOKING_FOR_SPS;
			printf("h264 state look for sps\n");
		} else if (j > 0) {
			memcpy(&opBufs->ptr[opBufs->bufused], &inp[tmp], j);
			opBufs->bufused += j;
		}

		if (sc_found) {
			/* Set the attribute at rioWptr */
			c->state = H264_ST_INSIDE_PICTURE;
		}
	}

	if (H264_ST_STREAM_ERR == c->state) {
		while (i < bytes) {
			z = c->workingWord << 8;
			w = c->fifthByte;
			c->workingWord = z | w;
			c->fifthByte = inp[i++];

			if (z == 0x100) {
				w &= 0x1f;
				if (w == H264_NAL_ACCESS_UNIT_CODEDSLICE_CODE_FOR_NONIDR
						|| w == H264_NAL_ACCESS_UNIT_CODEDSLICE_CODE_FOR_IDR) {
					/* chueck for MB number in slice header */
					if (c->fifthByte & 0x80) {
						c->state = H264_ST_HOLDING_SC;
						break;
					}
				}
				if (w == H264_PPS_START_CODE || w == H264_SPS_START_CODE) {
					c->state = H264_ST_HOLDING_SC;
					break;
				} /* if (w...) */
			} /* if (z...) */
		} /* while (i) */
	} /* if (c->state...) */

	if (H264_ST_LOOKING_FOR_SPS == c->state) {
		while (i < bytes) {
			z = c->workingWord << 8;
			w = c->fifthByte;
			c->workingWord = z | w;
			c->fifthByte = inp[i++];

			if (z == 0x100) {
				w &= 0x1f;
				if (w == H264_SPS_START_CODE) {
					c->state = H264_ST_HOLDING_SC;
					break;
				} /* if (w...) */
			} /* if (z...) */
		} /* while (i) */
	} /* if (c->state...) */

	if (H264_ST_HOLDING_SC == c->state) {
		w = c->workingWord;
		opBufs->bufused = 0;
		if (opBufs->bufsize < 5) {
		}
		/* Copy these 5 bytes into output */
		for (j = 0; j < 4; j++, w <<= 8) {
			opBufs->ptr[opBufs->bufused + j] = ((w >> 24) & 0xFF);
		}
		opBufs->ptr[opBufs->bufused + j] = c->fifthByte;
		opBufs->bufused += 5;
		/* Copying frame start code done, now proceed to next state */
		w = c->workingWord & 0x1f;
		if (w == H264_PPS_START_CODE || w == H264_SPS_START_CODE) {
			c->state = H264_ST_LOOKING_FOR_ZERO_SLICE;
		} else {
			c->state = H264_ST_INSIDE_PICTURE;
		}

		c->workingWord = H264_WORKING_WORD_INIT;
		c->fifthByte = 0xFF;
	}

	if (i < bytes) {
		goto BACK;
	}

	inBuf->bufused += i;
	return AVC_ERR_INSUFFICIENT_INPUT;
}
