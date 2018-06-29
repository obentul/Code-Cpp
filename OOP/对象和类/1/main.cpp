#include "Stock0.h"

int main()
{
	//实例化一个类很简单，但是这个类是创建在栈里面的，因为没有使用new和malloc，JAVA强制使用new，就默认是创建在堆里的
		Stock zhangsan, lisi;
		int i;
	
		zhangsan.show();
		std::cin >> i;
	
		return 0;
}
