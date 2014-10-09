#include "colorConvert.h"

#ifdef WORDS_BIGENDIAN
#define RGBDO {s++; r=*(s++); g=*(s++); b=(*s++);}
#else
#define RGBDO {b=(*s++); g=*(s++); r=*(s++); s++;}
#endif
#define Y(r,g,b) (((16829*r+31913*g+6416*b)>>16)+16)
#define U(r,g,b) (((-9750*r-19148*g+28898*b)>>16)+128)
#define V(r,g,b) (((28898*r-24199*g-4699*b)>>16)+128)
#define sat(x,max) if(x<0) x=0; if(x>max) x=max;

#ifdef WORDS_BIGENDIAN
#define RGBDO2 {pix_yuv_to_rgb(*y,*u,*v,&d[1],&d[2],&d[3]);}
#else
#define RGBDO2 {pix_yuv_to_rgb(*y,*u,*v,&d[2],&d[1],&d[0]);}
#endif

void ColorConvertUtils::rgb4b_to_rgb32(unsigned char *dest, unsigned char *s,
		int w, int h) {
	int i, *d = (int *) dest;
	for (i = w * h; i > 0; i--, d++, s++) {
		unsigned int c = *s;
		*d = ((c & 0x3) << (16 + 6)) | (((c >> 2) & 0x1) << (8 + 7))
				| (c >> 3 << 7);
	}
}

void ColorConvertUtils::rgb8b_to_rgb32(unsigned char *dest, unsigned char *s,
		int w, int h) {
	int i, *d = (int *) dest;
	for (i = w * h; i > 0; i--, d++, s++) {
		unsigned int c = *s;
		*d = ((c & 0x7) << (16 + 5)) | (((c >> 3) & 0x7) << (8 + 5))
				| (c >> 6 << 6);
	}
}

void ColorConvertUtils::rgb32_to_rgb8b(unsigned char *d, unsigned char *src,
		int w, int h) {
	int i, *s = (int *) src;
	for (i = w * h; i > 0; i--, d++, s++) {
		unsigned int c = *s;
		*d = (c >> (16 + 5)) | ((c & 0xFF00) >> (8 + 5) << 3)
				| ((c & 0xFF) >> 6 << 6);
	}
}

void ColorConvertUtils::rgb8_to_rgb32(unsigned char *dest, unsigned char *s,
		int w, int h) {
	int i, *d = (int *) dest;
	for (i = w * h; i > 0; i--, d++, s++) {
		unsigned int c = *s;
		*d = ((c >> 6) << (16 + 6)) | (((c >> 3) & 0x7) << (8 + 5))
				| ((c & 0x7) << 5);
	}
}

void ColorConvertUtils::rgb32_to_rgb8(unsigned char *d, unsigned char *src,
		int w, int h) {
	int i, *s = (int *) src;
	for (i = w * h; i > 0; i--, d++, s++) {
		unsigned int c = *s;
		*d = ((c >> (16 + 6)) << 6) | ((c & 0xFF00) >> (8 + 5) << 3)
				| ((c & 0xFF) >> 5);
	}
}

void ColorConvertUtils::rgb15_to_rgb32(unsigned char *dest, unsigned char *src,
		int w, int h) {
	int i, *d = (int *) dest;
	short *s = (short *) src;
	for (i = w * h; i > 0; i--, d++, s++) {
		unsigned int c = *s;
		*d = ((c >> 10) << (16 + 3)) | (((c >> 5) & 0x1F) << (8 + 3))
				| ((c & 0x1F) << 3);
	}
}

void ColorConvertUtils::rgb32_to_rgb15(unsigned char *dest, unsigned char *src,
		int w, int h) {
	int i, *s = (int *) src;
	short *d = (short *) dest;
	for (i = w * h; i > 0; i--, d++, s++) {
		unsigned int c = *s;
		*d = ((c >> (16 + 3)) << 10) | ((c & 0xFF00) >> (8 + 3) << 5)
				| ((c & 0xFF) >> 3);
	}
}

