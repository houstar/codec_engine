#include "modelStatus.h"
#include "tinyxml.h"
#include "../kernel.h"
#include "../KVideo/VideoFactory.h"
#include "ACCWebService.h"
#include "../utils/liveCourseUtils.h"
#include "../utils/commonUtils.h"
#include "../constStringDefine.h"

void ModelStatus::onRecvMtgData(MtgClient* pMtg, const char* path,
			const char*data, int dataLen, int datatype, int action) {

}

void ModelStatus::onJoinCallback(MtgClient* pMtg, bool bJoinOK) {
	m_bMtgConnect = bJoinOK;
	m_bMtgValid = bJoinOK;
	sem_post(&m_MtgSem);
	printf("after join callback \n");
}

bool ModelStatus::notifyRtmpConnect(RtmpHandle* pRtmpHandle, bool bConnect) {
	m_bRTMPValid = bConnect;
	m_bRTMPConnect = bConnect;
	sem_post(&m_RtmpSem);
	printf("after rtmp connect call back\n");

	return true;
}

ModelStatus::ModelStatus()
: m_pKernel(NULL)
, m_bCameraTeacher1Valid(false)
, m_bCameraTeacher1Connect(false)
, m_bCameraTeacher2Valid(false)
, m_bCameraTeacher2Connect(false)
, m_bCameraStudent1Valid(false)
, m_bCameraStudent1Connect(false)
, m_bCameraStudent2Valid(false)
, m_bCameraStudent2Connect(false)
, m_bBackCamera1Valid(false)
, m_bBackCamera1Connect(false)
, m_bBackCamera2Valid(false)
, m_bBackCamera2Connect(false)
, m_bVideoCenterValid(false)
, m_bVideoCenterConnect(false)
, m_bRTMPValid(false)
, m_bRTMPConnect(false)
, m_bMtgValid(false)
, m_bMtgConnect(false)
, m_bLiveCourseValid(false)
, m_bLiveCourseConnect(false)
, m_bDetailValid(true)
, m_detailConnectInfo(0)
, m_pMtgClient(NULL)
, m_rtmpHandle(NULL){
	sem_init(&m_MtgSem, 0, 0);
	sem_init(&m_RtmpSem, 0, 0);

}

ModelStatus::~ModelStatus() {
	printf("uninit step 1\n");
	if (NULL != m_pMtgClient) {
		delete m_pMtgClient;
		m_pMtgClient = NULL;
	}
	printf("uninit step 2\n");
	if (NULL != m_rtmpHandle) {
		delete m_rtmpHandle;
		m_rtmpHandle = NULL;
	}
	printf("uninit step 3\n");
	sem_destroy(&m_MtgSem);
	sem_destroy(&m_RtmpSem);
	printf("uninit step 4\n");
}

bool ModelStatus::init(Kernel* pKernel) {
	m_pKernel = pKernel;
	return true;
}

bool ModelStatus::uninit() {
	return true;
}

bool ModelStatus::queryStatus() {
	queryCameraStatus();
	queryVideoCenterStatus();
	printf("after query video center\n");


	queryMtgStatus();

	queryLiveCourseStatus();


	queryRTMPStatus();

	queryDetailStatus();

	writeStatusXML();

	printf("write status xml end\n");


	return true;
}

