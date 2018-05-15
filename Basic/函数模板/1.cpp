#include <stdio.h>

template <typename T>//申明和定义都要添加此行
void swap(T &a,T &b);//这里的&符号是为了swap功能方便，不是模板功能的标配,也可以 void swap(T a,T b),只不过在swap的时候需要使用*运算


int main(int argc,char ** argv)
{
	int a=4,b=6;
	printf("before swap,a=%d,b=%d\n",a,b);
	swap(a,b);
	printf("after swap,a=%d,b=%d\n",a,b);

	float c=1.1,d=2.2;
	printf("before swap,c=%f,d=%f\n",c,d);
	swap(c,d);
	printf("after swap,c=%f,d=%f\n",c,d);

}

template <typename T>//申明和定义都要添加此行
void swap(T &a,T &b)
{
	T swap;
	
	swap=a;
	a=b;
	b=swap;
}


/*模板函数在此处被不同参数类型调用了两次，在编译的时候会对应生成两个函数
 *1.   void swap(int &a,int &b)
 *2.   void swap(float &a,float &b)
 * */
