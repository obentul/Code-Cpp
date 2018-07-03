#include "stdafx.h"
#include "brass.h"
#include <iostream>

using std::cout;
using std::endl;
using std::string;

typedef std::ios_base::fmtflags format;
typedef	std::streamsize precis;
format setFormat();
void restore(format f,precis p);

/*******************************基类*******************************/
/************构造函数************/
Brass::Brass(const std::string &fullname_, long acctNum_, double balance_) {
	fullname = fullname_;
	acctNum = acctNum_;
	balance = balance_;
}
/************成员函数************/
void Brass::Depoit(double amt) {
	if (amt < 0) {
		cout << "Negative deposit not allowed; "
			<< "deposit is cancelled ";
	}
	else {
		balance += amt;
	}
}

void Brass::Withdraw(double amt) {
	format initialState = setFormat();
	precis prec = cout.precision(2);

	if (amt < 0)
	{
		cout << "Withdraw amout must be positive;"
			<< "withdraw canceled;";
	}
	else if (amt <= balance)
		balance -= amt;
	else
	{
		cout << "Withdraw amount of $" << amt
			<< "withdraw canceled;";
	}
	restore(initialState, prec);
}

double Brass::Balance() const {
	return balance;
}

/*虚函数，实现是不需要加virtual关键字*/
void Brass::ViewAcct() const {
	format initialState = setFormat();
	precis prec = cout.precision(2);

	cout << "Client: " << fullname << endl;
	cout << "Account Number: " << acctNum << endl;
	cout << "Balance: $" << balance << endl;
	restore(initialState, prec);
}


void Brass::ViewAcct_isV()const {
	cout << "Brass" << endl;
}
void Brass::ViewAcct_notV()const {
	cout << "Brass" << endl;
}



/*******************************派生类*******************************/
/************构造函数************/
BrassPlus::BrassPlus(const std::string &fullname_, long acctNum_, double balance_, double maxLoan_, double rate_)
	:Brass(fullname_, acctNum_, balance_) 
{
	maxLoan = maxLoan_;
	owesBank = 0.0; /*这个成员变量一直没在构造函数中赋值，而且不需要外界传值进来*/
	rate = rate_;
}

/*Brass中并没定义Brass(cosnt Brass &ptr)这样的构造体，但是这里用到了，这叫做“隐式拷贝构造体”，具体见12章*/
BrassPlus::BrassPlus(const Brass & ptr, double maxLoan_,double rate_) 
	:Brass(ptr)
{
	maxLoan = maxLoan_;
	rate = rate_;
	owesBank = 0.0;
}

/************成员函数************/
/*和Brass的同名同参函数，内部实现完全不同*/
void BrassPlus::ViewAcct() const{
	format initialStat = setFormat();
	precis prec = cout.precision(2);

	Brass::ViewAcct();			/*使用基类的public方法，不需要创建实例，直接“类名::方法名即可”*/
	cout << "Maximum loan: $" << maxLoan << endl;
	cout << "owe to bank : $" << owesBank << endl;
	cout.precision(3);
	cout << "Loan Rate : " << 100 * rate << "%\n";
	restore(initialStat, prec);
}

/*虚函数与非虚函数的比较*/
void BrassPlus::ViewAcct_isV()const {
	cout << "BrassPlus" << endl;
};
void BrassPlus::ViewAcct_notV()const {
	cout << "BrassPlus" << endl;
}

void BrassPlus::Withdraw(double amt) {
	format initialState = setFormat();
	precis prec = cout.precision(2);

	double bal = Balance();
	if (amt <= bal)
		Brass::Withdraw(amt); /*使用基类的public方法，不需要创建实例，直接“类名::方法名即可”*/
	else if (amt <= bal + maxLoan - owesBank);
	{
		double advance = amt + bal;
		owesBank += advance*(1.0 + rate);
		cout << "Bank advance: $" << advance << endl;
		cout << "Fiance charge : $" << advance*rate << endl;
		Depoit(advance);
		Brass::Withdraw(amt);
	}
	restore(initialState, prec);
}

format setFormat() {
	return cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
}

void restore(format f, precis p)
{
	cout.setf(f, std::ios_base::floatfield);
	cout.precision(p);
}




