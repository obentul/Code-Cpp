#pragma once


#include <Windows.h>
#include <string>
#include <queue>
#include <process.h>

#define CONNECT_RETRY_TIMES 3		//��������
#define SEND_QUEUE_DEPTH	50			//���ͻ��ζ������Ϊ50������������ʾ��������������(socket��������ʧ��)�����ᵼ�»�����
#define RECV_QUEUE_DEPTH	50			//���ͻ��ζ������Ϊ50������������ʾ��������������(��Ƶ�����̲߳��ٴ�������)�����ᵼ�»�����
#define UDP_MAX_LENGTH 1400			//һ�����ݰ�����󳤶ȣ����������ַ�Ƭ��һ����Ƭ��ʧ�������������ݰ��Ķ�����������Ӳ��ȶ��ԣ��շ�˫����Ӧ�������Լ���������

using namespace std;

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

	bool isReady()const {//�Ƿ�׼������
		return m_isReady;
	}
	string usage()const {//��ǰudp����;
		return m_usage;
	}
	void appendToSendQ(string arg);					//�����ṩ�ӿڣ����Ͷ��������һ������
	string readFromRecvQ();							//�����ṩ�ӿڣ��ӽ��ն�����ȡһ�����ݷ���

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

private:
	/*ͨѶ����*/
	int dst_port;		//Ŀ��  port
	int src_port;		//Դ	port
	string dst_ip;	//Ŀ��  ip
	string src_ip;	//Դ	ip
	sockaddr_in dst_addr;
	sockaddr_in src_addr;
	SOCKET sock_fd;		//socket������

	/*ʵ������*/
	string m_usage = "common-use";				//��ǰudpʵ������;
	bool m_isReady = false;				//��ǰsocket�Ƿ����

	/*���ݴ洢*/
	queue<string> sendQ;			//���ݽ��ն���
	queue<string> recvQ;			//���ݷ��Ͷ���

	/*�������������� �� �����̶߳��ڲ�ͣ�ز������У����ݴ����߳�ͨ��udpsend �� udprecv��ͣ�ز�������*/
	HANDLE sendqueue_mutex=NULL;
	HANDLE recvqueue_mutex=NULL;

	/*�߳���*/
	HANDLE sendthread_mutex = NULL;
	HANDLE recvthread_mutex = NULL;

	/*�߳�flag*/
	bool sendoff=false;
	bool recvoff=false;
};

