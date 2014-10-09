#pragma once

#include "OMXCommonDef.h"
#include "../typedef.h"

#define MAX_DELAY_FRAME_COUNT 20
typedef struct stCameraFrameBuffer
{
	char*	pBuffer;
	BOOL	bIsValid;
	int		iBufferLen;
	int		iDataLen;
	int 	channel;
	stCameraFrameBuffer()
	{
		pBuffer = NULL;
		bIsValid = FALSE;
		iBufferLen = 0;
		iDataLen = 0;
	}
}CameraFrameBuffer;

class CameraBufferPool
{
public:
	CameraBufferPool();
	~CameraBufferPool();

	void PutFrame(char* pFrame, int len, int channel);
	char* GetFrame(int &frameLen, int& channel);

	bool init(int count = MAX_DELAY_FRAME_COUNT);

	bool uninit();

private:
	CameraFrameBuffer* m_pFrameBuffer;
	int m_iPutIndex;
	int m_iGetIndex;
	int m_count;
};
