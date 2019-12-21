#include "udpInstanceCustomer.h"
#include "trace.h"

extern HWND *g_soundWnd;
extern HWND *g_soundWnd_1;
MeshVoice* g_MeshVoice = NULL;


udpInstanceCustomerManager* udpInstanceCustomerManager::_instance = nullptr;

udpInstanceCustomerManager::udpInstanceCustomerManager()
{

}

udpInstanceCustomerManager* udpInstanceCustomerManager::getudpInstanceCustomerManager()
{
	if (nullptr == _instance){
		return new udpInstanceCustomerManager();
	}
	return _instance;
}

udpInstanceCustomerManager::~udpInstanceCustomerManager()
{
	for (vector<udpInstanceCustomer*>::iterator it = udpInstanceCustomerList.begin(); it != udpInstanceCustomerList.end(); it++)
	{
		udpInstanceCustomer* tmp = *it;
		if (nullptr != tmp){
			delete tmp;
			tmp = nullptr;
		}
	}
}

MeshVoice* udpInstanceCustomerManager::createMeshVoice()
{
	MeshVoice* tmp = new MeshVoice();
	if (nullptr != tmp){
		udpInstanceCustomerList.push_back(tmp);
		return tmp;
	}
	return nullptr;
}

Locker::Locker(CRITICAL_SECTION& csLocker)
:m_csLocker(csLocker)
{
	EnterCriticalSection(&m_csLocker);
}

Locker::~Locker()
{
	LeaveCriticalSection(&m_csLocker);
}

udpInstanceCustomer::udpInstanceCustomer()
{
}

udpInstanceCustomer::~udpInstanceCustomer()
{
}

