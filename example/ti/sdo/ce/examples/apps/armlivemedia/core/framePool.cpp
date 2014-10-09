#include "framePool.h"

FrameBufferPool::FrameBufferPool() {
	m_iPutIndex = 0;
	m_iGetIndex = 0;


	m_count = 0;
	m_pFrameBuffer = NULL;
}

FrameBufferPool::~FrameBufferPool() {
	uninit();
}

bool FrameBufferPool::init(int count) {
	if (count <= 0) {
		return false;
	}
	m_pFrameBuffer = new FRAMEBUFFER[count];

	for (int i = 0; i < count; i++) {
		m_pFrameBuffer[i].pBuffer = NULL;
		m_pFrameBuffer[i].bIsValid = FALSE;
		m_pFrameBuffer[i].iBufferLen = 0;
		m_pFrameBuffer[i].iDataLen = 0;
	}
	m_count = count;
}

bool FrameBufferPool::uninit() {
	if (m_count <= 0) {
		return false;
	}
	for (int i = 0; i < m_count; i++) {
		if (m_pFrameBuffer[i].pBuffer) {
			delete[] (m_pFrameBuffer[i].pBuffer);
			m_pFrameBuffer[i].pBuffer = NULL;
			m_pFrameBuffer[i].bIsValid = FALSE;
			m_pFrameBuffer[i].iBufferLen = 0;
			m_pFrameBuffer[i].iDataLen = 0;
		}
	}
	delete [] m_pFrameBuffer;
	m_pFrameBuffer = NULL;
	m_count = 0;
	return true;
}

void FrameBufferPool::PutFrame(char* pFrameBuf, int frameLen) {
	if (pFrameBuf == NULL || frameLen == 0 || 0 == m_count) {
		return;
	}
	if ((!m_pFrameBuffer[m_iPutIndex].pBuffer)
			|| (m_pFrameBuffer[m_iPutIndex].iBufferLen < frameLen)) {
		if (m_pFrameBuffer[m_iPutIndex].pBuffer) {
			delete[] (m_pFrameBuffer[m_iPutIndex].pBuffer);
			m_pFrameBuffer[m_iPutIndex].pBuffer = NULL;
		}
		m_pFrameBuffer[m_iPutIndex].pBuffer = new char[frameLen];
		m_pFrameBuffer[m_iPutIndex].iBufferLen = frameLen;

	}
	memcpy(m_pFrameBuffer[m_iPutIndex].pBuffer, pFrameBuf, frameLen);

	m_pFrameBuffer[m_iPutIndex].iDataLen = frameLen;
	m_pFrameBuffer[m_iPutIndex].bIsValid = TRUE;

	m_iPutIndex = (m_iPutIndex + 1) % m_count;
}

char* FrameBufferPool::GetFrame(int &frameLen) {
	if (0 == m_count) {
		frameLen = 0;
		return NULL;
	}
	if (m_pFrameBuffer[m_iGetIndex].bIsValid) {
		frameLen = m_pFrameBuffer[m_iGetIndex].iDataLen;
		int iFrameIndex = m_iGetIndex;
		m_pFrameBuffer[iFrameIndex].bIsValid = FALSE;

		m_iGetIndex = (m_iGetIndex + 1) % m_count;
		return m_pFrameBuffer[iFrameIndex].pBuffer;
	} else {
		frameLen = 0;
		return NULL;
	}

}



