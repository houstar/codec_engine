#include "Mutex.h"

Mutex::Mutex() {

}
Mutex::~Mutex() {

}

bool Mutex::init() {
	int rc = pthread_mutexattr_init(&attr);
	if (rc != 0)
	{
		return false;
	}
	rc = pthread_mutex_init(&mutex, &attr);
	if (rc != 0)
	{
		return false;
	}
	return true;
}

bool Mutex::uninit() {
	pthread_mutexattr_destroy(&attr);
	pthread_mutex_destroy(&mutex);
	return true;
}

void Mutex::lock() {
	pthread_mutex_lock(&mutex);
}

void Mutex::unlock() {
	pthread_mutex_unlock(&mutex);
}
