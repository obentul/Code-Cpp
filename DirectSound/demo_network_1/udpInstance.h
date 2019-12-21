#pragma once


#include <Windows.h>
#include <string>
#include <queue>
#include <process.h>

#define CONNECT_RETRY_TIMES 3		//重连次数
#define SEND_QUEUE_DEPTH	50			//发送环形队列深度为50，超过长度提示缓存满，消费者(socket持续发送失败)卡死会导致缓存满
#define RECV_QUEUE_DEPTH	50			//发送环形队列深度为50，超过长度提示缓存满，消费者(音频播放线程不再处理数据)卡死会导致缓存满
#define UDP_MAX_LENGTH 1400			//一个数据包的最大长度，超过则会出现分片，一个分片丢失便会造成整个数据包的丢弃，这会增加不稳定性，收发双方都应当限制自己的数据量

using namespace std;

class udpInstance
{
public:
	udpInstance();
	virtual ~udpInstance();
	udpInstance& operator=(const udpInstance&)=delete;		//不允许赋值
	udpInstance(const udpInstance&)=delete;			//不允许拷贝

public:
	int udpinit(int dstport,int srcport,string dstip,string srcip);	//初始化socket的参数,并尝试链接服务端
	int udpinit(int dstport, string dstip);	//初始化socket的参数,并尝试链接服务端

	bool isReady()const {//是否准备就绪
		return m_isReady;
	}
	string usage()const {//当前udp的用途
		return m_usage;
	}
	void appendToSendQ(string arg);					//对外提供接口，向发送队列中添加一条数据
	string readFromRecvQ();							//对外提供接口，从接收队列中取一条数据返回

private:
	int startSendThread();		//启动发送线程，持续从发送队列中取数据，然后通过sock_fd向外发送
	int startRecvThread();		//启动接收线程，持续从发送队列中取数据，然后通过sock_fd向外发送
	int stopSendThread();		//停止发送线程
	int stopRecvThread();		//停止接收线程
	static unsigned int __stdcall SendThread(void* pParam);			//发送线程执行体
	static unsigned int __stdcall RecvThread(void* pParam);			//接收线程执行体

	void writeToSendQueue(string data);		//向发送队列中增加一条数据
	void readFromRecvQueue(string data);		//从接收队列中取一条数据
	void sendlock();				//加锁发送队列
	void sendunlock();				//解锁发送队列
	void recvlock();				//加锁接收队列
	void recvunlock();				//解锁接收队列

private:
	/*通讯参数*/
	int dst_port;		//目的  port
	int src_port;		//源	port
	string dst_ip;	//目的  ip
	string src_ip;	//源	ip
	sockaddr_in dst_addr;
	sockaddr_in src_addr;
	SOCKET sock_fd;		//socket描述符

	/*实例属性*/
	string m_usage = "common-use";				//当前udp实例的用途
	bool m_isReady = false;				//当前socket是否可用

	/*数据存储*/
	queue<string> sendQ;			//数据接收队列
	queue<string> recvQ;			//数据发送队列

	/*队列上锁，发送 和 接收线程都在不停地操作队列，数据处理线程通过udpsend 和 udprecv不停地操作队列*/
	HANDLE sendqueue_mutex=NULL;
	HANDLE recvqueue_mutex=NULL;

	/*线程锁*/
	HANDLE sendthread_mutex = NULL;
	HANDLE recvthread_mutex = NULL;

	/*线程flag*/
	bool sendoff=false;
	bool recvoff=false;
};

