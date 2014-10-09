#include "setting.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include "./core/constDefines.h"
#include "./utils/commonUtils.h"

#include "./liveCourse/livemtgConstDefine.h"
#include "./utils/liveCourseUtils.h"

#include <iostream>
using namespace std;

const string KLCRecordPrefixPath = string(LIVECOURSE_PATH_VIDEO_RECODER_LOW) + string("/");
const string KLCVGAPrefixPath = string(LIVECOURSE_PATH_VIDEO_VGA_MID) + string("/");
const string KLCSpeexPrefixPath = string(LIVEMTG_PATH_AUDIO_SPEEX) + string("/");
const string KLCTeacherPrefixPath = string(LIVECOURSE_PATH_VIDEO_TEACHER_LOW_1) + string("/");
const string KLCStudentPrefixPath = string(LIVECOURSE_PATH_VIDEO_STUDENT_LOW_1) + string("/");
const string KLCAACPrexfixPath = string(LIVEMTG_PATH_AUDIO_AAC) + string("/");



Setting::Setting()
: m_eventId(0) {
	for (int i = 0; i < MAX_IPC; ++i) {
		m_IPCInfo[i].dwSubCode = 0;
	}

	for (int i = 0; i < SOURCE_SCENE_COUNT; ++i) {
		m_sceneVideoInfo[i].dwLeft = 0;
		m_sceneVideoInfo[i].dwRight = 0;
		m_sceneVideoInfo[i].dwTop = 0;
		m_sceneVideoInfo[i].dwBottom = 0;
	}

	m_IPCChannel.dwTeacherChannel = 0;
	m_IPCChannel.dwStudentChannel = 1;
	m_IPCChannel.dwDeviceChannel = 0;

	m_oldIPCChannel.dwTeacherChannel = 0;
	m_oldIPCChannel.dwStudentChannel = 0;
	m_oldIPCChannel.dwDeviceChannel = 0;

	m_bHDMIDisplayEnable = false;

	for (int i = 0; i < MAX_RESOLUTION_COUNT; ++i) {
		m_videoDelayParams[i].nCameraRepeatFrame = 0;
		m_videoDelayParams[i].nVGASlowFrameCount = 0;
	}

	m_bLiveEncHighProfile = true;
	m_bAutoUploadFile = false;
	m_nDeleteFrameCount = 0;
	m_bAutoDeleteFiles = false;
	m_nDaysBeforeCanDelete = 7;

	m_strDestPath = "";
	m_bEncodeQulityEnable = false;
	m_bNotifyVGAChangeEnable = false;


	m_bDualScreenCrop = true;
	m_bVGASwitchOldPanel = false;

	m_bLCSendResourceEnable = false;

	m_bRoomIDValid = false;

	m_liveVideoWidth = 1280;
	m_liveVideoHeight = 720;

	m_strSchool = new char[100];
	m_strCampus = new char[100];
	m_strBuilding = new char[100];
	m_strRoomName = new char[100];
	memset(m_strSchool, 0, 100);
	memset(m_strCampus, 0, 100);
	memset(m_strBuilding, 0, 100);
	memset(m_strRoomName, 0, 100);
}

Setting::~Setting() {
	if (NULL != m_strSchool) {
		delete []m_strSchool;
		m_strSchool = NULL;
	}
	if (NULL != m_strCampus) {
		delete []m_strCampus;
		m_strCampus = NULL;
	}
	if (NULL != m_strBuilding) {
		delete []m_strBuilding;
		m_strBuilding = NULL;
	}
	if (NULL != m_strRoomName) {
		delete []m_strRoomName;
		m_strRoomName = NULL;
	}
}

