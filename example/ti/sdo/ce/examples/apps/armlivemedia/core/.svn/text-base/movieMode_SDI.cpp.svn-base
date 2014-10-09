#include "movieMode_SDI.h"

extern "C" Ptr DomxCore_mapUsrVirtualAddr2phy (UInt32 pUsrVirtAddr);

MovieMode_SDI::MovieMode_SDI(Kernel* pKernel)
: MixtureMode_SDI(pKernel) {

}

MovieMode_SDI::~MovieMode_SDI() {

}

bool MovieMode_SDI::calcScalerWidthHeight() {
	bool bRet = false;
	int nOutScaleWidth0 = m_outScaleWidth[0];
	int nOutScaleHeight0 = m_outScaleHeight[0];
	int nOutScaleWidth1 = m_outScaleWidth[1];
	int nOutScaleHeight1 = m_outScaleHeight[1];
	int nOutVGAWidth = m_outVGAWidth;
	int nOutVGAHeight = m_outVGAHeight;
	memcpy(&m_teacherSceneInfo, m_pSetting->getTeacherSceneVideoInfo(), sizeof(SceneVideoInfo));
	memcpy(&m_studentSceneInfo, m_pSetting->getStudentSceneVideoInfo(), sizeof(SceneVideoInfo));
	memcpy(&m_deviceSceneInfo, m_pSetting->getDeviceSceneVideoInfo(), sizeof(SceneVideoInfo));


	//m_outScaleWidth[0] = m_pSetting->getBaseRecordParams()->dwWidth;
	//m_outScaleHeight[0] = m_pSetting->getBaseRecordParams()->dwHeight;

	//m_outScaleWidth[1] = m_outScaleWidth[0];
	//m_outScaleHeight[1] = m_outScaleHeight[0];

	//m_outVGAWidth = m_outScaleWidth[0];
	//m_outVGAHeight = m_outScaleHeight[0];

	if (m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft == HD_WIDTH
			&& m_teacherSceneInfo.dwBottom - m_teacherSceneInfo.dwTop == HD_HEIGHT) {
		m_bDirectCopyChannel1 = true;
	}

	if (m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft == HD_WIDTH
			&& m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop == HD_HEIGHT) {
		m_bDirectCopyChannel2 = true;
	}

	if (m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft != 0) {
		m_outScaleWidth[0] = m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft;

		if (m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft != 0) {
			m_outScaleWidth[1] = m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft;
		} else {
			m_outScaleWidth[1] = m_outScaleWidth[0];
		}

		if (m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft != 0) {
			m_outVGAWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
		} else {
			m_outVGAWidth = m_outScaleWidth[0];
		}
	} else {
		if (m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft != 0) {
			m_outScaleWidth[1] = m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft;
			m_outScaleWidth[0] = m_outScaleWidth[1];

			if (m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft != 0) {
				m_outVGAWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
			} else {
				m_outVGAWidth = m_outScaleWidth[0];
			}
		} else {
			if (m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft != 0) {
				m_outVGAWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
				m_outScaleWidth[0] = m_outVGAWidth;
				m_outScaleWidth[1] = m_outVGAWidth;
			} else {
				m_outVGAWidth = m_pSetting->getBaseRecordParams()->dwWidth;
				m_outScaleWidth[0] = m_outVGAWidth;
				m_outScaleWidth[1] = m_outVGAWidth;
			}
		}
	}

	if (m_teacherSceneInfo.dwBottom - m_teacherSceneInfo.dwTop != 0) {
		m_outScaleHeight[0] = m_teacherSceneInfo.dwBottom - m_teacherSceneInfo.dwTop;

		if (m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop != 0) {
			m_outScaleHeight[1] = m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop;
		} else {
			m_outScaleHeight[1] = m_outScaleHeight[0];
		}

		if (m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop != 0) {
			m_outVGAHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;
		} else {
			m_outVGAHeight = m_outScaleHeight[0];
		}
	} else {
		if (m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop != 0) {
			m_outScaleHeight[1] = m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop;
			m_outScaleHeight[0] = m_outScaleHeight[1];

			if (m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop != 0) {
				m_outVGAHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;
			} else {
				m_outVGAHeight = m_outScaleHeight[0];
			}
		} else {
			if (m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop != 0) {
				m_outVGAHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;
				m_outScaleHeight[0] = m_outVGAHeight;
				m_outScaleHeight[1] = m_outVGAHeight;
			} else {
				m_outVGAHeight = m_pSetting->getBaseRecordParams()->dwHeight;
				m_outScaleHeight[0] = m_outVGAHeight;
				m_outScaleHeight[1] = m_outVGAHeight;
			}
		}
	}

	m_bTeacherContainStud = false;
	m_bTeacherContainVGA = false;

	m_bStudContainTeacher = false;
	m_bStudContainVGA = false;

	m_bVGAContainTeacher = false;
	m_bVGAContainStud = false;

	bRet = 	(nOutScaleWidth0 != m_outScaleWidth[0])
	   || (nOutScaleHeight0 != m_outScaleHeight[0])
	   || (nOutScaleWidth1 != m_outScaleWidth[1])
	   || (nOutScaleHeight1 != m_outScaleHeight[1])
	   || (nOutVGAWidth != m_outVGAWidth)
	   || (nOutVGAHeight != m_outVGAHeight);

	return bRet;
}



