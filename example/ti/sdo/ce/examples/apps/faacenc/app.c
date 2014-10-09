/* 
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*
 *  ======== app.c ========
 */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>

#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/osal/Memory.h>
#include <ti/sdo/ce/audio/auddec.h>
#include <ti/sdo/ce/audio/audenc.h>

#include <string.h>  /* for memset */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

typedef void (*sighandler_t) (int);

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


static XDAS_Int8 *inBuf;
static XDAS_Int8 *encodedBuf;
static XDAS_Int8 *outBuf;

static String decoderName  = "auddec_copy";
String encoderName  = "faacenc";

static Void encode_decode(AUDENC_Handle enc, AUDDEC_Handle dec, FILE *in,
    FILE *out);


/*
 *  ======== createInFileIfMissing ========
 */
static void createInFileIfMissing(char *inFileName)
{
    int i;
    FILE *f = fopen(inFileName, "rb");
    if (f == NULL) {
        printf("Input file '%s' not found, generating one.\n", inFileName);
        f = fopen(inFileName, "wb");
        for (i = 0; i < 1024; i++) {
            fwrite(&i, sizeof(i), 1, f);
        }
    }
    fclose(f);
}

int _bekilled = 0;

void SignalProcess(int sig)
{
    printf("STOP!!\n");

	_bekilled = 1;
}

int NSAMPLES;
int IFRAMESIZE;
int EFRAMESIZE;
int OFRAMESIZE;

Engine_Handle _ce;
/*
 *  ======== smain ========
 */
Int smain(String progName, String procId, String engineName,
          String inFile, String outFile)
{
    Engine_Handle ce = NULL;
    Engine_Attrs attrs;
    AUDDEC_Handle dec = NULL;
    AUDENC_Handle enc = NULL;
    FILE *in = NULL;
    FILE *out = NULL;
    Memory_AllocParams allocParams;

if(!strcmp(encoderName,"faacenc")){
	 NSAMPLES  =  (1024);  /* must be multiple of 128 for cache/DMA reasons */
	 IFRAMESIZE = (NSAMPLES * 8);  /* raw frame (input) */
	 EFRAMESIZE = (NSAMPLES * 8);  /* encoded frame */
	 OFRAMESIZE = (NSAMPLES * 8);  /* decoded frame (output) */
}else if(!strcmp(encoderName,"speexenc")){
	 NSAMPLES  =  (384);  /* must be multiple of 128 for cache/DMA reasons */
	 IFRAMESIZE = (NSAMPLES * 2);  /* raw frame (input) */
	 EFRAMESIZE = (NSAMPLES * 2);  /* encoded frame */
	 OFRAMESIZE = (NSAMPLES * 2);  /* decoded frame (output) */
}else{
	NSAMPLES  =  (1024);  /* must be multiple of 128 for cache/DMA reasons */
	 IFRAMESIZE = (NSAMPLES * sizeof(Int8));  /* raw frame (input) */
	 EFRAMESIZE = (NSAMPLES * sizeof(Int8));  /* encoded frame */
	 OFRAMESIZE = (NSAMPLES * sizeof(Int8));  /* decoded frame (output) */
}


(void) signal(SIGINT, (sighandler_t) SignalProcess);

    /* create the input file if it doesn't already exist */
    createInFileIfMissing(inFile);

    Log_print4(Diags_USER1, "[+1] App-> Application started, procId %s "
               "engineName %s input-file %s output-file %s.",
               (IArg)procId, (IArg)engineName, (IArg)inFile, (IArg)outFile);

    /* allocate buffers */
    allocParams.type = Memory_CONTIGPOOL;
    allocParams.flags = Memory_NONCACHED;
    allocParams.align = BUFALIGN;
    allocParams.seg = 0;

    inBuf = (XDAS_Int8 *)Memory_alloc(IFRAMESIZE, &allocParams);
    encodedBuf = (XDAS_Int8 *)Memory_alloc(EFRAMESIZE, &allocParams);
    outBuf = (XDAS_Int8 *)Memory_alloc(OFRAMESIZE, &allocParams);

    if ((inBuf == NULL) || (encodedBuf == NULL) || (outBuf == NULL)) {
        goto end;
    }

    /* open file streams for input and output */
    if ((in = fopen(inFile, "rb")) == NULL) {
        printf("App-> ERROR: can't read file %s\n", inFile);
        goto end;
    }
    if ((out = fopen(outFile, "wb")) == NULL) {
        printf("App-> ERROR: can't write to file %s\n", outFile);
        goto end;
    }

    /* Initialize attrs fields to default values, and set the procId */
    Engine_initAttrs(&attrs);
    attrs.procId = procId;

    /* Open the Engine */
    if ((ce = Engine_open(engineName, &attrs, NULL)) == NULL) {
        fprintf(stderr, "%s: error: can't open engine %s\n",
            progName, engineName);
        goto end;
    }
	_ce = ce;

    /* allocate and initialize video decoder on the engine */
    dec = AUDDEC_create(ce, decoderName, NULL);
    if (dec == NULL) {
        printf( "App-> ERROR: can't open codec %s\n", decoderName);
        goto end;
    }

    /* allocate and initialize video encoder on the engine */
    enc = AUDENC_create(ce, encoderName, NULL);
    if (enc == NULL) {
        fprintf(stderr, "%s: error: can't open codec %s\n",
            progName, encoderName);
        goto end;
    }

	printf("here.\n");

    /* use engine to encode, then decode the data */
    encode_decode(enc, dec, in, out);

end:
    /* teardown the codecs */
    if (enc) {
        AUDENC_delete(enc);
    }
    if (dec) {
        AUDDEC_delete(dec);
    }

    /* close the engine */
    if (ce) {
        Engine_close(ce);
    }

    /* close the files */
    if (in) {
        fclose(in);
    }
    if (out) {
        fclose(out);
    }

    /* free buffers */
    if (inBuf) {
        Memory_free(inBuf, IFRAMESIZE, &allocParams);
    }
    if (encodedBuf) {
        Memory_free(encodedBuf, EFRAMESIZE, &allocParams);
    }
    if (outBuf) {
        Memory_free(outBuf, OFRAMESIZE, &allocParams);
    }

    Log_print0(Diags_USER1, "[+1] app done.");
    return (0);
}

