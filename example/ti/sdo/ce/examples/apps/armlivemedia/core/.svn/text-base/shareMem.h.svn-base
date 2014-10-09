#pragma once
#include "Mutex.h"

struct stShareMem {
	char* pBuffer;
	bool* pValid;
	int	 len;
	int	 count; //could be used by how many custom
};

#define KShareMemCount	60

class ShareMemManager {
public:
	ShareMemManager();
	~ShareMemManager();

public:
	bool init(int nCustomCount);//how many object own this buf

	bool uninit();

	bool pushFrame(char* pBuf, int len);

	char* consumeFrame(int &len, int nCustomIndex);

private:

	stShareMem	m_ShareMem[KShareMemCount];
	int*		m_pCustomReadIndex;
	int			m_nWriteIndex;
	Mutex		m_ShareMutex;
	int			m_nCustomCount;
};
