#pragma once

#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <string>
using namespace std;

class CommonUtils {
public :
	static int calc_result_time(struct timeval *result, struct timeval *after,
			 struct timeval *before);

	static unsigned long GetTickCount();

	static int omx_printf(const char *fmt, ...);


	static int	getDiskFreeSpace(const char* pPath);

	//遍历磁盘，如果时间早于time,则把文件删除
	static int delete_dir(const char* path, time_t time);

	static void changeUtcToBeijing(tm* &t);

	static void createInFileIfMissing(char *inFileName);

	static off_t getFileSize(const char* pFileName);

	static int str2int(const string& str);

	static bool isIPValid(const char* str);

	static bool getMacInfo(string& strMacInfo, const char* pEthName);

	static int getCpuLoad();

	static int getMemLoad();

private:
	static void get_file_path(const char *path, const char *file_name,  char *file_path);
};
