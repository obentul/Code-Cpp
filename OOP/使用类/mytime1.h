#pragma once

#ifndef _MYTIME_H_
#define _MYTIME_H_

class Time {
private:
	int hours;
	int minutes;

public:
	Time();
	Time(int hours_, int minutes_);
	void AddMin(int m);
	void AddHr(int h);
	void Reset(int h = 0, int m = 0);	/*靠靠*/
	/*函数尾部的const表示:当前函数不能够修改本类的成员变量*/
	/*下面三个函数，都不会修改本实例的成员变量，而是生成新的成员变量值，放入临时实例中返回*/
	Time operator+(const Time & t) const;	
	Time Sum(const Time & t) const;
	void Show() const;


};

#endif
