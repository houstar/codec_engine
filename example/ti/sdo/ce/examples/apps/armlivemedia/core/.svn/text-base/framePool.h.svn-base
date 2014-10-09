#pragma once
#include "OMXCommonDef.h"
#include "../typedef.h"

#define MAX_DELAY_FRAME_COUNT 20
typedef struct stFrameBuffer
{
	char*	pBuffer;
	BOOL	bIsValid;
	int		iBufferLen;
	int		iDataLen;
	stFrameBuffer()
	{
		pBuffer = NULL;
		bIsValid = FALSE;
		iBufferLen = 0;
		iDataLen = 0;
	}
}FRAMEBUFFER;

class FrameBufferPool
{
public:
	FrameBufferPool();
	~FrameBufferPool();

	void PutFrame(char* pFrame, int len);
	char* GetFrame(int &frameLen);

	bool init(int count = MAX_DELAY_FRAME_COUNT);

	bool uninit();

private:
	FRAMEBUFFER* m_pFrameBuffer;
	int m_iPutIndex;
	int m_iGetIndex;
	int m_count;
};