void ColorConvertUtils::rgb16_to_rgb32(unsigned char *dest, unsigned char *src,
		int w, int h) {
	int i, *d = (int *) dest;
	short *s = (short *) src;
	for (i = w * h; i > 0; i--, d++, s++) {
		unsigned int c = *s;
		*d = ((c >> 11) << (16 + 3)) | (((c >> 5) & 0x3F) << (8 + 2))
				| ((c & 0x1F) << 3);
	}
}

void ColorConvertUtils::rgb32_to_rgb16(unsigned char *dest, unsigned char *src,
		int w, int h) {
	int i, *s = (int *) src;
	short *d = (short *) dest;
	for (i = w * h; i > 0; i--, d++, s++) {
		unsigned int c = *s;
		*d = ((c >> (16 + 3)) << 11) | ((c & 0xFF00) >> (8 + 2) << 5)
				| ((c & 0xFF) >> 3);
	}
}

void ColorConvertUtils::rgb24_to_rgb32(unsigned char *dest, unsigned char *src,
		int w, int h) {
	int i = w * h;

	while (i--) {
#ifdef WORDS_BIGENDIAN
		*(dest++) = 0;
#endif
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		*(dest++) = *(src++);
#ifndef WORDS_BIGENDIAN
		*(dest++) = 0;
#endif
	}
}

void ColorConvertUtils::rgb32_to_rgb24(unsigned char *dest, unsigned char *src,
		int w, int h) {
	int i = w * h;

	while (i--) {
#ifdef WORDS_BIGENDIAN
		src++;
#endif
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		*(dest++) = *(src++);
#ifndef WORDS_BIGENDIAN
		src++;
#endif
	}
}

void ColorConvertUtils::gray1_to_gray8(unsigned char *d, unsigned char *s,
		int w, int h) {
	int x, y, z, dpad = ((w + 31) / 32 * 4 - w / 8);
	/* the width must be multiple of 32,
	 (just assumed to be multiple of 4 in xdtv, see x11.c) */
	for (y = h; y > 0; y--) {
		for (x = w; x > 7; x -= 8, s++, d += 8)
			for (z = 0; z < 8; z++)
#ifdef WORDS_BIGENDIAN
				d[z]=(*s&(1<<(8-z)))?255:0;
#else
				d[z] = (*s & (1 << z)) ? 255 : 0;
#endif
		s += dpad;
	}
}

void ColorConvertUtils::gray8_to_gray1(unsigned char *d, unsigned char *s,
		int w, int h) {
	int x, y, dpad = ((w + 31) / 32 * 4 - w / 8);
	/* the width must be multiple of 32,
	 (just assumed to be multiple of 4 in xdtv, see x11.c) */
	for (y = h; y > 0; y--) {
		for (x = w; x > 7; x -= 8, d++, s += 8)
#ifdef WORDS_BIGENDIAN
			*d=(s[7]>>7)|(s[6]>>7<<1)|(s[5]>>7<<2)|(s[4]>>7<<3)|(s[3]>>7<<4)|(s[2]>>7<<5)
			|(s[1]>>7<<6)|(s[0]>>7<<7);
#else
			*d = (s[0] >> 7) | (s[1] >> 7 << 1) | (s[2] >> 7 << 2)
					| (s[3] >> 7 << 3) | (s[4] >> 7 << 4) | (s[5] >> 7 << 5)
					| (s[6] >> 7 << 6) | (s[7] >> 7 << 7);
#endif
		d += dpad;
	}
}

void ColorConvertUtils::gray4_to_gray8(unsigned char *d, unsigned char *s,
		int w, int h) {
	int i;
	for (i = w * h; i > 0; i--, d++, s++)
		*d = *s << 4;
}

void ColorConvertUtils::gray8_to_gray4(unsigned char *d, unsigned char *s,
		int w, int h) {
	int i;
	for (i = w * h; i > 0; i--, d++, s++)
		*d = *s >> 4;
}

void ColorConvertUtils::yuv420_to_gray8(unsigned char *dest, unsigned char *src,
		int width, int height) {
	memcpy(dest, src, width * height);
}

void ColorConvertUtils::gray8_to_yuv420(unsigned char *dest, unsigned char *src,
		int width, int height) {
	int size = width * height;
	memcpy(dest, src, size);
	dest += size;
	size /= 4;
	memset(dest, 128, size);
	dest += size;
	memset(dest, 128, size);
}

