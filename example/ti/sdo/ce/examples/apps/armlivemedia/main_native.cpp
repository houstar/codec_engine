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
 *  ======== main_native.c ========
 */
/**
 *  @file       ti/sdo/ce/examples/apps/audio_copy/main_native.c
 */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>

#include <ti/sdo/ce/CERuntime.h>
#include <ti/sdo/ce/Engine.h>

#include <unistd.h>
#include <stdlib.h>
#include "./core/baseMode.h"
#include "./core/audioEncodeInterface.h"

static String usage = "%s [-p procId] [-e engineName] [-m mapFileName] [-s serverSuffix] input-file output-file\n";

extern Int smain();
/*
 *  ======== main ========
 */
Int main(Int argc, String argv[])
{
#ifdef CODEC_ENGINE_ENABLE
    String procId;
    String procSuffix;
    String engineName;
	String encoderName;
    String mapFileName = NULL;
    String inFile;
    String outFile;
    Char defaultEngineName[32];
    Bool useExtLoader = FALSE;  // Set to TRUE if using external loader
    Engine_Error status;
    String options = "c:e:m:p:s:";
    Int option;
    Engine_Desc desc;

    // Initialize defaults.
    procId = "DSP";
    procSuffix = "xe674";
    engineName = defaultEngineName;
	encoderName = "speexenc";
    sprintf(defaultEngineName, "remote_copy_%s", procId);

    // init Codec Engine
    CERuntime_init();

    // Enable all trace for this "xdc.runtime.Main" module
    //Diags_setMask("xdc.runtime.Main+EX1234567");
   //Diags_setMask("EX1234567");

   // Log_print0(Diags_USER2, "[+2] main> Welcome to app's main().");

	Engine_Desc engDesc;
	char serverName[32];
	int retVal;
	
	Engine_initDesc(&engDesc);
    engDesc.name = engineName;
    engDesc.memMap = mapFileName;
    engDesc.useExtLoader = useExtLoader;
    sprintf(serverName, "all_%s.%s", procId, procSuffix);
    engDesc.remoteName = serverName;

    retVal = Engine_add(&engDesc);
    if (retVal != Engine_EOK) {
        Log_print1(Diags_ERROR, "main-> ERROR: Runtime Engine_add() "
                "failed (0x%x)\n", retVal);
        // don't continue
        while (1) {}
    }
#endif
	
    return (smain());
}
/*
 *  @(#) ti.sdo.ce.examples.apps.audio_copy; 1, 0, 0,1; 2-24-2012 19:27:46; /db/atree/library/trees/ce/ce-t06/src/ xlibrary

 */

