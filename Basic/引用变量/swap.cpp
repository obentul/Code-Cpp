#include <stdio.h>

//ref
void swap_r(int &a,int &b);
//point
void swap_p(int *a,int *b);
//value
void swap_v(int a,int b);

void print_reset(void);

int a=4,b=6;

int main(int argc,char ** argv)
{
	swap_r(a,b);
	print_reset();

	swap_p(&a,&b);
	print_reset();

	swap_v(a,b);
	print_reset();



}


void swap_r(int &a,int &b)
{
	int swap;

	swap=a;
	a=b;
	b=swap;
}

void swap_p(int *a,int *b)
{
	int swap;

	swap=*a;
	*a=*b;
	*b=swap;
}

void swap_v(int a,int b)
{
	int swap;
	
	swap=a;
	a=b;
	b=swap;
}

void print_reset(void)
{
	printf("a=%d,b=%d\n",a,b);
	a=4;
	b=6;
}



/*
结果：
a=6,b=4
a=6,b=4
a=4,b=6

理解：swap_p和swap_v不用解释了，都是C的常规操作。

swap_r中，相当于定义了两个引用变量a和b，分别和目标全局变量a和b关联起来，
那么“后续所有对引用变量的操作都将影响被关联的变量，所有对被关联变量的操作也同样影响引用变量”，
二者的虚拟地址空间都是一样的，可以理解为就是一体的，所以早swap_r中对引用变量的赋值动作直接影响
了被引用的全局变量。

*/


