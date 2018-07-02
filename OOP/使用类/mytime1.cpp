#include "mytime1.h"
#include <iostream>

Time::Time()
{
	hours = minutes = 0;
}

Time::Time(int hours_, int minutes_)
{
	hours = hours_;
	minutes = minutes_;
}

void Time::AddMin(int m)
{
	minutes += m;
	hours += minutes / 60;
	minutes %= 60;
}

void Time::AddHr(int h)
{
	hours += h;
}

void Time::Reset(int h, int m)
{
	hours = h;
	minutes = m;
}

Time Time::operator+(const Time & t) const
{	
	Time sum;	/*如果不指定初始值，则使用默认构造器*/
	std::cout << "xxxxx" << std::endl;
	sum.Show();
	std::cout << "xxxxx" << std::endl;

	sum.minutes = minutes + t.minutes;
	sum.hours = hours + t.hours + sum.minutes / 60;
	sum.minutes %= 60;
	return sum;
}

/*这里返回值是类，不同于结构体，类可以按值传递*/
Time Time::Sum(const Time & t) const	/*入参是引用，传地址比传值速度更快，使用的内存也更少*/
{
	Time sum;
	sum.minutes = minutes + t.minutes;
	sum.hours = hours + t.hours + sum.minutes / 60;
	sum.minutes %= 60;
	return sum;
}

void Time::Show() const
{
	std::cout << hours << "hours," << minutes << "minutes";
}











