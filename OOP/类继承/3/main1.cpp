// ConsoleApplication6.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "brass.h"
#include <iostream>

int main()
{
	using std::cout;
	using std::endl;

	Brass *p_P;
	BrassPlus *p_H;

	Brass Piggy("yk",382199,4000.00);
	BrassPlus Hoggy("ykk",382288,3000.00);


	p_P = &Hoggy;	/*基类指针指向扩展类*/
	p_P->ViewAcct_isV();	/*因为ViewAcct是虚函数，		所以跟着实例走，那么这里输出Hoggy的ViewAcct_isV   ,  即BrassPlus*/
	p_P->ViewAcct_notV();   /*因为ViewAcct_notV是非虚函数， 所以跟着指针走，那么这里输出piggy的ViewAcct_notV  ， 即Brass*/


	int tmp;
	std:: cin >> tmp;

    return 0;
}

