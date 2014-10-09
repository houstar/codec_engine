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
 *  ======== audenc_copy.c ========
 *  Audio Encoder "copy" algorithm.
 *
 *  This file contains an implementation of the IAUDENC interface
 *  defined by XDM.
 */
#include <xdc/std.h>
#include <string.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>

#include <ti/xdais/dm/iaudenc.h>

#include "faacenc.h"
#include "frame.h"
#include "faaccfg.h"
#include "util.h"

#ifdef __TI_COMPILER_VERSION__
/* xDAIS Rule 13 - this #pragma should only apply to TI codegen */
#pragma CODE_SECTION(faacenc_TI_control, ".text:algControl")
#pragma CODE_SECTION(faacenc_TI_process, ".text:algProcess")
#pragma CODE_SECTION(faacenc_TI_initObj, ".text:algInit")
#pragma CODE_SECTION(faacenc_TI_free,    ".text:algFree")
#pragma CODE_SECTION(faacenc_TI_alloc,   ".text:algAlloc")
#endif

#include "faacenc_copy_ti.h"
#include "faacenc_copy_ti_priv.h"

/* buffer definitions */
#define MININBUFS       1
#define MINOUTBUFS      1
#define MININBUFSIZE    1
#define MINOUTBUFSIZE   1

#define IALGFXNS  \
    &faacenc_TI_IALG,/* module ID */                         \
    NULL,               /* activate */                          \
    faacenc_TI_alloc,/* alloc */                             \
    NULL,               /* control (NULL => no control ops) */  \
    NULL,               /* deactivate */                        \
    faacenc_TI_free, /* free */                              \
    faacenc_TI_initObj, /* init */                           \
    NULL,               /* moved */                             \
    NULL                /* numAlloc (NULL => IALG_MAXMEMRECS) */

/*
 *  ======== AUDENCCOPY_TI_IAUDENCCOPY ========
 *  This structure defines TI's implementation of the IAUDENC interface
 *  for the AUDENCCOPY_TI module.
 */
IAUDENC_Fxns faacenc_TI_faacenc = {    /* module_vendor_interface */
    {IALGFXNS},
    faacenc_TI_process,
    faacenc_TI_control,
};

/*
 *  ======== AUDENCCOPY_TI_IALG ========
 *  This structure defines TI's implementation of the IALG interface
 *  for the AUDENCCOPY_TI module.
 */
#ifdef __TI_COMPILER_VERSION__
/* satisfy XDAIS symbol requirement without any overhead */
#if defined(__TI_ELFABI__) || defined(__TI_EABI_SUPPORT__)

/* Symbol doesn't have any leading underscores */
asm("faacenc_TI_IALG .set faacenc_TI_faacenc");

#else

/* Symbol has a single leading underscore */
asm("_faacenc_TI_IALG .set _faacenc_TI_faacenc");

#endif
#else

/*
 *  We duplicate the structure here to allow this code to be compiled and
 *  run using non-TI toolchains at the expense of unnecessary data space
 *  consumed by the definition below.
 */
IALG_Fxns faacenc_TI_IALG = {      /* module_vendor_interface */
    IALGFXNS
};

#endif

/*
 *  ======== AUDENCCOPY_TI_alloc ========
 */
Int faacenc_TI_alloc(const IALG_Params *algParams,
    IALG_Fxns **pf, IALG_MemRec memTab[])
{
    /* Request memory for my object */
    memTab[0].size = sizeof(faacenc_TI_Obj);
    memTab[0].alignment = 0;
    memTab[0].space = IALG_EXTERNAL;
    memTab[0].attrs = IALG_PERSIST;

    return (1);
}


static void * hfaac = 0;
static unsigned long inputSamples;
static unsigned long maxOutputBytes;

/*
 *  ======== AUDENCCOPY_TI_free ========
 *  Return a table of memory pointers that should be freed.  Note
 *  that this should include *all* memory requested in the
 *  alloc operation above.
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int faacenc_TI_free(IALG_Handle handle, IALG_MemRec memTab[])
{
    /*
     * Because our internal object size doesn't depend on create
     * params, we can just leverage the algAlloc() call with default
     * (NULL) create params.
     */
     faacenc_TI_Obj * obj = (faacenc_TI_Obj *)memTab[0].base;

	faacEnc_Close(hfaac);
	
    return (faacenc_TI_alloc(NULL, NULL, memTab));
}


