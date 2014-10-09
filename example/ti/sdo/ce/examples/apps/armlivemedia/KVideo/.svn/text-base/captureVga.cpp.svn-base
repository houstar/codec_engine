#include "captureVga.h"
#include "../utils/colorConvert.h"
#include "../utils/commonUtils.h"
#include <signal.h>
#include "../kernel.h"
#include "../core/constDefines.h"
#include "../constStringDefine.h"


/* Pixel format for capture and display. Capture supports
 * V4L2_PIX_FMT_NV12, V4L2_PIX_FMT_NV16, V4L2_PIX_FMT_RGB24 but display
 * supports only V4L2_PIX_FMT_YUYV. So this has to be V4L2_PIX_FMT_YUYV
 */
#define DEF_PIX_FMT		V4L2_PIX_FMT_NV12

/* Pointers for storing buffer addresses */
extern unsigned char *buffer_addr[MAX_BUFFER];
extern Kernel kernel;


/* Utility function for printing format */
static void printFormat(char *string, struct v4l2_format *fmt) {
	printf("=============================================================\n");
	printf("%s Format:\n", string);
	printf("=============================================================\n");
	printf("fmt.type\t\t = %d\n", fmt->type);
	printf("fmt.width\t\t = %d\n", fmt->fmt.pix.width);
	printf("fmt.height\t\t = %d\n", fmt->fmt.pix.height);
	printf("fmt.pixelformat\t = %d\n", fmt->fmt.pix.pixelformat);
	printf("fmt.bytesperline\t = %d\n", fmt->fmt.pix.bytesperline);
	printf("fmt.sizeimage\t = %d\n", fmt->fmt.pix.sizeimage);
	printf("=============================================================\n");
}

/*void savetofile(char * ptr) {
	int fd;

	fd = open("dump_yuv.yuv", O_CREAT | O_WRONLY);

	if (fd < 0)
		perror("open");

	char* pTemp = new char[capt.fmt.fmt.pix.width * capt.fmt.fmt.pix.height * 2];
	ColorConvertUtils::yuyv_to_yuv420((unsigned char*)pTemp,
			(unsigned char*)ptr, capt.fmt.fmt.pix.width,
			capt.fmt.fmt.pix.height);
	write(fd, pTemp, capt.fmt.fmt.pix.sizeimage);
	delete []pTemp;
	close(fd);
}*/


CaptureVGA::CaptureVGA() : m_bExit(false) {
	SempUtils::semp_init(&m_exitSem, 0);
	m_bInited = false;
	memset(&capt, 0, sizeof(capt_obj));
	m_threadID = 0;
}

CaptureVGA::~CaptureVGA() {
	SempUtils::semp_deinit(&m_exitSem);
	uninit();
}

