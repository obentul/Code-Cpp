#include "udpInstance.h"
#include "trace.h"
#include "udpInstanceCustomer.h"

udpInstanceManager* udpInstanceManager::_instance = nullptr;

udpInstanceManager::udpInstanceManager()
{

}

udpInstanceManager::~udpInstanceManager()
{
	for (vector<udpInstance*>::iterator it = udpInstanceList.begin(); it != udpInstanceList.end(); it++)
	{
		udpInstance* tmp = *it;
		if (nullptr != tmp){
			delete tmp;
			tmp = nullptr;
		}
	}
}

udpInstanceManager* udpInstanceManager::getudpInstanceManager()
{
	if (nullptr == _instance){
		return new udpInstanceManager();
	}
	return _instance;
}

udpInstance* udpInstanceManager::createudpInstance()
{
	udpInstance* tmp = new udpInstance();
	if (nullptr != tmp){
		udpInstanceList.push_back(tmp);
		return tmp;
	}
	return nullptr;
}

udpInstance::udpInstance()
{
	/*��ʼ������*/
	//if (false == sendQ.empty() || 0!=sendQ.size()) {
	//	for (size_t i = 0; i < sendQ.size(); i++)
	//	{
	//		sendQpop();
	//	}
	//}
	//if (false == recvQ.empty() || 0 != recvQ.size()) {
	//	for (size_t i = 0; i < recvQ.size(); i++)
	//	{
	//		recvQpop();
	//	}
	//}

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

//������߳�delete udpinstance������������ִ��ʱ��Ӧ���ǹ̶��ģ�����ᵼ�����߳̿���
udpInstance::~udpInstance()
{
	/*�ر��߳�(������ͺͽ����߳�ִ�еĽϿ죬��˱�־λ���ã�����û��)*/
	sendoff = true;
	recvoff = true;

	/* ���������߳� */
	if (h_RecvThread != NULL)
	{
		int ret = TerminateThread(h_RecvThread, 0);
		if (0 == ret) LOG(INFO) << "TerminateThread [RecvThread] ERROR";
		CloseHandle(h_RecvThread);
		h_RecvThread = NULL;
	}

	/* ���������߳� */
	if (h_SendThread != NULL)
	{
		int ret = TerminateThread(h_SendThread, 0);
		if (0 == ret) LOG(INFO) << "TerminateThread [SendThread] ERROR";
		CloseHandle(h_SendThread);
		h_SendThread = NULL;
	}

	/*��� ���Ͷ��� �� ���ն���*/
	sendlock();
	int sendQsize = sendQ.size();
	LOG(INFO) << "[CLEAR SENDQ ] sendQ size is -> " << sendQsize;
	for (size_t i = 0; i < sendQsize; i++)
	{
		char* tmp;
		tmp = sendQ.front();
		if (NULL != tmp){
			LOG(INFO) << "[CLEAR SENDQ ] SEGMENT IS NOT NULL, DELETE && POP";
			sendQpop();
		}
	}
	sendunlock();

	recvlock();
	int recvQsize = recvQ.size();
	LOG(INFO) << "[CLEAR SENDQ ] recvQ size is -> " << recvQsize;
	for (size_t i = 0; i < recvQsize; i++)
	{
		char* tmp;
		tmp = recvQ.front();
		if (NULL != tmp){
			LOG(INFO) << "[CLEAR recvQ ] SEGMENT IS NOT NULL, DELETE && POP";
			recvQpop();
		}
	}
	recvunlock();

	/*���ٻ����� 4��HANDLE*/
	::CloseHandle(sendqueue_mutex);
	sendqueue_mutex = NULL;
	::CloseHandle(recvqueue_mutex);
	recvqueue_mutex = NULL;
	::CloseHandle(sendthread_mutex);
	sendthread_mutex = NULL;
	::CloseHandle(recvthread_mutex);
	recvthread_mutex = NULL;

	/*��ն��Ķ���*/
	for (size_t i = 0; i < customerList.size(); i++){
		udpInstanceCustomer* tmp = customerList[i];
		tmp = NULL;
	}
}

int udpInstance::udpinit(int dstport, int srcport, string dstip, string srcip)
{
	LOG(INFO) << "[udpinit] start";
	m_isReady = false;	//udpʵ��Ĭ�ϲ�����

	/*0.�������*/
	dst_ip = dstip;
	src_ip = srcip;
	dst_port = dstport;
	src_port = srcport;

	/*1.����socket*/
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(dst_port);
	dst_addr.sin_addr.s_addr = inet_addr(dst_ip.c_str());
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sock_fd) {
		LOG(INFO) << "create socket error , code is "<<GetLastError();
		return -1;
	}

	/*�󶨱��ض˿�*/
	/*���󶨵Ļ����ᵼ�½��շ���10022 �� 10040�������ڷ���udp�����ٱ���Ŀǰ��û��ȷ����Ϊ�ͻ���Ӧ��ʹ�õĶ˿ڣ��������Ҳ��󶨹̶��˿�*/
	//src_addr.sin_family = AF_INET;
	//src_addr.sin_port = htons(src_port);
	//src_addr.sin_addr.s_addr = inet_addr(src_ip.c_str());
	//bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

	/*2.(UDP����Ҫconnect)����CONNECT_RETRY_TIMES �����ӷ�����*/

	/*3.ʹ���������߳�*/
	if (0 != startSendThread()) {
		LOG(INFO) << "[udpinit] start udp send thread error";
		return -1;
	}
	LOG(INFO) << "[udpinit] startSendThread success";

	/*4.ʹ���������߳�*/
	if (0 != startRecvThread()) {
		LOG(INFO) << "[udpinit] start udp recv thread error";
		return -1;
	}
	LOG(INFO) << "[udpinit] startRecvThread success";

	m_isReady = true;		//udp��ʼ����ϣ�ͬʱ�շ��߳�������ϣ��������״̬

	LOG(INFO) << "[udpinit] CREATE UDP Instance SUCCESS ," << " [usage]:" << m_usage << " [sendQ Deepth ]:" << SEND_QUEUE_DEPTH
		<< " [recvQ Deepth ]:" << SEND_QUEUE_DEPTH << " [fd_socket]:" << sock_fd << " [sendThread id]:" << sendThreadID
		<< " [recvThread id]:" << recvThreadID;

	LOG(INFO) << "[udpinit] success";
	return 0;
}