/**************helper function***************/
/* This is a utility function to calculate time  between start and stop. This
 * is used to calculate FPS
 */
static int calc_result_time(struct timeval *result, struct timeval *after,
		struct timeval *before)
{
	/* Perform the carry for the later subtraction by updating "before" */
	if (after->tv_usec < before->tv_usec) {
		int nsec = (before->tv_usec - after->tv_usec) / 1000000 + 1;
		before->tv_usec -= 1000000 * nsec;
		before->tv_sec += nsec;
	}
	if (after->tv_usec - before->tv_usec > 1000000) {
		int nsec = (after->tv_usec - before->tv_usec) / 1000000;

		before->tv_usec += 1000000 * nsec;
		before->tv_sec -= nsec;
	}
	/* Compute the time remaining to wait, tv_usec is certainly positive.
	 * */
	result->tv_sec = after->tv_sec - before->tv_sec;
	result->tv_usec = after->tv_usec - before->tv_usec;
	/* Return 1 if result is negative. */
	return after->tv_sec < before->tv_sec;
}

#define SPEEX_SETENC 10
#define SPEEX_SETDEC 11

/*
 *  ======== encode_decode ========
 */
static Void encode_decode(AUDENC_Handle enc, AUDDEC_Handle dec, FILE *in,
    FILE *out)
{
    Int                         n;
    Int32                       status;
/*
    AUDDEC_InArgs               decInArgs;
    AUDDEC_OutArgs              decOutArgs;
    AUDDEC_DynamicParams        decDynParams;
    AUDDEC_Status               decStatus;
*/
	AUDENC_InArgs               decInArgs;
    AUDENC_OutArgs              decOutArgs;
    AUDENC_DynamicParams        decDynParams;
    AUDENC_Status               decStatus;
		
    AUDENC_InArgs               encInArgs;
    AUDENC_OutArgs              encOutArgs;
    AUDENC_DynamicParams        encDynParams;
    AUDENC_Status               encStatus;

    XDM_BufDesc                 inBufDesc;
    XDAS_Int8                  *src[XDM_MAX_IO_BUFFERS];
    XDAS_Int32                  inBufSizes[XDM_MAX_IO_BUFFERS];

    XDM_BufDesc                 encodedBufDesc;
    XDAS_Int8                  *encoded[XDM_MAX_IO_BUFFERS];
    XDAS_Int32                  encBufSizes[XDM_MAX_IO_BUFFERS];

    XDM_BufDesc                 outBufDesc;
    XDAS_Int8                  *dst[XDM_MAX_IO_BUFFERS];
    XDAS_Int32                  outBufSizes[XDM_MAX_IO_BUFFERS];

    /* clear and initialize the buffer descriptors */
    memset(src,     0, sizeof(src[0])     * XDM_MAX_IO_BUFFERS);
    memset(encoded, 0, sizeof(encoded[0]) * XDM_MAX_IO_BUFFERS);
    memset(dst,     0, sizeof(dst[0])     * XDM_MAX_IO_BUFFERS);

    src[0]     = inBuf;
    encoded[0] = encodedBuf;
    dst[0]     = outBuf;

    inBufDesc.numBufs = encodedBufDesc.numBufs = outBufDesc.numBufs = 1;

    inBufDesc.bufSizes      = inBufSizes;
    encodedBufDesc.bufSizes = encBufSizes;
    outBufDesc.bufSizes     = outBufSizes;

    inBufSizes[0] = IFRAMESIZE;
		encBufSizes[0] = EFRAMESIZE;
			outBufSizes[0] = OFRAMESIZE;

    inBufDesc.bufs      = src;
    encodedBufDesc.bufs = encoded;
    outBufDesc.bufs     = dst;

    /* initialize all "sized" fields */
    encInArgs.size    = sizeof(encInArgs);
    decInArgs.size    = sizeof(decInArgs);
    encOutArgs.size   = sizeof(encOutArgs);
    decOutArgs.size   = sizeof(decOutArgs);
    encDynParams.size = sizeof(encDynParams);
    decDynParams.size = sizeof(decDynParams);
    encStatus.size    = sizeof(encStatus);
    decStatus.size    = sizeof(decStatus);

    /*
     * Query the encoder and decoder.
     * This app expects the encoder to accept 1 buf in and get 1 buf out,
     * and the buf sizes of the in and out buffer must be able to handle
     * NSAMPLES bytes of data.
     */
    status = AUDENC_control(enc, XDM_GETSTATUS, &encDynParams, &encStatus);
    if (status != AUDENC_EOK) {
        /* failure, report error and exit */
        Log_print1(Diags_USER7, "[+7] encode control status = %ld",
                (IArg)status);
        return;
    }

	printf("encStatus.bufInfo.minInBufSize[0] = %d :%d\n",encStatus.bufInfo.minInBufSize[0],IFRAMESIZE);
	printf("encStatus.bufInfo.minOutBufSize[0] = %d :%d\n",encStatus.bufInfo.minOutBufSize[0],EFRAMESIZE);

    /* Validate this encoder codec will meet our buffer requirements */
    if ((inBufDesc.numBufs < encStatus.bufInfo.minNumInBufs) ||
        (IFRAMESIZE < encStatus.bufInfo.minInBufSize[0]) ||
        (encodedBufDesc.numBufs < encStatus.bufInfo.minNumOutBufs) ||
        (EFRAMESIZE < encStatus.bufInfo.minOutBufSize[0])) {

        /* failure, report error and exit */
        Log_print0(Diags_USER7, "[+7] Error:  encoder codec feature conflict");
        return;
    }
#if 0
    status = AUDDEC_control(dec, XDM_GETSTATUS, &decDynParams, &decStatus);
    if (status != AUDDEC_EOK) {
        /* failure, report error and exit */
        Log_print1(Diags_USER7, "[+7] decode control status = %ld",
                (IArg)status);
        return;
    }

    /* Validate this decoder codec will meet our buffer requirements */
    if ((encodedBufDesc.numBufs < decStatus.bufInfo.minNumInBufs) ||
        (EFRAMESIZE < decStatus.bufInfo.minInBufSize[0]) ||
        (outBufDesc.numBufs < decStatus.bufInfo.minNumOutBufs) ||
        (OFRAMESIZE < decStatus.bufInfo.minOutBufSize[0])) {

        /* failure, report error and exit */
        Log_print0(Diags_USER7,
                "[+7] App-> ERROR: decoder does not meet buffer requirements.");
        return;
    }
#endif
    /*
     * Read complete frames from in, encode, decode, and write to out.
     */
    for (n = 0; !_bekilled && fread(inBuf, encStatus.bufInfo.minInBufSize[0], 1, in) == 1; n++) {

        /* Deal with cache issues, if necessary */
#ifdef CACHE_ENABLED
#if defined(xdc_target__isaCompatible_64P) || \
    defined(xdc_target__isaCompatible_64T)
        /*
         *  fread() on this processor is implemented using CCS's stdio, which
         *  is known to write into the cache, not physical memory.  To meet
         *  XDAIS DMA Rule 7, we must writeback the cache into physical
         *  memory.  Also, per DMA Rule 7, we must invalidate the buffer's
         *  cache before providing it to any xDAIS algorithm.
         */
        Memory_cacheWbInv(inBuf, IFRAMESIZE);
#else
#error Unvalidated config - add appropriate fread-related cache maintenance
#endif

        /* Per DMA Rule 7, our output buffer cache lines must be cleaned */
        Memory_cacheInv(encodedBuf, EFRAMESIZE);
#endif

        //Log_print1(Diags_USER1, "[+1] App-> Processing frame %d...", (IArg)n);
#if 0
		status = AUDENC_control(enc, SPEEX_SETENC, &encDynParams, &encStatus);
		    if (status != AUDENC_EOK) {
				printf("SPEEX_SETENC error.\n");
		    }
#endif	
        /* encode the frame */
		inBufSizes[0] = encStatus.bufInfo.minInBufSize[0];
        status = AUDENC_process(enc, &inBufDesc, &encodedBufDesc, &encInArgs,
            &encOutArgs);

		if(encOutArgs.bytesGenerated==0)continue;

        //Log_print2(Diags_USER2,
               // "[+2] App-> Encoder frame %d process returned - 0x%x)",
               // (IArg)n, encOutArgs.bytesGenerated);

		//if(n%43==0)printf("\b\b\b\b\b\b\b\b\b\b\b\b\b%07d->%04d",n,encOutArgs.bytesGenerated);fflush(stdout);

#ifdef CACHE_ENABLED
        /* Writeback this outBuf from the previous call.  Also, as encodedBuf
         * is an inBuf to the next process call, we must invalidate it also, to
         * clean buffer lines.
         */
        Memory_cacheWbInv(encodedBuf, EFRAMESIZE);

        /* Per DMA Rule 7, our output buffer cache lines must be cleaned */
        Memory_cacheInv(outBuf, OFRAMESIZE);
#endif

        if (status != AUDENC_EOK) {
            Log_print3(Diags_USER7,
                    "[+7] App-> Encoder frame %d processing FAILED, status = 0x%x, "
                    "extendedError = 0x%x",
                    (IArg)n, (IArg)status, (IArg)(encOutArgs.extendedError));
            break;
        }

		/* write to file */
		fwrite(encodedBuf, encOutArgs.bytesGenerated, 1, out);
		
		//if(n%43==0)printf("\b\b\b\b\b\b\b\b\b\b\b\b\b%07d->%04d",n,encOutArgs.bytesGenerated);fflush(stdout);

#if 0
        /* decode the frame */
        decInArgs.numBytes = EFRAMESIZE;
        status = AUDDEC_process(dec, &encodedBufDesc, &outBufDesc, &decInArgs,
           &decOutArgs);

        Log_print2(Diags_USER2,
                "[+2] App-> Decoder frame %d process returned - 0x%x)",
                (IArg)n, (IArg)status);

        if (status != AUDDEC_EOK) {
            Log_print3(Diags_USER7,
                    "[+7] App-> Decoder frame %d processing FAILED, status = 0x%x, "
                    "extendedError = 0x%x",
                    (IArg)n, (IArg)status, (IArg)(decOutArgs.extendedError));
            break;
        }

#ifdef CACHE_ENABLED
        /* Writeback the outBuf. */
        Memory_cacheWb(outBuf, OFRAMESIZE);
#endif
        
#endif
#if 0
		status = AUDENC_control(enc, SPEEX_SETDEC, &encDynParams, &encStatus);
		    if (status != AUDENC_EOK) {
				printf("SPEEX_SETDEC error.\n");
		    }

		 /* decode the frame */
		 encBufSizes[0] = encOutArgs.bytesGenerated;
        status = AUDENC_process(enc, &encodedBufDesc, &outBufDesc, &decInArgs,
            &decOutArgs);	


		if(n%43==0)printf("\b\b\b\b\b\b\b\b\b\b\b\b\b%07d->%04d",n,decOutArgs.bytesGenerated);fflush(stdout);


	/* write to file */
        fwrite(outBuf, decOutArgs.bytesGenerated, 1, out);
#endif

	{
		static int bstart = 1;
		static 	struct timeval before, after, result;
		static unsigned int nfcount=0;
		nfcount++;
		if(bstart){
			gettimeofday(&before,NULL);
			bstart=0;
		}else if(nfcount%250==0){
			int cpu = Engine_getCpuLoad(_ce);
			gettimeofday(&after, NULL);
			calc_result_time(&result, &after, &before);
			if(result.tv_sec)printf(">>>%lu %lu%%\n", nfcount/result.tv_sec,cpu);
		}
	}			

    }

    Log_print1(Diags_USER1, "[+1] %d frames encoded/decoded", (IArg)n);
}
/*
 *  @(#) ti.sdo.ce.examples.apps.audio_copy; 1, 0, 0,1; 2-24-2012 19:27:46; /db/atree/library/trees/ce/ce-t06/src/ xlibrary

 */

