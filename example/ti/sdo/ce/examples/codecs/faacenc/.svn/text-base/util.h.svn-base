/*
 * FAAC - Freeware Advanced Audio Coder
 * Copyright (C) 2001 Menno Bakker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: util.h,v 1.8 2003/12/20 04:32:48 stux Exp $
 */

#ifndef UTIL_H
#define UTIL_H

#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Memory.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//#include <stdlib.h>
//#include <memory.h>

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

/* Memory functions */
#define AllocMemory(size) (void*)malloc(size)	//(void*) Memory_calloc(0,size,sizeof(size),0) 
#define FreeMemory(block) free(block) //Memory_free(block,sizeof(block),0) //	//Memory_free(block)
#define SetMemory(block, value, size) memset(block, value, size)

#if 0
extern unsigned int  g_ptr ;
inline void * AllocMemory(size)
{
	
	g_ptr = g_ptr + size;

	return (void *)(g_ptr - size);
}
#endif

#define DBG_SET(val) do{/**(volatile int *)0x8a500000 = (int)val;*/}while(0)
#define DBG_SETOFFSET(off,val) do{/**(volatile int *)(0x8a500000+(off)) = (int)(val);*/}while(0)

int GetSRIndex(unsigned int sampleRate);
int GetMaxPredSfb(int samplingRateIdx);
unsigned int MaxBitrate(unsigned long sampleRate);
unsigned int MinBitrate();
unsigned int MaxBitresSize(unsigned long bitRate, unsigned long sampleRate);
unsigned int BitAllocation(double pe, int short_block);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UTIL_H */
