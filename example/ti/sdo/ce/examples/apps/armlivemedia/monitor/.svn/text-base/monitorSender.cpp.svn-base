#include "monitorSender.h"

#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Integer.h>
#include <decaf/util/Date.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
//#include <cms/ExceptionListener.h>
//#include <cms/MessageListener.h>

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

#include <stdlib.h>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <time.h>

#include <stdio.h>
#include <string.h>
#include "json.h"
#include "../utils/commonUtils.h"

void* procThreadSend(void* pParam) {
	MonitorSender* pSelf = (MonitorSender*) pParam;
	while (pSelf->m_bInited) {
		printf("before send message to web\n");
		fflush(stdout);
		pSelf->sendMessage();
		printf("after send message to web\n");
		fflush(stdout);
		sleep(30);
	}
	return NULL;
}

void* procThreadRouter(void* param) {
	MonitorSender* pSelf = (MonitorSender*) param;
	char buf[64] = {0};
	char bufRead[512] = {0};
	while (1) {
		if (pSelf->m_strMtgIp != pSelf->m_pLiveCourseManager->getCourseWxManager()->getPlatformIP()) {
			printf("start traceroute %s\n", pSelf->m_pLiveCourseManager->getCourseWxManager()->getPlatformIP().c_str());
			sprintf(buf, "traceroute %s > /home/root/router.txt", pSelf->m_pLiveCourseManager->getCourseWxManager()->getPlatformIP().c_str());
			system(buf);
			printf("after trace route\n");
			pSelf->m_mutex.lock();
			pSelf->m_strRouter = "";
			pSelf->m_mutex.unlock();
			FILE* pfd = fopen("/home/root/router.txt", "rb+");
			if (NULL == pfd){
				perror("open");
				continue;
			}
			while (!feof(pfd))  {
				fgets(bufRead, 512, pfd);  //读取一行
				pSelf->m_mutex.lock();
				pSelf->m_strRouter.append(bufRead);
				pSelf->m_mutex.unlock();
			}

			fclose(pfd);
			pSelf->m_mutex.lock();
			printf("stop traceroute\n");
			pSelf->m_strMtgIp = pSelf->m_pLiveCourseManager->getCourseWxManager()->getPlatformIP();
			pSelf->m_mutex.unlock();
		}
		sleep(3);
	}
	return NULL;
}

MonitorSender::MonitorSender()
: m_bInited(false)
, m_pSetting(NULL)
, m_pLiveCourseManager(NULL)
, m_threadId(0)
, m_strMtgIp("")
, m_threadSendId(0) {
	m_mutex.init();
}

MonitorSender::~MonitorSender() {
	m_mutex.uninit();
}

bool MonitorSender::init(Setting* pSetting, LiveCourseManager* pLiveCourseManager) {
	if (m_bInited) {
		return false;
	}
	m_pSetting = pSetting;
	m_pLiveCourseManager = pLiveCourseManager;
   activemq::library::ActiveMQCPP::initializeLibrary();

	m_bInited = true;

	CommonUtils::getMacInfo(m_strMac, "eth1");
	printf("after get mac info\n");


	pthread_attr_init(&m_thread_attr);
	if (0 != pthread_create(&m_threadId,
		&m_thread_attr,
		procThreadRouter,
		this)) {
		return false;
	}

	pthread_attr_init(&m_threadSend_attr);
	if (0 != pthread_create(&m_threadSendId,
			&m_threadSend_attr,
			procThreadSend,
			this)) {
		return false;
	}

	return true;
}

bool MonitorSender::uninit() {
	if (!m_bInited) {
		return false;
	}
	activemq::library::ActiveMQCPP::shutdownLibrary();
	m_bInited = false;
	if (0 != m_threadSendId) {
		pthread_cancel(m_threadSendId);
		pthread_join(m_threadSendId, NULL);
	}
	return true;
}

