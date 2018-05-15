#include <stdio.h>

template <typename T>
void swap(T &a,T &b);

template <typename T>			//每个template只影响紧跟着的那个函数，所以每次都要在写一遍
void swap(T a[],T b[],int num);


int main(int argc,char ** argv)
{

	int a=4,b=6;
	
	char c[3]={'a','b','c'};
	char d[3]={'x','y','z'};
	
	printf("before swap,a=%d,b=%d\n",a,b);
	swap(a,b);
	printf("after swap,a=%d,b=%d\n",a,b);

	printf("before swap,c={%c%c%c},d={%c%c%c}\n",c[0],c[1],c[2],d[0],d[1],d[2]);
	swap(c,d,3);
	printf("after swap,c={%c%c%c},d={%c%c%c}\n",c[0],c[1],c[2],d[0],d[1],d[2]);
}

template <typename T>
void swap(T &a,T &b)
{
	T swap;
	swap = a;
	a = b;
	b = swap;
}

template <typename T>
void swap(T a[],T b[],int num)	//模板修饰的未必一定是引用类型，模板中也可以有通用类型
{
	int i;
	T swap;
	for(i=0;i<num;i++)
	{
		swap = *(a+i);
		*(a+i) = *(b+i);
		*(b+i) = swap;
	}
}

/*这里对swap函数做了重载，同时参数列表也包含了更多类型，但是需要注意一点：如果使用了模板，那么紧接着的这个函数
 * 一定要在入参里面使用模板变量T，否则编译时会报错“定义了模板没使用”，所以不要尝试定义模板仅仅在函数实现内使用，
 * 比如:
 * 	template <typename T>
 * 	void myprint(void)
 *	{
 *		T i;
 *		i=10;
 *		printf("%d\n",i);
 *	}
 * 上面那个例子编译不通过。
 *
 * */