/* rgb4 is xdtv-specific ! (just for fun :>)*/
void ColorConvertUtils::rgb3_to_rgb32(unsigned char *dd, unsigned char *s,
		int w, int h) {
	unsigned int *d = (unsigned int *) dd;
	int i;
	for (i = w * h; i > 0; i--, d++, s++)
		*d = ((*s & 1) << (7 - 0)) | ((*s & 2) << (15 - 1))
				| ((*s & 4) << (23 - 2));
}
void ColorConvertUtils::rgb32_to_rgb3(unsigned char *d, unsigned char *ss,
		int w, int h) {
	unsigned int *s = (unsigned int *) ss;
	int i;
	for (i = w * h; i > 0; i--, d++, s++)
		*d = ((s[0] >> 7) & 1) | (((s[0] >> 15) & 1) << 1)
				| (((s[0] >> 23) & 1) << 2);
}

void ColorConvertUtils::hi240_to_rgb32(unsigned char *dest, unsigned char *src,
		int w, int h) {
	static int *rgb32_conv = NULL;
	int i;
	int *dest2 = (int *) dest;
	if (rgb32_conv == NULL) {
		rgb32_conv = (int *) malloc(256 * sizeof(int));
		for (i = 0; i < 225; i++) {
			int r = ((i / 5) % 5) * 255.0 / 4 + 0.5;
			int g = (i / 25) * 255.0 / 8 + 0.5;
			int b = (i % 5) * 255.0 / 4 + 0.5;
			rgb32_conv[16 + i] = (b << 0) | (g << 8) | (r << 16);
		}
	}
	for (i = w * h; i > 0; i--, dest2++, src++)
		*dest2 = rgb32_conv[*src];
}

void ColorConvertUtils::rgb32_to_hi240(unsigned char *d, unsigned char *s,
		int w, int h) {
	int i;
	for (i = w * h; i > 0; i--, d++, s += 4) {
#ifdef WORDS_BIGENDIAN
		*d = 16 + s[3]/52 + s[1]/52*5 + s[2]/29*25;
#else
		*d = 16 + s[0] / 52 + s[2] / 52 * 5 + s[1] / 29 * 25;
#endif
	}
}

void ColorConvertUtils::yvu420_yuv420(unsigned char *d, unsigned char *s, int w,
		int h) {
	unsigned char *as, *bs, *ad, *bd;
	int t = w * h / 4;
	as = s + t * 4;
	bs = as + t;
	ad = d + t * 4;
	bd = ad + t;
	memcpy(d, s, t * 4);
	memcpy(ad, bs, t);
	memcpy(bd, as, t);
}

void ColorConvertUtils::rgb32_rgb32b(unsigned char *d, unsigned char *s, int w,
		int h) {
	int t = 4 * w, y;
	s += t * (h - 1);
	for (y = h; y > 0; y--) {
		memcpy(d, s, t);
		d += t;
		s -= t;
	}
}

void ColorConvertUtils::rgb24_rgb24b(unsigned char *d, unsigned char *s, int w,
		int h) {
	int t = 3 * w, y;
	s += t * (h - 1);
	for (y = h; y > 0; y--) {
		memcpy(d, s, t);
		d += t;
		s -= t;
	}
}

void ColorConvertUtils::swap1bpp32(unsigned char *dest, unsigned char *src,
		int w, int h) {
	int *d = (int *) dest, *s = (int *) src, ss, dd, i, j;
	for (i = w * h / 32; i > 0; i--, s++, d++) {
		ss = *s;
		dd = 0;
		for (j = 0; j < 32; j++)
			if (ss & (1 << j))
				dd |= (1 << (32 - j));
		*d = dd;
	}
}

void ColorConvertUtils::swap16bpp(unsigned char *d, unsigned char *s, int w,
		int h) {
	int i;
	//swab(d, s, 2*w*h);
	for (i = w * h; i > 0; i--, s += 2, d += 2) {
		d[0] = s[1];
		d[1] = s[0];
	}
}

