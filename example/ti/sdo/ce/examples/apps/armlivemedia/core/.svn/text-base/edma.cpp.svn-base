#include "edma.h"
#include <math.h>

EdmaUtils::EdmaUtils() {
	m_edmafd = -1;
}

EdmaUtils::~EdmaUtils() {
	uninit();
}

bool EdmaUtils::init() {
	if ((m_edmafd = open("/dev/ti81xx_edma", O_RDWR)) == -1) {
		printf("can not open edma \n");
		return false;
	}
	return true;
}

bool EdmaUtils::uninit() {
	if (-1 != m_edmafd) {
		close(m_edmafd);
		m_edmafd = -1;
	}
	return true;
}

bool EdmaUtils::copy(void* dest, void* src, int srcWidth, int dstWidth, int size, int mode) {
	edma_arg m_edmaArg;
	m_edmaArg.acnt = srcWidth;
	if (0 == size/m_edmaArg.acnt % 8) {
		m_edmaArg.bcnt = 8;
	} else if (0 == size/m_edmaArg.acnt % 4) {
		m_edmaArg.bcnt = 4;//8
	} else if (0 == size/m_edmaArg.acnt % 2){
		m_edmaArg.bcnt = 2;
	} else {
		m_edmaArg.bcnt = 1;
	}

	m_edmaArg.ccnt = size/m_edmaArg.acnt/m_edmaArg.bcnt;
	m_edmaArg.mode = mode;
	m_edmaArg.dmaphyssrc1 = src;
	m_edmaArg.dmaphysdest1 = dest;
	m_edmaArg.srcbidx = srcWidth;
	m_edmaArg.desbidx = dstWidth;
	if (m_edmaArg.mode == 0){
		// A Sync Transfer Mode
		m_edmaArg.srccidx = m_edmaArg.acnt;
		m_edmaArg.descidx = m_edmaArg.acnt;
	} else {
		m_edmaArg.srccidx = m_edmaArg.srcbidx * m_edmaArg.bcnt;
		m_edmaArg.descidx = m_edmaArg.desbidx * m_edmaArg.bcnt;
	}


	int ret = ioctl(m_edmafd, TI81XX_EDMA_MOVE, &m_edmaArg);
	if (ret != size) {
		printf("copy error copy len %d, src len%d, acnt %d, bcnt %d\n", ret, size, m_edmaArg.acnt, m_edmaArg.bcnt);
		ret = ioctl(m_edmafd, TI81XX_EDMA_MOVE, &m_edmaArg);
		return false;
	}
	return true;
}

bool EdmaUtils::copy(void* dest, void* src, int srcWidth, int srcIndex,
		int dstWidth, int dstIndex, int size, int mode) {
	edma_arg m_edmaArg;
	m_edmaArg.acnt = srcWidth;
	if (0 == size/m_edmaArg.acnt % 8) {
		m_edmaArg.bcnt = 8;
	} else if (0 == size/m_edmaArg.acnt % 4) {
		m_edmaArg.bcnt = 4;//8
	} else if (0 == size/m_edmaArg.acnt % 2){
		m_edmaArg.bcnt = 2;
	} else {
		m_edmaArg.bcnt = 1;
	}

	m_edmaArg.ccnt = size/m_edmaArg.acnt/m_edmaArg.bcnt;
	m_edmaArg.mode = mode;
	m_edmaArg.dmaphyssrc1 = src;
	m_edmaArg.dmaphysdest1 = dest;
	m_edmaArg.srcbidx = srcIndex;
	m_edmaArg.desbidx = dstIndex;
	if (m_edmaArg.mode == 0){
		// A Sync Transfer Mode
		m_edmaArg.srccidx = m_edmaArg.acnt;
		m_edmaArg.descidx = m_edmaArg.acnt;
	} else {
		m_edmaArg.srccidx = m_edmaArg.srcbidx * m_edmaArg.bcnt;
		m_edmaArg.descidx = m_edmaArg.desbidx * m_edmaArg.bcnt;
	}


	int ret = ioctl(m_edmafd, TI81XX_EDMA_MOVE, &m_edmaArg);
	if (ret != size) {
		printf("copy error copy len %d, src len%d, acnt %d, bcnt %d\n", ret, size, m_edmaArg.acnt, m_edmaArg.bcnt);
		return false;
	}
	return true;
}