bool Setting::readSetting() {
	try {
		//创建一个XML的文档对象。
		TiXmlDocument *myDocument = new TiXmlDocument("setting.xml");
		myDocument->LoadFile();

		TiXmlHandle docHandle(myDocument);
		TiXmlHandle pSettingElement = docHandle.FirstChild("Setting");

		TiXmlElement *pRecordMode = pSettingElement.Child("RecordMode", 0).ToElement();
		if (NULL != pRecordMode) {
			m_recordMode = atoi(pRecordMode->FirstChild()->Value());
		}

		TiXmlElement *pVideoFrameRate = pSettingElement.Child("VideoFrameRate", 0).ToElement();
		if (NULL != pVideoFrameRate) {
			m_videoFramerate = atoi (pVideoFrameRate->FirstChild()->Value());
		}

		TiXmlElement *pVideoWidth0 = pSettingElement.Child("VideoWidth0", 0).ToElement();
		if (NULL != pVideoWidth0) {
			m_IPCInfo[0].dwIPCWidth = atoi (pVideoWidth0->FirstChild()->Value());
		}


		TiXmlElement *pVideoHeight0 = pSettingElement.Child("VideoHeight0", 0).ToElement();
		if (NULL != pVideoHeight0) {
			m_IPCInfo[0].dwIPCHeight = atoi (pVideoHeight0->FirstChild()->Value());
		}


		TiXmlElement *pCameraIP0 = pSettingElement.Child("CameraIP0", 0).ToElement();
		if (NULL != pCameraIP0) {
			m_IPCInfo[0].dwIPCIP = ntohl(inet_addr(pCameraIP0->FirstChild()->Value()));
		}

		TiXmlElement *pVideoWidth1 = pSettingElement.Child("VideoWidth1", 0).ToElement();
		if (NULL != pVideoWidth1) {
			m_IPCInfo[1].dwIPCWidth = atoi (pVideoWidth1->FirstChild()->Value());
		}


		TiXmlElement *pVideoHeight1 = pSettingElement.Child("VideoHeight1", 0).ToElement();
		if (NULL != pVideoHeight1) {
			m_IPCInfo[1].dwIPCHeight = atoi (pVideoHeight1->FirstChild()->Value());
		}


		TiXmlElement *pCameraIP1 = pSettingElement.Child("CameraIP1", 0).ToElement();
		if (NULL != pCameraIP1) {
			m_IPCInfo[1].dwIPCIP = ntohl(inet_addr(pCameraIP1->FirstChild()->Value()));
		}
		printf("step cameraIP1\n");

		TiXmlElement *pVideoWidth2 = pSettingElement.Child("VideoWidth2", 0).ToElement();
		if (NULL != pVideoWidth2) {
			m_IPCInfo[2].dwIPCWidth = atoi (pVideoWidth2->FirstChild()->Value());
		}


		TiXmlElement *pVideoHeight2 = pSettingElement.Child("VideoHeight2", 0).ToElement();
		if (NULL != pVideoHeight2) {
			m_IPCInfo[2].dwIPCHeight = atoi (pVideoHeight2->FirstChild()->Value());
		}


		TiXmlElement *pCameraIP2 = pSettingElement.Child("CameraIP2", 0).ToElement();
		if (NULL != pCameraIP2) {
			m_IPCInfo[2].dwIPCIP = ntohl(inet_addr(pCameraIP2->FirstChild()->Value()));
		}


		TiXmlElement *pOutFileName = pSettingElement.Child("OutFileName", 0).ToElement();
		if (NULL != pOutFileName) {
			m_strSaveFile = pOutFileName->FirstChild()->Value();
		}


		TiXmlElement *pVideoBitrate = pSettingElement.Child("VideoBitrate", 0).ToElement();
		if (NULL != pVideoBitrate) {
			m_recordParam.dwRecordKBitrate = atoi(pVideoBitrate->FirstChild()->Value());
		}


		TiXmlElement *pSaveWidth = pSettingElement.Child("SaveWidth", 0).ToElement();
		if (NULL != pSaveWidth) {
			m_recordParam.dwWidth = atoi(pSaveWidth->FirstChild()->Value());
		}


		TiXmlElement *pSaveHeight = pSettingElement.Child("SaveHeight", 0).ToElement();
		if (NULL != pSaveHeight) {
			m_recordParam.dwHeight = atoi(pSaveHeight->FirstChild()->Value());
		}
		printf("step saveHeigt\n");

		TiXmlElement *pAudioSampleRate = pSettingElement.Child("AudioSampleRate", 0).ToElement();
		if (NULL != pAudioSampleRate) {
			m_audioSampleRate = atoi(pAudioSampleRate->FirstChild()->Value());
		}


		TiXmlElement* pAACBitRate = pSettingElement.Child("AACBitRate", 0).ToElement();
		if (NULL != pAACBitRate) {
			m_aacBitRate = atoi(pAACBitRate->FirstChild()->Value());
		}


		TiXmlElement *pPCMNonBlock = pSettingElement.Child("PCMNonBlock", 0).ToElement();
		if (NULL != pPCMNonBlock) {
			m_bPCMNonBlock = strcmp(pPCMNonBlock->FirstChild()->Value(), "true") == 0
							|| strcmp(pPCMNonBlock->FirstChild()->Value(), "TRUE") == 0;
		}


		TiXmlElement* pAudioDeviceName = pSettingElement.Child("AudioDeviceName", 0).ToElement();
		if (NULL != pAudioDeviceName) {
			m_strAudioDeviceName = pAudioDeviceName->FirstChild()->Value();
		}

		printf("step AudioDeviceName\n");

		TiXmlElement *pRtmpUrl = pSettingElement.Child("RtmpUrl", 0).ToElement();
		if (NULL != pRtmpUrl) {
			m_strInterRtmpUrl = pRtmpUrl->FirstChild()->Value();
		}


		TiXmlElement *pRtmpEnable = pSettingElement.Child("RtmpEnable", 0).ToElement();
		if (NULL != pRtmpEnable) {
			m_bRtmpEnable = strcmp(pRtmpEnable->FirstChild()->Value(), "true") == 0
					|| strcmp(pRtmpEnable->FirstChild()->Value(), "TRUE") == 0;
		}


		TiXmlElement *pMtgIp = pSettingElement.Child("MtgIP", 0).ToElement();
		if (NULL != pMtgIp) {
			m_strMtgIp = pMtgIp->FirstChild()->Value();
		}


		TiXmlElement *pMtgEnable = pSettingElement.Child("MtgEnable", 0).ToElement();
		if (NULL != pMtgEnable) {
			m_bMtgEnable = strcmp(pMtgEnable->FirstChild()->Value(), "true") == 0
					|| strcmp(pMtgEnable->FirstChild()->Value(), "TRUE") == 0;
		}

		printf("step MtgEnable\n");

		TiXmlElement *pMtgConfId = pSettingElement.Child("RoomId", 0).ToElement();
		if (NULL != pMtgConfId) {
			m_strClassRoomID = pMtgConfId->FirstChild()->Value();
		}
		printf("step class room\n");

		m_strLCRecordPath = KLCRecordPrefixPath + m_strClassRoomID;
		m_strLCVGAPath = KLCVGAPrefixPath + m_strClassRoomID;
		m_strLCSpeexPath = KLCSpeexPrefixPath + m_strClassRoomID;
		m_strLCAACPath = KLCAACPrexfixPath + m_strClassRoomID;
		m_strLCTeacherPath = KLCTeacherPrefixPath + m_strClassRoomID;
		m_strLCStudentPath = KLCStudentPrefixPath + m_strClassRoomID;

		TiXmlElement* pUnitIp = pSettingElement.Child("UnitIP", 0).ToElement();
		m_strUnitIP = pUnitIp->FirstChild()->Value();

		TiXmlElement *pUnitPort = pSettingElement.Child("UnitPort", 0).ToElement();
		m_unitPort = atoi(pUnitPort->FirstChild()->Value());

		TiXmlElement *pOwnIP = pSettingElement.Child("OwnIP", 0).ToElement();
		m_strOwnIP = pOwnIP->FirstChild()->Value();

		TiXmlElement *pOwnPort = pSettingElement.Child("OwnPort", 0).ToElement();
		m_ownPort = atoi(pOwnPort->FirstChild()->Value());
		printf("step OwnPort\n");

		TiXmlElement *pTemplate = pSettingElement.Child("TemplateXMLName", 0).ToElement();
		m_strRecordTemplateXMl = pTemplate->FirstChild()->Value();

		TiXmlElement* pTemplatePath = pSettingElement.Child("RecordTemplatePath", 0).ToElement();
		m_strRecordTemplatePath = pTemplatePath->FirstChild()->Value();

		TiXmlElement* pTempPath = pSettingElement.Child("TempPath", 0).ToElement();
		m_strTempPath = pTempPath->FirstChild()->Value();

		TiXmlElement* pSavePath = pSettingElement.Child("SavePath", 0).ToElement();
		m_strSavePath = pSavePath->FirstChild()->Value();
		printf("step SavePath\n");

		TiXmlElement* pACCLoginName = pSettingElement.Child("ACCLoginName", 0).ToElement();
		m_strACCLoginName = pACCLoginName->FirstChild()->Value();

		TiXmlElement* pACCLoginPassword = pSettingElement.Child("ACCLoginPassword", 0).ToElement();
		m_strACCLoginPassword = pACCLoginPassword->FirstChild()->Value();

		TiXmlElement* pACCWebService = pSettingElement.Child("ACCWebServiceUrl", 0).ToElement();
		strcpy(m_webServiceParam.ACCUrl, pACCWebService->FirstChild()->Value());
		printf("step ACCWebServiceUrl\n");

		TiXmlElement* pLiveEncBitrate = pSettingElement.Child("LiveEncBitrate", 0).ToElement();
		m_liveEncBitrate = atoi(pLiveEncBitrate->FirstChild()->Value());

		TiXmlElement* pVGAEncBitrate = pSettingElement.Child("VGAEncBitrate", 0).ToElement();
		m_vgaEncBitrate = atoi(pVGAEncBitrate->FirstChild()->Value());

		TiXmlElement* pAudioDelayFrames = pSettingElement.Child("AudioDelayFrame", 0).ToElement();
		m_audioDelayFrames = atoi(pAudioDelayFrames->FirstChild()->Value());

		TiXmlElement* pHDMIDisplayEnable = pSettingElement.Child("HDMIDisplayEnable", 0).ToElement();
		m_bHDMIDisplayEnable = strcmp(pHDMIDisplayEnable->FirstChild()->Value(), "true") == 0
						|| strcmp(pHDMIDisplayEnable->FirstChild()->Value(), "TRUE") == 0;

		TiXmlElement* pHDMIName = pSettingElement.Child("HDMIDisplayDeviceName", 0).ToElement();
		m_strHDMIDisplayDeviceName = pHDMIName->FirstChild()->Value();

		TiXmlElement* pVGADisplayEnable = pSettingElement.Child("VGADisplayEnable", 0).ToElement();
		m_bVGADisplayEnable = strcmp(pVGADisplayEnable->FirstChild()->Value(), "true") == 0
				|| strcmp(pVGADisplayEnable->FirstChild()->Value(), "TRUE") == 0;

		TiXmlElement* pVGADisplayName = pSettingElement.Child("VGADisplayDeviceName", 0).ToElement();
		m_strVGADisplayDeviceName = pVGADisplayName->FirstChild()->Value();
		printf("step VGADisplayDeviceName\n");

		TiXmlElement* pVGASlowFrame = pSettingElement.Child("VGASlowFrameCount_1920_1080", 0).ToElement();
		m_videoDelayParams[0].nVGASlowFrameCount = atoi(pVGASlowFrame->FirstChild()->Value());

		TiXmlElement* pCameraRepeatFrame = pSettingElement.Child("CameraRepeatCount_1920_1080", 0).ToElement();
		m_videoDelayParams[0].nCameraRepeatFrame = atoi(pCameraRepeatFrame->FirstChild()->Value());

		pVGASlowFrame = pSettingElement.Child("VGASlowFrameCount_1536_768", 0).ToElement();
		m_videoDelayParams[1].nVGASlowFrameCount = atoi(pVGASlowFrame->FirstChild()->Value());

		pCameraRepeatFrame = pSettingElement.Child("CameraRepeatCount_1536_768", 0).ToElement();
		m_videoDelayParams[1].nCameraRepeatFrame = atoi(pCameraRepeatFrame->FirstChild()->Value());

		pVGASlowFrame = pSettingElement.Child("VGASlowFrameCount_1280_720", 0).ToElement();
		m_videoDelayParams[2].nVGASlowFrameCount = atoi(pVGASlowFrame->FirstChild()->Value());

		pCameraRepeatFrame = pSettingElement.Child("CameraRepeatCount_1280_720", 0).ToElement();
		m_videoDelayParams[2].nCameraRepeatFrame = atoi(pCameraRepeatFrame->FirstChild()->Value());

		pVGASlowFrame = pSettingElement.Child("VGASlowFrameCount_1024_768", 0).ToElement();
		m_videoDelayParams[3].nVGASlowFrameCount = atoi(pVGASlowFrame->FirstChild()->Value());

		pCameraRepeatFrame = pSettingElement.Child("CameraRepeatCount_1024_768", 0).ToElement();
		m_videoDelayParams[3].nCameraRepeatFrame = atoi(pCameraRepeatFrame->FirstChild()->Value());

		pVGASlowFrame = pSettingElement.Child("VGASlowFrameCount_1280_1024", 0).ToElement();
		m_videoDelayParams[4].nVGASlowFrameCount = atoi(pVGASlowFrame->FirstChild()->Value());

		pCameraRepeatFrame = pSettingElement.Child("CameraRepeatCount_1280_1024", 0).ToElement();
		m_videoDelayParams[4].nCameraRepeatFrame = atoi(pCameraRepeatFrame->FirstChild()->Value());

		TiXmlElement* pLiveEncVOPInter = pSettingElement.Child("LiveEncVOPInter", 0).ToElement();
		m_nLiveEncVOPInter = atoi(pLiveEncVOPInter->FirstChild()->Value());

		TiXmlElement* pLiveEncHighProfile = pSettingElement.Child("LiveEncHighProfile", 0).ToElement();
		m_bLiveEncHighProfile = strcmp(pLiveEncHighProfile->FirstChild()->Value(), "true") == 0
				|| strcmp(pLiveEncHighProfile->FirstChild()->Value(), "TRUE") == 0;

		TiXmlElement* pDiskSpaceAlertSize = pSettingElement.Child("DiskSpaceAlertSize", 0).ToElement();
		m_nDiskSpaceAlertSize = atoi(pDiskSpaceAlertSize->FirstChild()->Value());

		TiXmlElement* pDeleteFrameCount = pSettingElement.Child("DeleteFrameCount", 0).ToElement();
		m_nDeleteFrameCount = atoi(pDeleteFrameCount->FirstChild()->Value());

		TiXmlElement* pLiveDeleteFrameCount = pSettingElement.Child("LiveFrameCountDelete", 0).ToElement();
		m_nLiveDeleteFrameCount = atoi(pLiveDeleteFrameCount->FirstChild()->Value());

		m_bLiveEncEnable = true;
		//m_bLiveEncEnable = (m_bRtmpEnable || m_bMtgEnable);
		m_bVGAEncodeEnable = m_bMtgEnable;


		TiXmlElement* pAutoDeleteFiles = pSettingElement.Child("AutoDeleteFiles", 0).ToElement();
		if (NULL != pAutoDeleteFiles) {
			m_bAutoDeleteFiles = strcmp(pAutoDeleteFiles->FirstChild()->Value(), "true") == 0
					|| strcmp(pAutoDeleteFiles->FirstChild()->Value(), "TRUE") == 0;
		}

		TiXmlElement* pDaysBeforeCanDelete = pSettingElement.Child("DaysBeforeCanDelete", 0).ToElement();
		if (NULL != pDaysBeforeCanDelete) {
			m_nDaysBeforeCanDelete = atoi(pDaysBeforeCanDelete->FirstChild()->Value());
		}

		TiXmlElement* pDestPath = pSettingElement.Child("DestPath", 0).ToElement();
		if (NULL != pDestPath) {
			m_strDestPath = pDestPath->FirstChild()->Value();
		}

		TiXmlElement* pEncodeQuality = pSettingElement.Child("EncodeQualityEnable", 0).ToElement();
		if (NULL != pEncodeQuality) {
			m_bEncodeQulityEnable = strcmp(pEncodeQuality->FirstChild()->Value(), "true") == 0
					|| strcmp(pEncodeQuality->FirstChild()->Value(), "TRUE") == 0;
		}

		TiXmlElement* pNotifyVGAChangeEnable = pSettingElement.Child("NotifyVGAChangeEnable", 0).ToElement();
		if (NULL != pNotifyVGAChangeEnable) {
			m_bNotifyVGAChangeEnable = strcmp(pNotifyVGAChangeEnable->FirstChild()->Value(), "true") == 0
					|| strcmp(pNotifyVGAChangeEnable->FirstChild()->Value(), "TRUE") == 0;
		}
		printf("vga change enable %d\n", m_bNotifyVGAChangeEnable);

		TiXmlElement* pSharePathElement = pSettingElement.Child("SharePath", 0).ToElement();
		if (NULL != pSharePathElement) {
			m_strSharePath = pSharePathElement->FirstChild()->Value();
			printf("the share path is %s\n", m_strSharePath.c_str());
		}

		TiXmlElement* pSharePathUsernameElement = pSettingElement.Child("ShareUsername", 0).ToElement();
		if (NULL != pSharePathUsernameElement) {
			m_strSharePathUsername = pSharePathUsernameElement->FirstChild()->Value();
			printf("the share path username is %s\n", m_strSharePathUsername.c_str());
		}

		TiXmlElement* pDualScreenCrop = pSettingElement.Child("DualScreenCrop", 0).ToElement();
		if (NULL != pDualScreenCrop) {
			m_bDualScreenCrop = strcmp(pDualScreenCrop->FirstChild()->Value(), "true") == 0
					|| strcmp(pDualScreenCrop->FirstChild()->Value(), "TRUE") == 0;
		}

		TiXmlElement* pVGASwitchOldPanel = pSettingElement.Child("VGASwitchOldPanel", 0).ToElement();
		if (NULL != pVGASwitchOldPanel) {
			m_bVGASwitchOldPanel = strcmp(pVGASwitchOldPanel->FirstChild()->Value(), "true") == 0
					|| strcmp(pVGASwitchOldPanel->FirstChild()->Value(), "TRUE") == 0;
		}

		TiXmlElement* pLiveVideoWidth = pSettingElement.Child("LiveVideoWidth", 0).ToElement();
		if (NULL != pLiveVideoWidth) {
			m_liveVideoWidth = atoi(pLiveVideoWidth->FirstChild()->Value());
		}

		TiXmlElement* pLiveVideoHeight = pSettingElement.Child("LiveVideoHeight", 0).ToElement();
		if (NULL != pLiveVideoHeight) {
			m_liveVideoHeight = atoi(pLiveVideoHeight->FirstChild()->Value());
		}

		TiXmlElement* pLCSendResource = pSettingElement.Child("MtgSendResourceEnable", 0).ToElement();
		if (NULL != pLCSendResource) {
			m_bLCSendResourceEnable = strcmp(pLCSendResource->FirstChild()->Value(), "true") == 0
					|| strcmp(pLCSendResource->FirstChild()->Value(), "TRUE") == 0;
		}
		printf("mtg send resource enable %d\n", m_bLCSendResourceEnable);
		printf("before read template\n");
		readTemplate();

		delete myDocument;
	} catch (string & e) {
		return false;
	}
	return true;
}

