#include "shareMem.h"
#include <string.h>
#include <stdio.h>

ShareMemManager::ShareMemManager() {
	m_pCustomReadIndex = NULL;
	for (int i = 0; i < KShareMemCount; ++i) {
		m_ShareMem[i].pValid = NULL;
		m_ShareMem[i].pBuffer = NULL;
		m_ShareMem[i].len = 0;
		m_ShareMem[i].count = 0;
	}
	m_nWriteIndex = 0;
	m_ShareMutex.init();
	m_nCustomCount = 0;
}

ShareMemManager::~ShareMemManager() {
	uninit();
	m_ShareMutex.uninit();
}

bool ShareMemManager::init(int count) {
	if (count <= 0) {
		return false;
	}
	m_pCustomReadIndex = new int[count];
	for (int i = 0; i < count; ++i) {
		*(m_pCustomReadIndex + i) = 0;
	}
	for (int i = 0; i < KShareMemCount; ++i) {
		m_ShareMem[i].pValid = new bool[count];
		for (int j = 0; j < count; ++j) {
			*(m_ShareMem[i].pValid + j) = false;
		}
	}
	m_nCustomCount = count;
}

bool ShareMemManager::uninit() {
	if (NULL != m_pCustomReadIndex) {
		delete []m_pCustomReadIndex;
		m_pCustomReadIndex = NULL;
	}
	m_nWriteIndex = 0;
	for (int i = 0; i < KShareMemCount; ++i) {
		if (NULL != m_ShareMem[i].pBuffer) {
			delete []m_ShareMem[i].pBuffer;
			m_ShareMem[i].pBuffer = NULL;
		}
		if (NULL != m_ShareMem[i].pValid) {
			delete []m_ShareMem[i].pValid;
			m_ShareMem[i].pValid = NULL;
		}
		m_ShareMem[i].len = 0;
	}
}

bool ShareMemManager::pushFrame(char* pBuf, int len) {
	if (pBuf == NULL || len == 0) {
		return false;
	}
	m_ShareMutex.lock();
	if ((NULL == m_ShareMem[m_nWriteIndex].pBuffer) || (m_ShareMem[m_nWriteIndex].len < len)) {
		if (NULL != m_ShareMem[m_nWriteIndex].pBuffer) {
			delete[] (m_ShareMem[m_nWriteIndex].pBuffer);
			m_ShareMem[m_nWriteIndex].pBuffer = NULL;
		}
		m_ShareMem[m_nWriteIndex].pBuffer = new char[len];
		m_ShareMem[m_nWriteIndex].len = len;
	}
	memcpy(m_ShareMem[m_nWriteIndex].pBuffer, pBuf, len);

	m_ShareMem[m_nWriteIndex].len = len;
	for (int j = 0; j < m_nCustomCount; ++j) {
		*(m_ShareMem[m_nWriteIndex].pValid + j) = true;
	}
	m_ShareMem[m_nWriteIndex].count = m_nCustomCount;

	m_nWriteIndex = (m_nWriteIndex + 1) % KShareMemCount;
	m_ShareMutex.unlock();
	return true;
}

char* ShareMemManager::consumeFrame(int &len, int nCustomIndex) {
	if (nCustomIndex < 0 || nCustomIndex >= m_nCustomCount) {
		len = 0;
		return false;
	}

	m_ShareMutex.lock();
	int nFrameIndex = *(m_pCustomReadIndex + nCustomIndex);
	//printf("consume frame index %d, count %d, nFrameIndex %d\n", nCustomIndex, m_ShareMem[nFrameIndex].count, nFrameIndex);
	if (m_ShareMem[nFrameIndex].count > 0) {
		if (NULL == m_ShareMem[nFrameIndex].pValid) {
			len = 0;
			m_ShareMutex.unlock();
			return NULL;
		}
		if (*(m_ShareMem[nFrameIndex].pValid + nCustomIndex)) {
			len = m_ShareMem[nFrameIndex].len;
			*(m_ShareMem[nFrameIndex].pValid + nCustomIndex) = false;
			m_ShareMem[nFrameIndex].count--;

			*(m_pCustomReadIndex + nCustomIndex) = (nFrameIndex + 1) % KShareMemCount;
			m_ShareMutex.unlock();
			return m_ShareMem[nFrameIndex].pBuffer;
		} else {
			len = 0;
			m_ShareMutex.unlock();
			return NULL;
		}
	} else {
		len = 0;
		m_ShareMutex.unlock();
		return NULL;
	}
}
