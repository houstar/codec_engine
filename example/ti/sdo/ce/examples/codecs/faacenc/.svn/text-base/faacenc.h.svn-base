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
 * $Id: frame.h,v 1.30 2009/01/25 18:50:32 menno Exp $
 */

#ifndef FAAC_ENC_H
#define FAAC_ENC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	void* faacEnc_Open(unsigned int ObjectType,
		unsigned int sampleRate,
		unsigned int numChannels,
		unsigned int bitRate,
                                  unsigned long *inputSamples,
                                  unsigned long *maxOutputBytes);

int  faacEnc_Encode(void* hEncoder,
                          short *inputBuffer,
                          unsigned int samplesInput,
                          unsigned char *outputBuffer,
                          unsigned int bufferSize
                          );

int  faacEnc_Close(void* hEncoder);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAAC_ENC_H */
