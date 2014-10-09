#include "recordList.h"
#include "../kernel.h"
#include "../utils/commonUtils.h"
#include "../constStringDefine.h"

RecordList::RecordList()
: m_pKernel(NULL) {

}


RecordList::~RecordList() {

}

bool RecordList::init(Kernel* pKernel) {
	m_pKernel = pKernel;
	return true;
}

bool RecordList::uninit() {
	return true;
}

bool RecordList::addItem(TiXmlElement* pItem) {
	if (NULL == pItem) {
		return false;
	}
	try {
		//创建一个XML的文档对象。
		TiXmlDocument *myDocument = new TiXmlDocument("RecordList.xml");
		if (!myDocument->LoadFile()) {
			//create new file
			delete myDocument;
			myDocument = new TiXmlDocument();

			char bufVersion[32] = {0};
			strcpy(bufVersion, "?xml version=\"1.0\"?");
			TiXmlElement* pVersion = new TiXmlElement(bufVersion);
			//创建一个根元素并连接。
			TiXmlElement *RootElement = new TiXmlElement("RecordList");
			myDocument->LinkEndChild(pVersion);
			myDocument->LinkEndChild(RootElement);
			//创建一个Model元素并连接。
			TiXmlElement* Item0 = new TiXmlElement("File");
			Item0->SetAttribute("mode", pItem->Attribute("mode"));
			Item0->SetAttribute("rate", pItem->Attribute("rate"));
			Item0->SetAttribute("resolution",  pItem->Attribute("resolution"));
			Item0->SetAttribute("size",  pItem->Attribute("size"));
			Item0->SetAttribute("name", pItem->Attribute("name"));
			Item0->SetAttribute("time", pItem->Attribute("time"));

			RootElement->LinkEndChild(Item0);

			myDocument->SaveFile("RecordList.xml");//保存到文件

			delete myDocument;
			return true;
		}

		TiXmlHandle docHandle(myDocument);
		TiXmlElement* RootElement = docHandle.FirstChild("RecordList").ToElement();
		TiXmlElement* Item0 = new TiXmlElement("File");
		Item0->SetAttribute("mode", pItem->Attribute("mode"));
		Item0->SetAttribute("rate", pItem->Attribute("rate"));
		Item0->SetAttribute("resolution",  pItem->Attribute("resolution"));
		Item0->SetAttribute("size",  pItem->Attribute("size"));
		Item0->SetAttribute("name", pItem->Attribute("name"));
		Item0->SetAttribute("time", pItem->Attribute("time"));

		RootElement->LinkEndChild(Item0);

		myDocument->SaveFile();

		delete myDocument;
	} catch (string & e) {
		return false;
	}
	return true;
}