bool Setting::writeSetting() {
	try {
		//创建一个XML的文档对象。
		TiXmlDocument *myDocument = new TiXmlDocument("setting.xml");
		if (!myDocument->LoadFile()) {
			//create new file
			delete myDocument;
			return true;
		}

		TiXmlHandle docHandle(myDocument);
		TiXmlHandle pSettingElement = docHandle.FirstChild("Setting");

		TiXmlElement *pCameraIP0 = pSettingElement.FirstChildElement("CameraIP0").ToElement();
		if (NULL != pCameraIP0) {
			TiXmlNode * oldnode =  pCameraIP0->FirstChild();
			IPCInfo Info = m_IPCInfo[m_IPCChannel.dwTeacherChannel];
			char ipBuf[32] = {0};
			sprintf(ipBuf, "%d.%d.%d.%d", (Info.dwIPCIP >> 24) & 0xFF,
							(Info.dwIPCIP >> 16) & 0xFF,
							(Info.dwIPCIP >> 8) & 0xFF,
							(Info.dwIPCIP) & 0xFF );
			TiXmlText newText(ipBuf);
			pCameraIP0->ReplaceChild(oldnode, newText);
		}

		TiXmlElement *pCameraIP1 = pSettingElement.FirstChildElement("CameraIP1").ToElement();
		if (NULL != pCameraIP1) {
			TiXmlNode * oldnode =  pCameraIP1->FirstChild();
			IPCInfo Info = m_IPCInfo[m_IPCChannel.dwStudentChannel];
			char ipBuf[32] = {0};
			sprintf(ipBuf, "%d.%d.%d.%d", (Info.dwIPCIP >> 24) & 0xFF,
							(Info.dwIPCIP >> 16) & 0xFF,
							(Info.dwIPCIP >> 8) & 0xFF,
							(Info.dwIPCIP) & 0xFF );
			TiXmlText newText(ipBuf);
			pCameraIP1->ReplaceChild(oldnode, newText);
		}

		TiXmlElement *pCameraIP2 = pSettingElement.FirstChildElement("CameraIP2").ToElement();
		if (NULL != pCameraIP2) {
			TiXmlNode * oldnode =  pCameraIP2->FirstChild();
			IPCInfo Info = m_IPCInfo[m_IPCChannel.dwStudentChannel];
			char ipBuf[32] = {0};
			sprintf(ipBuf, "%d.%d.%d.%d", (Info.dwIPCIP >> 24) & 0xFF,
							(Info.dwIPCIP >> 16) & 0xFF,
							(Info.dwIPCIP >> 8) & 0xFF,
							(Info.dwIPCIP) & 0xFF );
			TiXmlText newText(ipBuf);
			pCameraIP2->ReplaceChild(oldnode, newText);
		}
		myDocument->SaveFile();
		sync();
		delete myDocument;
	} catch (string & e) {
		return false;
	}
	return true;
}

