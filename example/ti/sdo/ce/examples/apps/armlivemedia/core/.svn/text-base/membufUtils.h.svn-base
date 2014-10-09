#pragma once
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <linux/videodev2.h>
#include <linux/fb.h>
#include <linux/ti81xxfb.h>
#include <linux/ti81xxhdmi.h>
#include <linux/ti81xxvin.h>
#include "../config.h"

#ifdef CAMERA_SDI
#define MAX_BUFFER			4
#else
#define MAX_BUFFER			6
#endif
//24M
//#define MAP_SIZE (0x1800000)

#define MAP_SIZE (0x7800000)
#define MAP_MASK (MAP_SIZE - 1)

#define VGA_BASE_ADDR (0x89500000)

class MemBufUtils {

public:
	MemBufUtils();
	~MemBufUtils();

public:

	bool open();

	bool close();

private:
	int m_fdmem;
};
