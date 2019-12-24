#pragma once
#include "udpInstance.h"
#include "dsound.h"
#include <mmsystem.h>

#define AUDIOCAPTURE_EVENT_NUM 40		//语音采集缓冲区锚点数量
#define AUDIOPLAY_EVENT_NUM 40		//语音播放缓冲区锚点数量
#define BYTES_PER_FRAME	320				//每帧多少个比特 (WAVE_SAMPLESPERSEC * WAVE_BITSPERSAMPLE * WAVE_CHANNELS * 20ms)
#define FRAMES_PER_READ_DS 1			//语音采集时，每次从directsound的缓冲区中读出来多少帧数据 到 外部缓存中
#define FRAMES_PER_WRITE_DS 1			//语音播放时，每次向directsound的缓冲区中写入多少帧，以供directsound播放

#define AUDIOPLAY_BUFFERSIZE AUDIOPLAY_EVENT_NUM*BYTES_PER_FRAME*FRAMES_PER_WRITE_DS	//播放缓冲区尺寸
#define AUDIOCAPTURE_BUFFERSIZE AUDIOCAPTURE_EVENT_NUM*BYTES_PER_FRAME*FRAMES_PER_READ_DS	//采集缓冲区尺寸
/* PCM 声音格式：8k,16位，单声道*/
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
	virtual int readData(char* address,DWORD size);				//从当前绑定的udpInstance的接收队列中读数据
	virtual int writeData(char* address, DWORD size);		//向当前绑定的udpInstance的发送队列中写数据

public:
	virtual int onRecvQFull()=0;							//各个消费者进行自检
	virtual int onSendQFull()=0;							//各个消费者进行自检

public:
    int bindTo(udpInstance* instance);				//与一个udpInstance关联，以接收和发送数据

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

	void startCapture();		//开始语音采集
	void stopCapture();			//directsound暂停语音采集 && capture线程挂起
	void terminateCapture();			//完全停止语音采集(清理资源、停止线程)
	bool setCaptureVolume(long value);	//设置麦克风的采集音量

	void startPlay();			//开始语音播放
	void stopPlay();			//directsound暂停语音播放 && play线程挂起
	void terminatePlay();		//完全停止语音播放(清理资源、停止线程)
	bool setPlayVolume(long value);	//设置播放音量
	long GetPlayVolume();		//获取当前播放音量，用来返回给界面，以方便显示

private:
	bool createInputProxy();
	bool createOutputProxy(HWND * pWnd);

	virtual int startCaptureThread();									//启动语音捕获线程
	void restartCaptureThread();
	static unsigned int __stdcall CaptureThread(void* pParam);	//语音捕获线程，持续从输入设备捕获语音，做一定处理(按照业务需求进行整理)，丢入udpinstance的发送队列

	virtual int startPlayThread();										//启动语音播放线程
	void restartPlayThread();
	static unsigned int __stdcall PlayThread(void* pParam);		//语音播放线程，从udpinstance的接收队列中取数据，做一定的处理(规整化为可播放的数据)，输出语音到播放设备

	bool getCapturedSoundData(DWORD startPos, char *pBuffer, LPDWORD pOutSize);		//从缓冲区中取数据
	bool writeDirectSoundBuffer(LPDIRECTSOUNDBUFFER pDirectSoundDeviceBuffer, DWORD dwWritePosition, DWORD dwSize,BYTE *SoundData, DWORD dwFlag);	//向播放缓冲区中写数据
	bool createSoundCapturedEvent();							//创建语音捕获通知事件
	bool createSoundPlayedEvent();							//创建语音捕获通知事件

	bool setSoundTrack();

protected:
	using udpInstanceCustomer::readData;
	using udpInstanceCustomer::writeData;

public:
	int onRecvQFull();
	int onSendQFull();

public:
	UINT32 CaptureThreadID = 0;			//发送线程id
	UINT32 PlayThreadID = 0;			//接收线程id

private:
	LPDIRECTSOUNDCAPTURE _InputProxy;			//音频输入代理
	LPDIRECTSOUND _OutputProxy;					//音频输出代理
	LPDIRECTSOUNDCAPTUREBUFFER _InputBuffer=NULL;	//音频采集缓冲区
	LPDIRECTSOUNDBUFFER _OutputBuffer = NULL;	//音频采集缓冲区

	HANDLE h_CaptureThread=NULL;
	HANDLE h_PlayThread=NULL;
	HANDLE h_CaptureEvent[AUDIOCAPTURE_EVENT_NUM];
	HANDLE h_PlayEvent[AUDIOPLAY_EVENT_NUM];

	DWORD m_dwWriteCursor;		//记录播放缓冲区的当前可写入的位置，每次写入数据都应当以此为起始点

	HWND * m_pWnd;	//播放窗口

	bool captureoff=true;
	bool playoff=true;
};