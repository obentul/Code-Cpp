#include "Stock0.h"
#include <iostream>

void Stock::acquire(const std::string & co, long n, double pr) {
	company = co;
	if (n < 0)
	{
		std::cout << "Number of shares can not be negative;"
			<< company << "share set  to 0 \n";
		shares = 0;
	}
	else
		shares = n;	//函数定义处使用了Stock::来修饰，就代表本函数内部可以直接饮用Stock的成员变量
	share_val = pr;
	set_tot();

}

void Stock::buy(long num, double price) {
	if (num < 0)
	{
		std::cout << "Number of shares purchased can not be negative;"
			<< "Transaction is aborted. \n";
	}
	else
	{
		shares += num;
		share_val = price;
		set_tot();
	}
}

void Stock::sell(long num, double price) {
	using std::cout;
	if (num < 0) 
	{
		cout << "Number of shares purchased can not be negative;"
			<< "Transaction is aborted. \n";
	}
	else
	{
		shares = num;
		share_val = price;
		set_tot();
	}
}

void Stock::update(double price) {
	share_val = price;
	set_tot();
}

void Stock::show() {
	std::cout << "Company:" << company
		<< "Shares:" << shares << '\n'
		<< "Shares Price : $" << share_val
		<< "Total worth : $" << total_val << '\n';
}

/*构造函数可以从汇编的角度来理解，或者认为是c中的goto语句，就是跳到某个符号表的位置，然后执行后面紧跟着的代码*/
/*构造函数也和普通函数一样，如果想放在头文件中，必须加上inline修饰，不然就会重复定义*/
/*三种创建构造函数的方法：1.inline定义在头文件的类定义之外；  
						  2.定义在类里面，即申明构造函数的同时就把它定义了，类比get_tot函数；
						  3.和成员方法一起定义到实现代码中
*/
Stock::Stock(std::string company_, long shares_, double share_val_, double total_val_) {
	company = company_;
	shares = shares_;
	share_val = share_val_;
	total_val = total_val_;
}

Stock::~Stock() {
	std::cout << "do nothing\n";
}

