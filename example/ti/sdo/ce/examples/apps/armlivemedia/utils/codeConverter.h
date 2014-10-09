#pragma once

#include <iconv.h>
#include <iostream>

#define OUTLEN 255

using namespace std;

// 代码转换操作类
class CodeConverter {
public:
	// 构造
	CodeConverter(const char *from_charset,const char *to_charset);

	// 析构
	~CodeConverter();

	// 转换输出
	int convert(char *inbuf,int inlen,char *outbuf,int& outlen);

private:
	iconv_t m_cd;
};
