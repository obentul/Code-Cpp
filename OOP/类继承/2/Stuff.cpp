#include <iostream>
#include "Stuff.h"

//构造函数
Stuff::Stuff(char* name,int age,char* sex){
	_name=name;
	_age=age;
	_sex=sex;
}

//析构函数
Stuff::~Stuff(){
	
}

//获取基本信息
int Stuff::getBasic(Stuff_info &s){
	s->name=_name;
	s->age=_age;
	s->sex=_sex;

	printBasic();

	return 0;
}

//获取工作信息
int Stuff::getWork(Stuff_info &s){
	return 0;
}

//获取薪水信息
int Stuff::getSalary(Stuff_info &s){
	return 0;
}

//打印Basic信息
void Stuff::printBasic(void){
	
	return;
}
