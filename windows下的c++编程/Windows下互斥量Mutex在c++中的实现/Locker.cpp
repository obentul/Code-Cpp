#include "Locker.h"

Locker::Locker(){
	m_mutex = ::CreateMutex(NULL, FALSE, NULL);
}

Locker::~Locker(){
	if (m_mutex)//销毁锁
	{
		::CloseHandle(m_mutex);
		m_mutex = NULL;
	}
}

void Locker::lock(){
	if (m_mutex)//上锁
	{
		::WaitForSingleObject(m_mutex, INFINITE);
	}
}

void Locker::unlock(){
	if (m_mutex)//解锁
	{
		::ReleaseMutex(m_mutex);
	}
}
