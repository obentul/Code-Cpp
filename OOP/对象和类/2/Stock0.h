/* 说明：1.类的声明中，一般格式为划分为两大块，一块是public修饰的，另一块是private修饰的
*		 2.OOP的思想是“当前类对外只提供方法可使用，不可以直接访问成员变量”，因此一般都把所有成员变量放到private部分
*		 3.和java一样，在定义类的时候可以直接把实现也一并做了，但是一般不这样做
*		 4.类在定义的时候，一般都是把函数声明放在定义里，而把函数实现放到另外一个文件中(这点继承了C语言的传统，而没有和Java一样)
*		 5.不是方法一定放在public中，也不是域一定要放在private中，只不过是一种编程习惯/惯例
*/


#ifndef _STOCK0_H_
#define _STOCK0_H_

#include <string>
#include <iostream>

class Stock
{
	/*接口*/
public:
	void acquire(const std::string & co, long n, double pr);
	void buy(long num, double price);
	void sell(long num, double price);
	void update(double price);
	void show();
	Stock(std::string co, long shares, double share_val, double total_val);
	~Stock();

	/*域*/
private:
	std::string company;
	long shares;
	double share_val;
	double total_val;
	/*可以在类定义的时候就定义方法的实现，而不是一定要在类实现中实现*/
	void set_tot() { total_val = shares * share_val; }	
	void set_tot_tmp();

};

/*函数定义也可以放在类定义外面(印证了重新创建一个文件来编写函数实现)，但是，还是需要在函数名前面加上类名(Stock)
  这里定义的是内联函数，因为是在.h头文件中，头文件中切忌定义函数实现，因为在include的时候可能会造成重复定义，内
  联函数的实现比较特殊，无所谓*/
inline void Stock::set_tot_tmp() {
	std::cout << "do nothing\n";
}



#endif
