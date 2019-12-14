#include "udpInstance.h"
#include "trace.h"



udpInstance::udpInstance()
{
	/*��ʼ������*/
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

	/*����������*/
	sendqueue_mutex = ::CreateMutex(NULL, FALSE, NULL);
	if (NULL == sendqueue_mutex) {
		return;
	}
	recvqueue_mutex = ::CreateMutex(NULL, FALSE, NULL);
	if (NULL == recvqueue_mutex) {
		return;
	}

	/*�����߳���*/
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
	/*�ر��߳�*/
	sendoff = true;
	recvoff = true;

	while (true == sendoff || true == recvoff) {//�����ͺͽ����̶߳��˳�������whileѭ��
		Sleep(10);
	}

	/*���ٻ�����*/
	::CloseHandle(sendqueue_mutex);
	::CloseHandle(recvqueue_mutex);
}


int udpInstance::udpinit(int dstport, int srcport, string dstip, string srcip)
{
	bool isConnected=false;
	m_isReady = false;

	/*0.�������*/
	dst_ip = dstip;
	src_ip = srcip;
	dst_port = dstport;
	src_port = srcport;

	dst_ip = "192.168.13.97";
	src_ip = "192.168.13.97";
	dst_port = 60000;
	src_port = 1000;

	/*1.����socket*/
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(dst_port);
	dst_addr.sin_addr.s_addr = inet_addr(dst_ip.c_str());
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sock_fd) {
		LOG(INFO) << "create socket error , code is "<<GetLastError();
		return -1;
	}

	/*2.(UDP����Ҫconnect)����CONNECT_RETRY_TIMES �����ӷ�����*/

	/*3.ʹ���������߳�*/
	if (0 != startSendThread()) {
		LOG(INFO) << "start udp send thread error";
		return -1;
	}

	/*4.ʹ���������߳�*/
	if (0 != startRecvThread()) {
		LOG(INFO) << "start udp recv thread error";
		return -1;
	}

	m_isReady = true;
	return 0;
}

int udpInstance::udpinit(int dstport, string dstip)
{
	bool isConnected = false;
	m_isReady = false;

	/*0.�������*/
	dst_ip = dstip;
	dst_port = dstport;

	dst_ip = "192.168.13.97";
	src_ip = "192.168.13.97";
	dst_port = 60000;
	src_port = 1000;

	/*1.����socket*/
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(dst_port);
	dst_addr.sin_addr.s_addr = inet_addr(dst_ip.c_str());
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sock_fd) {
		LOG(INFO) << "create socket error , code is " << GetLastError();
		return -1;
	}

	/*2.(UDP����Ҫconnect)����CONNECT_RETRY_TIMES �����ӷ�����*/

	/*3.ʹ���������߳�*/
	if (0 != startSendThread()) {
		LOG(INFO) << "start udp send thread error";
		return -1;
	}

	/*4.ʹ���������߳�*/
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
		/*�������Ϊ�գ�������ж�*/
		if (true == pthis->sendQ.empty() || 0 == pthis->sendQ.size()) {
			LOG(INFO) << "[SendThread] sendQ is empty";
			continue;
		}

		/*������в�Ϊ��*/
		pthis->sendlock();
		string tmpsend = pthis->sendQ.front();		//ȡ����
		pthis->sendQ.pop();						//ɾ������
		pthis->sendunlock();

		int nSend = sendto(pthis->sock_fd, tmpsend.c_str(), tmpsend.length(), 0, (struct sockaddr*)&pthis->dst_addr, sizeof(pthis->dst_addr));
		if (0 < nSend) {
			//LOG(INFO) << "[SendThread] current send " << nSend << " characters";
		}else {
			int result = GetLastError();
			LOG(INFO)<<"[SendThread] sendto ERROR , code is "<<result;
			continue;
		}

		if (true == pthis->sendoff) {	//�ر��߳�
			break;
		}
	}
	pthis->sendoff = false;
	LOG(INFO) << "Send Thread terminate success";
	return 0;
}

unsigned int __stdcall  udpInstance::RecvThread(void* pParam)
{
	LOG(INFO) << "XXXXXXXXXX";
	sockaddr_in dstaddr;
	memset(&dstaddr,'\0',sizeof(dstaddr));
	int dstaddrlen = sizeof(dstaddr);
	udpInstance* pthis = (udpInstance*)pParam;

	//debug
	FILE * fp;
	if ((fp = fopen("E:\\caltta_eChat\\MDC\\1.pcm", "rb")) == NULL){
		LOG(INFO)<<"cannot open this file";
		return -1;
	}
	//debug


	while (1) {
		Sleep(2000);
		/*�������Ϊ����������������*/
		if (RECV_QUEUE_DEPTH == pthis->sendQ.size()) {
			LOG(INFO) << "queue is alredy full, num is -> " << pthis->sendQ.size();
			continue;
		}

		char buff[UDP_MAX_LENGTH];
		memset(buff, '\0', sizeof(buff));
		string result;
		memset(&dstaddr, '\0', sizeof(dstaddr));

		//����������ȴ�
		//int nRecv = recvfrom(pthis->sock_fd, buff, UDP_MAX_LENGTH-1, 0, (struct sockaddr*)&dstaddr, &dstaddrlen);
		//if (0 < nRecv) {
		//	LOG(INFO) << "current recvfrom " << nRecv << " characters";
		//	string senderip = inet_ntoa(dstaddr.sin_addr);
		//	u_short senderport = ntohs(dstaddr.sin_port);
		//	short senderfamily = dstaddr.sin_family;
		//	LOG(INFO) << "dst ip is -> " << senderip;
		//	LOG(INFO) << "dst port is -> " << senderport;
		//	if (senderip != pthis->dst_ip || senderport != pthis->dst_port || senderfamily != pthis->dst_addr.sin_family){
		//		LOG(INFO) << "dst addr does not match to last saved";
		//		continue;
		//	}
		//	result = buff;
		//	/*��buff���ݣ�д��recvQ*/
		//	pthis->recvlock();
		//	pthis->recvQ.push(result);						//ɾ������
		//	pthis->recvunlock();
		//}else {
		//	int result = GetLastError();
		//	LOG(INFO)<<"recvfrom ERROR , code is "<<result;
		//	if (result == 10040) LOG(INFO) << "received data is longer than UDP_MAX_LENGTH";
		//	continue;
		//}

		//debug
		Sleep(1000);
		char buf[321];
		memset(buff, '\0', sizeof(buff));
		int buf_len = 320;
		if (fread(buf, 1, buf_len, fp) != buf_len){
			//File End
			//Loop:
			fseek(fp, 0, SEEK_SET);
			fread(buf, 1, buf_len, fp);
			//Close:
			//isPlaying=0;
		}
		LOG(INFO) << "buf->" << buf;
		pthis->recvlock();
		pthis->recvQ.push(string(buf));						//ɾ������
		pthis->recvunlock();

		//debug


		if (true == pthis->recvoff) {	//�ر��߳�
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
	if (sendQ.size() >= SEND_QUEUE_DEPTH){
		LOG(INFO) << "SEND QUEUE IS FULL , RETURN";
		return;
	}

	sendlock();
	sendQ.push(arg);
	sendunlock();
}

string udpInstance::readFromRecvQ()
{
	if (true == recvQ.empty()){
		LOG(INFO) << "RECV QUEUE IS EMPTY , RETURN";
		return "";
	}

	string result;
	recvlock();
	result = recvQ.front();
	recvQ.pop();
	recvunlock();
	return result;
}