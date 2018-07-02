// ConsoleApplication4.cpp : 定义控制台应用程序的入口点。
//

#include "mytime1.h"
#include <iostream>


int main()
{
	using std::cout;
	using std::endl;
	using std::cin;

	Time planning;
	Time coding(2,40);
	Time fixing(5,55);
	Time total;

	cout << "planning time  =  ";
	planning.Show();
	cout << endl;

	cout << "coding time = ";
	planning.Show();
	cout << endl;

	cout << "fixing time = ";
	planning.Show();
	cout << endl;

	total = coding.Sum(fixing);
	cout << "coding.Sum(fixing) = ";
	total.Show();
	cout << endl;

	/*第一种调用方法：和调用成员函数一样*/
	total = coding.operator+(fixing);
	total.Show();
	cout << endl;
	/*第二种调用方法：和使用加号一样*/
	total = coding + fixing;
	total.Show();
	cout << endl;

	int tp;
	cin >> tp;

    return 0;
}

