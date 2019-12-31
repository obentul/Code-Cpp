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
	/*初始化队列*/
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

//如果主线程delete udpinstance，析构函数的执行时间应该是固定的，否则会导致主线程卡死
udpInstance::~udpInstance()
{
	/*关闭线程(如果发送和接收线程执行的较快，则此标志位有用，否则没用)*/
	sendoff = true;
	recvoff = true;

	/* 结束接收线程 */
	if (h_RecvThread != NULL)
	{
		int ret = TerminateThread(h_RecvThread, 0);
		if (0 == ret) LOG(INFO) << "TerminateThread [RecvThread] ERROR";
		CloseHandle(h_RecvThread);
		h_RecvThread = NULL;
	}

	/* 结束发送线程 */
	if (h_SendThread != NULL)
	{
		int ret = TerminateThread(h_SendThread, 0);
		if (0 == ret) LOG(INFO) << "TerminateThread [SendThread] ERROR";
		CloseHandle(h_SendThread);
		h_SendThread = NULL;
	}

	/*清空 发送队列 和 接收队列*/
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
	LOG(INFO) << "[CLEAR recvQ ] recvQ size is -> " << recvQsize;
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

	/*销毁互斥量 4个HANDLE*/
	::CloseHandle(sendqueue_mutex);
	sendqueue_mutex = NULL;
	::CloseHandle(recvqueue_mutex);
	recvqueue_mutex = NULL;
	::CloseHandle(sendthread_mutex);
	sendthread_mutex = NULL;
	::CloseHandle(recvthread_mutex);
	recvthread_mutex = NULL;

	/*清空订阅队列*/
	for (size_t i = 0; i < customerList.size(); i++){
		udpInstanceCustomer* tmp = customerList[i];
		tmp = NULL;
	}
}

int udpInstance::udpinit(int dstport, int srcport, string dstip, string srcip)
{
	LOG(INFO) << "[udpinit] start";
	m_isReady = false;	//udp实例默认不可用

	/*0.参数落地*/
	dst_ip = dstip;
	src_ip = srcip;
	dst_port = dstport;
	src_port = srcport;

	/*1.创建socket*/
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(dst_port);
	dst_addr.sin_addr.s_addr = inet_addr(dst_ip.c_str());
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sock_fd) {
		LOG(INFO) << "[udpinit]create socket error , code is "<<GetLastError();
		return -1;
	}


	/*绑定本地端口*/
	/*不绑定的话，会导致接收返回10022 和 10040，但是在发送udp包后不再报错，目前还没有确定作为客户端应当使用的端口，所以暂且不绑定固定端口*/
	src_addr.sin_family = AF_INET;
	src_addr.sin_port = htons(src_port);
	src_addr.sin_addr.s_addr = inet_addr(src_ip.c_str());
	int ret = ::bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));


	/*2.(UDP不需要connect)尝试CONNECT_RETRY_TIMES 次连接服务器*/

	/*3.使启动发送线程*/
	if (0 != startSendThread()) {
		LOG(INFO) << "[udpinit] start udp send thread error";
		return -1;
	}
	LOG(INFO) << "[udpinit] startSendThread success";

	/*4.使启动接收线程*/
	if (0 != startRecvThread()) {
		LOG(INFO) << "[udpinit] start udp recv thread error";
		return -1;
	}
	LOG(INFO) << "[udpinit] startRecvThread success";

	m_isReady = true;		//udp初始化完毕，同时收发线程启动完毕，进入可用状态

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

	/*0.参数落地*/
	dst_ip = dstip;
	dst_port = dstport;

	/*1.创建socket*/
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(dst_port);
	dst_addr.sin_addr.s_addr = inet_addr(dst_ip.c_str());
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sock_fd) {
		LOG(INFO) << "[udpinit] create socket error , code is " << GetLastError();
		return -1;
	}

	/*2.(UDP不需要connect)尝试CONNECT_RETRY_TIMES 次连接服务器*/

	/*3.使启动发送线程*/
	if (0 != startSendThread()) {
		LOG(INFO) << "[udpinit] start udp send thread error";
		return -1;
	}
	LOG(INFO) << "[udpinit] startSendThread success";

	/*4.使启动接收线程*/
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
	pthis->sendoff = false;		//线程已经启动，通知udpInstance配置标志位

	while (1) {
		Sleep(20);					//20ms发一次

		/*如果队列为空，则继续判断*/
		if (true == pthis->sendQ.empty() || 0 == pthis->sendQ.size()) {
			Sleep(20);				//队列为空，则额外sleep 20ms
			continue;
		}

		char tmpbuff[BYTES_PER_FRAME];
		memset(tmpbuff, '\0', BYTES_PER_FRAME);
		/*如果队列不为空*/
		pthis->sendlock();
		char* tmpsend = pthis->sendQ.front();		//取首元素
		memcpy(tmpbuff, tmpsend, BYTES_PER_FRAME);	//暂存首元素内容
		pthis->sendQpop();							//删除首元素
		pthis->sendunlock();

		/*数据异常，继续轮询*/
		if (nullptr == tmpbuff) continue;

		/*数据发送*/
		int nSend = sendto(pthis->sock_fd, tmpbuff, BYTES_PER_FRAME, 0, (struct sockaddr*)&pthis->dst_addr, sizeof(pthis->dst_addr));
		if (nSend > 0) 
		{
			//LOG(INFO) << "[SendThread] sendto SUCCESS " << tmpbuff << " | NUM IS -> " << nSend;
		}
		else
		{
			//LOG(INFO) << "[SendThread] sendto ERROR ,  CODE is ->" << GetLastError();
		}

		/*关闭线程*/
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
	pthis->recvoff = false;		//线程已经启动，通知udpInstance配置标志位

	//对端地址验证
	sockaddr_in dstaddr;
	char buff[UDP_MAX_LENGTH];

	while (1) {
		/*如果队列为满，进入消费者自检流程*/
		if (RECV_QUEUE_DEPTH == pthis->recvQ.size()) {
			//LOG(INFO) << "[RecvThread] recv queue is alredy full, num is -> " << pthis->recvQ.size();
			Sleep(10);
			continue;
		}

		//数据落地缓冲区
		memset(buff, '\0', sizeof(buff));
		memset(&dstaddr, '\0', sizeof(dstaddr));
		int dstaddrlen = sizeof(dstaddr);

		//这里会阻塞等待
		int nRecv = recvfrom(pthis->sock_fd, buff, UDP_MAX_LENGTH, 0, (sockaddr*)&dstaddr, &dstaddrlen);
		if (nRecv > 0) {
			//数据压入接收队列(如果收到的数据 大于 BYTES_PER_FRAME ，则在队列中分配多个段，保证每一段都是320字节，如果分割到最后 不足 BYTES_PER_FRAME，则使用\0作为空白填充 )
			while (nRecv > BYTES_PER_FRAME){	//如果数据长度大于320 字节
				char* Qbuf = pthis->mallocOneFrame(BYTES_PER_FRAME);		//meshvoice业务，数据帧的大小为320 byte
				if (nullptr == Qbuf) {
					break;
				}
				memcpy(Qbuf, buff, BYTES_PER_FRAME);			//大于320，则每次拷贝320
				pthis->recvlock();
				pthis->recvQ.push(Qbuf);
				pthis->recvunlock();
				nRecv -= BYTES_PER_FRAME;						//已消化BYTES_PER_FRAME字节
			}
			// 小于等于BYTES_PER_FRAME字节的部分
			char* Qbuf = pthis->mallocOneFrame(BYTES_PER_FRAME);		//meshvoice业务，数据帧的大小为320 byte
			if (nullptr == Qbuf) {
				continue;
			}
			memcpy(Qbuf, buff, nRecv);							//nRecv <= BYTES_PER_FRAME 的场景
			pthis->recvlock();
			pthis->recvQ.push(Qbuf);
			pthis->recvunlock();

		}else {
			int result = GetLastError();
			LOG(INFO) << "[RecvThread] ERROR , CODE IS -> " << result;
			if (result == 10040) LOG(INFO) << "[RecvThread] received data is longer than UDP_MAX_LENGTH";
			continue;
		}

		if (true == pthis->recvoff) {	//关闭线程
			break;
		}
	}
	pthis->recvoff = false;
	LOG(INFO) << "[RecvThread]Recv Thread terminate success";
	return 0;
}