int udpInstance::udpinit(int dstport, string dstip)
{
	LOG(INFO) << "[udpinit] start";
	m_isReady = false;

	/*0.�������*/
	dst_ip = dstip;
	dst_port = dstport;

	/*1.����socket*/
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(dst_port);
	dst_addr.sin_addr.s_addr = inet_addr(dst_ip.c_str());
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sock_fd) {
		LOG(INFO) << "[udpinit] create socket error , code is " << GetLastError();
		return -1;
	}

	/*2.(UDP����Ҫconnect)����CONNECT_RETRY_TIMES �����ӷ�����*/

	/*3.ʹ���������߳�*/
	if (0 != startSendThread()) {
		LOG(INFO) << "[udpinit] start udp send thread error";
		return -1;
	}
	LOG(INFO) << "[udpinit] startSendThread success";

	/*4.ʹ���������߳�*/
	if (0 != startRecvThread()) {
		LOG(INFO) << "[udpinit] start udp recv thread error";
		return -1;
	}
	LOG(INFO) << "[udpinit] startRecvThread success";

	m_isReady = true;

	LOG(INFO) << "[udpinit] CREATE UDP Instance SUCCESS ," << " [usage]:" << m_usage << " [sendQ Deepth ]:" << SEND_QUEUE_DEPTH
		<< " [recvQ Deepth ]:" << SEND_QUEUE_DEPTH << " [fd_socket]:" << sock_fd << " [sendThread id]:" << sendThreadID
		<< " [recvThread id]:" << recvThreadID;

	LOG(INFO) << "[udpinit] success";
	return 0;
}

int udpInstance::startRecvThread() {
	if (NULL != h_RecvThread){
		return 0;
	}
	h_RecvThread = (HANDLE)_beginthreadex(NULL, 0, RecvThread, this, 0, &recvThreadID);
	if (NULL == h_RecvThread){
		LOG(INFO) << "[startRecvThread] ERROR , " << m_usage << "udp instance create RecvThread error";
		return -1;
	}
	return 0;
}

int udpInstance::startSendThread() {
	if (NULL != h_SendThread){
		return 0;
	}
	h_SendThread = (HANDLE)_beginthreadex(NULL, 0, SendThread, this, 0, &sendThreadID);
	if (NULL == h_SendThread){
		LOG(INFO) << "[startSendThread] ERROR , " << m_usage << "udp instance create SendThread error";
		return -1;
	}
	return 0;
}