bool CaptureVGA::init() {
	int mode = O_RDWR;

	capt.fd = open((const char *) CAPTURE_DEVICE, mode);
	if (capt.fd == -1) {
		goto FAIL;
	}
	/* Query for capabilities */
	if (ioctl(capt.fd, VIDIOC_QUERYCAP, &capt.cap)) {
		printf("Query capability failed\n");
		goto FAIL;
	} else {
		printf("Driver Name: %s\n", capt.cap.driver);
		printf("Driver bus info: %s\n", capt.cap.bus_info);
		if (capt.cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
			printf("Driver is capable of doing capture\n");
		if (capt.cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)
			printf("Driver is capabled of scaling and cropping\n");

	}
	/* Query the preset. Set it to invalid and query for it */
	capt.dv_preset.preset = 0x0;
	if (ioctl(capt.fd, VIDIOC_QUERY_DV_PRESET, &capt.dv_preset)) {
		printf("Querying DV Preset failed\n");
		goto FAIL;
	}
	printf("capt.dv_preset.preset=%d\n", capt.dv_preset.preset);
	switch (capt.dv_preset.preset) {
	case 20:
	case V4L2_DV_720P60:
		break;
	case V4L2_DV_1080I60:
		break;
	case 21:
	case 22:
	case V4L2_DV_1080P60:
		break;
	case V4L2_DV_1080P30:
		break;
	default:
		break;
	}
	if (ioctl(capt.fd, VIDIOC_S_DV_PRESET, &capt.dv_preset)) {
		printf("Setting DV Preset failed\n");
		goto FAIL;
	}

	printf("before vga setup capture\n");
	if (!setupCapture() ) {
		goto FAIL;
	}
	printf("vga setup capture ok\n");

	if (!queueCaptureBuffers() ) {
		goto FAIL;
	}
	printf("vga init ok\n");
	return true;
FAIL:
	close(capt.fd);
	capt.fd = -1;
	return false;
}

bool CaptureVGA::uninit() {
	if (!m_bExit) {
		stop();
	}
	if (capt.fd != -1) {
		close(capt.fd);
		capt.fd = -1;
	}

	return true;
}

bool CaptureVGA::setupCapture() {
	int ret;

	/* Get current format */
	capt.fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(capt.fd, VIDIOC_G_FMT, &capt.fmt);
	if (ret < 0) {
		printf("Set Format failed\n");
		return false;
	}
	printf("step 1\n");
	/* Set format according to mode detected */
	/*if (capt.dv_preset.preset == V4L2_DV_720P60) {
		capt.fmt.fmt.pix.width = 1280;
		capt.fmt.fmt.pix.height = 720;
	} else if (capt.dv_preset.preset == V4L2_DV_1080P60) {
		capt.fmt.fmt.pix.width = 1920;
		capt.fmt.fmt.pix.height = 1080;
	} else if (capt.dv_preset.preset == 20) {
		capt.fmt.fmt.pix.width = 1024;
		capt.fmt.fmt.pix.height = 768;
	} else if (capt.dv_preset.preset == 21) {
		capt.fmt.fmt.pix.width = 1360;
		capt.fmt.fmt.pix.height = 768;
	} else if (capt.dv_preset.preset == 22) {
		capt.fmt.fmt.pix.width = 1600;
		capt.fmt.fmt.pix.height = 1200;
	} else if (capt.dv_preset.preset == 23) {
		capt.fmt.fmt.pix.width = 1280;
		capt.fmt.fmt.pix.height = 1024;
	} else {
		capt.fmt.fmt.pix.width = 1920;
		capt.fmt.fmt.pix.height = 1080;
	}*/

	/*capt.crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	capt.crop.c.left = (capt.fmt.fmt.pix.width - 1024)/2;
	capt.crop.c.top = (capt.fmt.fmt.pix.height - 768)/2;
	capt.crop.c.width = 1024;
	capt.crop.c.height = 768;

	if (ioctl(capt.fd, VIDIOC_S_CROP, &capt.crop)) {
		printf("Setting format failed\n");
		return false;
	}*/

	if (capt.dv_preset.preset == 24) {
		kernel.NotifyAssistMessage(gStrVGADonotSupport);
		return false;
	}

	if (capt.dv_preset.preset == 8 || capt.dv_preset.preset == 27) {
		//720P
		kernel.NotifyVGAResolution720P();
	}

	capt.fmt.fmt.pix.width = KCaptureVGAWidth;
	capt.fmt.fmt.pix.height = KCaptureVGAHeight;

	capt.fmt_win.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	capt.fmt_win.fmt.win.w.left = 0;
	capt.fmt_win.fmt.win.w.top = 0;
	capt.fmt_win.fmt.win.w.width = KCaptureVGAWidth;
	capt.fmt_win.fmt.win.w.height = KCaptureVGAHeight;

	if (ioctl(capt.fd, VIDIOC_S_FMT, &capt.fmt_win)) {
		printf("Setting window failed\n");
		return false;
	}
	capt.fmt.fmt.pix.pixelformat = DEF_PIX_FMT;
	capt.fmt.fmt.pix.bytesperline = capt.fmt.fmt.pix.width;
	capt.fmt.fmt.pix.sizeimage = capt.fmt.fmt.pix.bytesperline
			* capt.fmt.fmt.pix.height * 3 /2;
	printf("step 2\n");
	for (vector<ICaptureVGAListener*>::iterator it = m_vecListeners.begin();
						it != m_vecListeners.end(); ++it) {
		(*it)->onNotifyVGAFMT(this, capt.fmt.fmt.pix.width, capt.fmt.fmt.pix.height);
	}

	ret = ioctl(capt.fd, VIDIOC_S_FMT, &capt.fmt);
	if (ret < 0) {
		printf("Set Format failed\n");
		return false;
	}
	/* Get format again and print it on console */
	capt.fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(capt.fd, VIDIOC_G_FMT, &capt.fmt);
	if (ret < 0) {
		printf("Set Format failed\n");
		return false;
	}
	printFormat("Capture", &capt.fmt);

	printf("step 3\n");

	/* Request buffers. We are operating in userPtr mode */
	capt.reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	capt.reqbuf.count = MAX_BUFFER;
	capt.reqbuf.memory = V4L2_MEMORY_USERPTR;
	ret = ioctl(capt.fd, VIDIOC_REQBUFS, &capt.reqbuf);
	if (ret < 0) {
		printf("Could not allocate the buffers\n");
		return false;
	}

	for (int i = 1; i < MAX_BUFFER; i++)
		buffer_addr[i] = buffer_addr[i - 1] + capt.fmt.fmt.pix.sizeimage;

	return true;
}

bool CaptureVGA::stopCapture() {
	if (capt.fd == -1) {
		return true;
	}
	/* Stop capture */

	int a = V4L2_BUF_TYPE_VIDEO_CAPTURE, ret;
	ret = ioctl(capt.fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		printf("VIDIOC_STREAMOFF error\n");
		return false;
	}
	printf("stop capture vga ok\n");
	return true;
}

bool CaptureVGA::startCapture(void) {
	if (capt.fd == -1) {
		return true;
	}
	int a = V4L2_BUF_TYPE_VIDEO_CAPTURE, ret;
	ret = ioctl(capt.fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMON\n");
		return false;
	}

	ioctl(capt.fd, CMD_WAITCHANGE, 40);//every
	return true;
}

bool CaptureVGA::queueCaptureBuffers() {
	int ret, i;
	for (i = 0; i < MAX_BUFFER; i++) {
		capt.buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		capt.buf.memory = V4L2_MEMORY_USERPTR;
		capt.buf.index = i;
		capt.buf.m.userptr = (unsigned long) buffer_addr[capt.buf.index];
		capt.buf.length = capt.fmt.fmt.pix.sizeimage;
		ret = ioctl(capt.fd, VIDIOC_QBUF, &capt.buf);
		if (ret < 0) {
			perror("queue capture buffers VIDIOC_QBUF\n");
			return false;
		}
	}
	return true;
}

bool CaptureVGA::onCaptureTask() {

	int i = 0, ret = 0;

	while (1) {
		if (m_bExit) {
			break;
		}

			ret = ioctl(capt.fd, VIDIOC_DQBUF, &capt.buf);
			if (ret < 0) {
				perror("on capture task VIDIOC_DQBUF\n");
				printf("capture fd is %d, ret %d\n", capt.fd, ret);
				continue;
			}
			//printf("VIDIOC_DQBUF1\n");
			/* Because of IP bugs, capture hardware gets locked up once in
			 * a while. In that case DQbuf will return  V4L2_BUF_FLAG_ERROR
			 * in flags.
			 */
			if (capt.buf.flags & V4L2_BUF_FLAG_ERROR) {
				// If DQbuf returned error check for the hardware lockup
				ret = ioctl(capt.fd, TICAPT_CHECK_OVERFLOW, &capt.over_flow);
				if (ret < 0) {
					perror("TICAPT_CHECK_OVERFLOW\n");
					continue;
				} else {
					// If hardware locked up, restart display and capture driver
					if (capt.over_flow.porta_overflow) {
						printf("Port a overflowed\n\n\n\n\n\n\n");
						stopCapture();
						startCapture();
					}
					if (capt.over_flow.portb_overflow) {
						printf("Port b overflowed\n\n\n\n\n\n\n");
						stopCapture();
						startCapture();
					}
				}
			}


			for (vector<ICaptureVGAListener*>::iterator it = m_vecListeners.begin();
					it != m_vecListeners.end(); ++it) {
				(*it)->onCaptureVGABuffer(this, (unsigned char*)capt.buf.m.userptr,
					capt.fmt.fmt.pix.width, capt.fmt.fmt.pix.height);
			}
			//savetofile((char*)capt.buf.m.userptr);

			/* Queue the capture buffer with updated address */
			ret = ioctl(capt.fd, VIDIOC_QBUF, &capt.buf);
			if (ret < 0) {
				perror("on capture task VIDIOC_QBUF\n");
				continue;
			}
	}
	SempUtils::semp_post(&m_exitSem);
	pthread_exit(NULL);
	printf("exit capture vga thread \n");
	return true;
}

void* CaptureVGA::CaptureVGATask(void* pParam) {
	CaptureVGA* pSelf = (CaptureVGA*) pParam;
	if (NULL != pSelf) {
		pSelf->onCaptureTask();
	}
	return NULL;
}

bool CaptureVGA::start() {
	if (m_bExit) {
		return false;
	}

	if (!startCapture() ) {
		return false;
	}

	pthread_attr_init(&m_threadAttr);
	if (0 != pthread_create(&m_threadID,
					&m_threadAttr,
					CaptureVGATask,
					this)) {
		printf("Create vga capture Task failed !");
		return false;
	}
	m_bInited = true;
	return true;
}

bool CaptureVGA::stop() {
	printf("before capture vga stop\n");
	if (!m_bInited) {
		m_bExit = true;
		return true;
	}
	ioctl(capt.fd, CMD_WAITCHANGE, 0);
	printf("after ioctl wait change\n");
	m_bExit = true;
	if (m_threadID != 0) {
		SempUtils::semp_pend(&m_exitSem);

		pthread_attr_destroy(&m_threadAttr);
		pthread_join(m_threadID, NULL);
		printf("capture vga get exit sem\n");
	}

	m_threadID = 0;

	if (!stopCapture()) {
		printf("stop capture vga failed\n");
		return false;
	}

	printf("after capture vga stop\n");
	m_bInited = false;
	return true;
}

bool CaptureVGA::addListener(ICaptureVGAListener* pListener) {
	for (vector<ICaptureVGAListener*>::iterator it = m_vecListeners.begin();
			it != m_vecListeners.end(); ++it) {
		if (pListener == *(it)) {
			return true;
		}
	}
	m_vecListeners.push_back(pListener);
	return true;
}

bool CaptureVGA::removeListener(ICaptureVGAListener* pListener) {
	for (vector<ICaptureVGAListener*>::iterator it = m_vecListeners.begin();
			it != m_vecListeners.end(); ++it) {
		if (pListener == *(it)) {
			m_vecListeners.erase(it);
			return true;
		}
	}
	return true;
}