void ColorConvertUtils::swap24bpp(unsigned char *d, unsigned char *s, int w,
		int h) {
	int i;
	for (i = w * h; i > 0; i--, s += 3, d += 3) {
		d[0] = s[2];
		d[1] = s[1];
		d[2] = s[0];
	}
}

 void ColorConvertUtils::swap32bpp(unsigned char *d, unsigned char *s, int w, int h) {
	int i;
	for (i = w * h; i > 0; i--, s += 4, d += 4) {
		d[0] = s[3];
		d[1] = s[2];
		d[2] = s[1];
		d[3] = s[0];
	}
}

void ColorConvertUtils::swap24(unsigned char *m, int t) {
	int i;
	unsigned char c;
	for (i = t; i > 0; i--, m += 3) {
		c = m[0];
		m[0] = m[2];
		m[2] = c;
	}
}

 void ColorConvertUtils::yuyv_to_yuv420(unsigned char *d, unsigned char *s, int w, int h) {
	int a, b;
	unsigned char *y, *u, *v;
	y = d;
	u = y + w * h;
	v = u + w * h / 4;

	for (a = h; a > 0; a -= 2) {
		for (b = w; b > 0; b -= 2) {
			*(y++) = *(s++);
			*(u++) = *(s++);
			*(y++) = *(s++);
			*(v++) = *(s++);
		}
		for (b = w; b > 0; b -= 2) {
			*(y++) = *(s++);
			s++;
			*(y++) = *(s++);
			s++;
		}
	}
}

 void ColorConvertUtils::yuv420_to_yuyv(unsigned char *d, unsigned char *s, int w, int h) {
	unsigned char *y, *u, *v, *u2, *v2;
	int a, b;
	y = s;
	u = y + w * h;
	v = u + w * h / 4;
	for (b = h; b > 0; b -= 2) {
		u2 = u;
		v2 = v;
		for (a = w; a > 0; a -= 2) {
			*(d++) = *(y++);
			*(d++) = *(u++);
			*(d++) = *(y++);
			*(d++) = *(v++);
		}
		u = u2;
		v = v2;
		for (a = w; a > 0; a -= 2) {
			*(d++) = *(y++);
			*(d++) = *(u++);
			*(d++) = *(y++);
			*(d++) = *(v++);
		}
	}
}

/* NOT VERIFIED */
/* w must be a multiple of 8 ! */
 void ColorConvertUtils::y41p_to_yuyv(unsigned char *d, unsigned char *s, int w, int h) {
	int i;
	for (i = w * h; i > 0; i -= 8, s += 12, d += 16) {
		d[5] = d[1] = s[0];
		d[0] = s[1];
		d[7] = d[3] = s[2];
		d[2] = s[3];
		d[9] = d[13] = s[4];
		d[4] = s[5];
		d[11] = d[15] = s[6];
		d[6] = s[7];
		d[8] = s[8];
		d[10] = s[9];
		d[12] = s[10];
		d[14] = s[11];
	}
}