string Setting::getWebID(){
	return m_strWebID;
}

string Setting::getOwnIP(){
	return m_strOwnIP;
}

WORD Setting::getOwnPort(){
	return m_ownPort;
}

string Setting::getUnitIP(){
	return m_strUnitIP;
}

WORD Setting::getUnitPort(){
	return m_unitPort;
}

void Setting::setRecordMode(int mode) {
	m_recordMode = mode;
	printf("the record mode is %d\n", mode);
}

int Setting::getRecordMode() {
	return m_recordMode;
}

int Setting::getVideoFrameRate() {
	return m_videoFramerate;
}


string Setting::updateSaveFileName(bool bPreview) {
	time_t curTime = time(NULL);
	tm* t = localtime(&curTime);
	//CommonUtils::changeUtcToBeijing(t);

	char buf[255] = {0};
	if (0 == m_strWebID.length())  {
		//为了简单的将utc转成北京时间+8,不考虑日期的偏移
		sprintf(buf, "%s_%s_%4d%02d%02d_%02d%02d%02d.%s", m_recordUtf8Param.cUtf8CourseName,
				m_recordUtf8Param.cUtf8TeacherName, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
				t->tm_hour, t->tm_min, t->tm_sec, "mp4");
	} else {
		sprintf(buf, "%s_%s_%s_%4d%02d%02d_%02d%02d%02d.%s", m_strWebID.c_str(), m_recordUtf8Param.cUtf8CourseName,
				m_recordUtf8Param.cUtf8TeacherName, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
				t->tm_hour, t->tm_min, t->tm_sec, ".mp4");
	}

	if (bPreview) {
		m_strSaveFile = m_strTempPath + buf;
	} else {
		m_strSaveFile = m_strSavePath + buf;
	}
	printf("strSaveFile %s\n", m_strSaveFile.c_str());
	m_strUtf8RelateFileName = buf;

	if (0 == m_strWebID.length())  {
		sprintf(buf, "%s_%s_%4d%02d%02d_%02d%02d%02d.%s", m_recordParam.cCourseName,
				m_recordParam.cTeacherName, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
				t->tm_hour, t->tm_min, t->tm_sec, "mp4");
	} else {
		sprintf(buf, "%s_%s_%s_%4d%02d%02d_%02d%02d%02d.%s", m_strWebID.c_str(), m_recordParam.cCourseName,
				m_recordParam.cTeacherName, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
				t->tm_hour, t->tm_min, t->tm_sec, ".mp4");
	}
	m_strUnicodeRelateFileName = buf;
	return m_strSaveFile;
}

