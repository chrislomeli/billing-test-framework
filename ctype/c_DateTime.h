#ifndef c_DateTime__HEADER__H
#define c_DateTime__HEADER__H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <time.h>
#include <stdio.h>


class c_DateTime
{
public:
	void reset();

	c_DateTime();

	c_DateTime(int _year, int _mon, int _mday, int _hour, int _min, int _sec);
	c_DateTime(string datestr);
	c_DateTime(time_t ltime); 

	void setMidnight(void);
	bool set(int year, int mon, int mday, int hour, int min, int sec);
	bool set(string datestr);
	bool set(time_t ltime);
	bool set(c_DateTime& dtp);

	time_t	calcUTC();


	time_t		getUTC();
	string&		getPinTimeStr(string& pvtime); 
	struct tm*	getTm( struct tm* ptm );


	bool isLeapYear(int year) ;
	bool isValid() ;
	int getLastDay(int year, int mon) ;
	int set_isdst();
	bool isMidnight(void);
	bool addSeconds(int seconds);
	bool backToMidnight(void);

	string& formatDateString(string& str, string format);

	void tokenize(const string& str,
						  vector<string>& tokens,
						  const string& delimiters = " ");

	friend std::ostream& operator<<(std::ostream& os, const c_DateTime& _d ) 
	{

		return os	<< setw(2) << setfill('0') << (_d.osTime.tm_mon+1) 
					<< "/" << setw(2) << setfill('0') << _d.osTime.tm_mday
					<< "/" << setw(2) << setfill('0') 	<< (_d.osTime.tm_year-100) 
					<< ":" << setw(2) << setfill('0') << _d.osTime.tm_hour
					<< ":" << setw(2) << setfill('0') << _d.osTime.tm_min
					<< ":" << setw(2) << setfill('0') << _d.osTime.tm_sec
					<< ":";
	}


private:
     struct tm osTime;
	 time_t os_time_t;
   //  int isdst;
     bool m_valid;
public:
	int compare(c_DateTime *);
	int compareDays(c_DateTime *);


	void addMonths(int months);
};

#endif
