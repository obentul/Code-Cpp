#pragma once


#include <Windows.h>
#include <string>
#include <queue>


#define CONNECT_RETRY_TIMES 3		//重连次数
#define SEND_LIST_DEPTH	50			//发送环形队列深度为50，超过长度提示缓存满，消费者(socket持续发送失败)卡死会导致缓存满
#define RECV_LIST_DEPTH	50			//发送环形队列深度为50，超过长度提示缓存满，消费者(音频播放线程不再处理数据)卡死会导致缓存满

using namespace std;

class udpInstance
{
public:
	udpInstance();
	virtual ~udpInstance();
	udpInstance& operator=(const udpInstance&)=delete;		//不允许赋值
	udpInstance(const udpInstance&)=delete;			//不允许拷贝

public:
	int udpstart(int,int,string,string);	//初始化socket的参数,并尝试链接服务端
	int udpsend(string data);						//写入发送队列，由发送队列来处理
	int udprecv(string data);						//从接收队列中读数据
	int udpsend(char* data);						//写数据
	int udprecv(char* data);						//读数据
	bool isReady()const;							//是否准备就绪
	string usage()const;							//当前udp的用途

private:
	int startSendThread();		//启动发送线程，持续从发送队列中取数据，然后通过sock_fd向外发送
	int startRecvThread();		//启动接收线程，持续从发送队列中取数据，然后通过sock_fd向外发送
	int stopSendThread();		//停止发送线程
	int stopRecvThread();		//停止接收线程
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
	string dst_addr;	//目的  ip
	string src_addr;	//源	ip
	int sock_fd;		//socket描述符

	/*实例属性*/
	string m_usage = "common-use";				//当前udp实例的用途
	bool m_isReady;				//当前socket是否可用

	/*数据存储*/
	queue<string> sendQ;			//数据接收队列
	queue<string> recvQ;			//数据发送队列

	/*队列上锁，发送 和 接收线程都在不停地操作队列，数据处理线程通过udpsend 和 udprecv不停地操作队列*/
	HANDLE send_mutex;
	HANDLE recv_mutex;
};