/* NOT VERIFIED */
 void ColorConvertUtils::yuyv_to_y41p(unsigned char *d, unsigned char *s, int w, int h) {
	int i;
	for (i = w * h; i > 0; i -= 8, s += 16, d += 12) {
		d[0] = s[1];
		d[1] = s[0];
		d[2] = s[3];
		d[3] = s[2];
		d[4] = s[9];
		d[5] = s[4];
		d[6] = s[11];
		d[7] = s[6];
		d[8] = s[8];
		d[9] = s[10];
		d[10] = s[12];
		d[11] = s[14];
	}
}

 void ColorConvertUtils::y410_to_y420(unsigned char *d, unsigned char *s, int w, int h) {
	int t = w * h, i, x, y, w4 = w / 4;
	memcpy(d, s, t);
	s += t;
	d += t;
	for (i = 2; i > 0; i--)
		for (y = h; y > 0; y -= 4) {
			unsigned char *s0 = s;
			for (x = w4; x > 0; x--) {
				*(d++) = *s;
				*(d++) = *(s++);
			}
			s = s0;
			for (x = w4; x > 0; x--) {
				*(d++) = *s;
				*(d++) = *(s++);
			}
		}
}

 void ColorConvertUtils::y420_to_y410(unsigned char *d, unsigned char *s, int w, int h) {
	int i, t = w * h, x, y, w4 = w / 4;
	memcpy(d, s, t);
	s += t;
	d += t;
	for (i = 2; i > 0; i--)
		for (y = h; y > 0; y -= 4) {
			for (x = w4; x > 0; x--) {
				*(d++) = *s;
				s += 2;
			}
			s += 2 * w4;
			;
		}
}

 void ColorConvertUtils::yuyv_to_yuv422p(unsigned char *d, unsigned char *s, int w, int h) {
	int a, b;
	unsigned char *y, *u, *v;
	y = d;
	u = y + w * h;
	v = u + w * h / 2;
	for (a = h; a > 0; a--)
		for (b = w; b > 0; b -= 2) {
			*(y++) = *(s++);
			*(u++) = *(s++);
			*(y++) = *(s++);
			*(v++) = *(s++);
		}
}

 void ColorConvertUtils::yuv422p_to_yuyv(unsigned char *d, unsigned char *s, int w, int h) {
	unsigned char *y, *u, *v;
	int a, b;
	y = s;
	u = y + w * h;
	v = u + w * h / 2;
	for (b = h; b > 0; b--)
		for (a = w; a > 0; a -= 2) {
			*(d++) = *(y++);
			*(d++) = *(u++);
			*(d++) = *(y++);
			*(d++) = *(v++);
		}
}

 void ColorConvertUtils::yuv411p_to_yuv422p(unsigned char *d, unsigned char *s, int w,
		int h) {
	int i, t = w * h;
	memcpy(d, s, t);
	d += t;
	s += t;
	for (i = t / 2; i > 0; i--) {
		*(d++) = *s;
		*(d++) = *(s++);
	}
}

 void ColorConvertUtils::yuv422p_to_yuv411p(unsigned char *d, unsigned char *s, int w,
		int h) {
	int i, t = w * h;
	memcpy(d, s, t);
	d += t;
	s += t;
	for (i = t / 2; i > 0; i--) {
		*(d++) = *s;
		s += 2;
	}
}

/* NOT VERIFIED */
 void ColorConvertUtils::nv12_to_yuv420(unsigned char *d, unsigned char *s, int w, int h) {
	int i, t = w * h;
	unsigned char *y, *u, *v;
	y = d;
	u = y + t;
	v = u + t / 4;
	memcpy(d, s, t);
	s += t;
	for (i = t / 4; i > 0; i--) {
		*(u++) = *(s++);
		*(v++) = *(s++);
	}
}

