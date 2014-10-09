#include "osdManager.h"
#include <string.h>
#include <stdlib.h>

#include "../utils/bmpfile.h"

#ifndef MIN
#define MIN(a, b) ((a) > (b)) ? (b) : (a)
#endif

OSDManager::OSDManager()
	:	m_imageWidth(0), m_imageHeight(0),
	 	m_bksurface(NULL), m_bkcr(NULL),
	 	m_foresurface(NULL), m_forecr(NULL) {

}

OSDManager::~OSDManager() {
	uninit();
}

bool OSDManager::init(const char* pBkPngName,
		int aStartX, int aStartY,
		const char* pForegroundPngName) {

	m_strBkImgName = pBkPngName;
	m_forePosX = aStartX;
	m_forePosY = aStartY;
	if (pForegroundPngName != NULL) {
		m_strForegroundImgName = pForegroundPngName;
	}

	m_bksurface = cairo_image_surface_create_from_png(pBkPngName);
	if (!m_bksurface) {
		printf("can not create surface\n");
		return false;
	}

	m_bkcr = cairo_create (m_bksurface);
	if (!m_bkcr) {
		 cairo_surface_destroy (m_bksurface);
		 printf("can not create cairo\n");
		return false;
	}

	cairo_select_font_face (m_bkcr, "WenQuanYi Zen Hei Sharp",
			CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

	m_imageWidth = cairo_image_surface_get_width(m_bksurface);
	m_imageHeight = cairo_image_surface_get_height(m_bksurface);
	printf("the width is %d\n", m_imageWidth);
	printf("the height is %d\n", m_imageHeight);
	if (m_imageWidth == 0 || m_imageHeight == 0) {
		return false;
	}

	if (pForegroundPngName != NULL) {
		m_foresurface = cairo_image_surface_create_from_png(pForegroundPngName);

		if (NULL != m_foresurface) {
			unsigned char* pForeData = cairo_image_surface_get_data(m_foresurface);
			int nForeWidth = cairo_image_surface_get_width(m_foresurface);
			int nForeHeight = cairo_image_surface_get_height(m_foresurface);
			int nForeStride = cairo_image_surface_get_stride(m_foresurface);
			cairo_format_t nForeFormat = cairo_image_surface_get_format(m_foresurface);
			int nForeBytesPerPixel = 3;
			if (CAIRO_FORMAT_ARGB32 == nForeFormat) {
				nForeBytesPerPixel = 4;
			}

			unsigned char* pBkData = cairo_image_surface_get_data(m_bksurface);
			int nBkStride = cairo_image_surface_get_stride(m_bksurface);
			cairo_format_t nBkFormat = cairo_image_surface_get_format(m_bksurface);
			int nBkBytesPerPixel = 3;
			if (CAIRO_FORMAT_ARGB32 == nBkFormat) {
				nBkBytesPerPixel = 4;
			}

			int nOffsetBk = 0;
			int nOffsetFore = 0;
			for (int i = aStartY; i < nForeHeight; ++i) {
				nOffsetBk = i * nBkStride;
				nOffsetFore = i * nForeStride;
				for (int j = aStartX; j < nForeWidth; ++j) {
					memcpy(pBkData + nOffsetBk + j * nBkBytesPerPixel,
							pForeData + nOffsetFore + j * nForeBytesPerPixel,
							nForeBytesPerPixel);
				}
			}
		}
	}
	return true;
}

bool OSDManager::uninit() {
	if (NULL != m_bkcr) {
		cairo_destroy(m_bkcr);
		m_bkcr = NULL;
	}
	if (NULL != m_bksurface) {
		cairo_surface_destroy(m_bksurface);
		m_bksurface = NULL;
	}

	if (NULL != m_forecr) {
		cairo_destroy(m_forecr);
		m_forecr = NULL;
	}
	if (NULL != m_foresurface) {
		cairo_surface_destroy(m_foresurface);
		m_foresurface = NULL;
	}
	return true;
}

int OSDManager::setInputText(char* pIntext, int aPosX, int aPosY,
		float r, float g, float b, int fontSize) {

	cairo_set_font_size (m_bkcr, fontSize);
	cairo_set_source_rgb (m_bkcr, r, g, b);
	cairo_move_to(m_bkcr, aPosX, aPosY);
	cairo_show_text (m_bkcr, pIntext);
	cairo_stroke (m_bkcr);

	return 0;
}

unsigned char* OSDManager::getImage() {
	return cairo_image_surface_get_data(m_bksurface);

}

int OSDManager::getImageWidth() {
	return m_imageWidth;
}

int OSDManager::getImageHeight() {
	return m_imageHeight;
}

