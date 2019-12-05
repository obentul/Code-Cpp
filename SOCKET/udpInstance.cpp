#include "udpInstance.h"
#include "trace.h"



udpInstance::udpInstance()
{
	/*初始化队列*/
	if (false == sendQ.empty() || 0!=sendQ.size()) {
		for (size_t i = 0; i < sendQ.size(); i++)
		{
			sendQ.pop();
		}
	}
	if (false == recvQ.empty() || 0 != recvQ.size()) {
		for (size_t i = 0; i < recvQ.size(); i++)
		{
			recvQ.pop();
		}
	}

	/*debug*/
	for (size_t i = 0; i < 50; i++)
	{
		sendQ.push("this is debug data");
	}
	/*debug*/

	/*创建队列锁*/
	sendqueue_mutex = ::CreateMutex(NULL, FALSE, NULL);
	if (NULL == sendqueue_mutex) {
		return;
	}
	recvqueue_mutex = ::CreateMutex(NULL, FALSE, NULL);
	if (NULL == recvqueue_mutex) {
		return;
	}

	/*创建线程锁*/
	sendthread_mutex = ::CreateMutex(NULL,FALSE,NULL);
	if (NULL == sendqueue_mutex) {
		return;
	}
	recvthread_mutex = ::CreateMutex(NULL, FALSE, NULL);
	if (NULL == recvthread_mutex) {
		return;
	}
}

udpInstance::~udpInstance()
{
	/*关闭线程*/
	sendoff = true;
	recvoff = true;

	while (true == sendoff || true == recvoff) {//当发送和接收线程都退出后，跳出while循环
		Sleep(10);
	}

	/*销毁互斥量*/
	::CloseHandle(sendqueue_mutex);
	::CloseHandle(recvqueue_mutex);
}


int udpInstance::udpinit(int dstport, int srcport, string dstip, string srcip)
{
	bool isConnected=false;
	m_isReady = false;

	/*0.参数落地*/
	dst_ip = dstip;
	src_ip = srcip;
	dst_port = dstport;
	src_port = srcport;

	dst_ip = "192.168.13.97";
	src_ip = "192.168.13.97";
	dst_port = 60000;
	src_port = 1000;

	/*1.创建socket*/
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(dst_port);
	dst_addr.sin_addr.s_addr = inet_addr(dst_ip.c_str());
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sock_fd) {
		LOG(INFO) << "create socket error , code is "<<GetLastError();
		return -1;
	}

	/*2.(UDP不需要connect)尝试CONNECT_RETRY_TIMES 次连接服务器*/

	/*3.使启动发送线程*/
	if (0 != startSendThread()) {
		LOG(INFO) << "start udp send thread error";
		return -1;
	}

	/*4.使启动接收线程*/
	if (0 != startRecvThread()) {
		LOG(INFO) << "start udp recv thread error";
		return -1;
	}

	m_isReady = true;
	return 0;
}

unsigned int __stdcall  udpInstance::SendThread(void* pParam)
{
	udpInstance* pthis = (udpInstance*)pParam;
	while (1) {
		Sleep(20);
		/*如果队列为空，则继续判断*/
		if (true == pthis->sendQ.empty() || 0 == pthis->sendQ.size()) {
			continue;
		}

		/*如果队列不为空*/
		pthis->sendlock();
		string tmpsend = pthis->sendQ.front();		//取数据
		pthis->sendQ.pop();						//删除数据
		pthis->sendunlock();

		int nSend = sendto(pthis->sock_fd, tmpsend.c_str(), tmpsend.length(), 0, (struct sockaddr*)&pthis->dst_addr, sizeof(pthis->dst_addr));
		if (0 < nSend) {
			LOG(INFO) << "current send " << nSend << " characters";
		}else {
			int result = GetLastError();
			LOG(INFO)<<"sendto ERROR , code is "<<result;
		}

		if (true == pthis->sendoff) {	//关闭线程
			break;
		}
	}
	pthis->sendoff = false;
	LOG(INFO) << "Send Thread terminate success";
	return 0;
}

unsigned int __stdcall  udpInstance::RecvThread(void* pParam)
{
	sockaddr_in dstaddr;
	int dstaddrlen;
	memset(&dstaddr,'\0',sizeof(dstaddr));
	udpInstance* pthis = (udpInstance*)pParam;
	while (1) {
		Sleep(20);
		/*如果队列为满，跳出继续接收*/
		if (RECV_QUEUE_DEPTH == pthis->sendQ.size()) {
			LOG(INFO) << "queue is alredy full, num is -> " << pthis->sendQ.size();
			continue;
		}

		char buff[1024];
		string result;

		int nRecv = recvfrom(pthis->sock_fd, buff, 1024, 0, (struct sockaddr*)&dstaddr, &dstaddrlen);
		if (0 < nRecv) {
			LOG(INFO) << "current send " << nRecv << " characters";
			memset(&dstaddr, '\0', sizeof(dstaddr));
			if (dstaddrlen != sizeof(&dst_addr) || memcmp(&pthis->dst_addr, &dstaddr, dstaddrlen) != 0) {
				LOG(INFO) << "dst addr does not match to last saved";
			}
			result = buff;
		}else {
			int result = GetLastError();
			LOG(INFO)<<"sendto ERROR , code is "<<result;
		}

		/*如果队列不为满*/
		pthis->recvlock();
		pthis->recvQ.push(result);						//删除数据
		pthis->recvunlock();

		if (true == pthis->recvoff) {	//关闭线程
			break;
		}
	}
	pthis->recvoff = false;
	LOG(INFO) << "Recv Thread terminate success";
	return 0;
}



int udpInstance::startRecvThread() {
	UINT32 nThreadId = 0;
	HANDLE threadhandle = (HANDLE)_beginthreadex(NULL, 0, RecvThread, this, 0, &nThreadId);
	return 0;
}

int udpInstance::startSendThread() {
	UINT32 nThreadId = 0;
	HANDLE threadhandle = (HANDLE)_beginthreadex(NULL, 0, SendThread, this, 0, &nThreadId);
	return 0;
}


void udpInstance::sendlock()
{
	::WaitForSingleObject(sendqueue_mutex, INFINITE);
}

void udpInstance::recvlock()
{
	::WaitForSingleObject(recvqueue_mutex, INFINITE);
}

void udpInstance::sendunlock()
{
	::ReleaseMutex(sendqueue_mutex);
}

void udpInstance::recvunlock()
{
	::ReleaseMutex(recvqueue_mutex);
}

void udpInstance::appendToSendQ(string arg)
{
	sendlock();
	sendQ.push(arg);
	sendunlock();
}

string udpInstance::readFromRecvQ()
{
	string result;
	recvlock();
	result = recvQ.front();
	recvQ.pop();
	recvunlock();
	return result;
}
