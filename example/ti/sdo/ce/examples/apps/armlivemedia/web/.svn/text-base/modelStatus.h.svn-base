#pragma once

#include "../core/rtmpHandle.h"
#include "../core/mtgClient.h"
#include "../core/mtgClient.h"

#include <semaphore.h>

class Kernel;

//teacher1, use channel 4, teacher2, use channel 3
//student 1 ,use channel 1, student 2, use channel 2S


#define DEVICE_ERROR_NO_SPACE -1


class ModelStatus : public IRtmpListener, public IMtgListener {
public:
	ModelStatus();

	virtual ~ModelStatus();

public:
	virtual void onRecvMtgData(MtgClient* pMtg, const char* path,
			const char*data, int dataLen, int datatype, int action);

	virtual void onJoinCallback(MtgClient* pMtg, bool bJoinOK);

	virtual bool notifyRtmpConnect(RtmpHandle* pRtmpHandle, bool bConnect);

public:
	bool init(Kernel* pKernel);

	bool uninit();

	bool queryStatus();



	bool writeStatusXML();


private:
	//when it is at recording status, then get the status from mixture mode
	bool queryCameraStatus();

	bool queryVideoCenterStatus();

	//when it is at recording status, then get the status from mixture mode
	bool queryRTMPStatus();

	bool queryMtgStatus();

	bool queryLiveCourseStatus();

	bool queryDetailStatus();

private:

	Kernel* m_pKernel;

	bool	m_bCameraTeacher1Valid;
	bool	m_bCameraTeacher1Connect;

	bool 	m_bCameraTeacher2Valid;
	bool	m_bCameraTeacher2Connect;

	bool	m_bCameraStudent1Valid;
	bool	m_bCameraStudent1Connect;

	bool	m_bCameraStudent2Valid;
	bool	m_bCameraStudent2Connect;

	bool	m_bBackCamera1Valid;
	bool	m_bBackCamera1Connect;

	bool	m_bBackCamera2Valid;
	bool	m_bBackCamera2Connect;

	bool	m_bVideoCenterValid;
	bool	m_bVideoCenterConnect;

	bool	m_bRTMPValid;
	bool	m_bRTMPConnect;

	bool	m_bMtgValid;
	bool	m_bMtgConnect;

	bool	m_bLiveCourseValid;
	bool	m_bLiveCourseConnect;

	bool	m_bDetailValid;
	int		m_detailConnectInfo;

	sem_t 	m_RtmpSem;
	sem_t	m_MtgSem;

	RtmpHandle* m_rtmpHandle;
	MtgClient* m_pMtgClient;

};
