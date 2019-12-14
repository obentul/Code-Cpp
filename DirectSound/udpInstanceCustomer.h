#pragma once
#include "udpInstance.h"
#include "dsound.h"
#include <mmsystem.h>

#define AUDIOCAPTURE_EVENT_NUM 40		//�����ɼ�������ê������
#define AUDIOPLAY_EVENT_NUM 40		//�������Ż�����ê������
#define BYTES_PER_FRAME	320				//ÿ֡���ٸ�����
#define AUDIOPLAY_BUFFERSIZE AUDIOPLAY_EVENT_NUM*BYTES_PER_FRAME	//���Ż������ߴ� 320*40 = 12800�ֽ�
#define AUDIOCAPTURE_BUFFERSIZE AUDIOCAPTURE_EVENT_NUM*BYTES_PER_FRAME	//�ɼ��������ߴ� 320*40 = 12800�ֽ�
#define FRAMES_PER_TRANS 1				//ÿ�δ������֡
#define FRAMES_PER_READ 1				//�����ɼ�ʱ��ÿ�δӸ���������ȡ����֡���ݴ���
#define DIRECTSOUND_CAPTURECREATE_SUCESSED		TRUE
#define DIRECTSOUND_CAPTURECREATE_FAILED		FALSE
#define DIRECTSOUND_CAPTURECREATE_NODRIVER		(TRUE + FALSE + 1)

/* PCM ������ʽ��8k,16λ��������*/
#define WAVE_CHANNELS				1
#define WAVE_SAMPLESPERSEC			8000
//#define WAVE_AVGBYTESPERSEC			16000
#define WAVE_BLOCKALIGN				2
#define WAVE_BITSPERSAMPLE			16
#define WAVE_CBSIZE					0

#define DSBVOLUME_MIN               -10000
#define DSBVOLUME_MAX               0

using namespace std;

class Locker
{
public:
	Locker(CRITICAL_SECTION &csLocker);
	~Locker();
private:
	CRITICAL_SECTION &m_csLocker;
};

class udpInstanceCustomer
{
public:
	udpInstanceCustomer();
	virtual ~udpInstanceCustomer();

protected:
	virtual string readData();				//�ӵ�ǰ�󶨵�udpInstance�Ľ��ն����ж�����
	virtual int writeData(string data);		//��ǰ�󶨵�udpInstance�ķ��Ͷ�����д����

public:
    int bindTo(udpInstance*);				//��һ��udpInstance�������Խ��պͷ�������

private:
	udpInstance* _udpInstance=NULL;
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
	void setCaptureVolume(long value);	//������˷�Ĳɼ�����

	void startPlay(DWORD dwStartPosition, BOOL bLoop);			//��ʼ��������
	void stopPlay();			//directsound��ͣ�������� && play�̹߳���
	void terminatePlay();		//��ȫֹͣ��������(������Դ��ֹͣ�߳�)
	bool setPlayVolume(long value);	//���ò�������
	long GetPlayVolume();		//��ȡ��ǰ�����������������ظ����棬�Է�����ʾ

private:
	bool createInputProxy();
	bool createOutputProxy(HWND * pWnd);

	virtual int startCaptureThread();									//�������������߳�
	static unsigned int __stdcall CaptureThread(void* pParam);	//���������̣߳������������豸������������һ������(����ҵ�������������)������udpinstance�ķ��Ͷ���

	virtual int startPlayThread();										//�������������߳�
	static unsigned int __stdcall PlayThread(void* pParam);		//���������̣߳���udpinstance�Ľ��ն�����ȡ���ݣ���һ���Ĵ���(������Ϊ�ɲ��ŵ�����)����������������豸

	bool getCapturedSoundData(DWORD startPos, char *pBuffer, LPDWORD pOutSize);		//�ӻ�������ȡ����
	bool writeDirectSoundBuffer(LPDIRECTSOUNDBUFFER pDirectSoundDeviceBuffer, DWORD dwWritePosition, DWORD dwSize, string SoundData, DWORD dwFlag);	//�򲥷Ż�������д����
	bool createSoundCapturedEvent();							//������������֪ͨ�¼�
	bool createSoundPlayedEvent();							//������������֪ͨ�¼�
	
	bool setSoundTrack();

	bool SetSoundData(string data);							//�����ݶ���д��������������,�����漰��һЩ���ӵ�ҵ���߼����Ƕ�writeDirectSoundBuffer�İ�װ

protected:
	using udpInstanceCustomer::readData;
	using udpInstanceCustomer::writeData;

private:
	LPDIRECTSOUNDCAPTURE _InputProxy;			//��Ƶ�������
	LPDIRECTSOUND _OutputProxy;					//��Ƶ�������

	LPDIRECTSOUNDCAPTUREBUFFER _InputBuffer=NULL;	//��Ƶ�ɼ�������
	LPDIRECTSOUNDBUFFER _OutputBuffer = NULL;	//��Ƶ�ɼ�������

	HANDLE h_CaptureThread=NULL;
	HANDLE h_PlayThread=NULL;
	HANDLE h_CaptureEvent[AUDIOCAPTURE_EVENT_NUM];
	HANDLE h_PlayEvent[AUDIOPLAY_EVENT_NUM];


	DWORD m_dwWritePosition;		//��¼���Ż������ĵ�ǰ��д���λ�ã�ÿ��д�����ݶ�Ӧ���Դ�Ϊ��ʼ��
};