#pragma once
#include <time.h>
#include <string.h>
using namespace std;

#include "tinyxml.h"

class Kernel;

class RecordList {
public:
	RecordList();

	virtual ~RecordList();

public:

	bool init(Kernel* pKernel);

	bool uninit();

	bool addItem(const char* pFileName, int mode, int recordRate, int width, int height,  off_t fileSize, time_t time);

	bool deleteItem(const char* pFileName);

	bool deleteItemsBeforeTime(time_t t);

private:
	bool addItem(TiXmlElement* pItem);

private:
	bool changeStringToTime(const char* pTime, int& year, int &month, int& day, int& hour, int& min, int& sec);

private:

	Kernel* m_pKernel;

	//string 	m_strRecordMode;/**< 微格, interaction, */
	//int		m_recordRate;
	//int		m_recordWidth;
	//int		m_recordHeight;
	//int		m_fileSize;
	//time_t	m_fileTime;
	//string	m_strfileName;
};
