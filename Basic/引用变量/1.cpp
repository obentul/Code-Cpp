#include <stdio.h>

int xxx = 10;
int & r_xxx=xxx;



int main(int argc,char ** argv)
{

	printf("xxx=%d,r_xxx=%d\n",xxx,r_xxx);
	printf("address:xxx=%x,r_xxx=%x\n",&xxx,&r_xxx);
	
}