string Setting::getSaveFileName() {
	return m_strSaveFile;
}

string Setting::getUtf8RelateFileName() {
	return m_strUtf8RelateFileName;
}

string Setting::getUnicodeRelateFileName() {
	return m_strUnicodeRelateFileName;
}

string Setting::getSavePath() {
	return m_strSavePath;
}

int Setting::getSaveWidth() {
	return m_recordParam.dwWidth;
}

int Setting::getSaveHeight() {
	return m_recordParam.dwHeight;
}

int Setting::getAudioSampleRate() {
	return m_audioSampleRate;
}

int Setting::getAACBitRate() {
	return m_aacBitRate;
}

bool Setting::getPCMNonBlock() {
	return m_bPCMNonBlock;
}

string Setting::getAudioDeviceName() {
	return m_strAudioDeviceName;
}

int Setting::getVideoBitrate() {
	return m_recordParam.dwRecordKBitrate * 1000;
}

void Setting::setHeadEnable(bool bEnable) {
	m_bHeaderEnable = bEnable;
}

bool Setting::getHeaderEnable() {
	return m_bHeaderEnable;
}


bool Setting::getRtmpEnable() {
	return m_bRtmpEnable;
}

void Setting::setRtmpEnable(bool bRtmpEnable) {
	m_bRtmpEnable = bRtmpEnable;
}

string Setting::getInterRtmpUrl() {
	return m_strInterRtmpUrl;
}

string	Setting::getLCRtmpUrl() {
	return m_strLCRtmpUrl;
}

void Setting::setLCRtmpUrl(const char* pLCRtmpUrl) {
	m_strLCRtmpUrl = pLCRtmpUrl;
	printf("lc rtmp url length is %d\n", m_strLCRtmpUrl.length());
}

bool Setting::getMtgEnable() {
	return m_bMtgEnable;
}

void Setting::setMtgEnable(bool bMtgEnable) {
	m_bMtgEnable = bMtgEnable;
}

bool Setting::getLCSendResourceEnable() {
	return m_bLCSendResourceEnable;
}

void Setting::setLCSendResourceEnable(bool bLCSendResourceEnable) {
	m_bLCSendResourceEnable = bLCSendResourceEnable;
}

string Setting::getMtgIp() {
	return m_strMtgIp;
}

string Setting::getLCRecordPath() {
	return m_strLCRecordPath;
}

string Setting::getClassRoomID() {
	return m_strClassRoomID;
}

string Setting::getLCVGAPath() {
	return m_strLCVGAPath;
}

string Setting::getLCTeacherPath() {
	return m_strLCTeacherPath;
}

string Setting::getLCStudentPath() {
	return m_strLCStudentPath;
}

string Setting::getLCSpeexPath() {
	return m_strLCSpeexPath;
}

string Setting::getLCAACPath() {
	return m_strLCAACPath;
}

bool Setting::getVGAEncodeEnable() {
	return m_bVGAEncodeEnable;
}


void Setting::setBaseRecordParams(BaseRecordParam param) {
	memcpy(&m_recordParam, &param, sizeof(BaseRecordParam));
	printf("set record param \n");
	//由于使用osd中文只认utf8，而接收到的是unicode编码

	printf("course name %s\n", m_recordParam.cCourseName);
	printf("publish point %s\n ", m_recordParam.cPublishPoint);
	printf("teacher name %s\n", m_recordParam.cTeacherName);
	printf("record width %d\n", m_recordParam.dwWidth);
	printf("record height %d\n", m_recordParam.dwHeight);
	printf("record bitrate %d\n", m_recordParam.dwRecordKBitrate);
	printf("record bool is resource mode %d\n", m_recordParam.bResourceMode);
	printf("record template is %s\n", m_recordParam.cRecordStyleName);
}

BaseRecordParam* Setting::getBaseRecordParams() {
	return &m_recordParam;
}

void Setting::setUtf8RecordParam(stUtf8RecordParam* pParam) {
	memcpy(&m_recordUtf8Param, pParam, sizeof(stUtf8RecordParam));

	printf("utf8 course name is %s\n", m_recordUtf8Param.cUtf8CourseName);
	printf("utf8 teacher name is %s\n", m_recordUtf8Param.cUtf8TeacherName);
	printf("utf8 title name is %s\n", m_recordUtf8Param.cUtf8TitleName);
}

stUtf8RecordParam* Setting::getUtf8RecordParam() {
	return &m_recordUtf8Param;
}

void Setting::setACCParam(PRACCParam param) {
	m_accParam = param;
	printf("acc param %s\n", m_accParam.AutoSavePath);
	int nLength = strlen(m_accParam.AutoSavePath);
	printf("the aac param length is %d\n", nLength);
	int nStep = 0;
	m_strACCLoginName.clear();
	m_strACCLoginPassword.clear();
	m_strACCProductID.clear();
	m_strACCCourseID.clear();
	m_strACCServerID.clear();
	m_strACCFolderID.clear();
	m_strACCFolderTreeID.clear();
	m_strACCFolderName.clear();
	m_strACCFolderType.clear();
	for (int i = 0; i < nLength; ++i) {
		if (m_accParam.AutoSavePath[i] == ',') {
			nStep++;
		} else {
			if (nStep == 0) {
				m_strACCLoginName.push_back(m_accParam.AutoSavePath[i]);
			} else if (nStep == 1) {
				m_strACCLoginPassword.push_back(m_accParam.AutoSavePath[i]);
			} else if (nStep == 2) {
				//orig name
			} else if (nStep == 3) {
				//new name
			} else if (nStep == 4) {
				m_strACCProductID.push_back(m_accParam.AutoSavePath[i]);
			} else if (nStep == 5) {
				m_strACCCourseID.push_back(m_accParam.AutoSavePath[i]);
			} else if (nStep == 6) {
				m_strACCServerID.push_back(m_accParam.AutoSavePath[i]);
			} else if (nStep == 7) {
				m_strACCFolderID.push_back(m_accParam.AutoSavePath[i]);
			} else if (nStep == 8) {
				m_strACCFolderTreeID.push_back(m_accParam.AutoSavePath[i]);
			} else if (nStep == 9) {
				m_strACCFolderName.push_back(m_accParam.AutoSavePath[i]);
			} else if (nStep == 10) {
				m_strACCFolderType.push_back(m_accParam.AutoSavePath[i]);
			}
		}
	}
	printf("the acc loginName is %s\n", m_strACCLoginName.c_str());
	printf("the acc loginPassword is %s\n", m_strACCLoginPassword.c_str());
	printf("the acc ProductID is %s\n", m_strACCProductID.c_str());
	printf("the acc courseID is %s\n", m_strACCServerID.c_str());
	printf("the acc FolderID is %s\n", m_strACCFolderID.c_str());
	printf("the acc FolderTreeID is %s\n", m_strACCFolderTreeID.c_str());
	printf("the acc FolderName is %s\n", m_strACCFolderName.c_str());
	printf("the acc FolderType is %s\n", m_strACCFolderType.c_str());
	printf("end set acc param step %d\n", nStep);
	m_bAutoUploadFile = (nStep >= 10);
}

