#include "fifo.h"
#include <string.h>
#include "macrodef.h"


fifo::fifo() :
	_member_count(0),
	_type(0),
	_init(0),
	_buffer_falg(NULL),
	_start_pop(false),
	_enable_size(-1),
	_disable_count(0)
{
}

fifo::~fifo()
{
	uninit();
}

int fifo::init(int fifo_count)
{
	if (fifo_count <= 0)
		return kFalse;
	
	pthread_mutexattr_t attr;
	int re = pthread_mutexattr_init(&attr);
	if (re != 0)
		return kFalse;
	re = pthread_mutex_init(&_mutex, &attr);
	if (re != 0)
		return kFalse;
	
	re = pthread_mutex_init(&_lock, &attr);
	if (re != 0)
		return kFalse;
	
	_member_count = fifo_count;
	
	if (_buffer_falg == NULL) {
		_buffer_falg = new stbufferflag[_member_count];
	}
	
	for (int i = 0; i < _member_count; ++ i) {
		_buffer_falg[i].use_falg = 0;
		_buffer_falg[i].recv_buf = NULL;
	}
	
	_list.clear();
	_init = 1;
	
	return kTrue;
}

void fifo::uninit()
{
	if (_init == 0)
		return;
	_init = 0;
	_list.clear();
	for (int i = 0; i < _member_count; ++ i) {
		if (NULL != _buffer_falg[i].recv_buf) {
			delete[] _buffer_falg[i].recv_buf;
			_buffer_falg[i].recv_buf = NULL;
		}
	}
	
	if (NULL != _buffer_falg) {
		delete[] _buffer_falg;
		_buffer_falg = NULL;
	}
	
	pthread_mutex_destroy(&_mutex);
	pthread_mutex_destroy(&_lock);
}

void fifo::reset()
{
	pthread_mutex_lock(&_mutex);
	for (int i = 0; i < _member_count; ++ i) {
		if (NULL != _buffer_falg[i].recv_buf) {
			delete[] _buffer_falg[i].recv_buf;
			_buffer_falg[i].recv_buf = NULL;
		}
		_buffer_falg[i].use_falg = 0;
	}
	_list.clear();
	pthread_mutex_unlock(&_mutex);

	_disable_count = 0;
}

int fifo::push(char* data, int length, int frame_count, bool bCheckDisableCount)
{
	if (NULL == data || !_init) {
		return kFalse;
	}

	if (!_start_pop) {
		return kFalse;
	}

	if (bCheckDisableCount) {
		if (!checkdisablecount())
			return kFalse;
	}

	
	pthread_mutex_lock(&_mutex);	// lock
	int index = getvalidindex();

	if (index < 0 || index >= _member_count) {
		pthread_mutex_unlock(&_mutex); // unlock
		return kFalse;
	}
	
	int buf_len = length + 16;
	char *buf = new char[buf_len];
	if (NULL == buf) {
		pthread_mutex_unlock(&_mutex); // unlock
		return kFalse;
	}
	memcpy(buf, data, length);
	_buffer_falg[index].recv_buf = buf;
	
	_list.push_back(index);
	
	_buffer_falg[index].use_falg    = 1;
	_buffer_falg[index].length      = length;
	_buffer_falg[index].frame_count = frame_count;
	
	pthread_mutex_unlock(&_mutex); // unlock
	return kTrue;
}

int fifo::pop(char* data, int* length, int* frame_count)
{
	static bool flag = false;
	if (NULL == data || !_init) {
		printf("pop error 0\n");
		return kFalse;
	}
	
	if (!_start_pop) {
		_start_pop = true;
		return kFalse;
	}

	if (!flag) {
		if (_enable_size > 0) {
			if (_enable_size > _list.size())
				return kFalse;
			else
				flag = true;
		} else {
			flag = true;
		}
	} 
	
	//printf("fifo size: %d\n", _list.size());	
	
	pthread_mutex_lock(&_mutex);	// lock
 
	if (_list.empty()) {
		pthread_mutex_unlock(&_mutex); // unlock
		return kFalse;
	}
	
	int index = _list.front();
	if (index < 0 || index >= _member_count) {
		printf("pop error 2\n");	
		pthread_mutex_unlock(&_mutex); // unlock
		return kFalse;
	}
	
	_list.pop_front();
	if (_buffer_falg[index].recv_buf == NULL ||
		_buffer_falg[index].length <= 0) {
		printf("pop error 3\n");
		pthread_mutex_unlock(&_mutex); // unlock
		return kFalse;
	}
	
	memcpy(data, _buffer_falg[index].recv_buf,
		   _buffer_falg[index].length);
	delete[] _buffer_falg[index].recv_buf;
	_buffer_falg[index].recv_buf = NULL;
	_buffer_falg[index].use_falg = 0;
	
	if (NULL != length)
		*length = _buffer_falg[index].length;
	if (NULL != frame_count)
		*frame_count = _buffer_falg[index].frame_count;
	
	pthread_mutex_unlock(&_mutex); // unlock
	
	return kTrue;
}

int fifo::getcurlength()
{
	return _list.size();
}

void fifo::enablesize(int size)
{
	_enable_size = size;
}

bool fifo::checkdisablecount()
{
	pthread_mutex_lock(&_mutex);
	if (_disable_count > 0 && _list.size() > 5) {
		printf("fifo::checkdisablecount() delete one packet.\n");
		-- _disable_count;
		pthread_mutex_unlock(&_mutex);
		return false;
	}
	pthread_mutex_unlock(&_mutex);
	return true;
}

void fifo::disablepacket(int packet)
{
	pthread_mutex_lock(&_mutex);
	++ _disable_count;
	pthread_mutex_unlock(&_mutex);
}

int fifo::getvalidindex()
{
	int ret = -1;
	for (int i = 0; i < _member_count; ++ i) {
		if (_buffer_falg[i].use_falg == 0) {
			ret = i;
			break;
		}
	}
	return ret;
}
