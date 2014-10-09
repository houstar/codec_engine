
#include <stdio.h>
#include "type.h"
#include <time.h>

#ifndef _DSP_
#include  <vector>
using namespace std ;
#define JITTEBUFFER_MAX     200

#ifndef _LINUX_
#include <windows.h>
#else
#include<pthread.h>
#include <stdlib.h>
#include <string.h>
#endif

typedef struct avd_mutex_t
{
#ifndef _LINUX_
	CRITICAL_SECTION	crit;
#else
	pthread_mutex_t* crit;
#endif
}avd_mutex_t;

//////////////////////////mutex////////////////////////////////////////////////
int avd_mutex_init(avd_mutex_t *mutex)
{
#ifndef _LINUX_
	InitializeCriticalSection(&mutex->crit);
	return 1;
#else
	pthread_mutexattr_t attr;
	int rc;
	rc = pthread_mutexattr_init(&attr);
	if (rc != 0)
		return 0;
	rc = pthread_mutex_init(&mutex->crit, &attr);
	return rc;
#endif	
}

int avd_mutex_lock(avd_mutex_t *mutex)
{
#ifndef _LINUX_
	EnterCriticalSection(&mutex->crit);
	return 1;
#else
    return pthread_mutex_lock( &mutex->crit );
#endif
}

int avd_mutex_unlock(avd_mutex_t *mutex)
{
#ifndef _LINUX_
    LeaveCriticalSection(&mutex->crit);
    return 1;
#else
	return pthread_mutex_unlock( &mutex->crit );	
#endif	
}


int  avd_mutex_destroy(avd_mutex_t *mutex)
{
#ifndef _LINUX_
    DeleteCriticalSection(&mutex->crit);
    return 1;
#else
	int status = 0;
	unsigned retry;
	enum { RETRY = 4 };
	for (retry=0; retry<RETRY; ++retry) {
		status = pthread_mutex_destroy( &mutex->crit );
		if (status == 0)
			break;
		else if (retry<RETRY-1 && status == EBUSY)
			pthread_mutex_unlock(&mutex->crit);
	}
	return status;
#endif	
}

typedef struct JitterInfo
{
	char     *p_Data;
	int      m_Len;
}JitterInfo;

vector< JitterInfo> jitterinfo;
avd_mutex_t jittermutex;


void JitterUnInt()
{
	avd_mutex_lock(&jittermutex);
	while (jitterinfo.size() > 0)
	{
		JitterInfo it = jitterinfo.at(0);
		delete it.p_Data;
		it.p_Data = 0;
		jitterinfo.erase(jitterinfo.begin());
	}
	avd_mutex_unlock(&jittermutex);
	avd_mutex_destroy(&jittermutex);
}

void JitterInit(int maxsize)
{
	avd_mutex_init(&jittermutex);
	jitterinfo.clear();
}

int JitterRead(char **pdata,int *len)
{
	avd_mutex_lock(&jittermutex);
	if (jitterinfo.size() <= 0)
	{
		avd_mutex_unlock(&jittermutex);
		return 0;
	}
	JitterInfo *tempSeg = &jitterinfo.at(0);
	if (tempSeg == 0 || tempSeg->p_Data == 0)
	{
		avd_mutex_unlock(&jittermutex);
		return 0;
	}
	*len = tempSeg->m_Len;
	*pdata =  tempSeg->p_Data;
	jitterinfo.erase(jitterinfo.begin());
	avd_mutex_unlock(&jittermutex);
	return  1;
}

int JitterWrite(char *pdata,int len)
{
	if (pdata == 0 || len <= 0)
	{

		return 0;
	}
	avd_mutex_lock(&jittermutex);
	if (jitterinfo.size() >= JITTEBUFFER_MAX)
	{
		avd_mutex_unlock(&jittermutex);
		return 0;
	}
	JitterInfo tempSeg;
	tempSeg.m_Len = len;
	tempSeg.p_Data = new char[len];
	memcpy(tempSeg.p_Data,pdata,len);
	jitterinfo.push_back(tempSeg);
	avd_mutex_unlock(&jittermutex);
	return  1;
}


