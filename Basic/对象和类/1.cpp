#include <iostream>
#include <string>
#include <stdio.h>

class stock{

private:
	int share_;
	std::string holder_;

public:
	int getshare_(void)
	{
		return share_;		
	}
	std::string getholder_(void)
	{
		return holder_;
	}

stock(int share,std::string holder)
{
	share_=share;
	holder_=holder;
}

};



int main(int argc,char ** argv)
{
	class Stock *my_stock = new stock(100,"yk");
	printf("%d\n",my_stock.getshare());
}
