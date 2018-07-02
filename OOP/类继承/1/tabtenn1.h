#pragma once

#ifndef _TABTENN1_H_
#define _TABTENN1_H_

#include <String>
#include <iostream>

using std::string;

/**********************************基类*********************************/
class TableTennisPlayer {
private:
	string firstname;
	string lastname;
	bool hasTable;

public:
	/*构造函数*/
	TableTennisPlayer();
	TableTennisPlayer(const string &firstname_, const string &lastname_,bool hasTable_);


	/*成员函数*/
	void Name() const {
		std::cout << lastname << ", " << firstname;
	};
	bool HasTable() const { 
		std::cout << "hasTable is :" << hasTable <<std::endl;
		return hasTable;
	};
	void ResetTable(bool v) {
		hasTable = v;
	};
};

/*********************************扩展类*********************************/
class RatedPlayer : public TableTennisPlayer {
private:
	unsigned int rating;
public:
	/*构造函数*/
	RatedPlayer();
	/*要求在构造函数声明时，把基类的成员变量一并写进来，因为实例化一个扩展类的必要条件是先实例化一个基类*/
	RatedPlayer(const string &firstname_, const string &lastname, bool hasTable_,unsigned int rating_);
	/*另外一种基于当前已有实例的构造函数,注意这里传的是引用，那么如果传指针和传值行不行？*/

	/*成员函数*/
	RatedPlayer(const TableTennisPlayer & ptr, unsigned int rating_);
	unsigned int Rating() const {
		std::cout << "Rating is :" << rating << std::endl;
		return rating;
	};
	void ResetRating(unsigned int r) {
		rating = r;
	};
};






#endif