bool MonitorSender::sendMessage() {
	Connection* connection = NULL;
	Session* session = NULL;
	Destination* destination = NULL;
	MessageProducer* producer = NULL;
    try {
        // Create a ConnectionFactory
        auto_ptr<ConnectionFactory> connectionFactory( ConnectionFactory::createCMSConnectionFactory("tcp://jiankong.zhihuishu.com:80" ) );
        // Create a Connection
        try {
        	connection = connectionFactory->createConnection();
        	connection->start();
        }  catch( CMSException& e ) {
           // e.printStackTrace();
            throw e;
        }
         // Create a Session
        session = connection->createSession(cms::Session::AUTO_ACKNOWLEDGE );

        // Create the destination (Topic or Queue)
        destination = session->createQueue("monitor");
       // destination = session->createQueue("Test-MQ");//monitor

        // Create a MessageProducer from the Session to the Topic or Queue
        producer = session->createProducer( destination );
        producer->setDeliveryMode( DeliveryMode::PERSISTENT );

        // Create a messages
        generateMessage();
         TextMessage* message = session->createTextMessage(m_strMessage);
         producer->send(message );
         delete message;

      }catch ( CMSException& e ) {
    	 // e.printStackTrace();
      }

       //释放资源
       try{
           if( destination != NULL ) {
        	   delete destination;
           }
       }catch ( CMSException& e )  {
    	  // e.printStackTrace();
       }
       destination = NULL;

        try{
            if( producer != NULL ) {
            	delete producer;
            }
        }catch ( CMSException& e ) {
        	//e.printStackTrace();
        }
        producer = NULL;

        // Close open resources.
        try{
            if ( session != NULL ) {
            	session->close();
            }
            if( connection != NULL ) {
            	connection->close();
            }
        }catch ( CMSException& e ) {
        	//e.printStackTrace();
        }

        try{
            if( session != NULL ) {
            	delete session;
            }
        }catch ( CMSException& e ) {
        	//e.printStackTrace();
        }
        session = NULL;

        try{
            if( connection != NULL ) {
            	delete connection;
            }
        }catch ( CMSException& e ) {
        	//e.printStackTrace();
        }
        connection = NULL;


	return true;
}

bool MonitorSender::generateMessage() {
	Json::Value root;
	root["messageType"] = "1";   ///给root中添加属性(arrayObj变为map)
	time_t curTime = time(NULL);
	tm* t = localtime(&curTime);
	char buf[128]= {0};
	sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
			t->tm_hour, t->tm_min, t->tm_sec);
	root["messageTime"] = buf;

	sprintf(buf, "%d", m_pSetting->getEventId());
	root["incidentId"] = buf;

	sprintf(buf, "%d", m_pLiveCourseManager->getCourseWxManager()->getCourseId());
	root["courseId"] = buf;
	root["mac"] = m_strMac;
	root["classId"] = m_pSetting->getClassRoomID();

	root["className"] = m_pSetting->getRoomName(); //utf8 code, do not set unicode ,if set unicode, the web will get error
	sprintf(buf, "%d", CommonUtils::getCpuLoad());
	root["cpu1"] = buf;

	sprintf(buf, "%d", m_pLiveCourseManager->getCourseWxManager()->getWebMessage()->cpu);
	root["cpu2"] = buf;
	root["cpu3"] = "";
	sprintf(buf, "%d", CommonUtils::getMemLoad());
	root["memory1"] = buf;

	sprintf(buf, "%d", m_pLiveCourseManager->getCourseWxManager()->getWebMessage()->memUsed);
	root["memory2"] = buf;
	root["memory3"] = "";

	sprintf(buf, "%d", m_pLiveCourseManager->getCourseWxManager()->getSendCount());
	root["upNetWork"] = buf;

	sprintf(buf, "%d", m_pLiveCourseManager->getCourseWxManager()->getWebMessage()->downNetWork);
	root["downNetWork"] = buf;

	sprintf(buf, "%d", m_pLiveCourseManager->getCourseWxManager()->getSendLossCount());
	root["upPacketLoss"] = buf;

	sprintf(buf, "%d", m_pLiveCourseManager->getCourseWxManager()->getWebMessage()->downPacketLost);
	root["downPacketLoss"] = buf;

	root["routingTable"] = m_strRouter;

	root["address"] = m_pLiveCourseManager->getCourseWxManager()->getPlatformIP();

	sprintf(buf, "%d", m_pLiveCourseManager->getCourseWxManager()->getWebMessage()->status);
	root["assistStatus"] = buf;

	for (int i = 0; i < 10; ++i) {
		sprintf(buf, "streamContent%d", i + 1);
		root[buf] = "";
		sprintf(buf, "streamfps%d", i + 1);
		root[buf] = "";
	}

	vector<tagStreamInfo> vecStreamInfos = m_pLiveCourseManager->getCourseWxManager()->getStreaInfos();
	char bufName[32];


	for (int i = 0; i < vecStreamInfos.size(); ++i) {
		sprintf(bufName, "streamContent%d", i + 1);
		root[bufName] = vecStreamInfos.at(i).streamContent; //utf8 code
		//printf("the stream context is %s\n", vecStreamInfos.at(i).streamContent);
		sprintf(buf, "%d", vecStreamInfos.at(i).streamfps);
		sprintf(bufName, "streamfps%d", i + 1);
		root[bufName] = buf;
	}


	//m_strMessage = root.toStyledString();   ///转换为json格式字符串
	Json::FastWriter fast_writer;
	m_strMessage = fast_writer.write(root);
	return true;
}

