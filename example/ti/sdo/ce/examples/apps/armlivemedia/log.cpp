#include "log.h"

#include "kerneldef.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stddef.h>

Log::Log() :
		_fun(NULL) {
}

Log::~Log() {
}

bool Log::addListener(ILogListener *listener) {
	if (listener == NULL) {
		//系统中，不允许存在该现象
		return false;
	}

	if (std::find(this->_listeners.begin(), this->_listeners.end(), listener)
			!= this->_listeners.end()) {
		return false;
	}

	this->_listeners.push_back(listener);

	return true;
}

bool Log::delListener(ILogListener *listener) {
	ILogListener *tmp = listener;

	if (std::find(this->_listeners.begin(), this->_listeners.end(), tmp)
			== this->_listeners.end()) {
		return false;
	}

	this->_listeners.remove(listener);

	return true;
}

void Log::error(const char *str) {
	this->notifyListener(&ILogListener::onError, str);
}

void Log::warning(const char *str) {
	this->notifyListener(&ILogListener::onError, str);
}

void Log::message(const char *str) {
	this->notifyListener(&ILogListener::onMessage, str);
}

Log &Log::operator <<(void (Log::*fun)(const char *str)) {
	this->_fun = fun;
	return *this;
}

Log &Log::operator <<(const char *str) {
	if (!_fun) {
		(this->*_fun)(str);
	}

	return *this;
}

Log &Log::operator <<(int v) {
	char buf[10] = { 0 };

	sprintf(buf, "%d", v);

	return this->operator <<(buf);
}

Log &Log::operator <<(int *p) {
	return this->operator <<((void *) p);
}

Log &Log::operator <<(const std::string &str) {
	return this->operator <<(str.c_str());
}

Log &Log::operator <<(void *p) {
	char buf[10] = { 0 };

	sprintf(buf, "%p", p);

	return this->operator <<(buf);
}

std::string Log::bin2Str(const char *binary, int len, char space) {
	char buf[1024 * 3] = { 0 }; //最多支持1024个字符
	char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
			'C', 'D', 'E', 'F' };

	for (int i = 0; i < len && (size_t) i < sizeof(buf) / 3; i++) {
		buf[i * 3] = hex[binary[i] >> 4];
		buf[i * 3 + 1] = hex[binary[i] & 0x0F];
		buf[i * 3 + 2] = space;
	}

	return buf;
}

std::string Log::timestamp() {
	time_t timep;
	time(&timep);

	struct tm *p = localtime(&timep); //gmtime(&timep);

	char buf[50] = { 0 };

	sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d ", p->tm_year + 1900,
			p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

	return buf;
}

void Log::notifyListener(void (ILogListener::*fun)(Log *log, const char *),
		const char *str) {
	std::cout << str;

	for (std::list<ILogListener *>::iterator it = this->_listeners.begin();
			it != this->_listeners.end(); it++) {
		ILogListener *t = *it;

		(t->*fun)(this, str);
	}
}

Log glog;
