#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include "../KVideo/captureVga.h"
#include "../utils/commonUtils.h"
#include "v4lDisplay.h"
#include "constDefines.h"


extern unsigned char *buffer_addr[MAX_BUFFER];
extern long long gHDMIDisplayAddr;
extern long long gVGADisplayAddr;

DisplayHandle::DisplayHandle() {
	m_bStart = false;
	m_nDisplayCount = 0;
	memset(&disp, 0, sizeof(app_obj));
	m_type = 1;

	m_bInitedSucess = false;
	disp.fd = -1;
}

DisplayHandle::~DisplayHandle() {

}

bool DisplayHandle::setupDisplay(int width, int height) {
	/* Get format */
	disp.fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	int ret = ioctl(disp.fd, VIDIOC_G_FMT, &disp.fmt);
	if (ret < 0) {
		printf("Get Format failed\n");
		return false;
	}

	disp.fmt.fmt.pix.width = width;
	disp.fmt.fmt.pix.height = height;
	disp.fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	disp.fmt.fmt.pix.bytesperline = disp.fmt.fmt.pix.width * 2;
	disp.fmt.fmt.pix.sizeimage = disp.fmt.fmt.pix.bytesperline *
			disp.fmt.fmt.pix.height;

	ret = ioctl(disp.fd, VIDIOC_S_FMT, &disp.fmt);
	if (ret < 0) {
		printf("Set Format failed\n");
		return false;
	}

	/* Get format again and display it on console */
	disp.fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(disp.fd, VIDIOC_G_FMT, &disp.fmt);
	if (ret < 0) {
		printf("Get Format failed for display\n");
		return false;
	}

	/* Requests buffers, we are operating in userPtr mode */
	disp.reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	disp.reqbuf.count = DISPLAY_MAX_BUFFER;
	disp.reqbuf.memory = V4L2_MEMORY_USERPTR;
	ret = ioctl(disp.fd, VIDIOC_REQBUFS, &disp.reqbuf);
	if (ret < 0) {
		perror("Could not allocate the buffers\n");
		return false;
	}
	return true;
}

bool DisplayHandle::queueDisplayBuffers() {
	int ret;
	for (int i = 0; i < DISPLAY_MAX_BUFFER; i++) {
		disp.buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		disp.buf.memory = V4L2_MEMORY_USERPTR;
		disp.buf.index = i;
		//disp.buf.m.userptr = (long unsigned int)buffer_addr[0] + 0x100000 * 20;
		if (m_type == 2) {
			disp.buf.m.userptr = (long unsigned int)buffer_addr[0] + gVGADisplayAddr - BASE_ADDR;
		} else {
			disp.buf.m.userptr = (long unsigned int)buffer_addr[0] + gHDMIDisplayAddr - BASE_ADDR;
		}

		disp.buf.length = disp.fmt.fmt.pix.sizeimage;
		ret = ioctl(disp.fd, VIDIOC_QBUF, &disp.buf);

		if (ret < 0) {
			perror("VIDIOC_QBUF");
			return false;
		}
	}
	return true;
}

bool DisplayHandle::startDisplay() {
	if (disp.fd == -1) {
		return false;
	}
	int a = V4L2_BUF_TYPE_VIDEO_OUTPUT, ret;
	ret = ioctl(disp.fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMON\n");
		close(disp.fd);
		disp.fd = -1;
		return false;
	}
	m_bStart = true;
	m_nDisplayCount = 0;
	m_bInitedSucess = true;
	return true;
}

bool DisplayHandle::stopDisplay() {
	if (-1 == disp.fd) {
		return false;
	}
	int a = V4L2_BUF_TYPE_VIDEO_OUTPUT, ret;
	ret = ioctl(disp.fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMOFF\n");
		return false;
	}
	return true;
}


bool DisplayHandle::initv4lhandle(int width, int height, const char* pDeviceName, int type) {
	if (disp.fd != -1) {
		return false;
	}
	printf("init v4l handle\n");
	fflush(stdout);

	int mode = O_RDWR;
	struct v4l2_capability cap;
	m_type = type;

	/* Open display driver */
	disp.fd = open((const char *)pDeviceName, mode);
	if (disp.fd == -1) {
		printf("failed to open display device\n");
		return false;
	}

	/* Query driver capability */
	if (ioctl(disp.fd, VIDIOC_QUERYCAP, &disp.cap)) {
		printf("Query capability failed for display\n");
		return false;
	}  else {
		printf("Driver Name: %s\n", disp.cap.driver);
		printf("Driver bus info: %s\n", disp.cap.bus_info);
		if (disp.cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)
			printf("Driver is capable of doing display\n");
		if (disp.cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)
			printf("Driver is capabled of scaling and cropping\n");
	}
	if (!setupDisplay(width, height)) {
		close(disp.fd);
		disp.fd = -1;
		return false;
	}

	if (!queueDisplayBuffers() ) {
		close(disp.fd);
		disp.fd = -1;
		return false;
	}

	return startDisplay();
}

bool DisplayHandle::uninitv4lhandle() {
   	
	printf("uninit v4l handle\n");
	
	fflush(stdout);
	stopDisplay();

	if (-1 != disp.fd) {
		if (close(disp.fd)) {
			return false;
		}
		disp.fd = -1;
	}
	m_bStart = false;
	m_nDisplayCount = 0;
	m_bInitedSucess = false;
	return true;
}


bool DisplayHandle::doV4LDisplay(const char* pBuffer) {
	if (!m_bInitedSucess) {
		return false;
	}
	int ret;
	ret = ioctl(disp.fd, VIDIOC_DQBUF, &disp.buf);
	if (ret < 0) {
		printf("VIDIOC_DQBUF Display\n");
	}
	if (disp.buf.flags & V4L2_BUF_FLAG_ERROR){
		printf("V4L2_BUF_FLAG_ERROR2\n");
		ret = ioctl(disp.fd, TICAPT_CHECK_OVERFLOW, &disp.over_flow);
		if (ret < 0) {
			printf("TICAPT_CHECK_OVERFLOW\n");
			return false;
		} else {
			// If hardware locked up, restart display and capture driver
			if (disp.over_flow.porta_overflow) {
				printf("Port a overflowed\n\n\n\n\n\n\n");
				stopDisplay();
				startDisplay();
			}
			if (disp.over_flow.portb_overflow) {
				printf("Port b overflowed\n\n\n\n\n\n\n");
				stopDisplay();
				startDisplay();
			}
		}
	}

	disp.buf.m.userptr = (long unsigned int)pBuffer;

	disp.buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	disp.buf.memory = V4L2_MEMORY_USERPTR;
	disp.buf.length = disp.fmt.fmt.pix.sizeimage;

	ret = ioctl(disp.fd, VIDIOC_QBUF, &disp.buf);
	if (ret < 0) {
		perror("VIDIOC_QBUF Display");
	}

	m_nDisplayCount++;
	if (m_bStart){
		m_before = CommonUtils::GetTickCount();
		m_bStart = 0;
	} else if (m_nDisplayCount % 250 == 0) {
		m_after = CommonUtils::GetTickCount();
		//CommonUtils::calc_result_time(&result, &after, &before);
		if (m_after - m_before > 0) {
			printf("display framerate %lu\n", (int)(m_nDisplayCount * 1000/(m_after - m_before)));
		}
	}
	return true;
}