/* AEC初始化函数 */
int AEC_Init(T_avdAECParam *pParam)
{
	if (pParam == 0)
	{
		return 0;
	}
	if (pParam->buffercount <= 3)
	{
		pParam->buffercount = 3;
	}
	JitterInit(pParam->buffercount);
	return avd_AEC_Init(pParam);
}

/* 播放函数 */
int AEC_Play(char *playaudio,int length)
{
	if (playaudio == 0)
	{
		return 0;
	}
	return JitterWrite(playaudio,length);
}	
/* 采集函数  */
int AEC_Record(char *recorddata, int lenght,char * outdata,int involm)
{
	int playdata = 0;
	int length = 0;
	JitterRead((char **)&playdata,&length);
	int re = avd_AEC_Proc((short *)recorddata,(short *)playdata,(short *)outdata,involm);
	if (playdata)
	{
		delete (char*)playdata;
	}
	return re;
}

void AEC_UnInit()
{
	JitterUnInt();
	avd_AEC_UnInit();
}	

#define READSIZE   160
char  inputFromPeer[READSIZE*2];
char  inputToPeer[READSIZE*2];
char  inputToPeert[READSIZE*2];
int main_2(char*from,char*to,char*out)
{
	int nb_frame;
	T_avdAECParam   pParam;
	clock_t start, finish;
	double duration;
	FILE* fp_inFromPeer;
	FILE* fp_inToPeer;
	FILE* fp_out;
	if ( (fp_inFromPeer = fopen(from, "rb")) == NULL)
	{
		return 0;
	} 

	if ( (fp_inToPeer = fopen(to, "rb")) == NULL)
	{
		return 0;
	} 

	if ( (fp_out = fopen(out, "wb")) == NULL)
	{
		return 0;
	}

	/*-----------------------------------------------------------------------*
	 * AEC                                                                   *
	 *-----------------------------------------------------------------------*/

	memset(&pParam,0,sizeof(pParam));
	pParam.aec_param = 1;
	pParam.ns_param = 2;
	pParam.sample_rate = 1;
	pParam.buffercount = 10;
	pParam.avdAgcParam.hagc_max_vol = 65535;
	pParam.avdAgcParam.hagc_min_vol = 0;
	pParam.avdAgcParam.hagc_param = 1;
	pParam.avdAgcParam.sagc_param = 0;
	pParam.avdAgcParam.hagc_in_vol = 0;
	AEC_Init(&pParam);
	nb_frame = 0;
  	start = clock();
 
	while(fread(inputFromPeer, sizeof(short), READSIZE, fp_inFromPeer) == READSIZE  &&
		  fread(inputToPeer,   sizeof(short), READSIZE, fp_inToPeer  ) == READSIZE  
		) 
	{  
        //nb_frame++;
		printf("AEC process frame %d\r", ++nb_frame); 
		int hagc_in_vol = nb_frame *20;
		AEC_Play(inputFromPeer,READSIZE *  sizeof(short));
		AEC_Record(inputToPeer, READSIZE *  sizeof(short),inputToPeert,hagc_in_vol);		
		//printf( "pParam.avdAgcParam.hagc_in_vol = %d\n", pParam.avdAgcParam.hagc_out_vol );
		fwrite(inputToPeert, sizeof(short), READSIZE, fp_out);
		fflush(fp_out);
	}

	AEC_UnInit();
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf( "\n%2.1f seconds\n", duration );
	fclose(fp_out);
	fclose(fp_inFromPeer);
	fclose(fp_inToPeer);
	return 1;
}


int main()
{
	char *from0 = "pcm\\FromPeer.pcm";
	char *to0 = "pcm\\ToPeer.pcm";
	char *out0 = "pcm\\output.pcm";	
	printf("\n-------------        AEC            ------------\n");
	main_2(from0,to0,out0);
	return 0;
}

#endif