bool ModelStatus::writeStatusXML() {
	try {
		//创建一个XML的文档对象。
		printf("writeStatusXML\n");
		 //创建一个XML的文档对象。
		TiXmlDocument *myDocument = new TiXmlDocument();
		char bufVersion[32] = {0};
		strcpy(bufVersion, "?xml version=\"1.0\"?");
		TiXmlElement* pVersion = new TiXmlElement(bufVersion);
		//创建一个根元素并连接。
		TiXmlElement *RootElement = new TiXmlElement("Status");
		myDocument->LinkEndChild(pVersion);
		myDocument->LinkEndChild(RootElement);
		printf("step 1\n");
		//创建一个Model元素并连接。
		TiXmlElement* Item0 = new TiXmlElement("Model");
		Item0->SetAttribute("name", "teacherCamera1");
		Item0->SetAttribute("valid", m_bCameraTeacher1Valid);
		IPCInfo* pIPCinfo = m_pKernel->getSetting()->getIPCInfo(4);
		char ipBuf[32] = {0};
		if (NULL != pIPCinfo) {
			sprintf(ipBuf, "%d.%d.%d.%d", (pIPCinfo->dwIPCIP >> 24) & 0xFF,
						(pIPCinfo->dwIPCIP >> 16) & 0xFF,
						(pIPCinfo->dwIPCIP >> 8) & 0xFF,
						(pIPCinfo->dwIPCIP) & 0xFF);
		} else {
			strcpy(ipBuf, "0.0.0.0");
		}


		Item0->SetAttribute("ip",  ipBuf);
		Item0->SetAttribute("connect",  m_bCameraTeacher1Connect);
		printf("step 2\n");

		TiXmlElement* Item1 = new TiXmlElement("Model");
		Item1->SetAttribute("name", "teacherCamera2");
		Item1->SetAttribute("valid", m_bCameraTeacher2Valid);

		pIPCinfo = m_pKernel->getSetting()->getIPCInfo(3);
		if (NULL != pIPCinfo) {
			sprintf(ipBuf, "%d.%d.%d.%d", (pIPCinfo->dwIPCIP >> 24) & 0xFF,
						(pIPCinfo->dwIPCIP >> 16) & 0xFF,
						(pIPCinfo->dwIPCIP >> 8) & 0xFF,
						(pIPCinfo->dwIPCIP) & 0xFF);
		} else {
			strcpy(ipBuf, "0.0.0.0");
		}


		Item1->SetAttribute("ip", ipBuf);
		Item1->SetAttribute("connect",  m_bCameraTeacher2Connect);
		printf("step 3\n");

		TiXmlElement *Item2 = new TiXmlElement("Model");
		Item2->SetAttribute("name", "studentCamera1");
		Item2->SetAttribute("valid", m_bCameraStudent1Valid);

		pIPCinfo = m_pKernel->getSetting()->getIPCInfo(1);
		if (NULL != pIPCinfo) {
			sprintf(ipBuf, "%d.%d.%d.%d", (pIPCinfo->dwIPCIP >> 24) & 0xFF,
						(pIPCinfo->dwIPCIP >> 16) & 0xFF,
						(pIPCinfo->dwIPCIP >> 8) & 0xFF,
						(pIPCinfo->dwIPCIP) & 0xFF);
		} else {
			strcpy(ipBuf, "0.0.0.0");
		}


		Item2->SetAttribute("ip", ipBuf);
		Item2->SetAttribute("connect",  m_bCameraStudent1Connect);
		printf("step 4\n");

		TiXmlElement *Item3 = new TiXmlElement("Model");
		Item3->SetAttribute("name", "studentCamera2");
		Item3->SetAttribute("valid", m_bCameraStudent2Valid);
		pIPCinfo = m_pKernel->getSetting()->getIPCInfo(2);
		if (NULL != pIPCinfo) {
			sprintf(ipBuf, "%d.%d.%d.%d", (pIPCinfo->dwIPCIP >> 24) & 0xFF,
						(pIPCinfo->dwIPCIP >> 16) & 0xFF,
						(pIPCinfo->dwIPCIP >> 8) & 0xFF,
						(pIPCinfo->dwIPCIP) & 0xFF);
		} else {
			strcpy(ipBuf, "0.0.0.0");
		}

		Item3->SetAttribute("ip", ipBuf);
		Item3->SetAttribute("connect",  m_bCameraStudent2Connect);
		printf("step 5\n");

		TiXmlElement* pItemBackCamera1 = new TiXmlElement("Model");
		pItemBackCamera1->SetAttribute("name", "BackCamera1");
		pItemBackCamera1->SetAttribute("valid", m_bBackCamera1Valid);
		pItemBackCamera1->SetAttribute("ip", "0.0.0.0");
		pItemBackCamera1->SetAttribute("connect", m_bBackCamera1Connect);
		printf("step 6\n");

		TiXmlElement* pItemBackCamera2 = new TiXmlElement("Model");
		pItemBackCamera2->SetAttribute("name", "BackCamera2");
		pItemBackCamera2->SetAttribute("valid", m_bBackCamera2Valid);
		pItemBackCamera2->SetAttribute("ip", "0.0.0.0");
		pItemBackCamera2->SetAttribute("connect", m_bBackCamera2Connect);
		printf("step 7\n");

		TiXmlElement* pItemVideoCenter = new TiXmlElement("Model");
		pItemVideoCenter->SetAttribute("name", "VideoCenter");
		pItemVideoCenter->SetAttribute("valid", m_bVideoCenterValid);
		pItemVideoCenter->SetAttribute("ip", m_pKernel->getSetting()->getWebServiceParam()->ACCUrl);
		pItemVideoCenter->SetAttribute("connect", m_bVideoCenterConnect);
		printf("step 8\n");

		TiXmlElement* pItemRTMP = new TiXmlElement("Model");
		pItemRTMP->SetAttribute("name", "RTMPUrl");
		pItemRTMP->SetAttribute("valid", m_bRTMPValid);
		pItemRTMP->SetAttribute("ip", m_pKernel->getSetting()->getInterRtmpUrl().c_str());
		pItemRTMP->SetAttribute("connect", m_bRTMPConnect);
		printf("step 9\n");

		TiXmlElement* pItemMtg = new TiXmlElement("Model");
		pItemMtg->SetAttribute("name", "MtgInfor");
		pItemMtg->SetAttribute("valid", m_bMtgValid);
		pItemMtg->SetAttribute("ip", m_pKernel->getSetting()->getMtgIp().c_str());
		pItemMtg->SetAttribute("connect", m_bMtgConnect);
		printf("step 10\n");

		TiXmlElement* pItemLiveCourse = new TiXmlElement("Model");
		pItemLiveCourse->SetAttribute("name", "LiveCourse");
		pItemLiveCourse->SetAttribute("valid", m_bLiveCourseValid);
		pItemLiveCourse->SetAttribute("ip", "0");
		pItemLiveCourse->SetAttribute("connect", m_bLiveCourseConnect);
		printf("step 11\n");

		TiXmlElement* pItemDetail = new TiXmlElement("Model");
		pItemDetail->SetAttribute("name", "Detail");
		pItemDetail->SetAttribute("valid", m_bDetailValid);
		pItemDetail->SetAttribute("ip", "0");

		if (0 == m_detailConnectInfo)  {
			pItemDetail->SetAttribute("connect", gStrStatusOK);
		} else if (DEVICE_ERROR_NO_SPACE == m_detailConnectInfo) {
			pItemDetail->SetAttribute("connect", gStrDiskfull);
		} else {
			pItemDetail->SetAttribute("connect", sStrExceptionError);
		}



		RootElement->LinkEndChild(Item0);
		RootElement->LinkEndChild(Item1);
		RootElement->LinkEndChild(Item2);
		RootElement->LinkEndChild(Item3);
		RootElement->LinkEndChild(pItemBackCamera1);
		RootElement->LinkEndChild(pItemBackCamera2);
		RootElement->LinkEndChild(pItemVideoCenter);
		RootElement->LinkEndChild(pItemRTMP);
		RootElement->LinkEndChild(pItemMtg);
		RootElement->LinkEndChild(pItemLiveCourse);
		RootElement->LinkEndChild(pItemDetail);

		printf("step 12\n");
		myDocument->SaveFile("ModelStatus.xml");//保存到文件
		printf("step 13\n");

		delete myDocument;
	} catch (string & e) {
		return false;
	}
	return true;
}


