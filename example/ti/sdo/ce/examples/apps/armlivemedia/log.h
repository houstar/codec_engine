#pragma once

#include "kerneldef.h"

class Log;

class ILogListener {
public:
	virtual ~ILogListener(){

	}

	virtual void onError(Log *log, const char *str){

	}

	virtual void onWarning(Log *log, const char *str){

	}

	virtual void onMessage(Log *log, const char *str){

	}
};

class Log {
public:
	Log();

	virtual ~Log();

public:
	bool addListener(ILogListener *listener);

	bool delListener(ILogListener *listener);

public:
	/**
	 *@brief 写字符串
	 */
	void error(const char *str);

	void warning(const char *str);

	void message(const char *str);

	Log &operator <<(void (Log::*fun)(const char *str));

	Log &operator <<(const char *str);

	Log &operator <<(int v);

	Log &operator <<(int *p);

	Log &operator <<(void *p);

	Log &operator <<(const std::string &str);

	/**
	 *@brief 将二进制转换成字符串，最多支持1024字节
	 */
	std::string bin2Str(const char *binary, int len, char space = ' ');

	std::string timestamp();

private:
	void notifyListener(void (ILogListener::*fun)(Log *log, const char *),
			const char *str);

private:
	std::list<ILogListener *> _listeners;
	void (Log::*_fun)(const char *str);
};

//定义全局的，可以所有对象使用
extern Log glog;

#define LOG_ERROR(msg)          glog << &Log::error << glog.timestamp() << __PRETTY_FUNCTION__ << " " << msg << "\n";
#define LOG_WARNING(msg)        glog << &Log::warning << glog.timestamp() << __PRETTY_FUNCTION__ << " " << msg << "\n";
#define LOG_MSG(msg)            glog << &Log::message << glog.timestamp() << __PRETTY_FUNCTION__ << " " << msg << "\n";
//只会在debug版本中出现的日志
#define LOG_DEBUG(msg)          std::cout << "DEBUG  :" << __PRETTY_FUNCTION__ << " " << msg << "\n";
