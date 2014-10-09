#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define TI81XX_EDMA_MOVE  (('M'<<8)|0xa)	//ioctl命令
#define MAX_DMA_TRANSFER_IN_BYTES   (32768)


struct edma_arg{
	void * dmaphyssrc1 ; //源地址,注意是物理地址,比如0x89500000
	void * dmaphysdest1 ; //目的地址
	 int acnt ;
	 int bcnt ;	//acnt*bcnt 不能超过32768,即MAX_DMA_TRANSFER_IN_BYTES
	 int ccnt ;
	int mode; //0和1两种模式
	int srcbidx;
	int desbidx;
	int srccidx;
	int descidx;
};

class EdmaUtils {
public:
	EdmaUtils();
	~EdmaUtils();

public:
	bool init();

	bool uninit();

	bool copy(void* dest, void* src, int srcWidth, int dstWidth, int size, int mode);

	bool copy(void* dest, void* src, int srcWidth, int srcIndex, int dstWidth, int dstIndex, int size, int mode);
private:
	int 	 m_edmafd;
};
