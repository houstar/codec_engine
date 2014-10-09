#include "codeConverter.h"
#include <string.h>

CodeConverter::CodeConverter(const char *from_charset,const char *to_charset) {
	m_cd = iconv_open(to_charset,from_charset);
}


CodeConverter::~CodeConverter() {
	iconv_close(m_cd);
}


int CodeConverter::convert(char *inbuf,int inlen,char *outbuf,int& outlen) {
	char **pin = &inbuf;
	char **pout = &outbuf;

	memset(outbuf,0,outlen);
	return iconv(m_cd, pin, (size_t *)&inlen, pout, (size_t *)&outlen);
}
