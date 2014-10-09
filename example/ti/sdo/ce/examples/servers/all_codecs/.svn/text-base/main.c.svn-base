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
 *  ======== main.c ========
 */
/**
 *  @file       ti/sdo/ce/examples/servers/all_codecs/main.c
 */
#include <xdc/std.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

#include <ti/sdo/ce/CERuntime.h>
#include <ti/sysbios/BIOS.h>
#include <ti/ipc/Ipc.h>

#include <xdc/runtime/Error.h>
#include <ti/sysbios/hal/Timer.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>

/*
 *  ======== main ========
 */
 
//extern void RunThread_speex();

Void hwiFxn(UArg arg) 
{
  //RunThread_speex();
  //static int i = 0;
	//*(volatile int *)(0x89500000) = i++;
}

Void task_func1(UArg arg0, UArg arg1)
{
    
    while (1) {
        *(volatile int *)(0x89500000) = 2;       
        Task_sleep(300);
    }
}
Void task_func2(UArg arg0, UArg arg1)
{
    while (1) {
        *(volatile int *)(0x89500000) = 1;
        Task_sleep(200);
    }
}
Void main(Int argc, Char *argv[])
{ 
    Int status;
#if 0
    Timer_Handle timer;
    Timer_Params timerParams;    
    Error_Block eb;
    Error_init(&eb);

    Clock_Params clockParams;
    Clock_Handle myClock;
    Clock_Params_init(&clockParams);
    clockParams.period = 20;
    clockParams.startFlag = TRUE;
    clockParams.arg = (UArg)0x5555;
    myClock = Clock_create(hwiFxn, 5, &clockParams, &eb);
#endif
#if 0
    Semaphore_Handle sem;
    Task_Handle tsk1;
    Task_Handle tsk2;

    Task_Params taskParams;
    sem = Semaphore_create(1, NULL, NULL);

    Task_Params_init(&taskParams);
    taskParams.priority = 1;
    tsk1 = Task_create(task_func1, &taskParams, NULL);

    Task_Params_init(&taskParams);
    taskParams.priority = 2;
    tsk2 = Task_create(task_func2, &taskParams, NULL);
#endif
    
    do {
        /* init IPC */
        status = Ipc_start();
    } while (status < 0);

    /* init Codec Engine */
    CERuntime_init();
    
    Log_print0(Diags_ENTRY, "Welcome to Server's main()");
    
    BIOS_start();
}
/*
 *  @(#) ti.sdo.ce.examples.servers.all_codecs; 1, 0, 0,1; 2-24-2012 19:29:47; /db/atree/library/trees/ce/ce-t06/src/ xlibrary

 */

