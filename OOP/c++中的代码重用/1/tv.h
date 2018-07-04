#pragma once

#ifndef _TV_H_
#define _TV_H_

class Tv {
public:
	friend class Remote;	/*友元类*/
	enum {Off,On};
	enum {MinVal,MaxVal=20};
	enum {Antenna,Cable};
	enum {TV,DVD};

	Tv(int s = Off,int mc=125):state(s),volume(5),
		maxchannel(mc),channel(2),mode(Cable),input(TV) {}
	void onoff() { state = (state == On) ? Off : On; }
	bool ison() const { return state == On; }
	bool volup();
	bool voldown();
	void chanup();
	void chandown();
	void set_mode() { mode = (mode == Antenna) ? Cable : Antenna; }
	void set_input() { input = (input == TV) ? DVD:TV; }
	void settings()const;

private:
	int state;
	int volume;
	int maxchannel;
	int channel;
	int mode;
	int input;
};

/*友元类的实现一般就放在其协助的那个类的定义里面一起声明了*/
/*只在声明的是时候指明friend，定义的时候不加friend*/
class Remote
{
private:
	int mode;
public:
	/*友元类里的方法可以直接以类的实例作为参数*/
	/*可以理解为：友元类里的方法可以直接用来处理其协助的类，友元类的被辅助类的外援*/
	Remote(int m = Tv::TV) : mode(m) {}
	bool volup(Tv & t) { return t.volup(); }
	bool voldown(Tv &t) { return t.voldown(); }
	void onoff(Tv & t) { t.onoff(); }
	void chanup(Tv & t) { t.chanup(); }
	void chandown(Tv & t) { t.chandown(); }
	void set_chan(Tv & t, int c) { t.channel = c; }
	void set_mode(Tv & t) { t.set_mode(); }
	void set_input(Tv & t) { t.set_input(); }
};


#endif