/* NOT VERIFIED */
 void ColorConvertUtils::yuv420_to_nv12(unsigned char *d, unsigned char *s, int w, int h) {
	int i, t = w * h;
	unsigned char *y, *u, *v;
	y = s;
	u = y + t;
	v = u + t / 4;
	memcpy(d, s, t);
	d += t;
	for (i = t / 4; i > 0; i--) {
		*(d++) = *(u++);
		*(d++) = *(v++);
	}
}


 void ColorConvertUtils::rgb32_to_yuv420(unsigned char *d, unsigned char *s, int w, int h) {
	int aa, bb;
	unsigned char *y, *u, *v, r, g, b;
	y = d;
	u = y + w * h;
	v = u + w * h / 4;
	for (aa = h; aa > 0; aa -= 2) {
		for (bb = w; bb > 0; bb -= 2) {
			RGBDO;
			*(y++) = Y(r,g,b);
			*(u++) = U(r,g,b);
			RGBDO;
			*(y++) = Y(r,g,b);
		}
		for (bb = w; bb > 0; bb -= 2) {
			RGBDO;
			*(y++) = Y(r,g,b);
			RGBDO;
			*(y++) = Y(r,g,b);
			*(v++) = V(r,g,b);
		}
	}
}

 void ColorConvertUtils::rgb32_to_yuyv(unsigned char *d, unsigned char *s, int w, int h) {
	int i;
	for (i = w * h; i > 0; i -= 2, s += 8, d += 4) {
		unsigned char r, g, b;
#ifdef WORDS_BIGENDIAN
		r=s[1]; g=s[2]; b=s[3];
#else
		b = s[0];
		g = s[1];
		r = s[2];
#endif
		d[0] = Y(r,g,b);
		d[1] = U(r,g,b);
#ifdef WORDS_BIGENDIAN
		r=s[5]; g=s[6]; b=s[7];
#else
		b = s[4];
		g = s[5];
		r = s[6];
#endif
		d[2] = Y(r,g,b);
		d[3] = V(r,g,b);
	}
}

 void ColorConvertUtils::argb32_to_yuyv(unsigned char *d, unsigned char *s, int w, int h) {
	int i;
	for (i = w * h; i > 0; i -= 2, s += 8, d += 4) {
		unsigned char r, g, b;
		unsigned char alpha;
#ifdef WORDS_BIGENDIAN
		r=s[1]; g=s[2]; b=s[3];
#else
		alpha = s[3];
		b = (s[0] * (int)alpha)/255;
		g = s[1] * (int)alpha/255;
		r = s[2] * (int)alpha/255;

#endif
		d[0] = Y(r,g,b);
		d[1] = U(r,g,b);
#ifdef WORDS_BIGENDIAN
		r=s[5]; g=s[6]; b=s[7];
#else
		alpha = s[7];
		b = s[4] * (int)alpha/255;
		g = s[5] * (int)alpha/255;
		r = s[6] * (int)alpha/255;
#endif
		d[2] = Y(r,g,b);
		d[3] = V(r,g,b);
	}
}

 void ColorConvertUtils::rgb24_to_yuyv(unsigned char *d, unsigned char *s, int w, int h) {
	int i;
	for (i = w * h; i > 0; i -= 2, s += 6, d += 4) {
		unsigned char r, g, b;
#ifdef WORDS_BIGENDIAN
		r=s[0]; g=s[1]; b=s[2];
#else
		b = s[0];
		g = s[1];
		r = s[2];
#endif
		d[0] = Y(r,g,b);
		d[1] = U(r,g,b);
#ifdef WORDS_BIGENDIAN
		r=s[3]; g=s[4]; b=s[5];
#else
		b = s[3];
		g = s[4];
		r = s[5];
#endif
		d[2] = Y(r,g,b);
		d[3] = V(r,g,b);
	}
}

  void ColorConvertUtils::pix_yuv_to_rgb(unsigned char y, unsigned char u,
		unsigned char v, unsigned char *r, unsigned char *g, unsigned char *b) {
	int y2, r2, g2, b2;
	y2 = 76309 * y;
	r2 = (-14556884 + y2 + 104187 * v) >> 16;
	sat(r2, 255);
	*r = r2;
	g2 = (8842838 + y2 - 25564 * u - 53060 * v) >> 16;
	sat(g2, 255);
	*g = g2;
	b2 = (-18076354 + y2 + 131683 * u) >> 16;
	sat(b2, 255);
	*b = b2;
}

 void ColorConvertUtils::yuv420_to_rgb32(unsigned char *d, unsigned char *s, int w, int h) {
	unsigned char *y, *u, *v, *u2, *v2;
	int a, b;
	y = s;
	u = y + w * h;
	v = u + w * h / 4;
	for (a = h; a > 0; a -= 2) {
		u2 = u;
		v2 = v;
		for (b = w; b > 0; b -= 2) {
			RGBDO2;
			y++;
			d += 4;
			RGBDO2;
			y++;
			u++;
			v++;
			d += 4;
		}
		u = u2;
		v = v2;
		for (b = w; b > 0; b -= 2) {
			RGBDO2;
			y++;
			d += 4;
			RGBDO2;
			y++;
			u++;
			v++;
			d += 4;
		}
	}
}

 void ColorConvertUtils::yuyv_to_rgb32(unsigned char *d, unsigned char *s, int width,
		int height) {
	int i;
	for (i = width * height; i > 0; i -= 2, s += 4, d += 8) {
#ifdef WORDS_BIGENDIAN
		pix_yuv_to_rgb(s[0],s[1],s[3],&d[1],&d[2],&d[3]);
		pix_yuv_to_rgb(s[2],s[1],s[3],&d[5],&d[6],&d[7]);
#else
		pix_yuv_to_rgb(s[0], s[1], s[3], &d[2], &d[1], &d[0]);
		pix_yuv_to_rgb(s[2], s[1], s[3], &d[6], &d[5], &d[4]);
#endif
	}
}