bool ModelStatus::queryCameraStatus() {
	m_bCameraTeacher1Valid = false;
	m_bCameraTeacher1Connect = false;

	m_bCameraTeacher2Valid = false;
	m_bCameraTeacher2Connect = false;

	m_bCameraStudent1Valid = false;
	m_bCameraStudent1Connect = false;

	m_bCameraStudent2Valid = false;
	m_bCameraStudent2Connect = false;

	for (int i = 0; i < MAX_IPC; ++i) {
		IPCInfo* info = m_pKernel->getSetting()->getIPCInfo(i);
		if (NULL == info) {
			continue;
		}
		if (info->dwIPCIP == 0) {
			continue;
		}

		KVideo::KVideoInterface* pVideoInterface = KVideoCreatorFactory::createInstance("HikVision");
		if (NULL != pVideoInterface) {
			pVideoInterface->setChannelID(0);
			pVideoInterface->getCardInfo()->setCardDescription(info->cIPCTypeName);
			char ipBuf[32] = {0};
			sprintf(ipBuf, "%d.%d.%d.%d", (info->dwIPCIP >> 24) & 0xFF,
					(info->dwIPCIP >> 16) & 0xFF,
					(info->dwIPCIP >> 8) & 0xFF,
					(info->dwIPCIP) & 0xFF);
			pVideoInterface->getCardInfo()->setIP(ipBuf);
			if (pVideoInterface->open(info->dwSubCode)) {
				printf("open hikvision video ok index %d, subcode %d\n", i, info->dwSubCode);
				if (i == 4) {
					m_bCameraTeacher1Valid = true;
					m_bCameraTeacher1Connect = true;
				} else if (i == 3) {
					m_bCameraTeacher2Valid = true;
					m_bCameraTeacher2Connect = true;
				} else if (i == 1) {
					m_bCameraStudent1Valid = true;
					m_bCameraStudent1Connect = true;
				} else if (i == 2) {
					m_bCameraStudent2Valid = true;
					m_bCameraStudent2Connect = true;
				}
			} else {
				printf("open hikvision video failed index %d, subcode %d\n", i, info->dwSubCode);
				if (i == 4) {
					m_bCameraTeacher1Valid = true;
				} else if (i == 3) {
					m_bCameraTeacher2Valid = true;
				} else if (i == 1) {
					m_bCameraStudent1Valid = true;
				} else if (i == 2) {
					m_bCameraStudent2Valid = true;
				}
			}

			printf("close step 1\n");
			pVideoInterface->close();
			printf("close step 2\n");

			delete pVideoInterface;

			pVideoInterface = NULL;
		}
	}

	return true;
}

