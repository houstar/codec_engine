#pragma once

#include <string>
#include "mp4Writer.h"
#include "Mutex.h"

using namespace std;

class Mp4Output{
public:
	Mp4Output();
	~Mp4Output();

public:

	bool init(const string& strFilename, int videoWidth, int videoHeight
			, int frameRate, int audioChannel,
			int audioSampleRate, int audioSampleBit);

	bool uninit();

	bool writeH264Video(const char* pData, int size);

	bool writeAACAudio(const char* pData, int size);


private:

	string  m_strFilename;

	void*	m_pMp4WriterHandle;

	stmp4infor  m_infor;

	Mutex 	m_mp4Mutex;
};



