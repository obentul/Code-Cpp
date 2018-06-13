#ifndef _STUFF_H_
#define _STUFF_H_

typedef struct Stuff_info{
	char name[10];
	int age;
	char sex;
}Stuff_info;

class Stuff{
	
private:
        //Basic information
	char _name[10];  //姓名
	int _age;     //年龄
	char _sex;   //性别
	//Work information
	char _title[10]; //岗位
	int _work_age; //工龄
        //Salary infomation
        float _salary_m; //月薪
        float _salary_y; //年薪

public:
	int getBasic(Stuff_info &s);
	int getWork(Stuff_info &s);
	int getSalary(Stuff_info &s);

private:
	void printBasic(void);

};

#endif
