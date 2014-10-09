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
#include <vector>
#include <pthread.h>


/* device node to be used for capture */
#define CAPTURE_DEVICE		"/dev/video5"
#define CAPTURE_NAME		"Capture"

#define HD_SDI0 "/dev/video0"

#define HD_SDI1 "/dev/video4"

#define V4L2_DV_1024_768_60 20
#define V4L2_DV_1360_768_60 21
#define V4L2_DV_1600_1200_60 22

#define CMD_WAITCHANGE (('c'<<8)|'w')
#define SALOOPBACK_DEBUG

struct buf_info {
	int index;
	unsigned int length;
	char *start;
};

struct capt_obj {
	int fd;
	struct v4l2_capability cap;
	struct v4l2_format fmt;
	struct v4l2_dv_preset dv_preset;
	struct v4l2_requestbuffers reqbuf;
	int numbuffers;
	struct v4l2_buffer buf;
	struct ti81xxvin_overflow_status over_flow;
	struct v4l2_crop crop;
	struct v4l2_format fmt_win;
};