bool ModelStatus::queryVideoCenterStatus() {
	ACCWebService webService;
	//check acc url is right
	m_bVideoCenterValid = false;
	if (!webService.setWebServiceURL(m_pKernel->getSetting()->getWebServiceParam()->ACCUrl)) {
		printf("acc service not ready\n");
		return false;
	}
	m_bVideoCenterValid = true;

	bool bLoginOK = webService.login(m_pKernel->getSetting()->getACCLoginName().c_str(),
			m_pKernel->getSetting()->getACCLoginPassword().c_str());
	if (!bLoginOK) {
		printf("acc login failed\n");
		return false;
	}

	m_bVideoCenterValid = true;
	m_bVideoCenterConnect = true;
	return true;
}

//when it is at recording status, then get the status from mixture mode
bool ModelStatus::queryRTMPStatus() {
	m_bRTMPValid = false;
	m_bRTMPConnect = false;
	if (m_pKernel->getSetting()->getInterRtmpUrl().length() < 5) {
		return false;
	}
	string strTestPath = m_pKernel->getSetting()->getInterRtmpUrl() + string("test");
	if (NULL != m_rtmpHandle) {
		delete m_rtmpHandle;
		m_rtmpHandle = NULL;
	}
	timespec tvRtmp;
	tvRtmp.tv_sec = time(NULL) + 5;
	tvRtmp.tv_nsec = 0;
	m_rtmpHandle = new RtmpHandle();
	m_rtmpHandle->addListener(this);
	m_rtmpHandle->init(strTestPath.c_str(),
					m_pKernel->getSetting()->getLiveOutWidth(),
					m_pKernel->getSetting()->getLiveOutHeight(),
					m_pKernel->getSetting()->getVideoFrameRate(),
					2,
					m_pKernel->getSetting()->getAudioSampleRate(),
					16);

	sem_timedwait(&m_RtmpSem, &tvRtmp);
	return true;
}

bool ModelStatus::queryMtgStatus() {
	if (NULL != m_pMtgClient) {
		delete m_pMtgClient;
		m_pMtgClient = NULL;
	}

	timespec tvMtg;
	tvMtg.tv_sec = time(NULL) + 5;
	tvMtg.tv_nsec = 0;

	m_pMtgClient = new MtgClient();
	//test,
	int userId = LiveCourseUtils::generate_uid(m_pKernel->getSetting()->getClassRoomID(), 0);
	m_pMtgClient->init(m_pKernel->getSetting()->getMtgIp().c_str(), 10000, userId);
	m_pMtgClient->addListener(this);
	m_pMtgClient->joinConf();
	sem_timedwait(&m_MtgSem, &tvMtg);


	return true;
}

bool ModelStatus::queryLiveCourseStatus() {
	return true;
}


bool ModelStatus::queryDetailStatus() {
	m_detailConnectInfo = 0;
	int nDiskSpace = CommonUtils::getDiskFreeSpace(m_pKernel->getSetting()->getSavePath().c_str());
	if (-1 != nDiskSpace) {
		//printf("disk space is %d G\n", nDiskSpace);
		if (nDiskSpace <= m_pKernel->getSetting()->getDiskSpaceAlertSize()) {
			m_detailConnectInfo = DEVICE_ERROR_NO_SPACE;
		}
	} else {
		m_detailConnectInfo = DEVICE_ERROR_NO_SPACE;
	}

	return true;
}
