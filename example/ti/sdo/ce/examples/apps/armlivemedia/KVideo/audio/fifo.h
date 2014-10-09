#ifndef FIFO_H
#define FIFO_H

#include <pthread.h>
#include <list>
#include "structdef.h"

typedef std::list<int> mylist;

class fifo {
public:
	fifo();
	~fifo();
	
	int init(int fifo_count);
	void uninit(void);
	void reset(void);
	
	int push(char* data, int length, int frame_count, bool bCheckDisableCount = true);
	int pop(char* data, int* length, int* frame_count);

	void enablesize(int size);
	
	int getcurlength(void);

	bool checkdisablecount(void);
	void disablepacket(int packet = 0);
private:
	int getvalidindex(void);
	
private:
	int _member_count;
	int _type;
	int _init;
	stbufferflag* _buffer_falg;
	mylist _list;
	pthread_mutex_t _mutex;
	bool _start_pop;
	int  _enable_size;
	pthread_mutex_t _lock;
	int  _disable_count;
};

#endif	// FIFO_H
