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
