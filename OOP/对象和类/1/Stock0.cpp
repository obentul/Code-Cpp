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



