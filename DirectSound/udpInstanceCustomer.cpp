#include "udpInstanceCustomer.h"
#include "trace.h"

extern HWND *g_soundWnd;

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
	}
	return 0;
}


string udpInstanceCustomer::readData()
{
	string result = "";
	if (NULL != _udpInstance){
		result = _udpInstance->readFromRecvQ();
	}
	return result;
}

int udpInstanceCustomer::writeData(string data)
{
	if (NULL != _udpInstance){
		_udpInstance->appendToSendQ(data);
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

}

void MeshVoice::init()
{
	//1.����dsound����ʵ��
	createInputProxy();
	startCapture();

	//2.����dsound����ʵ��
	createOutputProxy(g_soundWnd);

	//3.���ò���

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

void setCaptureVolume(long value)
{

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
		else{
			value = DSBVOLUME_MAX;
		}
		if (_OutputBuffer->SetVolume(value) != DS_OK)
		{
			LOG(ERROR) << "Set Volume error";
			return false;
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

unsigned int _stdcall MeshVoice::CaptureThread(void* pParam)
{
	if (NULL == pParam) return -1;

	MeshVoice* pthis = (MeshVoice*)pParam;
	LOG(INFO) << "CaptureThread STARTED";
	while (1){
		string result;
		DWORD waitret = WAIT_TIMEOUT;
		DWORD num = 0;
		DWORD dwSize = 0;
		char cBuffer[BYTES_PER_FRAME * FRAMES_PER_TRANS + 1];	//ÿ�δ�����ֽ�����Ϊ 320*1 + '\0'
		memset(cBuffer, 0x00, sizeof(cBuffer));

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
				//LOG(INFO) << "[CaptureThread] WaitForMultipleObjects success";
				num = waitret - WAIT_OBJECT_0;	//��num ��HANDLE����WaitForMultipleObjects�ķ��أ�������ҪȥHANDLE�����ж�λ��numλ�ã����е����ݱ�����������
				if (pthis->getCapturedSoundData(num, cBuffer, &dwSize)){
					//LOG(INFO) << "[CaptureThread] Captured sound Data is -> " << cBuffer << "size is ->" << dwSize;

					//1.  �� outdata д��¼���ļ�

					//2.  ����д�뷢�Ͷ��У���udp������ⷢ��
					pthis->writeData(string(cBuffer));
				}
		}
	}
}

unsigned int _stdcall MeshVoice::PlayThread(void* pParam)
{
	if (NULL == pParam) return -1;

	MeshVoice* pthis = (MeshVoice*)pParam;
	LOG(INFO) << "PlayThread STARTED";
	
	pthis->setSoundTrack();		//����˫����
	pthis->startPlay(0, TRUE);	//�ȶ�λ����ʼλ�õȴ�����

	while (1){
		DWORD waitret = WAIT_TIMEOUT;

		DWORD dwDirectSoundBufferWritePosition = 0;		//��д��ʼλ��
		DWORD dwDirectSoundBufferPlayPosition = 0;		//��ǰ���ŵ���λ��
		pthis->_OutputBuffer->GetCurrentPosition(&dwDirectSoundBufferPlayPosition, &dwDirectSoundBufferWritePosition);

		waitret = ::WaitForMultipleObjects(AUDIOPLAY_EVENT_NUM, pthis->h_PlayEvent, false, INFINITE);
		//waitret = ::WaitForMultipleObjects(AUDIOPLAY_EVENT_NUM, pthis->h_PlayEvent, false, 2000);
		if (NULL == pthis){
			LOG(INFO) << "[PlayThread] MeshVoice has been released , break & terminate";
			break;
		}
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
					   LOG(INFO) << "[PlayThread] WaitForMultipleObjects success";
					   //1. ������udpinstance��recvQ�ж�ȡһ�����ݣ�������пգ��������ѯ
					   string result = pthis->readData();
					   LOG(INFO) << "[PlayThread] -> "<<result;
					   if (result == "") break;

					   //2. �������������д����
					   pthis->writeDirectSoundBuffer(pthis->_OutputBuffer, pthis->m_dwWritePosition, BYTES_PER_FRAME, result, 0);
					   //pthis->SetSoundData(result);
				}
		}
	}
}

bool MeshVoice::SetSoundData(string data)
{
	return true;
	//bool bRet = FALSE;
	///*40*320���������ռ�*/
	//DWORD dwDirectSoundBufferWritePosition = 0;		//��д��ʼλ��
	//DWORD dwDirectSoundBufferPlayPosition = 0;		//��ǰ���ŵ���λ��
	//DWORD dwTempTime = 0;							//������
	//long int dwTempInteval = 0;						//������
	//long int iInteval = 0;							//������
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

	startaddress = startPos * BYTES_PER_FRAME * FRAMES_PER_READ;	//��ʼ���ڼ������ݿ�

	HRESULT hr = _InputBuffer->Lock(startaddress, BYTES_PER_FRAME * FRAMES_PER_READ,&lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);	//�ӻ������ж�����

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
//bool MeshVoice::writeDirectSoundBuffer(LPDIRECTSOUNDBUFFER pDirectSoundDeviceBuffer, DWORD dwWritePosition, DWORD dwSize, void * pSoundData, DWORD dwFlag)
bool MeshVoice::writeDirectSoundBuffer(LPDIRECTSOUNDBUFFER pDirectSoundDeviceBuffer, DWORD dwWritePosition, DWORD dwSize, string SoundData, DWORD dwFlag)
{
	LOG(INFO) << "writeDirectSoundBuffer";
	BOOL bRet = FALSE;
	void *pSoundData = (void *)SoundData.c_str();

	//if (m_dwWritePosition >= AUDIOPLAY_BUFFERSIZE)
	//{
	//	return bRet;
	//}

	if (pSoundData == NULL || dwSize > AUDIOPLAY_BUFFERSIZE)
	{
		return bRet;
	}

	LPVOID lpvPtr1 = NULL, lpvPtr2 = NULL;
	DWORD dwBytes1 = 0, dwBytes2 = 0;

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
	}
	m_dwWritePosition += dwWritePosition;		//ÿ�β��Ŷ�����ƫ����
	m_dwWritePosition %= AUDIOPLAY_BUFFERSIZE;	//������ŵ������ôƫ��������
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

	/* ����_InputBuffer �� ��Ƶ�ɼ������¼���֪ͨ���� ��ͬʱָ��ÿ���¼���Ӧ�Ļ�����ƫ���� */
	/* �൱��ʹ��N���¼�ƽ�ֻ�������ǰ ��AUDIOCAPTURE_EVENT_NUM*BYTES_PER_FRAME*FRAMES_PER_TRANS�� �ֽڿռ䣬ÿ���¼�����һ�Σ������¼�����ʱ��*/
	/* ����ͨ���¼���������ж����ݱ��ŵ����Ķλ��������Ӷ�������Ķ���������*/
	/* ����Ҳ����һ�����氵ʾ�����¼�����ʱ���ܷ��ʵĻ������ռ������޵ģ�Ҳ����ϵͳΪ���Ƿ���Ļ������ǹ̶��� ��AUDIOCAPTURE_EVENT_NUM*BYTES_PER_FRAME*FRAMES_PER_TRANS�� �ֽ�*/
	DSBPOSITIONNOTIFY PositionNotify[AUDIOCAPTURE_EVENT_NUM];
	for (int i = 0; i < AUDIOCAPTURE_EVENT_NUM; i++)
	{
		//PositionNotify[i].dwOffset = BYTES_PER_FRAME * FRAMES_PER_TRANS * (i + 1) - 1;	//ÿ�η���һ֡
		PositionNotify[i].dwOffset = BYTES_PER_FRAME * FRAMES_PER_TRANS * i;	//ÿ�η���һ֡
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

	/* ����_OutputBuffer �� ��Ƶ�ɼ������¼���֪ͨ���� ��ͬʱָ��ÿ���¼���Ӧ�Ļ�����ƫ���� */
	/* �൱��ʹ��N���¼�ƽ�ֻ�������ǰ ��AUDIOPLAY_EVENT_NUM*BYTES_PER_FRAME*FRAMES_PER_TRANS�� �ֽڿռ䣬ÿ���¼�����һ�Σ������¼�����ʱ��*/
	/* ����ͨ���¼���������ж����ݱ��ŵ����Ķλ��������Ӷ�������Ķ���������*/
	/* ����Ҳ����һ�����氵ʾ�����¼�����ʱ���ܷ��ʵĻ������ռ������޵ģ�Ҳ����ϵͳΪ���Ƿ���Ļ������ǹ̶��� ��AUDIOPLAY_EVENT_NUM*BYTES_PER_FRAME*FRAMES_PER_TRANS�� �ֽ�*/
	DSBPOSITIONNOTIFY PositionNotify[AUDIOPLAY_EVENT_NUM];
	for (int i = 0; i < AUDIOPLAY_EVENT_NUM; i++)
	{
		//PositionNotify[i].dwOffset = BYTES_PER_FRAME * FRAMES_PER_TRANS * (i + 1) - 1;	//ÿ�η���һ֡
		PositionNotify[i].dwOffset = BYTES_PER_FRAME * FRAMES_PER_TRANS * i;	//ÿ�η���һ֡
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
	dscbd.dwBufferBytes = (FRAMES_PER_TRANS * BYTES_PER_FRAME) * AUDIOCAPTURE_EVENT_NUM;	//���òɼ��������ߴ� = 12800 < 16000��ÿ��Ĳɼ������ڻ�����������
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
	hr = _OutputProxy->SetCooperativeLevel(*pWnd, DSSCL_NORMAL);
	if (hr != DS_OK)
	{
		LOG(ERROR) << "[createOutputProxy] SetCooperativeLevel error!hr:" << hr << ", err: " << GetLastError();
		return FALSE;
	}


	/*3.������Ƶ��ʽ*/
	DSBUFFERDESC	dscbd;
	memset(&dscbd, 0, sizeof(DSBUFFERDESC));
	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = WAVE_CHANNELS;
	wfx.nSamplesPerSec = WAVE_SAMPLESPERSEC;
	wfx.nAvgBytesPerSec = WAVE_SAMPLESPERSEC*(WAVE_BITSPERSAMPLE / 8)*WAVE_CHANNELS;		//ÿ��16000�ֽ�
	wfx.nBlockAlign = (WAVE_BITSPERSAMPLE / 8)*WAVE_CHANNELS;
	wfx.wBitsPerSample = WAVE_BITSPERSAMPLE;
	wfx.cbSize = WAVE_CBSIZE;

	dscbd.dwSize = sizeof(DSBUFFERDESC);
	dscbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN;
	dscbd.dwBufferBytes = (FRAMES_PER_TRANS * BYTES_PER_FRAME) * AUDIOCAPTURE_EVENT_NUM;	//���òɼ��������ߴ� = 12800 < 16000��ÿ��Ĳɼ������ڻ�����������
	dscbd.dwReserved = 0;
	dscbd.lpwfxFormat = &wfx;

	/*4.����DirectSound���Ż�����*/
	hr = _OutputProxy->CreateSoundBuffer(&dscbd, &_OutputBuffer, NULL);
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