bool RecordList::addItem(const char* pFileName, int mode, int recordRate,
		int width, int height,  off_t fileSize, time_t time) {
	try {
		//创建一个XML的文档对象。
		char bufXMLName[255] = {0};
		sprintf(bufXMLName, "%s.xml", pFileName);
		TiXmlDocument *myDocument = new TiXmlDocument(bufXMLName);
		if (!myDocument->LoadFile()) {
			//create new file
			delete myDocument;
			myDocument = new TiXmlDocument();

			char bufVersion[32] = {0};
			strcpy(bufVersion, "?xml version=\"1.0\"?");
			TiXmlElement* pVersion = new TiXmlElement(bufVersion);
			//创建一个根元素并连接。
			TiXmlElement *RootElement = new TiXmlElement("RecordList");
			myDocument->LinkEndChild(pVersion);
			myDocument->LinkEndChild(RootElement);
			//创建一个Model元素并连接。
			TiXmlElement* Item0 = new TiXmlElement("File");
			switch (mode) {
			case eMovieMode:
				Item0->SetAttribute("mode", gStrMovie);
				break;
			case eResourceMode:
				Item0->SetAttribute("mode", gStrResource);
				break;
			case eInterMode:
				Item0->SetAttribute("mode", gStrInteraction);
				break;
			case eMicroMode:
				Item0->SetAttribute("mode", gStrMicro);
				break;
			default:
				break;
			}

			char Buf[32] = {0};
			if (recordRate > 1000000) {
				sprintf(Buf, "%dM", recordRate/1000000);
			} else {
				sprintf(Buf, "%dK", recordRate/1000);
			}
			Item0->SetAttribute("rate", Buf);
			sprintf(Buf, "%d*%d", width, height);
			Item0->SetAttribute("resolution",  Buf);
			if (fileSize > 1024 * 1024 * 1024) {
				sprintf(Buf, "%.2fG", fileSize/1024.0/1024.0/1024.0);
			} else  {
				sprintf(Buf, "%.2fM", fileSize/1024.0/1024.0);
			}

			Item0->SetAttribute("size",  Buf);
			Item0->SetAttribute("name", pFileName);
			tm* t = localtime(&time);

			sprintf(Buf, "%4d-%02d-%02d_%02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
						t->tm_hour, t->tm_min, t->tm_sec);

			Item0->SetAttribute("time", Buf);

			RootElement->LinkEndChild(Item0);

			myDocument->SaveFile(bufXMLName);//保存到文件

			delete myDocument;

			return true;
		}

		/*TiXmlHandle docHandle(myDocument);
		TiXmlElement* RootElement = docHandle.FirstChild("RecordList").ToElement();
		TiXmlElement* Item0 = new TiXmlElement("File");
		switch (mode) {
		case eMovieMode:
			Item0->SetAttribute("mode", gStrMovie);
			break;
		case eResourceMode:
			Item0->SetAttribute("mode", gStrResource);
			break;
		case eInterMode:
			Item0->SetAttribute("mode", gStrInteraction);
			break;
		case eMicroMode:
			Item0->SetAttribute("mode", gStrMicro);
			break;
		default:
			break;
		}
		char buf[32] = {0};
		if (recordRate > 1000000) {
			sprintf(buf, "%dM", recordRate/1000000);
		} else {
			sprintf(buf, "%dK", recordRate/1000);
		}
		Item0->SetAttribute("rate", buf);
		sprintf(buf, "%d*%d", width, height);
		Item0->SetAttribute("resolution",  buf);
		sprintf(buf, "%.2fM", fileSize/1024.0/1024.0);
		Item0->SetAttribute("size",  buf);
		Item0->SetAttribute("name", pFileName);

		tm* t = localtime(&time);

		sprintf(buf, "%4d-%02d-%02d_%02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
					t->tm_hour, t->tm_min, t->tm_sec);

		Item0->SetAttribute("time", buf);

		RootElement->LinkEndChild(Item0);

		myDocument->SaveFile();

		delete myDocument;*/
	} catch (string & e) {
		return false;
	}
	return true;
}

bool RecordList::deleteItem(const char* pFileName) {
	return true;
	try {
		//创建一个XML的文档对象。
		TiXmlDocument *myDocument = new TiXmlDocument("RecordList.xml");
		if (!myDocument->LoadFile()) {
			return false;
		}

		TiXmlHandle  docHandle(myDocument);
		TiXmlHandle pRecordListElement = docHandle.FirstChild("RecordList");

		int nIndex = 0;
		char* pTemp;
		char commandBuf[256] = {0};
		TiXmlElement* pItem;
		while (1) {
			pItem = pRecordListElement.Child("File", nIndex).ToElement();
			if (NULL == pItem) {
				break;
			}
			pTemp = (char*)pItem->Attribute("name");
			if (NULL != pTemp && strcmp(pTemp, pFileName) == 0) {
				sprintf(commandBuf, "rm -rf %s*\n", pFileName);
				system(commandBuf);
				printf("call system rm -rf %s*\n", pFileName);
				pRecordListElement.ToElement()->RemoveChild(pItem);
				break;
			}
			nIndex++;
		}

		myDocument->SaveFile();

		delete myDocument;
	} catch (string & e) {
		return false;
	}

	return true;
}


