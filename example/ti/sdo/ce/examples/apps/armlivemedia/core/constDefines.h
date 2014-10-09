#pragma once

#include "../config.h"


#define BASE_ADDR 		(0x89500000)

#ifdef CAMERA_SDI
#define CaptureVGASIZE	0x0500000
#define CaptureSDI0SIZE 0x1000000
#define CaptureSDI1SIZE 0x1000000

#define LOGO_ADDR_OFFSET 0x2500000
#define LOGO_IMAGE_SIZE  0x0100000

#else

#define LOGO_ADDR_OFFSET 0x1000000
#define LOGO_IMAGE_SIZE  0x0100000

#endif

#define COURSE_ADDR_OFFSET (LOGO_ADDR_OFFSET + LOGO_IMAGE_SIZE)
#define COURSE_IMAGE_SIZE 0x0100000

#define TEACHER_BACKIMAGE_ADDR_OFFSET (COURSE_ADDR_OFFSET + COURSE_IMAGE_SIZE)
#define TEACHER_BACKIMAGE_SIZE 0x400000

#define TEACHER_STUD_BACKIMAGE_ADDR_OFFSET  (TEACHER_BACKIMAGE_ADDR_OFFSET + TEACHER_BACKIMAGE_SIZE)
#define TEACHER_STUD_BACKIMAGE_SIZE 0x400000

#define TEACHER_VGA_BACKIMAGE_ADDR_OFFSET (TEACHER_STUD_BACKIMAGE_ADDR_OFFSET + TEACHER_STUD_BACKIMAGE_SIZE)
#define TEACHER_VGA_BACKIMAGE_SIZE 0x400000

#define TEACHER_STUD_VGA_BACKIMAGE_ADDR_OFFSET (TEACHER_VGA_BACKIMAGE_ADDR_OFFSET + TEACHER_VGA_BACKIMAGE_SIZE)
#define TEACHER_STUD_VGA_BACKIMAGE_SIZE 0x400000

#define MIX_IMAGE_ADDR_OFFSET 	(TEACHER_STUD_VGA_BACKIMAGE_ADDR_OFFSET + TEACHER_STUD_VGA_BACKIMAGE_SIZE)
#define MIX_IMAGE_SIZE 0x400000

#define PRE_MIX_IMAGE_ADDR_OFFSET (MIX_IMAGE_ADDR_OFFSET + MIX_IMAGE_SIZE)
#define PRE_MIX_IMAGE_SIZE 0x400000

#define HEAD_ADDR_OFFSET (PRE_MIX_IMAGE_ADDR_OFFSET + PRE_MIX_IMAGE_SIZE)
#define HEAD_IMAGE_SIZE  0x400000

#define TAIL_ADDR_OFFSET (HEAD_ADDR_OFFSET + HEAD_IMAGE_SIZE)
#define TAIL_IMAGE_SIZE	 0X400000

#define HDMI_DISPLAY_OFFSET (TAIL_ADDR_OFFSET + TAIL_IMAGE_SIZE)
#define HDMI_DISPLAY_SIZE (4 * 1920 * 1080 * 2)

#define VGA_DISPLAY_OFFSET (HDMI_DISPLAY_OFFSET + HDMI_DISPLAY_SIZE)
#define VGA_DISPLAY_SIZE (4 * 1920 * 1080 * 2)

#define VGA_POOL_ADDR_OFFSET	(HEAD_ADDR_OFFSET + HEAD_IMAGE_SIZE)
#define VGA_SIZE	(1024 * 768 * 3/2)


#define MAX_FILE_NAME_SIZE      256

#define LCD_WIDTH         (800)
#define LCD_HEIGHT        (480)
#define LCD_PIXEL_CLOCK   (33500)
#define LCD_H_FRONT_PORCH (164)
#define LCD_H_BACK_PORCH  (89)
#define LCD_H_SYNC_LENGTH (10)
#define LCD_V_FRONT_PORCH (10)
#define LCD_V_BACK_PORCH  (23)
#define LCD_V_SYNC_LENGTH (10)

/*IL_CLIENT_SR - defines the shred region ID.
 * This value is taken from the
 * http://processors.wiki.ti.com/index.php/EZSDK_Memory_Map
 * Also, this is the value used by the firmware loader.
 * firmware_loader\src\memsegdef_default.c
 */
#define IL_CLIENT_SR                2


#define MAX_NUM_IN_BUFS 8
#define MAX_NUM_OUT_BUFS 8

#define K720PBufferCountActual 6
#define K1080PBufferCountActual 6

#define KDecodeCmponentInOutNum 8

#define KOtherComponetInOutPutNum 5

#define KEncodeComponentInOutPutNum 5


/* SD display is through NF, and NF has limitation of multiple
   of 32 ; Netra PG1.1 SD width has to be 720 ; follwing is done for Scalar and NF*/

#define SD_DISPLAY_WIDTH    ((720 + 31) & 0xffffffe0)
#define SD_DISPLAY_HEIGHT   ((480 + 31) & 0xffffffe0)

#define DISPLAY_WIDTH      1920
#define DISPLAY_HEIGHT     1080
#define DISPLAY_VENC_MODE  OMX_DC_MODE_1080P_60

#define HD_WIDTH       (1920)
#define HD_HEIGHT      (1080)

#define KHeadPicWidth 1024
#define KHeadPicHeight 768

#define SD_WIDTH       (720)

#define     PADX_H264    32
#define     PADY_H264    24

#define KCaptureVGAWidth 1024
#define KCaptureVGAHeight 768

enum EPrimeScene {
	eTeacherScene,		/**< 教师场景 	*/
	eStudScene,			/**< 学生场景		*/
	eVGAScene,			/**< VGA场景		*/
	eTeacherStudScene	/**< 教师学生双画面*/
};

enum EPortType {
	eInputPort,
	eOutputPort,
	ePortMax = 0x7FFFFFFF
};


enum EPipeCmdType
{
	ePipeCmdEmptyThisBuffer,
	ePipeCmdFillThisBuffer,
	ePipeCmdEmptyBufferDone,
	ePipeCmdFillBufferDone,
	ePipeCmdExit,
	ePipeCmdMax = 0x7FFFFFFF
};


