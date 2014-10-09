#pragma once
#include "constDefines.h"
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

/* device node to be used for display */
#define DISPLAY_DEVICE		"/dev/video1"
#define DISPLAY_NAME		"Display"

#define DISPLAY_MAX_BUFFER	4


struct app_obj {
	int fd;
	struct v4l2_capability cap;
	struct v4l2_format fmt;
	struct v4l2_dv_preset dv_preset;
	struct v4l2_requestbuffers reqbuf;
	int numbuffers;
	struct v4l2_buffer buf;
	struct ti81xxvin_overflow_status over_flow;
};

class DisplayHandle {
public:
	DisplayHandle();
	~DisplayHandle();

public:
	bool initv4lhandle(int width, int height, const char* pDeviceName, int type);

	bool uninitv4lhandle();

	bool doV4LDisplay(const char* pBuffer);

protected:
	bool setupDisplay(int width, int height);

	bool queueDisplayBuffers();

	bool startDisplay();

	bool stopDisplay();

private:
	app_obj disp;
	bool m_bStart;
	unsigned int m_nDisplayCount;
	int	m_type; /**< 1:HDMI, 2:vga*/
	long long m_before;
	long long m_after;

	bool	m_bInitedSucess;
};


