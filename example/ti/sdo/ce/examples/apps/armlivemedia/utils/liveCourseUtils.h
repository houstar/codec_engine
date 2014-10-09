#pragma once
#include <string>
#include "../liveCourse/livemtgConstDefine.h"

using namespace std;


class LiveCourseUtils {
public:
	static int generate_uid(const string& roomID, int role);

	static int generate_course_id(const string& roomID);

	static bool parsePathToCmd(const char* pPath, ELiveCourseCmd &nCmd, string& strRoomID, string& strUid);

	static bool parseClassRoomId(const char* pInfo, string& strRoomID);

private:
	static const int KClassKey1 = 5008;
	static const int KClassRoomLength = 14;
};
