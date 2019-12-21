#pragma once


#include <Windows.h>
#include <string>
#include <queue>
#include <process.h>
#include <vector>

#define CONNECT_RETRY_TIMES 3		//��������
#define SEND_QUEUE_DEPTH	50		//���ͻ��ζ������Ϊ50������������ʾ������(����udp�ĵ���δ����320һ�������ʶ�����ȿ���ʱ����)��������(socket��������ʧ��)�����ᵼ�»�����
#define RECV_QUEUE_DEPTH	100		//���ͻ��ζ������Ϊ50������������ʾ������(����udp�ĵ���δ����320һ�������ʶ�����ȿ���ʱ����)��������(��Ƶ�����̲߳��ٴ�������)�����ᵼ�»�����
#define UDP_MAX_LENGTH 1400			//һ�����ݰ�����󳤶ȣ����������ַ�Ƭ��һ����Ƭ��ʧ�������������ݰ��Ķ�����������Ӳ��ȶ��ԣ��շ�˫����Ӧ�������Լ���������
#define FRAMES_PER_TRANS 1				//dup������ÿ�δ������֡ �� Ŀǰ������1֡��������ʱ��ʹ��

#define EMIT

using namespace std;

class udpInstance;
class udpInstanceCustomer;

class udpInstanceManager{

public:
	static udpInstanceManager* getudpInstanceManager();
	udpInstance* createudpInstance();

private:
	udpInstanceManager();
	~udpInstanceManager();

private:
	static udpInstanceManager* _instance;
	vector<udpInstance*> udpInstanceList;
};

class udpInstance
{
public:
	udpInstance();
	virtual ~udpInstance();
	udpInstance& operator=(const udpInstance&)=delete;		//������ֵ
	udpInstance(const udpInstance&)=delete;			//��������

public:
	int udpinit(int dstport,int srcport,string dstip,string srcip);	//��ʼ��socket�Ĳ���,���������ӷ����
	int udpinit(int dstport, string dstip);	//��ʼ��socket�Ĳ���,���������ӷ����

	bool isReady()const {return m_isReady;}
	string usage()const {return m_usage;}

	int appendToSendQ(char* address, DWORD size);					//�����ṩ�ӿڣ����Ͷ��������һ������
	int readFromRecvQ(char* address, DWORD size);							//�����ṩ�ӿڣ��ӽ��ն�����ȡһ�����ݷ���

	void sig_recvQFull();		//���ն�����ʱ�������ź�
	void sig_sendQFull();		//���Ͷ�����ʱ�������ź�

	void addsubscriber(udpInstanceCustomer* subscriber);		//���Ӽ�����
	void clearsubscriber();		//��ռ�����(removeָ��������Ŀǰ�����ã������ṩ��ղ���)

private:
	int startSendThread();		//���������̣߳������ӷ��Ͷ�����ȡ���ݣ�Ȼ��ͨ��sock_fd���ⷢ��
	int startRecvThread();		//���������̣߳������ӷ��Ͷ�����ȡ���ݣ�Ȼ��ͨ��sock_fd���ⷢ��
	int stopSendThread();		//ֹͣ�����߳�
	int stopRecvThread();		//ֹͣ�����߳�
	static unsigned int __stdcall SendThread(void* pParam);			//�����߳�ִ����
	static unsigned int __stdcall RecvThread(void* pParam);			//�����߳�ִ����

	void writeToSendQueue(string data);		//���Ͷ���������һ������
	void readFromRecvQueue(string data);		//�ӽ��ն�����ȡһ������
	void sendlock();				//�������Ͷ���
	void sendunlock();				//�������Ͷ���
	void recvlock();				//�������ն���
	void recvunlock();				//�������ն���

	char* mallocOneFrame(unsigned int size);		//���Ϸ���һ��size���Ѿ���ʼ�����ڴ�
	void recvQpop();								//�Ӷ���ͷ�Ƴ�һ��Ԫ�أ����ڶ����д�ŵ�Ϊ�ѵ�ַָ�룬����Ҫͬʱ�ͷŶ��ڴ�
	void sendQpop();								//�Ӷ���ͷ�Ƴ�һ��Ԫ�أ����ڶ����д�ŵ�Ϊ�ѵ�ַָ�룬����Ҫͬʱ�ͷŶ��ڴ�

private:
	/*ͨѶ����*/
	int dst_port;		//Ŀ��port
	int src_port;		//Դ	port
	string dst_ip;		//Ŀ��ip
	string src_ip;		//Դ	ip
	sockaddr_in dst_addr;
	sockaddr_in src_addr;
	SOCKET sock_fd;		//socket������

	/*ʵ������*/
	string m_usage = "common-use";				//��ǰudpʵ������;
	bool m_isReady = false;				//��ǰsocket�Ƿ����
	UINT32 sendThreadID = 0;			//�����߳�id
	UINT32 recvThreadID = 0;			//�����߳�id

	/*���ݴ洢*/
	queue<char*> sendQ;			//���ݽ��ն���
	queue<char*> recvQ;			//���ݷ��Ͷ���

	/*�������������� �� �����̶߳��ڲ�ͣ�ز������У����ݴ����߳�ͨ��udpsend �� udprecv��ͣ�ز�������*/
	HANDLE sendqueue_mutex=NULL;
	HANDLE recvqueue_mutex=NULL;

	/*�߳���(�ݲ�����)*/
	HANDLE sendthread_mutex = NULL;
	HANDLE recvthread_mutex = NULL;

	/*�߳�flag*/
	bool sendoff=false;		//true - �رշ����߳�
	bool recvoff=false;		//true - �رս����߳�

	/*���������¼�����������*/
	vector<udpInstanceCustomer*> customerList;
};