unsigned int __stdcall  udpInstance::SendThread(void* pParam)
{
	if (nullptr == pParam){
		LOG(INFO) << "[SendThread] ERROR , pParam is NULL , terminate SendThread";
		return -1;
	}
	udpInstance* pthis = (udpInstance*)pParam;
	pthis->sendoff = false;		//�߳��Ѿ�������֪ͨudpInstance���ñ�־λ

	while (1) {
		Sleep(20);					//20ms��һ��

		/*�������Ϊ�գ�������ж�*/
		if (true == pthis->sendQ.empty() || 0 == pthis->sendQ.size()) {
			Sleep(20);				//����Ϊ�գ������sleep 20ms
			continue;
		}

		/*������в�Ϊ��*/
		pthis->sendlock();
		char* tmpsend = pthis->sendQ.front();		//ȡ��Ԫ��
		pthis->sendQpop();							//ɾ����Ԫ��
		pthis->sendunlock();

		/*�����쳣��������ѯ*/
		if (nullptr == tmpsend) continue;

		/*���ݷ���*/
		int nSend = sendto(pthis->sock_fd, tmpsend, BYTES_PER_FRAME, 0, (struct sockaddr*)&pthis->dst_addr, sizeof(pthis->dst_addr));
		if (nSend > 0) 
		{
			//LOG(INFO) << "[SendThread] sendto SUCCESS " << tmpsend << " | NUM IS -> " << nSend;
		}
		else
		{
			//LOG(INFO) << "[SendThread] sendto ERROR ,  CODE is ->" << GetLastError();
		}

		/*�ر��߳�*/
		if (true == pthis->sendoff) {
			break;
		}
	}
	pthis->sendoff = false;
	LOG(INFO) << "[SendThread] Send Thread terminate success";
	return 0;
}

unsigned int __stdcall  udpInstance::RecvThread(void* pParam)
{
	if (nullptr == pParam){
		LOG(INFO) << "[RecvThread] ERROR , pParam is NULL , terminate RecvThread";
		return -1;
	}
	udpInstance* pthis = (udpInstance*)pParam;
	pthis->recvoff = false;		//�߳��Ѿ�������֪ͨudpInstance���ñ�־λ

	//�Զ˵�ַ��֤
	sockaddr_in dstaddr;
	int dstaddrlen = sizeof(dstaddr);

	while (1) {
		/*�������Ϊ���������������Լ�����*/
		if (RECV_QUEUE_DEPTH == pthis->recvQ.size()) {
			//LOG(INFO) << "[RecvThread] recv queue is alredy full, num is -> " << pthis->recvQ.size();
			Sleep(20);
			continue;
		}

		//������ػ�����
		char buff[UDP_MAX_LENGTH];
		memset(buff, '\0', sizeof(buff));
		memset(&dstaddr, '\0', sizeof(dstaddr));

		//����������ȴ�
		int nRecv = recvfrom(pthis->sock_fd, buff, UDP_MAX_LENGTH, 0, (struct sockaddr*)&dstaddr, &dstaddrlen);
		if (nRecv > 0) {
			//����ѹ����ն���(����յ������� ���� BYTES_PER_FRAME �����ڶ����з������Σ���֤ÿһ�ζ���320�ֽڣ�����ָ��� ���� BYTES_PER_FRAME����ʹ��\0��Ϊ�հ���� )
			while (nRecv > BYTES_PER_FRAME){	//������ݳ��ȴ���320 �ֽ�
				char* Qbuf = pthis->mallocOneFrame(BYTES_PER_FRAME);		//meshvoiceҵ������֡�Ĵ�СΪ320 byte
				if (nullptr == Qbuf) {
					break;
				}
				memcpy(Qbuf, buff, BYTES_PER_FRAME);			//����320����ÿ�ο���320
				pthis->recvlock();
				pthis->recvQ.push(Qbuf);
				pthis->recvunlock();
				nRecv -= BYTES_PER_FRAME;						//������BYTES_PER_FRAME�ֽ�
			}
			// С�ڵ���BYTES_PER_FRAME�ֽڵĲ���
			char* Qbuf = pthis->mallocOneFrame(BYTES_PER_FRAME);		//meshvoiceҵ������֡�Ĵ�СΪ320 byte
			if (nullptr == Qbuf) {
				continue;
			}
			memcpy(Qbuf, buff, nRecv);							//nRecv <= BYTES_PER_FRAME �ĳ���
			pthis->recvlock();
			pthis->recvQ.push(Qbuf);
			pthis->recvunlock();

		}else {
			int result = GetLastError();
			if (result == 10040) LOG(INFO) << "[RecvThread] received data is longer than UDP_MAX_LENGTH";
			continue;
		}

		if (true == pthis->recvoff) {	//�ر��߳�
			break;
		}
	}
	pthis->recvoff = false;
	LOG(INFO) << "[RecvThread]Recv Thread terminate success";
	return 0;
}

