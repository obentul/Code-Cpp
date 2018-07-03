#pragma once

#ifndef _BRASS_H_
#define _BRASS_H_

#include <string>

/*******************************基类*******************************/
class Brass {
private:
	std::string fullname;
	long acctNum;
	double balance;

public:
	/****************构造函数****************/
	/*申明同时初始化，这是一种简单的书写格式，相对高效*/
	Brass(const std::string &fullname_ = "Nullbody", long acctNum_ = -1, double balance_ = 0.0);
	

	/*虚函数需要结合类指针来说明和使用：
		如果某个函数是虚函数，那么通过“指针和引用”来调用方法，那么以指针指向的实例为准
		如果某个函数不是虚函数，那么通过“指针和引用”来调用方法，那么以指针类型为准
	*/


	/****************成员方法****************/
	void Depoit(double amt);
	virtual void Withdraw(double amt);	/*虚函数*/
	virtual void ViewAcct()const;		/*虚函数*/

	virtual void ViewAcct_isV()const;	/*虚函数，和扩展类同名*/
	void ViewAcct_notV()const;			/*非虚函数，和扩展类同名*/

	double Balance()const;

	/****************析构函数****************/
	virtual ~Brass() {};				/*虚函数析构函数：为了确保释放派生对象时，按正确的顺序调用析构函数*/
};

/*******************************派生类*******************************/
class BrassPlus : public Brass {
private:
	double maxLoan;
	double rate;
	double owesBank;

public:
	/*********构造函数*********/
	BrassPlus(const std::string &fullname_ = "NullBody", long acctNum_ = -1, double balance_ = 0.0,
		double maxLoan = 500, double rate = 0.11125);
	BrassPlus(const Brass &brass,double maxLoad_=500,double rate_=0.11125);

	/*********成员方法*********/
	virtual void ViewAcct()const;		/*和基类的函数同名同参*/
	virtual void Withdraw(double amt);	/*和基类的函数同名同参*/

	virtual void ViewAcct_isV()const;	/*虚函数，和扩展类同名*/
	void ViewAcct_notV()const;			/*和基类同名的非虚函数*/

	void ResetMax(double m) { maxLoan = m; };
	void ResetRate(double r) { rate = r; };
	void ResetOwes() { owesBank = 0; };


};



#endif