#include "commonUtils.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/sysinfo.h>

using namespace std;

int CommonUtils::calc_result_time(timeval *result,  timeval *after,
		 timeval *before) {
	/* Perform the carry for the later subtraction by updating "before" */
	if (after->tv_usec < before->tv_usec) {
		int nsec = (before->tv_usec - after->tv_usec) / 1000000 + 1;
		before->tv_usec -= 1000000 * nsec;
		before->tv_sec += nsec;
	}
	if (after->tv_usec - before->tv_usec > 1000000) {
		int nsec = (after->tv_usec - before->tv_usec) / 1000000;

		before->tv_usec += 1000000 * nsec;
		before->tv_sec -= nsec;
	}
	/* Compute the time remaining to wait, tv_usec is certainly positive.
	 * */
	result->tv_sec = after->tv_sec - before->tv_sec;
	result->tv_usec = after->tv_usec - before->tv_usec;
	/* Return 1 if result is negative. */
	return after->tv_sec < before->tv_sec;
}


unsigned long CommonUtils::GetTickCount()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec/1000000);
}

int CommonUtils::omx_printf(const char *fmt, ...)
{
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	static char buf[1024];
	va_list ap;
	int rc;

	va_start(ap, fmt);
	pthread_mutex_lock(&mutex);
	rc = snprintf(buf, sizeof(buf), fmt, ap);
	write(2, buf, rc);
	pthread_mutex_unlock(&mutex);
	va_end(ap);

	return rc;
}

int	CommonUtils::getDiskFreeSpace(const char* pPath) {
	sync();
	struct statvfs buf;
	int stat = statvfs(pPath, &buf); // path: /media/sda1
	if (stat != 0) {
		printf("statvfs error\n");
		return -1;
	}
	//printf("file system block size\t\t: %ld\n", buf.f_bsize);
	//printf("fragment size\t\t\t: %ld\n", buf.f_frsize);
	//printf("size of fs in f_frsize units\t: %ld\n", buf.f_blocks);
	//printf("free blocks\t\t\t: %ld\n", buf.f_bfree);
	//printf("free blocks for non-root\t: %ld\n", buf.f_bavail);
	//printf("inodes\t\t\t\t: %ld\n", buf.f_files);
	//printf("free inodes\t\t\t: %ld\n", buf.f_ffree);
	//printf("free inodes for non-root\t: %ld\n", buf.f_favail);
	//printf("file system ID\t\t\t: %ld\n", buf.f_fsid);
	//printf("mount flags\t\t\t: %ld\n", buf.f_flag);
	//printf("maximum filename length\t\t: %ld\n", buf.f_namemax);

	//printf("\n/media/sda1\t\tused: %4.1f%\n", 100 - 1.0 * buf.f_bfree / buf.f_blocks * 100);
	//printf("\t\t\tfree: %.1f G\n", 1.0 * buf.f_bfree * buf.f_bsize / 1024 / 1024 / 1024);
	//return 1.0 * buf.f_bfree * buf.f_bsize / 1024 / 1024 / 1024;

	return buf.f_bavail * buf.f_bsize /1024.0/1024.0/1024.0;
}

int CommonUtils::delete_dir(const char* path, time_t time)
{
    DIR *d; //声明一个句柄
    struct dirent *file; //readdir函数的返回值就存放在这个结构体中
    struct stat sb;
    char file_path[256];
    if(!(d = opendir(path)))
    {
           printf("error opendir %s!!!\n",path);
           return -1;
    }
    while((file = readdir(d)) != NULL)
    {
                //把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录
         if (strncmp(file->d_name, ".", 1) == 0)
               continue;
         //判断该文件是否是目为普通文件
         get_file_path(path, file->d_name, file_path);
         //printf("the file path is %s\n", file_path);
         if(stat(file_path, &sb) >= 0  && sb.st_mtime <= time) {
        	 string strFileName(file_path);
        	 printf("get file name is older %s\n", file_path);
        	 if (strFileName.find(".mp4") > 0 || strFileName.find(".h264") > 0
        			 || strFileName.find(".H264") > 0 || strFileName.find(".aac") > 0
				|| strFileName.find(".log") > 0) {
        		 printf("start delete file %s\n", file_path);
        		 //remove(file_path);
        		 char commandbuf[255] = {0};
        		 sprintf(commandbuf, "rm -rf %s", file_path);
        		 system(commandbuf);
        		 printf("end delete file %s\n", file_path);
        	 }
         }
    }
    closedir(d);
    return 0;
}

void CommonUtils::get_file_path(const char *path, const char *file_name,  char *file_path)
{
    strcpy(file_path, path);
    if(file_path[strlen(path) - 1] != '/')
        strcat(file_path, "/");
    strcat(file_path, file_name);
}


