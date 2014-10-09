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
 * $Id: frame.c,v 1.67 2004/11/17 14:26:06 menno Exp $
 */

/*
 * CHANGES:
 *  2001/01/17: menno: Added frequency cut off filter.
 *  2001/02/28: menno: Added Temporal Noise Shaping.
 *  2001/03/05: menno: Added Long Term Prediction.
 *  2001/05/01: menno: Added backward prediction.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "frame.h"
#include "faacenc.h"
#include "util.h"



void* faacEnc_Open(unsigned int ObjectType,
				   unsigned int sampleRate,
				    unsigned int numChannels,
					unsigned int bitRate,
				   unsigned long *inputSamples,
				   unsigned long *maxOutputBytes)
{
	faacEncHandle sts =  faacEncOpen(sampleRate,numChannels,inputSamples,maxOutputBytes);
	faacEncConfigurationPtr myFormat = faacEncGetCurrentConfiguration( sts) ; 
	myFormat->aacObjectType =  ObjectType;
	myFormat->mpegVersion =  MPEG4;
	myFormat->useTns =  0 ;
	myFormat->shortctl =  0 ;
	myFormat->useLfe =  1 ;
	myFormat->allowMidside =  1 ;
	myFormat->bitRate =  bitRate ;
	myFormat->bandWidth =  0 ;
	myFormat->quantqual =  100 ;
	myFormat->outputFormat =  1 ;
	myFormat->inputFormat =  FAAC_INPUT_16BIT ;

	if( !faacEncSetConfiguration( sts ,myFormat ) )         //  *************(¹Ø×¢)***************
	{
		DBG_SET(40);
		return NULL ;
	}

	return (void*)sts;
}

int  faacEnc_Encode(void* hEncoder,
					short *inputBuffer,
					unsigned int samplesInput,
					unsigned char *outputBuffer,
					unsigned int bufferSize
					)
{
	return faacEncEncode((faacEncHandle)hEncoder,inputBuffer,samplesInput,outputBuffer,bufferSize);
}

int  faacEnc_Close(void* hEncoder)
{
	return faacEncClose((faacEncHandle)hEncoder);
}