int udpInstanceCustomer::bindTo(udpInstance* instance)
{
	if (NULL != instance){
		_udpInstance = instance;
		instance->addsubscriber(this);
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////mesh///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
MeshVoice::~MeshVoice()
{
	try
	{
		if (_InputBuffer != NULL)
			_InputBuffer->Release();
		if (_InputProxy != NULL)
			_InputProxy->Release();
		if (_OutputBuffer != NULL)
			_OutputBuffer->Release();
		if (_OutputProxy != NULL)
			_OutputProxy->Release();

		/* ���������ɼ��߳� */
		if (h_CaptureThread != NULL)
		{
			TerminateThread(h_CaptureThread, 0);
			CloseHandle(h_CaptureThread);
			h_CaptureThread = NULL;
		}

		/* �������������߳� */
		if (h_PlayThread != NULL)
		{
			TerminateThread(h_PlayThread, 0);
			CloseHandle(h_PlayThread);
			h_PlayThread = NULL;
		}
	}
	catch (...)
	{
		LOG(INFO) << "Exception in ~AudioProc()";
	}
}

MeshVoice::MeshVoice()
{
	m_dwWriteCursor = BYTES_PER_FRAME;		//�ӵڶ���ê�㿪ʼ
}

void MeshVoice::init()
{
	//1.����dsound����ʵ��
	createInputProxy();
	startCapture();

	//2.����dsound����ʵ��
	createOutputProxy(g_soundWnd_1);

	//3.���ò���
	setPlayVolume(-5000);

	//4.���������߳� �� �����߳�
	startCaptureThread();
	startPlayThread();
}

void MeshVoice::startCapture()
{
	if (_InputBuffer){
		_InputBuffer->Start(DSCBSTART_LOOPING);//��������
	}
}

void MeshVoice::stopCapture()
{
	if (_InputBuffer){
		_InputBuffer->Stop();//��ͣ����
	}
}

bool MeshVoice::setCaptureVolume(long value)
{
	return false;
}

void MeshVoice::terminateCapture()
{
	if (_InputBuffer){
		_InputBuffer->Stop();//ֹͣ����

		/* ��ѯDirectSound�¼�֪ͨ�ӿ� */
		HRESULT hr;
		LPDIRECTSOUNDNOTIFY lpDsNotify;
		hr = _InputBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDsNotify);
		if (FAILED(hr))
		{
			LOG(ERROR) << "QueryInterface of IID_IDirectSoundNotify error! error: " << GetLastError() << ", hr: " << (unsigned long)hr;
		}
		else
		{
			hr = lpDsNotify->SetNotificationPositions(0, NULL);		//����¼��ɼ��¼�
			if (FAILED(hr))
			{
				LOG(ERROR) << "clear sound capture buffer notifications fail, error: " << GetLastError() << ", hr: " << (unsigned long)hr;
			}
		}

		_InputBuffer->Release();
		_InputBuffer = NULL;
		_InputProxy->Release();
		_InputProxy = NULL;
	}

	//�ͷ������¼�����
	for (int i = 0; i < AUDIOCAPTURE_EVENT_NUM; i++)
	{
		HANDLE  envent = h_CaptureEvent[i];
		if (envent != NULL)
		{
			CloseHandle(envent);
			h_CaptureEvent[i] = NULL;
		}
	}
}

int MeshVoice::startCaptureThread()
{
	if (NULL != h_CaptureThread){
		return 0;
	}
	UINT32 nThreadId = 0;
	HANDLE threadhandle = (HANDLE)_beginthreadex(NULL, 0, CaptureThread, this, 0, &nThreadId);
	return 0;
}

int MeshVoice::startPlayThread()
{
	if (NULL != h_PlayThread){
		return 0;
	}
	UINT32 nThreadId = 0;
	HANDLE threadhandle = (HANDLE)_beginthreadex(NULL, 0, PlayThread, this, 0, &nThreadId);
	return 0;
}

void MeshVoice::startPlay(DWORD dwStartPosition, BOOL bLoop)
{
	HRESULT hr;
	if (!_OutputProxy || !_OutputBuffer)
		return;

	if (DSERR_BUFFERLOST == _OutputBuffer->Play(0, 0, bLoop ? DSBPLAY_LOOPING : 0))
	{
		hr=_OutputBuffer->Play(0, 0, bLoop ? DSBPLAY_LOOPING : 0);
	}
}

void MeshVoice::stopPlay()
{
	if (_OutputProxy && _OutputBuffer)
		_OutputBuffer->Stop();
}

void MeshVoice::terminatePlay()
{
	if (_OutputBuffer){
		_OutputBuffer->Stop();//ֹͣ����

		/* ��ѯDirectSound�¼�֪ͨ�ӿ� */
		HRESULT hr;
		LPDIRECTSOUNDNOTIFY lpDsNotify;
		hr = _OutputBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDsNotify);
		if (FAILED(hr))
		{
			LOG(ERROR) << "QueryInterface of IID_IDirectSoundNotify error! error: " << GetLastError() << ", hr: " << (unsigned long)hr;
		}
		else
		{
			hr = lpDsNotify->SetNotificationPositions(0, NULL);		//����¼��ɼ��¼�
			if (FAILED(hr))
			{
				LOG(ERROR) << "clear sound capture buffer notifications fail, error: " << GetLastError() << ", hr: " << (unsigned long)hr;
			}
		}

		_OutputBuffer->Release();
		_OutputBuffer = NULL;
		_OutputProxy->Release();
		_OutputProxy = NULL;
	}

	//�ͷ������¼�����
	for (int i = 0; i < AUDIOCAPTURE_EVENT_NUM; i++)
	{
		HANDLE  envent = h_PlayEvent[i];
		if (envent != NULL)
		{
			CloseHandle(envent);
			h_PlayEvent[i] = NULL;
		}
	}
}

