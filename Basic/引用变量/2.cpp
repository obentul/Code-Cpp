#include <stdio.h>

void modi_xxx_1(int &xxx);
void modi_xxx_2(const int &xxx);
void print_reset(void);

int xxx=10;

struct stu{
	const char *name="zhangsan";
	int age;
	int sex;
}stu1;

void modi_stu_1(stu &stu1);

int main(int argc,char * argv)
{
	modi_xxx_1(xxx);
	print_reset();
        modi_xxx_2(xxx);
        print_reset();

	modi_stu_1(stu1);
	printf("name=%s,age=%d,sex=%d\n",stu1.name,stu1.age,stu1.sex);

        modi_stu_2(stu1);
        printf("name=%s,age=%d,sex=%d\n",stu1.name,stu1.age,stu1.sex);

}

void modi_xxx_1(int &xxx)
{
	xxx=20;
}

void modi_xxx_2(const int &xxx)
{
//	xxx=20;	//编译时报错，可以使用这个办法来阻止引用修改原值
}

void modi_stu_1(stu &stu1)
{
	stu1.name="lisi";//对结构体的某个成员做const限制是没用的，只能对整个结构体做限制
	stu1.age=10;
	stu1.sex=0;
}

void modi_stu_2(const stu &stu1)
{
	//由于入参的const 修饰，所以不能改变引用入参的值，所以这里在编译时会报错
        //stu1.name="lisi";
        //stu1.age=10;
        //stu1.sex=0;
}


void print_reset(void)
{
	printf("xxx=%d\n",xxx);
	xxx=10;
}