int udpInstance::appendToSendQ(char* address, DWORD size)
{
	if (sendQ.size() >= SEND_QUEUE_DEPTH || nullptr == address){	//发送队列满时，不再向队列中增加数据
		return -1;
	}
	char* Qbuf = NULL;
	while (size > BYTES_PER_FRAME){
		Qbuf = mallocOneFrame(BYTES_PER_FRAME);	//meshvoice业务，数据帧的大小为320 byte
		if (NULL == Qbuf) {						//malloc失败 ，或者发送队列已满
			return -1;
		}
		memcpy(Qbuf, address, BYTES_PER_FRAME);			//大于320，则每次拷贝320
		sendlock();
		sendQ.push(Qbuf); 
		sendunlock();
		size -= BYTES_PER_FRAME;						//已消化BYTES_PER_FRAME字节
	}
	Qbuf = mallocOneFrame(BYTES_PER_FRAME);
	if (NULL == Qbuf) {							//malloc失败 ，或者发送队列已满
		return -1;
	}
	memcpy(Qbuf, address, size);						//小于等于320 ，则拷贝size个字节
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

	vector<char*> result;	//存放指针

	//从队列中取 num 个元素(堆地址)
	recvlock();
	for (int i = 0; i < num; i++){
		if (false == recvQ.empty()){
			char* tmp = recvQ.front();
			if (tmp){
				result.push_back(recvQ.front());	//先把地址取出来
			}
			recvQ.pop();						//移除元素(这里先取出所有地址，然后把地址从队列中删除，实际上内存还没被释放)
		}
	}
	recvunlock();

	//把取出的num个元素对应堆地址内的内容依次写入address
	for (size_t i = 0; i < result.size(); i++)
	{
		memcpy(address + (i*BYTES_PER_FRAME), result[i], BYTES_PER_FRAME);
		free(result[i]);								//移除元素(把数据拷贝出来后，释放对应内存位置的数据)
	}

	return 0;
}

char* udpInstance::mallocOneFrame(unsigned int size)
{
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
	customerList.clear();	//释放指针，不析构
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