PRACCParam* Setting::getACCParam() {
	return &m_accParam;
}

string  Setting::getACCLoginName() {
	return m_strACCLoginName;
}

string  Setting::getACCLoginPassword() {
	return m_strACCLoginPassword;
}

string Setting::getACCProductID() {
	return m_strACCProductID;
}

string  Setting::getACCCourseID() {
	return m_strACCCourseID;
}

string  Setting::getACCServerID() {
	return m_strACCServerID;
}

string  Setting::getACCFolderID() {
	return m_strACCFolderID;
}

string  Setting::getACCFolderTreeID() {
	return m_strACCFolderTreeID;
}

string  Setting::getACCFolderName() {
	return m_strACCFolderName;
}

string  Setting::getACCFolderType() {
	return m_strACCFolderType;
}

void Setting::setWebServiceParam(WebServiceParam param) {
	m_webServiceParam = param;
	printf("web service param %s\n", param.ACCUrl);
}

WebServiceParam* Setting::getWebServiceParam() {
	return &m_webServiceParam;
}

tagRecordTemplate* Setting::getRecordTemplate(int width, int height) {
	if (width == 1920 && height == 1080) {
		return &m_recordTemplate[0];
	} else if (width == 1536 && height == 768) {
		return &m_recordTemplate[1];
	} else if (width == 1280 && height == 1024) {
		return &m_recordTemplate[2];
	} else if (width == 1280 && height == 720) {
		return &m_recordTemplate[3];
	} else if (width == 1024 && height == 768) {
		return &m_recordTemplate[4];
	}
	return NULL;
}

string Setting::getRecordTemplateName() {
	return m_strRecordTemplateXMl;
}

string Setting::getRecordTemplatePath() {
	return m_strRecordTemplatePath;
}

bool Setting::readTemplate() {
	try {
		//创建一个XML的文档对象。
		TiXmlDocument *myDocument = new TiXmlDocument(m_strRecordTemplateXMl.c_str());
		myDocument->LoadFile();

		TiXmlHandle docHandle(myDocument);
		TiXmlHandle pStreamMediaElement = docHandle.FirstChild("StreamMedia");

		TiXmlHandle pRecordTemplate = pStreamMediaElement.Child("RecordTemplate", 0);

		for (int i = 0; i < MAX_TEMPLATE_COUNT; ++i) {
			TiXmlHandle pTemplate0 = pRecordTemplate.Child("Template", i);

			TiXmlHandle pHeadInfo = pTemplate0.Child("HeadInfo", 0);
			readHeaderTemplate(&pHeadInfo, i);

			TiXmlHandle pTailInfo = pTemplate0.Child("TailInfo", 0);
			readTailTemplate(&pTailInfo, i);

			TiXmlHandle pLogoInfo = pTemplate0.Child("LogoInfo", 0);
			readLogoTemplate(&pLogoInfo, i);

			TiXmlHandle pCourseInfo = pTemplate0.Child("CourseInfo", 0);
			readCourseTemplate(&pCourseInfo, i);
		}

		delete myDocument;
		return true;
	} catch (string & e) {
		return false;
	}
}

void Setting::readHeaderTemplate(TiXmlHandle* pHeaderInfo, int index) {
	if (index < 0 && index > MAX_TEMPLATE_COUNT) {
		return;
	}
	strcpy(m_recordTemplate[index].HeadBackPic.cBackPic, pHeaderInfo->ToElement()->Attribute("BackPic"));
	printf("the head back pic is %s\n", m_recordTemplate[index].HeadBackPic.cBackPic);

	TiXmlElement* pCourse = pHeaderInfo->Child("Course", 0).ToElement();
	TiXmlAttribute* pCourseAttr = pCourse->FirstAttribute();

	getSceneStringInfo(pCourseAttr, &m_recordTemplate[index].HeadCourseName,
			m_recordTemplate[index].bHeadCourseNameShow);

	TiXmlElement* pViceTitle = pHeaderInfo->Child("ViceTitle", 0).ToElement();
	TiXmlAttribute* pViceTitleAttr = pViceTitle->FirstAttribute();

	getSceneStringInfo(pViceTitleAttr, &m_recordTemplate[index].HeadTitleName,
			m_recordTemplate[index].bHeadTitleShow);

	TiXmlElement* pTeacherName = pHeaderInfo->Child("TeacherName", 0).ToElement();
	TiXmlAttribute* pTeacherNameAttr = pTeacherName->FirstAttribute();

	getSceneStringInfo(pTeacherNameAttr, &m_recordTemplate[index].HeadTeacherName,
			m_recordTemplate[index].bHeadTeacherNameShow);

	TiXmlElement* pHeadTime = pHeaderInfo->Child("HeadTime", 0).ToElement();
	TiXmlAttribute* pHeadTimeAttr = pHeadTime->FirstAttribute();

	getSceneStringInfo(pHeadTimeAttr, &m_recordTemplate[index].HeadTime,
			m_recordTemplate[index].bHeadTimeShow);
}

void Setting::readTailTemplate(TiXmlHandle* pTailInfo, int index) {
	strcpy(m_recordTemplate[index].TailBackPic.cBackPic, pTailInfo->ToElement()->Attribute("BackPic"));
	strcpy(m_recordTemplate[index].TailTitle.cString, pTailInfo->ToElement()->Attribute("Text"));

    TiXmlElement* pTailText = pTailInfo->Child("TailText", 0).ToElement();
    TiXmlAttribute* pTailTextAttr = pTailText->FirstAttribute();
    getSceneStringInfo(pTailTextAttr, &m_recordTemplate[index].TailTitle, m_recordTemplate[index].bTailTitleShow);
    printf("the tail title size %d\n", m_recordTemplate[index].TailTitle.dwSize);
    printf("the tail text is %s\n", m_recordTemplate[index].TailTitle.cString);
    printf("the tail show enable %d\n", m_recordTemplate[index].bTailTitleShow);

    TiXmlElement* pTailTime = pTailInfo->Child("TailTime", 0).ToElement();
    TiXmlAttribute* pTailTimeAttr = pTailTime->FirstAttribute();
    getSceneStringInfo(pTailTimeAttr, &m_recordTemplate[index].TailTime, m_recordTemplate[index].bTailTimeShow);
}

