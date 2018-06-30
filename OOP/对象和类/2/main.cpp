// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//

#include "Stock0.h"

Stock *getobj(void);

int main()
{
	/*方法一，创建Stock实例，并初始化值，获得的是一个Stock实例，叫做zhangsan，可以想象成获得一个结构体，而不是结构体指针*/
	/*Stock zhangsan = Stock("fujistu", 1000, 1000, 1000);*/

	/*方法二，直接创建实例张三，等价于方法一，是方法一的紧凑版*/
	/*Stock zhangsan( "fujistu", 1000, 1000, 1000 );*/

	/*方法三，创建一个Stock指针pzhangsan，初始化一个无名Stock实例给pzhangsan，这样使用的就是实例的指针*/
	Stock *pzhangsan = new Stock( "fujistu", 1000, 1000, 1000 );

	int i;

	/*方法一*/
	/*zhangsan.show();*/

	/*方法二*/
	/*zhangsan.show();*/

	/*方法三*/
	pzhangsan->show();
	/*使用方面，和结构体一致，实例用“.”，指针用“->”*/

	/*访问常量*/
	std::cout<<pzhangsan->tmp<<std::endl;
	
	Stock *ptmp = getobj();	

	ptmp->show();

	std::cin >> i;

    return 0;
}

Stock *getobj(void)
{
	Stock *ptmp;
	Stock lisi("fujistu_1", 1000, 1000, 1000);
	ptmp = &lisi;						/*实验一：coredump,说明非new的内容在栈里*/
	//ptmp =new Stock("fujistu_1", 1000, 1000, 1000);	/*实验二：可以正常输出，说明new的内容是在堆里*/

	return ptmp;
}
