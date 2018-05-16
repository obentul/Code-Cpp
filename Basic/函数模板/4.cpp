/*显示具体化(是一种函数模板，对应编译器做的“隐式具体化”)
 * 前面介绍个函数的重载和函数的模板，现在做个小结：
 * 1.函数重载：通过改变函数列表的形式(返回值无影响)，达到多个函数实现公用一个函数名的目的
 * 2.函数模板：提供适用于多种入参的统一函数实现，达到多种入参类型公用一个函数逻辑的目的(入参格式不变)
 * 
 * 如果在使用函数模板的时候，对于多种入参，函数逻辑是一样的，在编译的时候会根据具体的被使用情况，具体化多个
 * 模板实例出来，比如 void func(int a,int b) , void func(int a[],int b[])  ,  void func(stu stu1,stu stu2)。(这的stu是结构体，C++使用结构体时可直接用结构体名)
 * 那么如果我想自己依据模板定义一个模板实例，这个实例的逻辑和模板不一样，那么该怎么做呢。这就是“显示具体化”，与之对应的就是在编译时自动生成的“隐式具体化”(见
 * 上面的三个func，都是隐式具体化)
 *
 * 直白点：针对当前模板，如果入参是某个参数类型，那就做和别的入参不一样的动作(显示具体化)
 *
 * 声明格式: template <> void Swap<int *>(int *a,int *b);        Swap后面的尖括号里面的内容就表示要特殊对待的入参类型
 * */


#include <stdio.h>

template <typename T>
void Swap(T a,T b);

template <> void Swap<int *>(int *a,int *b);
template <> void Swap<int &>(int &a,int &b);

int main(int argc,char ** argv)
{
	int *p_a,*p_b;
	int a=4,b=6;
	p_a=&a;
	p_b=&b;

	int &c=a;
	int &d=b;

	Swap(p_a,p_b);
	
	a=4;
	b=6;

	Swap(c,d);		//调用此函数会跳转到模板中进行，这里需要弄明白为什么
}

template <typename T>
void Swap(T a,T b)	//模板
{
	T swap;

	swap=a;
	a=b;
	b=swap;
}

template <> void Swap<int *>(int *a,int *b)	//显示具体化1
{
	printf("dosomething i want to do -------\n");


	printf("template <> void Swap(int *a,int *b) before : a=%d,b=%d\n",*a,*b);
	
	int swap;
	swap=*a;
	*a=*b;
	*b=swap;

	printf("template <> void Swap(int *a,int *b) after : a=%d,b=%d\n",*a,*b);
}

template <> void Swap<int &>(int &a,int &b)	//显示具体化2
{
	printf("dosomething i want to do ++++++++\n");

	int swap;
	printf("template <> void Swap(int &a,int &b) before : a=%d,b=%d\n",a,b);
	swap=a;
	a=b;
	b=swap;
	printf("template <> void Swap(int &a,int &b) after : a=%d,b=%d\n",a,b);
}