void CommonUtils::changeUtcToBeijing(tm* &t) {
	t->tm_hour = t->tm_hour + 8;
	if (t->tm_hour < 24) {
		return;
	}

	t->tm_hour -= 24;
	t->tm_mday = t->tm_mday + 1;

	char days[12]={31,28,31,30,31,30,31,31,30,31,30,31};

	if ((t->tm_year % 400 == 0)||((t->tm_year % 100 != 0) && (t->tm_year % 4 == 0))) {
		days[2]=29;
	}
	if (t->tm_mday > days[t->tm_mon]) {
		t->tm_mday = t->tm_mday - days[t->tm_mon];
		t->tm_mon += 1;
	}

	if (t->tm_mon > 11) {
		t->tm_mon -= 11;
		t->tm_year += 1;
	}
}

void CommonUtils::createInFileIfMissing(char *inFileName) {
    int i;
    FILE *f = fopen(inFileName, "rb");
    if (f == NULL) {
        printf("Input file '%s' not found, generating one.\n", inFileName);
        f = fopen(inFileName, "wb");
        for (i = 0; i < 1024; i++) {
            fwrite(&i, sizeof(i), 1, f);
        }
    }
    fclose(f);
}

off_t CommonUtils::getFileSize(const char* pFileName) {
	struct stat statbuff;
	if (stat(pFileName, &statbuff) < 0){
		printf("get file size failed\n");
	    return 0;
	} else {
	    return statbuff.st_size;
	}
}

int CommonUtils::str2int(const string& str) {
	int ret = 0;
	int step = 1;

	for (std::string::const_reverse_iterator it = str.rbegin(); it != str.rend(); it++)
	{
		if ((*it)>='0' && (*it)<='9')
		{
			ret += ((*it)-'0')*step;
			step *= 10;
		}
	}
	return ret;
}


bool CommonUtils::isIPValid(const char* str) {
	if (NULL == str) {
		return false;
	}
	char* p = (char*) str;
	char *tokenPtr = strtok(p, ".");
	string str1;
	string str2;
	string str3;
	string str4;
	int nStep = 0;
	while (NULL != tokenPtr)
	{
		if (nStep == 0) {
			str1 = tokenPtr;
		} else if (nStep == 1) {
			str2 = tokenPtr;
		} else if (nStep == 2) {
			str3 = tokenPtr;
		} else if (nStep == 3) {
			str4 = tokenPtr;
		}
		tokenPtr = strtok(NULL, ".");
		nStep++;
	}
	if (str1.length() == 0 || str2.length() == 0 || str3.length() == 0 || str4.length() == 0) {
		return false;
	}

	int ip1 = atoi(str1.c_str());

	if (ip1 < 0 || ip1 > 255) {
		return false;
	}
	int ip2 = atoi(str2.c_str());
	if (ip2 < 0 || ip2 > 255) {
		return false;
	}
	int ip3 = atoi(str3.c_str());
	if (ip3 < 0 || ip3 > 255) {
		return false;
	}

	return true;
}


bool CommonUtils::getMacInfo(string& strMacInfo, const char* pEthName) {
	if (NULL == pEthName) {
		return false;
	}
    struct	ifreq   ifreq;
    int 	sock;
	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	     return false;
	}

	strcpy(ifreq.ifr_name, pEthName);

	if ( ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0 ) {
		return false;
	}
	char buf[64] = {0};
	sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", (unsigned   char)ifreq.ifr_hwaddr.sa_data[0],
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[1],
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[2],
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[3],
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[4],
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[5]);
	strMacInfo = buf;
	return true;
}

typedef struct // PACKED
{
char name[20];
unsigned int user;
unsigned int nice;
unsigned int system;
unsigned int idle;
}CPU_OCCUPY;

int CommonUtils::getCpuLoad() {
    FILE *fd;
    char buff[256];
    CPU_OCCUPY cpu_occupy;

    fd = fopen ("/proc/stat", "r");
    if (NULL == fd) {
    	printf("can not open proc stat file\n");
    	return 0;
    }
    fgets (buff, sizeof(buff), fd);

    sscanf (buff, "%s %u %u %u %u", &cpu_occupy.name, &cpu_occupy.user, &cpu_occupy.nice,&cpu_occupy.system, &cpu_occupy.idle);
    fclose(fd);
    return 100 * (cpu_occupy.user + cpu_occupy.nice + cpu_occupy.system ) / (cpu_occupy.user + cpu_occupy.nice + cpu_occupy.system + cpu_occupy.idle);
}

int CommonUtils::getMemLoad() {
	struct sysinfo info ;
	sysinfo(&info) ;
	return 100.0 * (info.totalram - info.freeram) / info.totalram;
}
