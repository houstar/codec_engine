/*
  @author: wuminghao
  @data:   12-2, 2013
  @brief:  define struct  
 */
#ifndef STRUCT_DEF_H
#define STRUCT_DEF_H
#include <string>
#include <stdint.h>
#include <sys/types.h>
#include "macrodef.h"

struct stvideoinfor
{
	int id;
	std::string name;
	int width;
	int height;
};

struct stbufferflag{
	int   frame_count;
	int   length;
	int   use_falg;
	char* recv_buf;
};

struct stuserid				
{
	char uid[4];	
	char name[64];
	int  role;      // 1: watch    2: live
	int  state;	  // 1: receive  2: send & receive
};
	
const int kMaxBufferSize = 3110400; // 1920*1080*3/2
struct stsendinfor
{
	int  head;
	int  type;
	int  index;
	int  length;
	char buffer[kMaxBufferSize];    // it 's output buffer.
};


#endif  // STRUCT_DEF_H