void Setting::readLogoTemplate(TiXmlHandle* pLogoInfo, int index) {
	m_recordTemplate[index].bVideoLogoShow = false;
	TiXmlElement* pLogoElement = pLogoInfo->Child("Logo", 0).ToElement();
	TiXmlAttribute* pLogoAttr = pLogoElement->FirstAttribute();

	while (NULL != pLogoAttr) {
		if (strcmp(pLogoAttr->Name(), "File") == 0) {
			strcpy(m_recordTemplate[index].VideoLogo.cMixPic, pLogoAttr->Value());
		} else if (strcmp(pLogoAttr->Name(), "PosX") == 0) {
			m_recordTemplate[index].VideoLogo.dwPosX = atoi(pLogoAttr->Value());
		} else if (strcmp(pLogoAttr->Name(), "PosY") == 0) {
			m_recordTemplate[index].VideoLogo.dwPosY = atoi(pLogoAttr->Value());
		} else if (strcmp(pLogoAttr->Name(), "Show") == 0) {
			m_recordTemplate[index].bVideoLogoShow = (strcmp(pLogoAttr->Value(), "true") == 0)
					|| (strcmp(pLogoAttr->Value(), "TRUE") == 0);
		}
		pLogoAttr = pLogoAttr->Next();
	}
}

void Setting::readCourseTemplate(TiXmlHandle* pCourseInfo, int index) {
	TiXmlElement* pCourse = pCourseInfo->Child("Course", 0).ToElement();
	TiXmlAttribute* pCourseAttr = pCourse->FirstAttribute();

	getSceneStringInfo(pCourseAttr, &m_recordTemplate[index].VideoCourseName,
			m_recordTemplate[index].bVideoCourseNameShow);

	TiXmlElement* pTeacherName = pCourseInfo->Child("TeacherName", 0).ToElement();
	TiXmlAttribute* pTeacherNameAttr = pTeacherName->FirstAttribute();

	getSceneStringInfo(pTeacherNameAttr, &m_recordTemplate[index].VideoTeacherName,
			m_recordTemplate[index].bVideoTeacherNameShow);

	TiXmlElement* pCoursePic = pCourseInfo->Child("CoursePic", 0).ToElement();
	TiXmlAttribute* pCoursePicAttr = pCoursePic->FirstAttribute();

	while (NULL != pCoursePicAttr) {
		if (strcmp(pCoursePicAttr->Name(), "File") == 0) {
			strcpy(m_recordTemplate[index].VideoCoursePic.cMixPic, pCoursePicAttr->Value());
		} else if (strcmp(pCoursePicAttr->Name(), "Show") == 0) {
			m_recordTemplate[index].bVideoCoursePicShow = (strcmp(pCoursePicAttr->Value(), "true") == 0)
					|| (strcmp(pCoursePicAttr->Value(), "TRUE") == 0);
		} else if (strcmp(pCoursePicAttr->Name(), "PosX") == 0) {
			m_recordTemplate[index].VideoCoursePic.dwPosX = atoi(pCoursePicAttr->Value());
		} else if (strcmp(pCoursePicAttr->Name(), "PosY") == 0) {
			m_recordTemplate[index].VideoCoursePic.dwPosY = atoi(pCoursePicAttr->Value());
		}

		pCoursePicAttr = pCoursePicAttr->Next();
	}
	printf("video course pic pos y %d\n", (int)m_recordTemplate[index].VideoCoursePic.dwPosY);
}

bool Setting::getRgbValue(string strColor, int& r, int& g, int& b) {
	int nPos0 = strColor.find_first_of(",");
	if (nPos0 < 0) {
		return false;
	}
	int nPos1 = strColor.find_last_of(",");
	if (nPos1 < 0) {
		return false;
	}
	if (nPos0 >= nPos1) {
		return false;
	}
	r = atoi(strColor.substr(0, nPos0).c_str());
	g = atoi(strColor.substr(nPos0 + 1, nPos1 - nPos0 - 1).c_str());
	b = atoi(strColor.substr(nPos1 + 1, strColor.length() - nPos1 - 1).c_str());
	return true;
}

bool Setting::getSceneStringInfo(TiXmlAttribute* pAttr, SceneStringInfo* info, BOOL& bShow) {
	while (NULL != pAttr) {
		if (strcmp(pAttr->Name(), "Show") == 0) {
			bShow = (strcmp(pAttr->Value(), "true") == 0) || (strcmp(pAttr->Value(), "TRUE") == 0);
		} else if (strcmp(pAttr->Name(), "Font") == 0) {

		} else if (strcmp(pAttr->Name(), "Size") == 0) {
			info->dwSize = atoi(pAttr->Value());
		} else if (strcmp(pAttr->Name(), "Color") == 0) {
			string strCourseColor = pAttr->Value(); //color;
			int r, g, b;
			if (getRgbValue(strCourseColor, r, g, b) ) {
				info->dwColor = (r << 16) | (g << 8) | b;
			}
		} else if (strcmp(pAttr->Name(), "PosX") == 0) {
			info->dwPosX = atoi(pAttr->Value());
		} else if (strcmp(pAttr->Name(), "PosY") == 0) {
			info->dwPosY = atoi(pAttr->Value());
		}
		pAttr = pAttr->Next();
	}
	return true;
}

bool Setting::setIPCChannel(PRChannel channel) {
	if ((m_IPCChannel.dwTeacherChannel != channel.dwTeacherChannel)
		|| (m_IPCChannel.dwStudentChannel != channel.dwStudentChannel)
		|| (m_IPCChannel.dwDeviceChannel != channel.dwDeviceChannel)) {
		if (m_oldIPCChannel.dwTeacherChannel == 0 && m_oldIPCChannel.dwStudentChannel == 0 && m_oldIPCChannel.dwDeviceChannel == 0) {
			m_oldIPCChannel = channel;
		} else {
			m_oldIPCChannel = m_IPCChannel;
		}
		m_IPCChannel = channel;
		return true;
	}
	return false;
}

PRChannel* Setting::getIPCChannel() {
	return &m_IPCChannel;
}

PRChannel* Setting::getOldIPCChannel() {
	return &m_oldIPCChannel;
}

bool Setting::setOldIPCTeacherChannel(int dwTeacherChannel) {
	m_oldIPCChannel.dwTeacherChannel = dwTeacherChannel;
	bool ret = (m_oldIPCChannel.dwTeacherChannel == m_IPCChannel.dwTeacherChannel
		&& m_oldIPCChannel.dwStudentChannel == m_IPCChannel.dwStudentChannel);
	return ret;
}

bool Setting::setOldIPCStudentChannel(int dwStudentChannel) {
	m_oldIPCChannel.dwStudentChannel = dwStudentChannel;
	bool ret = (m_oldIPCChannel.dwTeacherChannel == m_IPCChannel.dwTeacherChannel
		&& m_oldIPCChannel.dwStudentChannel == m_IPCChannel.dwStudentChannel);
	return ret;
}

void Setting::setIPCInfo(IPCInfo* info, int index) {
	if (index < 0 || index >= MAX_IPC) {
		return;
	}
	memcpy(&m_IPCInfo[index], info, sizeof(IPCInfo) );
}