/*
 *  ======== AUDENCCOPY_TI_initObj ========
 *  Initialize the memory allocated on our behalf (including our object).
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int faacenc_TI_initObj(IALG_Handle handle, const IALG_MemRec memTab[],
        IALG_Handle p, const IALG_Params *algParams)
{
	faacenc_TI_Obj * obj = (faacenc_TI_Obj *)memTab[0].base;

#if 0
	Log_print0(Diags_ERROR,"faacenc_TI_initObj!!!!!!\n");

	obj->hfaac = faacEnc_Open(LTP, 44100, 2, 192000,&obj->inputSamples, &obj->maxOutputBytes);

	if(obj->hfaac==0){
		Log_print0(Diags_ERROR,"faacEnc_Open failed!!!!!!\n");
	}
		//return IALG_EFAIL;
#endif

	if(hfaac==0){
		hfaac = faacEnc_Open(LOW, 16000, 1, 100000,&inputSamples, &maxOutputBytes);
		DBG_SETOFFSET(4,hfaac);
	}
    return (IALG_EOK);
}


#define SAMPLE_BYTES 4
/*
 *  ======== AUDENCCOPY_TI_process ========
 */
XDAS_Int32 faacenc_TI_process(IAUDENC_Handle h, XDM_BufDesc *inBufs,
    XDM_BufDesc *outBufs, IAUDENC_InArgs *inArgs, IAUDENC_OutArgs *outArgs)
{
    XDAS_Int32 curBuf;
    XDAS_Int32 minSamples;
	//faacenc_TI_Obj * obj = (faacenc_TI_Obj *)h;
	int ret = 0;

DBG_SET(21);	


    /* validate arguments - this codec only supports "base" XDM. */
    if ((inArgs->size != sizeof(*inArgs)) ||
        (outArgs->size != sizeof(*outArgs))) {

        outArgs->extendedError = XDM_UNSUPPORTEDPARAM;

        return (IAUDENC_EFAIL);
    }
	
DBG_SET(22);
    /* outargs->outputBufferSize reports the total number of bytes generated */
    outArgs->bytesGenerated = 0;

    for (curBuf = 0; (curBuf < inBufs->numBufs) &&
        (curBuf < outBufs->numBufs); curBuf++) {
DBG_SET(23);
#if 0
        /* there's an available in and out buffer, how many samples? */
        minSamples = inBufs->bufSizes[curBuf] < outBufs->bufSizes[curBuf] ?
            inBufs->bufSizes[curBuf] : outBufs->bufSizes[curBuf];

        /* process the data: read input, produce output */
        memcpy(outBufs->bufs[curBuf], inBufs->bufs[curBuf], minSamples);
#endif
	DBG_SETOFFSET(8,inputSamples);
DBG_SETOFFSET(12,maxOutputBytes);	

	ret = faacEnc_Encode(hfaac, (short * )inBufs->bufs[curBuf], inputSamples, (unsigned char *)outBufs->bufs[curBuf], maxOutputBytes);

		//if(ret!=obj->inputSamples*SAMPLE_BYTES)return IAUDENC_EFAIL;
//DBG_SET(24);
        outArgs->bytesGenerated += ret;
    }

DBG_SET((int)hfaac);

    /* Fill out the rest of the outArgs struct */
    outArgs->extendedError = 0;


    return (IAUDENC_EOK);
}


/*
 *  ======== AUDENCCOPY_TI_control ========
 */
XDAS_Int32 faacenc_TI_control(IAUDENC_Handle handle, IAUDENC_Cmd id,
    IAUDENC_DynamicParams *params, IAUDENC_Status *status)
{
    XDAS_Int32 retVal;
	faacenc_TI_Obj * obj = (faacenc_TI_Obj *)handle;

    /* validate arguments - this codec only supports "base" xDM. */
    if ((params->size != sizeof(*params)) ||
        (status->size != sizeof(*status))) {

        return (IAUDENC_EFAIL);
    }

    switch (id) {
        case XDM_GETSTATUS:
            status->extendedError = 0;

            status->frameLen = 0;  /* TODO */

            /* Note, intentionally no break here so we fill in bufInfo, too */

        case XDM_GETBUFINFO:
            status->bufInfo.minNumInBufs = MININBUFS;
            status->bufInfo.minNumOutBufs = MINOUTBUFS;
            status->bufInfo.minInBufSize[0] = inputSamples * SAMPLE_BYTES;
            status->bufInfo.minOutBufSize[0] = maxOutputBytes;

            retVal = IAUDENC_EOK;

            break;

        case XDM_SETPARAMS:
        case XDM_SETDEFAULT:
        case XDM_RESET:
        case XDM_FLUSH:
            /* TODO - for now just return success. */

            retVal = IAUDENC_EOK;
            break;

        default:
            /* unsupported cmd */
            retVal = IAUDENC_EFAIL;

            break;
    }

    return (retVal);
}
/*
 *  @(#) ti.sdo.ce.examples.codecs.audenc_copy; 1, 0, 0,1; 2-24-2012 19:28:51; /db/atree/library/trees/ce/ce-t06/src/ xlibrary

 */

