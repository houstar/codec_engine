#ifndef IAUDIO_H
#define IAUDIO_H
#include <list>

class iaudio {
public:
	iaudio(void) {}
	virtual ~iaudio(void) {}

	virtual void OnPCM(char* pcm, int len) {}
	virtual void OnMixOut(char* mix, int len, char* all) {}
	virtual void OnAAC(char* aac, int len) {}
	virtual void OnSpeex(char* spx, int len) {}
};

typedef std::list<iaudio*>   listenerList;
#endif	// IAUDIO_H
