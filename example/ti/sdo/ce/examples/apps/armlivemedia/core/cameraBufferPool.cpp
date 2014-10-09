#include "cameraBufferPool.h"

CameraBufferPool::CameraBufferPool() {
	m_iPutIndex = 0;
	m_iGetIndex = 0;


	m_count = 0;
	m_pFrameBuffer = NULL;
}

CameraBufferPool::~CameraBufferPool() {
	uninit();
}

bool CameraBufferPool::init(int count) {
	if (count <= 0) {
		return false;
	}
	m_pFrameBuffer = new CameraFrameBuffer[count];

	for (int i = 0; i < count; i++) {
		m_pFrameBuffer[i].pBuffer = NULL;
		m_pFrameBuffer[i].bIsValid = FALSE;
		m_pFrameBuffer[i].iBufferLen = 0;
		m_pFrameBuffer[i].iDataLen = 0;
	}
	m_count = count;
}

bool CameraBufferPool::uninit() {
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

void CameraBufferPool::PutFrame(char* pFrameBuf, int frameLen, int channel) {
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
	m_pFrameBuffer[m_iPutIndex].channel = channel;

	m_iPutIndex = (m_iPutIndex + 1) % m_count;
}

char* CameraBufferPool::GetFrame(int &frameLen, int &channel) {
	if (0 == m_count) {
		frameLen = 0;
		return NULL;
	}
	if (m_pFrameBuffer[m_iGetIndex].bIsValid) {
		frameLen = m_pFrameBuffer[m_iGetIndex].iDataLen;
		int iFrameIndex = m_iGetIndex;
		m_pFrameBuffer[iFrameIndex].bIsValid = FALSE;
		channel = m_pFrameBuffer[iFrameIndex].channel;

		m_iGetIndex = (m_iGetIndex + 1) % m_count;

		return m_pFrameBuffer[iFrameIndex].pBuffer;
	} else {
		frameLen = 0;
		return NULL;
	}

}