bool MeshVoice::setPlayVolume(long value)
{
	if (_OutputBuffer != NULL)
	{
		if (value <= DSBVOLUME_MIN){
			value = DSBVOLUME_MIN;
		}
		else if (value >= DSBVOLUME_MIN){
			value = DSBVOLUME_MAX;
		}
		else{
			if (_OutputBuffer->SetVolume(value) != DS_OK)
			{
				LOG(ERROR) << "Set Volume error";
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

long MeshVoice::GetPlayVolume()
{
	long lRel = 0;
	if (_OutputBuffer != NULL)
	{
		if (_OutputBuffer->GetVolume(&lRel) != DS_OK)
		{
			LOG(ERROR) << "Get Volume error!";
			return 1;
		}
	}
	return lRel;
}

//�������д��size���ֽڵ����ݣ�������� BYTE_PER_FRAME�������N��
int udpInstanceCustomer::writeData(char* address, DWORD size)
{
	_ASSERT(address != NULL);
	int ret;
	if (NULL != _udpInstance){
		ret = _udpInstance->appendToSendQ(address, size);
	}
	return ret;
}

unsigned int _stdcall MeshVoice::CaptureThread(void* pParam)
{
	if (NULL == pParam) return -1;
	MeshVoice* pthis = (MeshVoice*)pParam;
	LOG(INFO) << "[CaptureThread] STARTED";

	//ÿ�δ� directsound �Ļ������ж� BYTES_PER_FRAME * FRAMES_PER_READ_DS ���ֽ� �� cBuffer �У������� ÿ�� BYTES_PER_FRAME �ĳߴ� �ֶ����� sendQ
	char cBuffer[BYTES_PER_FRAME * FRAMES_PER_READ_DS + 1];

	while (1){
		DWORD waitret = WAIT_TIMEOUT;
		DWORD num = 0;
		DWORD dwSize = 0;
		memset(cBuffer, '\0', sizeof(cBuffer));

		//1. �������ȴ���˷�¼�������
		waitret = ::WaitForMultipleObjects(AUDIOCAPTURE_EVENT_NUM, pthis->h_CaptureEvent, false, 2000);
		if (NULL == pthis){
			LOG(INFO) << "[CaptureThread] MeshVoice has been released , break & terminate";
			break;
		}

		switch (waitret)
		{
			case WAIT_TIMEOUT:
				LOG(INFO) << "[CaptureThread] WaitForMultipleObjects timeout";
				break;
			case WAIT_FAILED:
				LOG(INFO) << "[CaptureThread] WaitForMultipleObjects failed";
				break;
			default:
				num = waitret - WAIT_OBJECT_0;
				if (true==pthis->getCapturedSoundData(num, cBuffer, &dwSize))
				{
					if (NULL == pthis){
						LOG(INFO) << "[CaptureThread] pthis == NULL , break & terminate";
						continue;
					}
					pthis->writeData(cBuffer, dwSize);
				}
		}
	}
	LOG(INFO) << "[CaptureThread] STOPED";
	return 0;
}

//�Ӷ����ж�ȡ num ��Ԫ�أ������ֽ�Ϊ��λ���Ը���Ϊ��λ
int udpInstanceCustomer::readData(char* address, DWORD num)
{
	int ret;
	if (NULL != _udpInstance){
		ret = _udpInstance->readFromRecvQ(address, num);
	}
	return ret;
}

unsigned int _stdcall MeshVoice::PlayThread(void* pParam)
{
	if (NULL == pParam) return -1;

	MeshVoice* pthis = (MeshVoice*)pParam;
	LOG(INFO) << "[PlayThread] STARTED";

	DWORD waitret = WAIT_OBJECT_0;
	char data[BYTES_PER_FRAME*FRAMES_PER_WRITE_DS+1];
	pthis->setSoundTrack();		//����˫����
	pthis->_OutputBuffer->SetCurrentPosition(0);
	pthis->_OutputBuffer->Play(0, 0, DSBPLAY_LOOPING);

	while (1){	//ע����ѭ���� �������㣬��Ҫ����sleep���ֶ�����������ռ��cpu
		memset(data, '\0', sizeof(data));
		//ÿ�δӶ����ж� 4 ֡���ݣ����������û���㹻�����ݣ���ʹ�ÿհ�֡(\0)���
		int ret = pthis->readData(data, FRAMES_PER_WRITE_DS);
		if (ret == -1){
			Sleep(20);
			continue;
		}

		waitret = ::WaitForMultipleObjects(AUDIOPLAY_EVENT_NUM, pthis->h_PlayEvent, FALSE, INFINITE);
		if (NULL == pthis){
			LOG(INFO) << "[PlayThread] Customer object has been released , break & terminate";
			break;
		}
		if (waitret >= WAIT_OBJECT_0 && waitret <= WAIT_OBJECT_0 + AUDIOPLAY_EVENT_NUM -1){
			switch (waitret)
			{
				case WAIT_TIMEOUT:
					LOG(INFO) << "[PlayThread] WaitForMultipleObjects timeout";
					break;
				case WAIT_FAILED:
					LOG(INFO) << "[PlayThread] WaitForMultipleObjects failed";
					break;
				default:
				{
					pthis->writeDirectSoundBuffer(pthis->_OutputBuffer, pthis->m_dwWriteCursor, BYTES_PER_FRAME*FRAMES_PER_WRITE_DS, (BYTE *)data, 0);
				}
			}
		}
	}
	LOG(INFO) << "[PlayThread] STOPED";
	return 0;
}

bool MeshVoice::SetSoundData(string data)
{
	bool bRet = FALSE;
	/*40*320���������ռ�*/
	DWORD dwDirectSoundBufferWritePosition = 0;		//��д��ʼλ��
	DWORD dwDirectSoundBufferPlayPosition = 0;		//��ǰ���ŵ���λ��
	long int dwTempInteval = 0;						//������
	long int iInteval = 0;							//������
	return false;
	//void * pDirectSoundData = NULL;					//������	
	//void * pSoundData = NULL;						//Ҫд�뻺����������
	//bool bAdjust = FALSE;							//������


	//LPDIRECTSOUNDBUFFER tmp_pDirectSoundDeviceBuffer = m_pDirectSoundDeviceBuffer;

	//_OutputBuffer->GetCurrentPosition(&dwDirectSoundBufferPlayPosition, &dwDirectSoundBufferWritePosition);

	//iInteval = m_dwWritePosition - dwDirectSoundBufferWritePosition;

	//if (iInteval > 0)		//д�α� ��� ��д�α꣬���ⲻ��
	//{
	//	dwTempInteval = iInteval;
	//	if (!(iInteval >= 0 && iInteval <= m_dwWriteReadInteval))
	//	{
	//		m_dwWritePosition = dwDirectSoundBufferWritePosition + m_dwDelay;
	//		dwTempInteval = m_dwDelay;
	//		bAdjust = TRUE;
	//	}
	//}
	//else if (iInteval < 0)	//д�α� ���� ��д�α꣬������������Ḳ��δ���ŵ����ݣ�ͬʱ�������ַ�����д����->�������α� �� ��д�α� ֮��Ĳ��֣�
	//{
	//	dwTempInteval = iInteval + DIRECTSOUND_BUFFER_SIZE;
	//	if (!((iInteval + DIRECTSOUND_BUFFER_SIZE >= 0) && (iInteval + DIRECTSOUND_BUFFER_SIZE <= m_dwWriteReadInteval)))
	//	{
	//		m_dwWritePosition = dwDirectSoundBufferWritePosition + m_dwDelay;
	//		dwTempInteval = m_dwDelay;
	//		bAdjust = TRUE;
	//	}
	//}
	//else		//д�α� �� ��д�α��غϣ�����ֱ��д��
	//{
	//	//do nothing
	//}

	//if (m_dwWritePosition >= DIRECTSOUND_BUFFER_SIZE)
	//{
	//	m_dwWritePosition -= DIRECTSOUND_BUFFER_SIZE;
	//}

	//dwTempInteval = dwTempInteval + DIRECTSOUND_BLOCKSIZE;

	//if (dwTempInteval < m_dwWriteReadInteval)
	//{
	//	pSoundData = (void *)data.c_str();
	//}

	//if (pSoundData != NULL)
	//{
	//	m_bSilenceSign = FALSE;
	//	m_bInsert = FALSE;

	//	while (pSoundData != NULL && m_bDSPlayRunFlag)
	//	{

	//		const long int  max_can_read = 14;
	//		if (m_dwWritePosition >= DIRECTSOUND_BUFFER_SIZE)
	//		{
	//			m_dwWritePosition -= DIRECTSOUND_BUFFER_SIZE;
	//		}

	//		pDirectSoundData = pSoundData;

	//		bRet = WriteDirectSoundBuffer(tmp_pDirectSoundDeviceBuffer, m_dwWritePosition, DIRECTSOUND_BLOCKSIZE, pDirectSoundData);
	//		if (bRet == TRUE)
	//		{
	//			m_dwWritePosition += DIRECTSOUND_BLOCKSIZE;
	//		}


	//		pData->m_pCallcfgs[m_ucChannelId]->recordingOneRecvVoice(pDirectSoundData);

	//		++m_dwRecordInserted;

	//		/*loop step*/
	//		dwTempInteval = dwTempInteval + DIRECTSOUND_BLOCKSIZE;

	//		/*����ı�2���Ծ������ǵö�дPDSVoiceBuffer�еĶ�дָ�뿿��;���⣬Ҳ����ʹ��������ж��������ϵ��߼��ܹ�������������;ԭ���Ƚ�����������
	//		*ec ec611003573774 changyunlei 20130129
	//		*/
	//		if (dwTempInteval <  max_can_read)
	//		{
	//			//pdsSoundData = pData->m_pCallcfgs[m_ucChannelId].rcvVoiceBuffer.readOneFrame();
	//			pSoundData = (*(pData->m_pCallcfgs[m_ucChannelId])).readPdsVoice();
	//		}
	//		else
	//		{
	//			break;
	//		}

	//	}
	//}
	//else if (dwTempInteval < m_dwWriteReadInteval) /*������PDS����֡*/
	//{
	//	if (m_dwWritePosition >= DIRECTSOUND_BUFFER_SIZE)
	//	{
	//		m_dwWritePosition -= DIRECTSOUND_BUFFER_SIZE;
	//	}

	//	if (m_bSilenceSign == TRUE)
	//	{
	//		m_dwWritePosition = dwDirectSoundBufferWritePosition + m_dwDelay;
	//	}
	//	else
	//	{
	//		iInteval = m_dwWritePosition - dwDirectSoundBufferWritePosition;
	//		if (iInteval < 0)
	//		{
	//			iInteval += DIRECTSOUND_BUFFER_SIZE;
	//		}

	//		pDirectSoundData = m_SilenceDataBuf;

	//		if (iInteval == m_dwDelay && bAdjust == TRUE)
	//		{
	//			//if(m_dwRecordInserted == 0)
	//			{

	//				//����һ������
	//				//if(pData->m_pCallcfgs[m_ucChannelId]->m_callInfo.callStatus != TALKING_STAT)
	//				{
	//					bRet = WriteDirectSoundBuffer(tmp_pDirectSoundDeviceBuffer, m_dwWritePosition, DIRECTSOUND_BUFFER_SIZE, pDirectSoundData, 0/*DSBLOCK_ENTIREBUFFER*/);
	//				}
	//			}
	//			m_bSilenceSign = TRUE;
	//		}
	//		else if (iInteval <= INERTER_BLANK_INTEVAL)
	//		{

	//			if (m_bInsert == FALSE)
	//			{
	//				//if(m_dwRecordInserted == 0)
	//				{

	//					//����һ������
	//					//if(pData->m_pCallcfgs[m_ucChannelId]->m_callInfo.callStatus != TALKING_STAT)
	//					{
	//						bRet = WriteDirectSoundBuffer(tmp_pDirectSoundDeviceBuffer, m_dwWritePosition, INERTER_BLANK_SIZE, pDirectSoundData);
	//						m_bInsert = TRUE;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//if (m_dwWritePosition >= DIRECTSOUND_BUFFER_SIZE)
	//{
	//	m_dwWritePosition -= DIRECTSOUND_BUFFER_SIZE;
	//}

	//return FALSE;
}

bool MeshVoice::setSoundTrack()
{
	HRESULT ret;

	if (_OutputBuffer == NULL)
	{
		return false;
	}

	ret = _OutputBuffer->SetPan(DSBPAN_CENTER); //���ó�˫����

	return ret == DS_OK ? true : false;
}

/* ��inputbuffer�ж�����,��ȡ��ʼλ��Ϊ startPos����ȡ����Ϊ BYTES_PER_FRAME * FRAMES_PER_READ */
bool MeshVoice::getCapturedSoundData(DWORD startPos, char *pBuffer, LPDWORD pOutSize)
{
	DWORD startaddress;
	LPVOID lpvPtr1, lpvPtr2;
	DWORD dwBytes1, dwBytes2;

	if (pBuffer == NULL || pOutSize == NULL || _InputBuffer == NULL){
		return false;
	}

	if (startPos > AUDIOCAPTURE_EVENT_NUM){
		LOG(INFO) << "[getCapturedSoundData] ERROR , startPos is larger than AUDIOCAPTURE_EVENT_NUM";
		return false;
	}

	startaddress = startPos * BYTES_PER_FRAME * FRAMES_PER_READ_DS;	//��ʼ���ڼ������ݿ�

	HRESULT hr = _InputBuffer->Lock(startaddress, BYTES_PER_FRAME * FRAMES_PER_READ_DS,&lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);	//�ӻ������ж�����

	if (DS_OK == hr)
	{
		// Write to pointers.
		::CopyMemory(pBuffer, lpvPtr1, dwBytes1);
		if (lpvPtr2 != NULL)
		{
			::CopyMemory((BYTE *)pBuffer + dwBytes1, lpvPtr2, dwBytes2);
		}
		// Release the data back to DirectSound.
		hr = _InputBuffer->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);

		*pOutSize = dwBytes1 + dwBytes2;
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

/* ��outputbuffer��д�����ݣ�дƫ�Ƶ�ַΪ dwWritePosition ��  д������Ϊ dwSize */
bool MeshVoice::writeDirectSoundBuffer(LPDIRECTSOUNDBUFFER pDirectSoundDeviceBuffer, DWORD dwWritePosition, DWORD dwSize, BYTE *pSoundData, DWORD dwFlag)
{
	BOOL bRet = FALSE;

	if (pSoundData == NULL || dwSize > AUDIOPLAY_BUFFERSIZE)
	{
		return bRet;
	}

	LPVOID lpvPtr1 = NULL, lpvPtr2 = NULL;
	DWORD dwBytes1 = 0, dwBytes2 = 0, dwBytesTotal=0;

	if (pDirectSoundDeviceBuffer != NULL)
	{
		HRESULT hr = pDirectSoundDeviceBuffer->Lock(dwWritePosition, dwSize, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, dwFlag);

		if (DSERR_BUFFERLOST == hr) {
			pDirectSoundDeviceBuffer->Restore();
			hr = pDirectSoundDeviceBuffer->Lock(dwWritePosition, dwSize, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, dwFlag);
		}

		if (DS_OK == hr)
		{
			::CopyMemory(lpvPtr1, pSoundData, dwBytes1);
			if (lpvPtr2 != NULL)
			{
				::CopyMemory(lpvPtr2, (BYTE *)pSoundData + dwBytes1, dwBytes2);
			}
			hr = pDirectSoundDeviceBuffer->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);

			if (DS_OK == hr)
			{
				bRet = TRUE;
			}
		}

		dwBytesTotal = dwBytes1 + dwBytes2;
	}
	m_dwWriteCursor += dwBytesTotal;		//ÿ�β��Ŷ�����ƫ����
	m_dwWriteCursor %= AUDIOPLAY_BUFFERSIZE;	//������ŵ������ôƫ��������
	//LOG(INFO) << "[writeDirectSoundBuffer] reach to position of PLAY BUFFER -> " << m_dwWriteCursor;
	return bRet;
}

bool MeshVoice::createSoundCapturedEvent()
{
	HRESULT hr;

	/* ������ʼ�����¼����� */
	for (int i = 0; i < AUDIOCAPTURE_EVENT_NUM; i++)
	{
		h_CaptureEvent[i] = CreateEvent(NULL, false, false, NULL);
		if (NULL == h_CaptureEvent[i])
		{
			LOG(ERROR) << "CreateSoundCaptureEvent() CreateEvent error! Getlasterror = " << ::GetLastError();
			return false;
		}
	}

	/* ��ȡDirectSound�� �¼�֪ͨ�ӿ� */
	LPDIRECTSOUNDNOTIFY lpDsNotify;
	if (NULL != _InputBuffer){
		hr = _InputBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDsNotify);
		if (FAILED(hr))
		{
			LOG(ERROR) << "[createSoundCapturedEvent] QueryInterface of IID_IDirectSoundNotify error! Getlasterror = " << ::GetLastError() << ", hr = " << (unsigned long)hr;
			lpDsNotify->Release();
			return false;
		}
	}else{
		LOG(INFO) << "[createSoundCapturedEvent] ERROR , _InputBuffer is NULL";
		lpDsNotify->Release();
		return false;
	}

	DSBPOSITIONNOTIFY PositionNotify[AUDIOCAPTURE_EVENT_NUM];
	for (int i = 0; i < AUDIOCAPTURE_EVENT_NUM; i++)
	{
		PositionNotify[i].dwOffset = BYTES_PER_FRAME * FRAMES_PER_READ_DS * i;	//directsound�Ļ������У�ÿ���� dwOffset ��ƫ������֪ͨ WaitForMultipleObjects һ��
		PositionNotify[i].hEventNotify = h_CaptureEvent[i];
	}

	/* �����¼�֪ͨ */
	if (FAILED(hr = lpDsNotify->SetNotificationPositions(AUDIOCAPTURE_EVENT_NUM, PositionNotify)))
	{
		LOG(ERROR) << "SetNotificationPositions error! Getlasterror = " << ::GetLastError() << ", hr = " << (unsigned long)hr;
		lpDsNotify->Release();
		return false;
	}

	lpDsNotify->Release();
	LOG(INFO) << "[createSoundCapturedEvent] SUCCESS";
	return true;
}

bool MeshVoice::createSoundPlayedEvent()
{
	HRESULT hr;

	/* ������ʼ�����¼����� */
	for (int i = 0; i < AUDIOPLAY_EVENT_NUM; i++)
	{
		h_PlayEvent[i] = CreateEvent(NULL, false, false, NULL);
		if (NULL == h_PlayEvent[i])
		{
			LOG(ERROR) << "[createSoundPlayedEvent] CreateEvent error! Getlasterror = " << ::GetLastError();
			return false;
		}
	}

	/* ��ȡDirectSound�� �¼�֪ͨ�ӿ� */
	LPDIRECTSOUNDNOTIFY lpDsNotify;
	if (NULL != _OutputBuffer){
		hr = _OutputBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDsNotify);
		if (FAILED(hr))
		{
			LOG(ERROR) << "[createSoundPlayedEvent] QueryInterface of IID_IDirectSoundNotify error! Getlasterror = " << ::GetLastError() << ", hr = " << (unsigned long)hr;
			return false;
		}
	}
	else{
		LOG(INFO) << "[createSoundPlayedEvent] ERROR , _OutputBuffer is NULL";
		return false;
	}

	DSBPOSITIONNOTIFY PositionNotify[AUDIOPLAY_EVENT_NUM];
	for (int i = 0; i < AUDIOPLAY_EVENT_NUM; i++)
	{
		PositionNotify[i].dwOffset = BYTES_PER_FRAME * FRAMES_PER_WRITE_DS * i;
		PositionNotify[i].hEventNotify = h_PlayEvent[i];
	}

	/* �����¼�֪ͨ */
	hr = lpDsNotify->SetNotificationPositions(AUDIOPLAY_EVENT_NUM, PositionNotify);
	if (DS_OK != hr)
	{
		LOG(ERROR) << "SetNotificationPositions error! Getlasterror = " << ::GetLastError() << ", hr = " << (unsigned long)hr;
		lpDsNotify->Release();
		return false;
	}
	lpDsNotify->Release();
	return true;
}

bool MeshVoice::createInputProxy()
{
	HRESULT hr = ::DirectSoundCaptureCreate(NULL, &_InputProxy, NULL);
	if (hr != DS_OK)
	{
		LOG(ERROR) << "[createInputProxy] DirectSoundCaptureCreate() error! GetLastError = " << ::GetLastError() << ", hr = " << (unsigned long)hr;
		if (hr == DSERR_NODRIVER)
		{
			//MessageBox(NULL, L"���黰Ͳ�Ƿ�������", L"����", 0);
			LOG(INFO) << "[createInputProxy] No sound driver is available for use";
			return DIRECTSOUND_CAPTURECREATE_NODRIVER;
		}
		LOG(INFO) << "[createInputProxy] UNKONWN ERROR";
		return DIRECTSOUND_CAPTURECREATE_FAILED;
	}

	/* ������Ƶ��ʽ */
	DSCBUFFERDESC	dscbd;
	memset((void*)&dscbd, 0, sizeof(DSCBUFFERDESC));
	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = WAVE_CHANNELS;
	wfx.nSamplesPerSec = WAVE_SAMPLESPERSEC;
	wfx.nAvgBytesPerSec = WAVE_SAMPLESPERSEC*(WAVE_BITSPERSAMPLE / 8)*WAVE_CHANNELS;		//ÿ��16000�ֽ�
	wfx.nBlockAlign = (WAVE_BITSPERSAMPLE / 8)*WAVE_CHANNELS;
	wfx.wBitsPerSample = WAVE_BITSPERSAMPLE;
	wfx.cbSize = WAVE_CBSIZE;

	dscbd.dwSize = sizeof(DSCBUFFERDESC);
	dscbd.dwFlags = 0;
	dscbd.dwBufferBytes = (FRAMES_PER_READ_DS * BYTES_PER_FRAME) * AUDIOCAPTURE_EVENT_NUM;
	dscbd.dwReserved = 0;
	dscbd.lpwfxFormat = &wfx;

	/* ����DirectSound�ɼ�������*/
	hr = _InputProxy->CreateCaptureBuffer(&dscbd, &_InputBuffer, NULL);
	if (hr != DS_OK)
	{
		LOG(ERROR) << " CreateCaptureBuffer() error! GetLastError = " << ::GetLastError() << ", hr = " << (unsigned long)hr;
		LOG(ERROR) << "UNINITIALIZED:" << (unsigned long)DSERR_UNINITIALIZED;
		LOG(ERROR) << "DSERR_GENERIC:" << (unsigned long)DSERR_GENERIC;
		LOG(ERROR) << "DSERR_NODRIVER:" << (unsigned long)DSERR_NODRIVER;
		return DIRECTSOUND_CAPTURECREATE_FAILED;
	}

	/* ����DirectSound�ɼ�֪ͨ�¼� */
	if (createSoundCapturedEvent() == FALSE)
	{
		LOG(ERROR) << "CreateSoundCaptureEvent failed!";
		return DIRECTSOUND_CAPTURECREATE_FAILED;
	}

	LOG(INFO) << "[createInputProxy] SUCCESS";
	return DIRECTSOUND_CAPTURECREATE_SUCESSED;
}

bool MeshVoice::createOutputProxy(HWND * pWnd)
{
	/*1.������Ƶ�ɼ�����*/
	HRESULT hr = ::DirectSoundCreate(NULL, &_OutputProxy, NULL);
	if (hr != DS_OK)
	{
		LOG(ERROR) << "[createOutputProxy] DirectSoundCreate() error! GetLastError = " << ::GetLastError() << ", hr = " << (unsigned long)hr;
		if (hr == DSERR_NODRIVER)
		{
			//MessageBox(NULL, L"���黰Ͳ�Ƿ�������", L"����", 0);
			LOG(INFO) << "[createOutputProxy] No sound driver is available for use";
			return DIRECTSOUND_CAPTURECREATE_NODRIVER;
		}
		LOG(INFO) << "[createOutputProxy] UNKONWN ERROR";
		return DIRECTSOUND_CAPTURECREATE_FAILED;
	}
	if (_OutputProxy == NULL)
	{
		LOG(ERROR) << "[createOutputProxy]_OutputProxy = NULL";
		return FALSE;
	}

	/*2.����Э�����ȼ�*/
	m_pWnd = pWnd;
	//hr = _OutputProxy->SetCooperativeLevel(*m_pWnd, DSSCL_NORMAL);
	hr = _OutputProxy->SetCooperativeLevel(*m_pWnd, DSSCL_EXCLUSIVE);
	if (hr != DS_OK)
	{
		LOG(ERROR) << "[createOutputProxy] SetCooperativeLevel error!hr:" << hr << ", err: " << GetLastError();
		return FALSE;
	}

	/*3.������Ƶ��ʽ*/
	DSBUFFERDESC	dsbd;
	memset(&dsbd, 0, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN;
	dsbd.dwBufferBytes = (FRAMES_PER_WRITE_DS * BYTES_PER_FRAME) * AUDIOCAPTURE_EVENT_NUM;	//12800*4	,  ����4����Ϊ���Ż�����ÿ�β���4֡���ݣ���˲��Ż����� �� ������Ƶ��������4������Ϊ���Ż�������С�ᵼ��������

	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = WAVE_CHANNELS;
	wfx.nSamplesPerSec = WAVE_SAMPLESPERSEC;
	wfx.nAvgBytesPerSec = WAVE_SAMPLESPERSEC*(WAVE_BITSPERSAMPLE / 8)*WAVE_CHANNELS;		//ÿ��16000�ֽ�
	wfx.nBlockAlign = (WAVE_BITSPERSAMPLE / 8)*WAVE_CHANNELS;
	wfx.wBitsPerSample = WAVE_BITSPERSAMPLE;
	wfx.cbSize = WAVE_CBSIZE;
	dsbd.dwReserved = 0;
	dsbd.lpwfxFormat = &wfx;

	/*4.����DirectSound���Ż�����*/
	hr = _OutputProxy->CreateSoundBuffer(&dsbd, &_OutputBuffer, NULL);
	if (hr != DS_OK)
	{
		LOG(ERROR) << " CreateSoundBuffer() error! GetLastError = " << ::GetLastError() << ", hr = " << (unsigned long)hr;
		LOG(ERROR) << "UNINITIALIZED:" << (unsigned long)DSERR_UNINITIALIZED;
		LOG(ERROR) << "DSERR_GENERIC:" << (unsigned long)DSERR_GENERIC;
		LOG(ERROR) << "DSERR_NODRIVER:" << (unsigned long)DSERR_NODRIVER;
		return DIRECTSOUND_CAPTURECREATE_FAILED;
	}

	/* ����DirectSound����֪ͨ�¼� */
	if (createSoundPlayedEvent()== FALSE)
	{
		LOG(ERROR) << "createSoundPlayedEvent failed!";
		return DIRECTSOUND_CAPTURECREATE_FAILED;
	}

	LOG(INFO) << "[createOutputProxy] SUCCESS";
	return DIRECTSOUND_CAPTURECREATE_SUCESSED;
}

int MeshVoice::onRecvQFull()
{
	if (selfChecking){
		return 0;
	}
	selfChecking = true;

	LOG(INFO) << "[onRecvQFull] MeshVoice selfChecking ... ";

	selfChecking = false;
	return 0;
}

int MeshVoice::onSendQFull()
{
	if (selfChecking){
		return 0;
	}
	selfChecking = true;

	LOG(INFO) << "[onSendQFull] MeshVoice selfChecking ... ";

	selfChecking = false;
	return 0;
}