#include "stdafx.h"
#include "tabtenn1.h"
#include <iostream>

/*************************************基类构造函数***********************************/
/*  #1   默认构造函数  */
TableTennisPlayer::TableTennisPlayer()
{
	/*如果不使用“初始化器列表语法”来调用基类的构造函数，那么不论怎么显式指定基类的构造函数都没用，都会使用默认构造函数*/
	firstname = lastname = "none";
	hasTable = false;
}

/*为了满足调用构造函数时，直接传字符串，这里对于入参，使用const string &修饰，这也从侧面说明一点：
  “构造函数”仅仅是用来对成员变量赋值，它也是“函数”，入参是什么格式完全无所谓，甚至传个结构体进来都行，
  只要把成员变量的值初始化好就行了
*/
/*  #2   构造函数2  */
TableTennisPlayer::TableTennisPlayer(const string &firstname_, const string &lastname_, bool hasTable_) {
	firstname = firstname_;
	lastname = lastname_;
	hasTable = hasTable_;
}


/***********************************扩展类构造函数***********************************/
/*  #1   默认构造函数  */
RatedPlayer::RatedPlayer()
{
	TableTennisPlayer();
	rating = 0;
}

/*子类构造函数里“必须”使用基类的构造函数来创建一个基类，以供自己使用*/
/*这里如果不指明使用基类的哪个构造函数，那么会使用默认构造函数(貌似实际过程中也是先执行以下默认构造，然后再执行指定构造的)*/
/*显式调用没有效果，还是会仅默认初始化函数,下面这个实现是错误案例，注释掉*/
/*  #2   构造函数2  */
/*
RatedPlayer::RatedPlayer(const string &firstname_,const string &lastname_, bool hasTable_,unsigned int rating_) {
	TableTennisPlayer(firstname_, lastname_, hasTable_);
	rating = rating_;
}*/


/*  #3   构造函数3  */
/* ((((((重点))))))如果不想使用基类的默认构造函数，而想使用基类的某个构造函数(这里使用#2构造函数)，那么““必须””使用这种扩展类构造函数格式*/
/* 这叫做“初始化器列表语法”*/
RatedPlayer::RatedPlayer(const string &firstname_, const string &lastname_, bool hasTable_, unsigned int rating_)
	:TableTennisPlayer(firstname_,lastname_,hasTable_)
{
	rating = rating_;
}

/*  #4   构造函数4  */
/* 参照#3构造函数的语法*/
RatedPlayer::RatedPlayer(const TableTennisPlayer & ptr, unsigned int rating_)
	:TableTennisPlayer(ptr){
	rating = rating_;
}