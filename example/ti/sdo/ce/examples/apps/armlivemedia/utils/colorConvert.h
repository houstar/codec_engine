#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


enum EColorFormat{
	VIDEO_RGB03 = 0,
	VIDEO_RGB04b,
	VIDEO_RGB08,
	VIDEO_RGB08b,
	VIDEO_RGB15,
	VIDEO_RGB16,
	VIDEO_RGB24,
	VIDEO_RGB24B,
	VIDEO_RGB32,
	VIDEO_RGB32B,
	VIDEO_RGB32P,
	VIDEO_RGB15X,
	VIDEO_RGB16X,
	VIDEO_RGB24X,
	VIDEO_RGB32X,
	VIDEO_RGB32PX,
	VIDEO_HI240,
	VIDEO_GRAY1,
	VIDEO_GRAY1X,
	VIDEO_GRAY4,
	VIDEO_GRAY8,
	VIDEO_YUYV,
	VIDEO_UYVY,
	VIDEO_YUV420,
	VIDEO_YVU420,
	VIDEO_Y41P,
	VIDEO_YVU410,
	VIDEO_YUV410,
	VIDEO_YUV422P,
	VIDEO_YUV411P,
	VIDEO_NV12, //NV12 也就是yuv422sp

	VIDEO_INVALID = 0xFF
};


class ColorConvertUtils {
public:

	static void rgb4b_to_rgb32(unsigned char *dest,
			unsigned char *s, int w, int h);


	static void rgb8b_to_rgb32 (unsigned char *dest,
			unsigned char *s, int w, int h);


	static void rgb32_to_rgb8b (unsigned char *d,
			unsigned char *src, int w, int h);


	static void	rgb8_to_rgb32 (unsigned char *dest,
			unsigned char *s, int w, int h);

	static void rgb32_to_rgb8 (unsigned char *d,
			unsigned char *src, int w, int h);

	static void rgb15_to_rgb32 (unsigned char *dest,
			unsigned char *src, int w, int h);

	static void rgb32_to_rgb15 (unsigned char *dest,
			unsigned char *src, int w, int h);

	static void rgb16_to_rgb32 (unsigned char *dest,
			unsigned char *src, int w, int h);

	static void rgb32_to_rgb16 (unsigned char *dest,
			unsigned char *src, int w, int h);

	static void rgb24_to_rgb32 (unsigned char *dest,
			unsigned char *src, int w, int h);

	static void rgb32_to_rgb24 (unsigned char *dest,
			unsigned char *src, int w, int h);


	static void gray1_to_gray8(unsigned char *d,
			unsigned char *s, int w, int h);


	static void gray8_to_gray1(unsigned char *d,
			unsigned char *s, int w, int h);

	static void gray4_to_gray8(unsigned char *d,
			unsigned char *s, int w, int h);

	static void gray8_to_gray4(unsigned char *d,
			unsigned char *s, int w, int h);

	static void yuv420_to_gray8(unsigned char *dest,
			unsigned char *src, int width, int height);

	static void gray8_to_yuv420(unsigned char *dest,
			unsigned char *src, int width, int height);

/* rgb4 is xdtv-specific ! (just for fun :>)*/
	static void rgb3_to_rgb32(unsigned char *dd,
			unsigned char *s, int w, int h);

	static void rgb32_to_rgb3(unsigned char *d,
			unsigned char *ss, int w, int h);

	static void hi240_to_rgb32(unsigned char *dest,
			unsigned char *src, int w, int h);

	static void rgb32_to_hi240(unsigned char *d,
			unsigned char *s, int w, int h);

	static void yvu420_yuv420(unsigned char *d,
			unsigned char *s, int w, int h);

	static void rgb32_rgb32b(unsigned char *d,
			unsigned char *s, int w, int h) ;

	static void rgb24_rgb24b(unsigned char *d,
			unsigned char *s, int w, int h) ;

	static void swap1bpp32(unsigned char *dest,
			unsigned char *src, int w, int h);

	static void swap16bpp(unsigned char *d,
			unsigned char *s, int w, int h);

	static void swap24bpp(unsigned char *d,
			unsigned char *s, int w, int h);

	static void swap32bpp(unsigned char *d,
			unsigned char *s, int w, int h);

	static void swap24(unsigned char *m, int t) ;

	static void yuyv_to_yuv420 (unsigned char *d,
			unsigned char *s, int w, int h);

	static void yuv420_to_yuyv(unsigned char *d,
			unsigned char *s,int w, int h);

	/* NOT VERIFIED */
	/* w must be a multiple of 8 ! */
	static void y41p_to_yuyv(unsigned char *d,
			unsigned char *s,int w, int h);

	/* NOT VERIFIED */
	static void yuyv_to_y41p(unsigned char *d,
			unsigned char *s,int w, int h);

	static void y410_to_y420(unsigned char *d,
			unsigned char *s,int w, int h);

	static void y420_to_y410(unsigned char *d,
			unsigned char *s,int w, int h);

	static void yuyv_to_yuv422p (unsigned char *d,
			unsigned char *s, int w, int h);

	static void yuv422p_to_yuyv(unsigned char *d,
			unsigned char *s,int w, int h);

	static void yuv411p_to_yuv422p (unsigned char *d,
			unsigned char *s, int w, int h);

	static void yuv422p_to_yuv411p(unsigned char *d,
			unsigned char *s,int w, int h);

	/* NOT VERIFIED */
	static void nv12_to_yuv420 (unsigned char *d,
			unsigned char *s, int w, int h);

	/* NOT VERIFIED */
	static void yuv420_to_nv12(unsigned char *d,
			unsigned char *s,int w, int h);

	/******** RGB to YUV functions **********/
	static void rgb32_to_yuv420(unsigned char *d,
			unsigned char *s, int w, int h);

	static void rgb32_to_yuyv(unsigned char *d,
			unsigned char *s, int w, int h);

	static void argb32_to_yuyv(unsigned char* d, unsigned char* s, int w, int h);

	static void rgb24_to_yuyv(unsigned char *d,
			unsigned char *s, int w, int h);

	/****** YUV to RGB functions *******/
	static void pix_yuv_to_rgb(unsigned char y,
			unsigned char u,
			unsigned char v,
			unsigned char *r,
			unsigned char *g,
			unsigned char *b);


	static void yuv420_to_rgb32(unsigned char *d,
			unsigned char *s, int w, int h);

	static void yuyv_to_rgb32(unsigned char *d,
			unsigned char *s, int width, int height) ;

};


