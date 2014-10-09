#pragma once
#include <pthread.h>


class Mutex{
public:
	Mutex();
	~Mutex();

public:
	bool init();
	bool uninit();
	void lock();
	void unlock();

private:
	pthread_mutexattr_t attr;
	pthread_mutex_t mutex;
};


