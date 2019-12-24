#pragma once
#include "udpInstance.h"
#include "dsound.h"
#include <mmsystem.h>

#define AUDIOCAPTURE_EVENT_NUM 40		//�����ɼ�������ê������
#define AUDIOPLAY_EVENT_NUM 40		//�������Ż�����ê������
#define BYTES_PER_FRAME	320				//ÿ֡���ٸ����� (WAVE_SAMPLESPERSEC * WAVE_BITSPERSAMPLE * WAVE_CHANNELS * 20ms)
#define FRAMES_PER_READ_DS 1			//�����ɼ�ʱ��ÿ�δ�directsound�Ļ������ж���������֡���� �� �ⲿ������
#define FRAMES_PER_WRITE_DS 1			//��������ʱ��ÿ����directsound�Ļ�������д�����֡���Թ�directsound����

#define AUDIOPLAY_BUFFERSIZE AUDIOPLAY_EVENT_NUM*BYTES_PER_FRAME*FRAMES_PER_WRITE_DS	//���Ż������ߴ�
#define AUDIOCAPTURE_BUFFERSIZE AUDIOCAPTURE_EVENT_NUM*BYTES_PER_FRAME*FRAMES_PER_READ_DS	//�ɼ��������ߴ�
/* PCM ������ʽ��8k,16λ��������*/
#define WAVE_CHANNELS				1
#define WAVE_SAMPLESPERSEC			8000
#define WAVE_BITSPERSAMPLE			16
#define WAVE_CBSIZE					0

#define DIRECTSOUND_CAPTURECREATE_SUCESSED		TRUE
#define DIRECTSOUND_CAPTURECREATE_FAILED		FALSE
#define DIRECTSOUND_CAPTURECREATE_NODRIVER		(TRUE + FALSE + 1)

using namespace std;
class MeshVoice;
class udpInstanceCustomer;

class Locker
{
public:
	Locker(CRITICAL_SECTION &csLocker);
	~Locker();
private:
	CRITICAL_SECTION &m_csLocker;
};

class udpInstanceCustomerManager
{
public:
	static udpInstanceCustomerManager* getudpInstanceCustomerManager();
	MeshVoice* createMeshVoice();

private:
	udpInstanceCustomerManager();
	~udpInstanceCustomerManager();

private:
	static udpInstanceCustomerManager* _instance;
	vector<udpInstanceCustomer* > udpInstanceCustomerList;
};

class udpInstanceCustomer
{
public:
	udpInstanceCustomer();
	virtual ~udpInstanceCustomer();

protected:
	virtual int readData(char* address,DWORD size);				//�ӵ�ǰ�󶨵�udpInstance�Ľ��ն����ж�����
	virtual int writeData(char* address, DWORD size);		//��ǰ�󶨵�udpInstance�ķ��Ͷ�����д����

public:
	virtual int onRecvQFull()=0;							//���������߽����Լ�
	virtual int onSendQFull()=0;							//���������߽����Լ�

public:
    int bindTo(udpInstance* instance);				//��һ��udpInstance�������Խ��պͷ�������

protected:
	udpInstance* _udpInstance=NULL;

protected:
	bool selfChecking = false;
};

class MeshVoice :public udpInstanceCustomer
{
public:
	MeshVoice();
	virtual ~MeshVoice();

public:
	void init();

	void startCapture();		//��ʼ�����ɼ�
	void stopCapture();			//directsound��ͣ�����ɼ� && capture�̹߳���
	void terminateCapture();			//��ȫֹͣ�����ɼ�(������Դ��ֹͣ�߳�)
	bool setCaptureVolume(long value);	//������˷�Ĳɼ�����

	void startPlay();			//��ʼ��������
	void stopPlay();			//directsound��ͣ�������� && play�̹߳���
	void terminatePlay();		//��ȫֹͣ��������(������Դ��ֹͣ�߳�)
	bool setPlayVolume(long value);	//���ò�������
	long GetPlayVolume();		//��ȡ��ǰ�����������������ظ����棬�Է�����ʾ

private:
	bool createInputProxy();
	bool createOutputProxy(HWND * pWnd);

	virtual int startCaptureThread();									//�������������߳�
	void restartCaptureThread();
	static unsigned int __stdcall CaptureThread(void* pParam);	//���������̣߳������������豸������������һ������(����ҵ�������������)������udpinstance�ķ��Ͷ���

	virtual int startPlayThread();										//�������������߳�
	void restartPlayThread();
	static unsigned int __stdcall PlayThread(void* pParam);		//���������̣߳���udpinstance�Ľ��ն�����ȡ���ݣ���һ���Ĵ���(������Ϊ�ɲ��ŵ�����)����������������豸

	bool getCapturedSoundData(DWORD startPos, char *pBuffer, LPDWORD pOutSize);		//�ӻ�������ȡ����
	bool writeDirectSoundBuffer(LPDIRECTSOUNDBUFFER pDirectSoundDeviceBuffer, DWORD dwWritePosition, DWORD dwSize,BYTE *SoundData, DWORD dwFlag);	//�򲥷Ż�������д����
	bool createSoundCapturedEvent();							//������������֪ͨ�¼�
	bool createSoundPlayedEvent();							//������������֪ͨ�¼�

	bool setSoundTrack();

protected:
	using udpInstanceCustomer::readData;
	using udpInstanceCustomer::writeData;

public:
	int onRecvQFull();
	int onSendQFull();

public:
	UINT32 CaptureThreadID = 0;			//�����߳�id
	UINT32 PlayThreadID = 0;			//�����߳�id

private:
	LPDIRECTSOUNDCAPTURE _InputProxy;			//��Ƶ�������
	LPDIRECTSOUND _OutputProxy;					//��Ƶ�������
	LPDIRECTSOUNDCAPTUREBUFFER _InputBuffer=NULL;	//��Ƶ�ɼ�������
	LPDIRECTSOUNDBUFFER _OutputBuffer = NULL;	//��Ƶ�ɼ�������

	HANDLE h_CaptureThread=NULL;
	HANDLE h_PlayThread=NULL;
	HANDLE h_CaptureEvent[AUDIOCAPTURE_EVENT_NUM];
	HANDLE h_PlayEvent[AUDIOPLAY_EVENT_NUM];

	DWORD m_dwWriteCursor;		//��¼���Ż������ĵ�ǰ��д���λ�ã�ÿ��д�����ݶ�Ӧ���Դ�Ϊ��ʼ��

	HWND * m_pWnd;	//���Ŵ���

	bool captureoff=true;
	bool playoff=true;
};