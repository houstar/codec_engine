#pragma once
#include <cairo.h>

#include <string>
using namespace std;

class OSDManager {
public:
	OSDManager();
	~OSDManager();

public:

	/**
	 * \brief 初始化
	 * \param[in] pBkPngName 背景图片的名称
	 * \param[in] fontSize 文字的大小
	 * \param[in] aStartX 前景图像所在位置
	 * \param[in] aStartY 前景图像所在位置
	 * \param[in] pForegroundPngName 前景图片的名称
	 */
	bool init(const char* pBkPngName,
			int aStartX = 0, int aStartY = 0,
			const char* pForegroundPngName = NULL);

	bool uninit();

	int setInputText(char* pIntext, int aPosX, int aPosY,
			float r = 1, float g = 1, float b = 1, int fontSize = 50);

	unsigned char* getImage();

	int  getImageWidth();

	int  getImageHeight();

public:
	cairo_surface_t* m_bksurface;
	cairo_t *m_bkcr;
	cairo_surface_t* m_foresurface;
	cairo_t	*m_forecr;

	string  m_strBkImgName; 		/**< 背景图片		*/
	string	m_strForegroundImgName;	/**< 前景图片 	*/

	int 	m_imageWidth;
	int 	m_imageHeight;

	int		m_forePosX;	/**< 前景图片所在位置X	*/
	int		m_forePosY;	/**< 前景图片所在位置Y */
};




