// ConsoleApplication7.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "tv.h"


int main()
{
	using std::cout;

	Tv s42;
	cout << "Initial setting for 42 \"TV:\n";
	s42.settings();
	s42.onoff();
	s42.chanup();
	cout << "\nAdjusted settings for 42 \" TV:\n";
	s42.chanup();
	cout << "\nAdjusted settings for 42 \" TV:\n";
	s42.settings();

	Remote f_tv;
	f_tv.set_chan(s42, 10);
	f_tv.volup(s42);
	f_tv.volup(s42);
	cout << "\n42\" settings after using remote: \n";
	s42.settings();

	Tv s58(Tv::On);
	s58.set_mode();
	f_tv.set_chan(s58,28);
	cout << "\n58\" settings:\n";
	s58.settings();

	int tmp;

	std::cin >> tmp;

    return 0;
}