int udpInstance::appendToSendQ(char* address, DWORD size)
{
	if (sendQ.size() >= SEND_QUEUE_DEPTH || nullptr == address){	//���Ͷ�����ʱ���������������������
		return -1;
	}
	char* Qbuf = NULL;
	while (size > BYTES_PER_FRAME){
		Qbuf = mallocOneFrame(BYTES_PER_FRAME);	//meshvoiceҵ������֡�Ĵ�СΪ320 byte
		if (NULL == Qbuf) {						//mallocʧ�� �����߷��Ͷ�������
			return -1;
		}
		memcpy(Qbuf, address, BYTES_PER_FRAME);			//����320����ÿ�ο���320
		sendlock();
		sendQ.push(Qbuf); 
		sendunlock();
		size -= BYTES_PER_FRAME;						//������BYTES_PER_FRAME�ֽ�
	}
	Qbuf = mallocOneFrame(BYTES_PER_FRAME);
	if (NULL == Qbuf) {							//mallocʧ�� �����߷��Ͷ�������
		return -1;
	}
	memcpy(Qbuf, address, size);						//С�ڵ���320 ���򿽱�size���ֽ�
	sendlock();
	sendQ.push(Qbuf);
	sendunlock();
	return 0;
}

int udpInstance::readFromRecvQ(char* address,DWORD num)
{
	if (true == recvQ.empty()){
		return -1;
	}

	vector<char*> result;	//���ָ��
	char* tmp;

	//�Ӷ�����ȡ num ��Ԫ��(�ѵ�ַ)
	recvlock();
	for (int i = 0; i < num; i++){
		if (false == recvQ.empty()){
			char* tmp = recvQ.front();
			if (tmp){
				result.push_back(recvQ.front());	//�Ȱѵ�ַȡ����
			}
			recvQ.pop();						//�Ƴ�Ԫ��
		}
	}
	recvunlock();

	//��ȡ����num��Ԫ�ض�Ӧ�ѵ�ַ�ڵ���������д��address
	for (size_t i = 0; i < result.size(); i++)
	{
		tmp = result[i];
		memcpy(address + (i*BYTES_PER_FRAME), tmp, BYTES_PER_FRAME);
		free(tmp);
	}

	return 0;
}

char* udpInstance::mallocOneFrame(unsigned int size)
{
	if (RECV_QUEUE_DEPTH == recvQ.size()) {
		//LOG(INFO) << "[mallocOneFrame] ERROR , recvQ is FULL";
		return NULL;
	}
	char *ret = (char*)malloc(size);
	if (nullptr == ret)
	{
		//LOG(INFO) << "[mallocOneFrame] ERROR , malloc error";
		return NULL;
	}
	memset(ret, '\0', size);
	return ret;
}

void udpInstance::recvQpop()
{
	char* result = recvQ.front();
	if (result){
		free(result);
	}
	recvQ.pop();
}

void udpInstance::sendQpop()
{
	char* result = sendQ.front();
	if (result){
		free(result);
	}
	sendQ.pop();
}

void udpInstance::sig_recvQFull()
{
	for (vector<udpInstanceCustomer*>::iterator it = customerList.begin(); it != customerList.end(); it++){
		if (nullptr != *it){
			udpInstanceCustomer* tmp = *it;
			tmp->onRecvQFull();
		}
	}
}

void udpInstance::sig_sendQFull()
{
	for (vector<udpInstanceCustomer*>::iterator it = customerList.begin(); it != customerList.end(); it++){
		if (nullptr != *it){
			udpInstanceCustomer* tmp = *it;
			tmp->onSendQFull();
		}
	}
}

void udpInstance::addsubscriber(udpInstanceCustomer* subscriber)
{
	customerList.push_back(subscriber);
}

void udpInstance::clearsubscriber()
{
	customerList.clear();	//�ͷ�ָ�룬������
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