bool RecordList::deleteItemsBeforeTime(time_t time) {
	return true;
	try {
		//创建一个XML的文档对象。
		system("mv RecordList.xml RecordListbak.xml");
		system("sync");
		TiXmlDocument *myDocument = new TiXmlDocument("RecordListbak.xml");
		if (!myDocument->LoadFile()) {
			delete myDocument;
			return false;
		}

		TiXmlElement* pRecordListElement = myDocument->FirstChildElement("RecordList");


		char* pTemp;
		char commandBuf[256] = {0};
		TiXmlElement* pItem;
		int tempYear,  tempMonth,  tempDay,  tempHour,  tempMin, tempSec;
		char* pFileName;

		tm* t = localtime(&time);

		int baseYear = t->tm_year + 1900;
		int baseMonth = t->tm_mon + 1;
		int baseDay = t->tm_mday;
		int baseHour = t->tm_hour;
		int baseMin = t->tm_min;
		int baseSec = t->tm_sec;

		if (NULL != pRecordListElement) {
			pItem = pRecordListElement->FirstChildElement("File");
			for (; NULL != pItem; pItem = pItem->NextSiblingElement("File")) {
				pTemp = (char*)pItem->Attribute("time");
				changeStringToTime(pTemp, tempYear, tempMonth, tempDay, tempHour, tempMin, tempSec);
				if (tempYear > baseYear ||
						(tempYear == baseYear && tempMonth > baseMonth) ||
						(tempYear == baseYear && tempMonth == baseMonth && tempDay > baseDay) ||
						(tempYear == baseYear && tempMonth == baseMonth && tempDay == baseDay
								&& tempHour > baseHour) ||
						(tempYear == baseYear && tempMonth == baseMonth && tempDay == baseDay
								&& tempHour == baseHour && tempMin > baseMin) ||
						(tempYear == baseYear && tempMonth == baseMonth && tempDay == baseDay
								&& tempHour == baseHour && tempMin == baseMin && tempSec > baseMin) ) {
					addItem(pItem);
				} else {
					pFileName = (char*)pItem->Attribute("name");
					sprintf(commandBuf, "rm -rf %s*\n", pFileName);
					system(commandBuf);
					printf("call system rm -rf %s*\n", pFileName);
					fflush(stdout);

							// TiXmlNode *pParNode =  pItem->Parent();
							// if (NULL==pParNode) {
							//	 break;
							// }

							//  TiXmlElement* pParentEle = pParNode->ToElement();
							//  if (NULL != pParentEle) {
							//  if (pParentEle->RemoveChild(pItem))
							//       break;
							// }

							//printf("remove child item\n");
							//fflush(stdout);
				}
			}
		}

		myDocument->SaveFile();
		delete myDocument;
	} catch (string & e) {
		return false;
	}

	return true;
}

bool RecordList::changeStringToTime(const char* pTime, int& year, int &month, int& day, int& hour, int& min, int& sec) {
	/*sprintf(buf, "%4d-%02d-%02d_%02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
				t->tm_hour, t->tm_min, t->tm_sec);*/
	printf("before change string to time\n");
	char bufYear[5] = {0};
	char bufMonth[3] = {0};
	char bufDay[3] = {0};
	char bufHour[3] = {0};
	char bufMinute[3] = {0};
	char bufSecond[3] = {0};
	strncpy(bufYear, pTime, 4);
	strncpy(bufMonth, pTime + 5, 2);
	strncpy(bufDay, pTime + 8, 2);
	strncpy(bufHour, pTime + 11, 2);
	strncpy(bufMinute, pTime + 14, 2);
	strncpy(bufSecond, pTime + 17, 2);
	year = atoi(bufYear);
	month = atoi(bufMonth);
	day = atoi(bufDay);
	hour = atoi(bufHour);
	min = atoi(bufMinute);
	sec = atoi(bufSecond);
	return true;
}

