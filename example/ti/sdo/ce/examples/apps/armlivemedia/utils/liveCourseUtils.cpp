#include "liveCourseUtils.h"
#include <string.h>
#include "commonUtils.h"
#include <stdlib.h>

int LiveCourseUtils::generate_uid(const string& roomID, int role) {
	int courseID = CommonUtils::str2int(roomID);
	return courseID * 10 + role;
}


int LiveCourseUtils::generate_course_id(const string& roomID) {
	int courseID = CommonUtils::str2int(roomID);
	return courseID+10000000;				//保证比较大
}


bool LiveCourseUtils::parsePathToCmd(const char* pPath, ELiveCourseCmd &nCmd, string& strRoomID, string& strUid) {
	if (NULL == pPath) {
		return false;
	}

	if (strncmp(pPath, LIVEMTG_PATH_CMD, KLiveMtgPathCmdLength) != 0) {
		return false;
	}
	//
	if (strncmp(pPath, LIVEMTG_PATH_CMD_ENTER, KLiveMtgPathCmdEnterLength) == 0) {
		nCmd = eLiveCourseCmdEnter;
		/*char* p = pPath + strlen() + 1;
		char* p = strstr(pPath, LIVEMTG_PATH_CMD);
		if (NULL == p) {
			return false;
		}

		p = p + 1;
		char* p1 = strstr(p, "/");
		if (NULL == p1) {
			return fasle;
		}*/
		return true;
	} else if (strncmp(pPath, LIVEMTG_PATH_CMD_LEAVE, KLiveMtgPathCmdLeaveLength) == 0) {
		nCmd = eLiveCourseCmdLeave;
		return true;
	} else {
		nCmd = eLiveCourseCmdNULL;
		return true;
	}

}


bool LiveCourseUtils::parseClassRoomId(const char* pInfo, string& strRoomID) {
	char *tokenPtr = strtok((char*)pInfo, "-");
	string str1;
	string str2;
	string str3;
	int nStep = 0;
	while (NULL != tokenPtr)
	{
		printf("class %s\n", tokenPtr);
		if (nStep == 0) {
			str1 = tokenPtr;
		} else if (nStep == 1) {
			str2 = tokenPtr;
		} else if (nStep == 2) {
			str3 = tokenPtr;
		}
		tokenPtr = strtok(NULL, "-");
		nStep++;
	}
	printf("nstep %d\n", nStep);
	if (nStep != 3) {
		return false;
	}

	int nKey1 = atoi(str1.c_str());

	if (nKey1 != KClassKey1) {
		return false;
	}
	strRoomID = str2;
	int nRoomId = atoi(strRoomID.c_str());

	int nCheckValue = atoi(str3.c_str());
	return ((KClassKey1 + nRoomId) * (KClassKey1 + nRoomId) % 10000) == nCheckValue;
}
