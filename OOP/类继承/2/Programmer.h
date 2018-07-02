#ifndef _PROGRAMMER_H_
#define _PROGRAMMER_H_

#include "Stuff.h"

typedef struct Programmer_Skills{
	char _programmer_language[10];
	char _Makefile:1;
	char _Shell:1;
	char _Sed:1;
	char _Awk:1;
	char _Kernel:1;
	char _Driver:1;
}Programmer_Skills;



class Programmer : public Stuff {
	
private:
	//Skills
	char _programmer_language[10];	//编程语言
	char _Makefile:1;		//是否会Makefile
	char _Shell:1;			//是否会Shell
	char _Sed:1;			//是否会sed
	char _Awk:1;			//是否会awk
	char _Kernel:1;			//是否会内核
	char _Driver:1;			//是否会驱动

public:
	int getSkills(Programmer_Skills &s);
}
