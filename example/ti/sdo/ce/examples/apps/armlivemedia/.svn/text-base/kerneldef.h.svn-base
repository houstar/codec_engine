#pragma once

#include <iostream>
#include <stdint.h>
#include <list>
#include <pthread.h>
#include <semaphore.h>
#include <algorithm>

#define SYNC_HEAD               0x3230424C      //UDP通信，同步头
#define UDP_MAX_LEN             2048            //UDP通信最大长度
#define CMD_AD_SENDMSG          0x00010000      //一体机发出的命令
#define CMD_AD_RECVMSG          0x00010001      //发给一体机的命令
#define FUSION_ID               0x22            //协议类型
#define MEDIA_ID                0x23            //Media的协议类型
#define UNIT_ID                 0x14            //一体机的协议类型
#define ICII_ID                 0x15            //集控制的协议类型


typedef struct _s_udp_header
{
    unsigned int syncHead;      //同步头
    unsigned int fromIP;        //源IP
    unsigned int toIP;          //目的IP
    unsigned char frameIndex;   //包序号
    unsigned char protocolType; //协议类型
    unsigned short dataLen;     //数据长度(去头的长度)
}s_udp_header;

typedef struct _s_udp_cmd
{
    unsigned int cmd;           //命令字
    unsigned int flag;          //命令标识
    char userID[10];            //user id
    char guid[10];              //
}s_udp_cmd;

/**
  *@breif 在arm上 sizeof(s_udp_ad) = 4
  *       在x86上 sizeof(s_udp_ad) = 2
  */
typedef struct _s_udp_ad
{
    unsigned char deviceID;     //设备码
    unsigned char actionID;     //动作码
}__attribute__((packed)) s_udp_ad;