IPCInfo* Setting::getIPCInfo(int index) {
	if (index < 0 || index >= MAX_IPC) {
		return NULL;
	}

	if (m_IPCInfo[index].dwIPCWidth == 0 || m_IPCInfo[index].dwIPCHeight == 0 || m_IPCInfo[index].dwIPCIP == 0) {
		return NULL;
	}
	return &m_IPCInfo[index];
}

void Setting::resetIPCInfos() {
	for (int i = 0; i < MAX_IPC; ++i) {
		m_IPCInfo[i].dwSubCode = 0;
	}
}

void Setting::resetSceneInfos() {
	for (int i = 0; i < SOURCE_SCENE_COUNT; ++i) {
		m_sceneVideoInfo[i].dwLeft = 0;
		m_sceneVideoInfo[i].dwRight = 0;
		m_sceneVideoInfo[i].dwTop = 0;
		m_sceneVideoInfo[i].dwBottom = 0;
	}
}

void Setting::setSceneVideoInfo(const SceneVideoInfo* info) {
	if (info->dwVideoSource < 0 || info->dwVideoSource >= SOURCE_SCENE_COUNT) {
		return;
	}
	memcpy(&m_sceneVideoInfo[info->dwVideoSource], info, sizeof(SceneVideoInfo));
}

SceneVideoInfo*	Setting::getTeacherSceneVideoInfo() {
	return &m_sceneVideoInfo[SOURCE_TEACHER];
}

SceneVideoInfo* Setting::getStudentSceneVideoInfo() {
	return &m_sceneVideoInfo[SOURCE_STUDENT];
}

SceneVideoInfo* Setting::getDeviceSceneVideoInfo() {
	return &m_sceneVideoInfo[SOURCE_DEVICE];
}

void Setting::setSceneBackPicInfo(const SceneBackPicInfo* info) {
	memcpy(&m_sceneBackPicInfo, info, sizeof(SceneBackPicInfo));
}

SceneBackPicInfo* Setting::getSceneBackPicInfo() {
	return &m_sceneBackPicInfo;
}

void Setting::setSceneStringInfo(const SceneStringInfo* info) {
	memcpy(&m_sceneStringInfo, info, sizeof(SceneStringInfo));
}

SceneStringInfo* Setting::getSceneStringInfo() {
	return &m_sceneStringInfo;
}

void Setting::setSceneMixPicInfo(const SceneMixPicInfo* info) {
	memcpy(&m_sceneMixPicInfo, info, sizeof(SceneMixPicInfo));
}

SceneMixPicInfo* Setting::getSceneMixPicInfo() {
	return &m_sceneMixPicInfo;
}

void Setting::setSceneMediaFileInfo(const SceneMeidaFileInfo* info) {
	memcpy(&m_sceneMediaFileInfo, info, sizeof(SceneMeidaFileInfo));
}

SceneMeidaFileInfo* Setting::getSceneMediaFileInfo() {
	return &m_sceneMediaFileInfo;
}

bool Setting::getLiveEncEnable() {
	return m_bLiveEncEnable;
}

int	 Setting::getLiveEncBitrate() {
	return m_liveEncBitrate;
}

int Setting::getVGAEncBitrate() {
	if (m_vgaEncBitrate < 2048000) {
		return m_vgaEncBitrate;
	}
	return 2048000;
}

bool Setting::getAutoUploadFile() {
	return m_bAutoUploadFile;
}

void Setting::setAutoUploadFile(bool bAutoUploadFile) {
	m_bAutoUploadFile = bAutoUploadFile;
}

int Setting::getAudioDelayFrames() {
	return m_audioDelayFrames;
}

void Setting::setBPIPInfo(BPIPINFO info) {
	m_BPIPInfo = info;
}

BPIPINFO *Setting::getBPIPInfo() {
	return &m_BPIPInfo;
}

string Setting::getSharePath() {
	return m_strSharePath;
}

string Setting::getSharePathUsername() {
	return m_strSharePathUsername;
}

bool Setting::getHDMIDisplayEnable() {
	return m_bHDMIDisplayEnable;
}

bool Setting::getVGADisplayEnable() {
	return m_bVGADisplayEnable;
}

string	Setting::getHDMIDisplayDeviceName() {
	return m_strHDMIDisplayDeviceName;
}

string  Setting::getVGADisplayDeviceName() {
	return m_strVGADisplayDeviceName;
}


int Setting::getLiveOutWidth() {
	return m_liveVideoWidth;
}

int Setting::getLiveOutHeight() {
	return m_liveVideoHeight;
}

int Setting::getVGASlowFrameCount(int width, int height) {
	if (1920 == width && 1080 == height) {
		return m_videoDelayParams[0].nVGASlowFrameCount;
	} else if (1536 == width && 768 == height) {
		return m_videoDelayParams[1].nVGASlowFrameCount;
	} else if (1280 == width && 720 == height) {
		return m_videoDelayParams[2].nVGASlowFrameCount;
	} else if (1024 == width && 768 == height) {
		return m_videoDelayParams[3].nVGASlowFrameCount;
	} else if (1280 == width && 1024 == height) {
		return m_videoDelayParams[4].nVGASlowFrameCount;
	}

	return 0;
}

int Setting::getCameraRepeatFrameCount(int width, int height) {

	if (1920 == width && 1080 == height) {
		return m_videoDelayParams[0].nCameraRepeatFrame;
	} else if (1536 == width && 768 == height) {
		return m_videoDelayParams[1].nCameraRepeatFrame;
	} else if (1280 == width && 720 == height) {
		return m_videoDelayParams[2].nCameraRepeatFrame;
	} else if (1024 == width && 768 == height) {
		return m_videoDelayParams[3].nCameraRepeatFrame;
	} else if (1280 == width && 1024 == height) {
		return m_videoDelayParams[4].nCameraRepeatFrame;
	}
	return 0;
}

int	Setting::getLiveEncVOPInter() {
	return m_nLiveEncVOPInter;
}

bool Setting::getLiveEncHighProfile() {
	return m_bLiveEncHighProfile;
}

int Setting::getDiskSpaceAlertSize() {
	return m_nDiskSpaceAlertSize;
}

int Setting::getDeleteFrameCount() {
	return m_nDeleteFrameCount;
}

int Setting::getLiveDeleteFrameCount() {
	return m_nLiveDeleteFrameCount;
}

bool Setting::getAutoDeleteFiles() {
	return m_bAutoDeleteFiles;
}

int	Setting::getDaysBeforeCanDelete() {
	return m_nDaysBeforeCanDelete;
}

string Setting::getDestPath() {
	return m_strDestPath;
}

bool Setting::getEncodeQualityEnable() {
	return m_bEncodeQulityEnable;
}

bool Setting::getNotifyVGAChangedEnable() {
	return m_bNotifyVGAChangeEnable;
}

bool Setting::getDualScreenCrop() {
	return m_bDualScreenCrop;
}

bool Setting::getVGASwitchOldPanel() {
	return m_bVGASwitchOldPanel;
}



