// ConsoleApplication5.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "tabtenn1.h"

int main()
{
	using std::cout;
	using std::endl;
	using std::cin;

	/*注意：调用构造函数RatedPlayer或TableTennisPlayer时，传参是常字符串，那么这就反过来要求构造函数的参数列表也要是常字符串，即const string &类型
			如果构造函数的参数列表是string 类型，那么在引用构造函数的时候，就需要传string实例，即：
			TableTennisPlayer(string firstname_,string lastname_,bool hasTable_)
				等价于
			string firstname = "Tara";
			string lastname = "Boomdea";
			TableTennisPlayer(firstname,lastname,false);
	*/

	/*重点：本DEMO演示了如何对子类进行初始化(调用构造函数)
		    默认情况下，构造子类的构造函数只会使用基类的默认构造函数作为唯一选择(逐步跟踪可以看到虽然进入了相应的构造，但是值却没有改变，这比较奇怪，不知道是
			调试器的问题，还是c++就是这个特性)
			如果想指定调用基类的某个构造函数，那么就需要使用“初始化器列表语法”，见构造函数的#3 和 #4
	*/

	/*使用实例化的那个构造方法*/
	RatedPlayer player1 = RatedPlayer("Mallory", "Duck", true, 2);
	/*使用传引用的那个构造方法*/
	TableTennisPlayer & refplayer2 = TableTennisPlayer("Tara", "Boomdea", true);
	RatedPlayer rplayer2 = RatedPlayer(refplayer2, 1);

	cout << "xxx" << endl;
	player1.Name();
	player1.HasTable();
	player1.Rating();
	cout << endl;

	cout << "xxx" << endl;
	rplayer2.Name();
	rplayer2.HasTable();
	rplayer2.Rating();
	cout << endl;

	int tmp;
	cin >> tmp;

    return 0;
}

