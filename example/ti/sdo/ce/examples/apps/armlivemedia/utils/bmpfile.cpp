#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmpfile.h"


int BMPUtils::genBmpFile(U8* pData, U8 bitCountPerPix,
	U32 width, U32 height, const char* filename) {
	FILE* fp = fopen(filename, "wb");
	if (!fp) {
		return 0;
	}
	U32 bmppitch = ((width * bitCountPerPix + 31) >> 5 ) << 2;
	U32 filesize = bmppitch * height;

	BITMAPFILE bmpfile;
	bmpfile.bfHeader.bfType = 0x4D42;
	bmpfile.bfHeader.bfSize = filesize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpfile.bfHeader.bfReserved1 = 0;
	bmpfile.bfHeader.bfReserved2 = 0;
	bmpfile.bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bmpfile.biInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpfile.biInfo.bmiHeader.biWidth = width;
	bmpfile.biInfo.bmiHeader.biHeight = height;
	bmpfile.biInfo.bmiHeader.biPlanes = 1;
	bmpfile.biInfo.bmiHeader.biBitCount =bitCountPerPix;
	bmpfile.biInfo.bmiHeader.biCompression = 0;
	bmpfile.biInfo.bmiHeader.biSizeImage = 0;
	bmpfile.biInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpfile.biInfo.bmiHeader.biYPelsPerMeter = 0;
	bmpfile.biInfo.bmiHeader.biClrUsed = 0;
	bmpfile.biInfo.bmiHeader.biClrImportant = 0;

	fwrite(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, fp);

	U8* pEachLinBuf = (U8*)malloc(bmppitch);
	memset(pEachLinBuf, 0, bmppitch);
	U8 BytePerPix = bitCountPerPix >> 3;
	U32 pitch = width * BytePerPix;
	if (pEachLinBuf) {
		for (int h = height - 1; h >= 0; --h) {
			for (int w = 0; w < width; ++w) {
				pEachLinBuf[w* BytePerPix + 0] = pData[h*pitch + w* BytePerPix + 0];
				pEachLinBuf[w* BytePerPix + 1] = pData[h*pitch + w* BytePerPix + 1];
				pEachLinBuf[w* BytePerPix + 2] = pData[h*pitch + w* BytePerPix + 2];
			}
			fwrite(pEachLinBuf, bmppitch, 1, fp);
		}
		free(pEachLinBuf);
	}
	fclose(fp);
	return 1;
}

U8* BMPUtils::getBmpData(U8* bitCountPerPix,
		U32* width, U32* height, const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return NULL;
	}

	BITMAPFILE bmpfile;
	fread(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, fp);

	if (bitCountPerPix) {
		*bitCountPerPix = bmpfile.biInfo.bmiHeader.biBitCount;
	}
	if (width) {
		*width = bmpfile.biInfo.bmiHeader.biWidth;
	}
	if (height) {
		*height = bmpfile.biInfo.bmiHeader.biHeight;
	}

	U32 bmppitch = (((*width) * (*bitCountPerPix) + 31 ) >> 5) << 2;
	U8* pdata = (U8*)malloc((*height) * bmppitch);

	U8* pEachLinBuf = (U8*) malloc(bmppitch);
	memset(pEachLinBuf, 0, bmppitch);

	U8 BytePerPix = (*bitCountPerPix) >> 3;
	U32 pitch = (*width) * BytePerPix;

	if (pdata && pEachLinBuf) {
		for (int h = (*height) - 1; h >= 0; --h) {
			fread(pEachLinBuf, bmppitch, 1, fp);
			for (int w = 0; w < (*width); ++w) {
				pdata[h * pitch + w * BytePerPix + 0] = pEachLinBuf[w*BytePerPix + 0];
				pdata[h * pitch + w * BytePerPix + 1] = pEachLinBuf[w*BytePerPix + 1];
				pdata[h * pitch + w * BytePerPix + 2] = pEachLinBuf[w*BytePerPix + 2];
			}
		}
		free(pEachLinBuf);
	}

	fclose(fp);
	return pdata;

}

void BMPUtils::freeBmpData(U8* pdata) {
	if (pdata) {
		free(pdata);
		pdata = NULL;
	}
}

