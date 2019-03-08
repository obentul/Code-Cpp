#include "windows.h"

class Locker{

public:
	Locker();		//创建锁
	~Locker();		//销毁锁

public:
	lock();			//上锁
	unlock();		//解锁

private:
	HANDLE m_mutex;         //锁

}
