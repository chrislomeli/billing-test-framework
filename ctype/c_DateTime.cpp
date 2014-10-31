#include <time.h>
#include <stdio.h>
#include <ctype/c_DateTime.h>


c_DateTime::c_DateTime() : m_valid(false)
{

}  

c_DateTime::c_DateTime(int _year, int _mon, int _mday, int _hour, int _min, int _sec) 
	: m_valid(false)
{
	set(_year, _mon, _mday, _hour, _min, _sec);	
}  


c_DateTime::c_DateTime(time_t ltime) : m_valid(false)
{
	set(ltime);	
}  


c_DateTime::c_DateTime(string mmddyyyhhmiss) : m_valid(false)
{
	set(mmddyyyhhmiss);	
}  


int c_DateTime::set_isdst() 
{
	time_t ltime;
	time(&ltime);
	struct tm * ptm = localtime(&ltime);
	return ptm->tm_isdst;
}

bool c_DateTime::set(time_t ltime) 
{ 
	reset();
	struct tm * ptm = localtime(&ltime);

	osTime.tm_sec		=   ptm->tm_sec;    // seconds after the minute -- [0,60] /
	osTime.tm_min		=	ptm->tm_min;    // minutes after the hour -- [0,59] /
	osTime.tm_hour		=	ptm->tm_hour;   // hours since midnight -- [0,23] /
	osTime.tm_mday		=	ptm->tm_mday;   // day of the month -- [1,31] /
	osTime.tm_mon		=	ptm->tm_mon;    // months since January -- [0,11] /
	osTime.tm_year		=	ptm->tm_year; // years since 1900 /
	osTime.tm_wday		=	ptm->tm_wday;
	osTime.tm_yday		=	ptm->tm_yday;
	osTime.tm_isdst		=	-1; // Daylight Savings Time flag /
	
	if ((this->m_valid = isValid()) == true )
		this->os_time_t = this->calcUTC();
	return this->m_valid;
}


bool c_DateTime::set(c_DateTime& dtp)
{
	reset();
	osTime.tm_sec		=   dtp.osTime.tm_sec;    // seconds after the minute -- [0,60] /
	osTime.tm_min		=	dtp.osTime.tm_min;    // minutes after the hour -- [0,59] /
	osTime.tm_hour		=	dtp.osTime.tm_hour;   // hours since midnight -- [0,23] /
	osTime.tm_mday		=	dtp.osTime.tm_mday;   // day of the month -- [1,31] /
	osTime.tm_mon		=	dtp.osTime.tm_mon;    // months since January -- [0,11] /
	osTime.tm_year		=	dtp.osTime.tm_year; // years since 1900 /
	osTime.tm_isdst		=	-1; // Daylight Savings Time flag /
	
	if ((this->m_valid = isValid()) == true )
		this->os_time_t = this->calcUTC();
	return this->m_valid;
}

bool c_DateTime::set(int year, int mon, int mday, int hour, int min, int sec) 
{ 

	reset();
	osTime.tm_sec		=   sec;    // seconds after the minute -- [0,60] /
	osTime.tm_min		=	min;    // minutes after the hour -- [0,59] /
	osTime.tm_hour		=	hour;   // hours since midnight -- [0,23] /
	osTime.tm_mday		=	mday;   // day of the month -- [1,31] /
	osTime.tm_mon		=	mon -1;    // months since January -- [0,11] /
	osTime.tm_year		=	year - 1900; // years since 1900 /
	osTime.tm_isdst		=	-1; // Daylight Savings Time flag /
	
	if ((this->m_valid = isValid()) == true )
		this->os_time_t = this->calcUTC();
	return this->m_valid;
}
 
bool c_DateTime::set(string mmddyyyhhmiss)
/*
	mmddyyyhhmiss = mm/dd/yyyy:24:00::0
*/
{
	//-- valid input ---
	if ( mmddyyyhhmiss.empty() || mmddyyyhhmiss.length() < 10 )
		return false;

	//-- declarations ------
	int yyyy= 0;
	int mm= 0;
	int dd= 0;
	int hh= 0;
	int mi= 0;
	int ss= 0;

	vector<string> tokens;

	//-- split into tokens ------
	tokenize(mmddyyyhhmiss,tokens," :\t-/");

	//-- validate ------
	if (tokens.size() < 3 )
		return false;

	//-- numeric values ------------
	if (tokens[0].size() == 4) {
		yyyy = atoi(tokens[0].c_str());
		mm = atoi(tokens[1].c_str());
		dd = atoi(tokens[2].c_str());
	} 
	else if  (tokens[2].size() == 4) {
		yyyy = atoi(tokens[2].c_str());
		mm = atoi(tokens[0].c_str());
		dd = atoi(tokens[1].c_str());

	}
	else
		return false;

	if  ( tokens.size() > 3 ) 
		hh = atoi(tokens[3].c_str());
	if  ( tokens.size() > 4 ) 
		mi = atoi(tokens[4].c_str());
	if  ( tokens.size() > 5 ) 
		ss = atoi(tokens[5].c_str());


	//-- now make the date
	return set(yyyy, mm, dd, hh, mi, ss); 
}

time_t c_DateTime::calcUTC() 
{ 
	if (!m_valid) return 0; 

	time_t ltime;
	this->osTime.tm_isdst = -1;  // just to be sure

	ltime = mktime(&this->osTime);
	return ltime;
}


time_t c_DateTime::getUTC() 
{ 
	if (!m_valid) return 0; 
	return this->os_time_t;
}


string& c_DateTime::getPinTimeStr(string& pvtime) 
{ 
	pvtime.erase();
	if (!m_valid) return pvtime; 

	ostringstream os;
	os	<< setw(2) << setfill('0') << this->osTime.tm_mon +1
		<< setw(2) << setfill('0') << this->osTime.tm_mday 
		<< setw(2) << setfill('0') << this->osTime.tm_hour 
		<< setw(2) << setfill('0') << this->osTime.tm_min 
		<< setw(4) <<  this->osTime.tm_year+1900;

	pvtime.assign(os.str());

	return pvtime;
}


struct tm* c_DateTime::getTm( struct tm* ptm) 
{ 
	if (!m_valid) return NULL; 

	ptm->tm_mday	= this->osTime.tm_mday; 
	ptm->tm_mon		= this->osTime.tm_mon; 
	ptm->tm_year	= this->osTime.tm_year; 
	ptm->tm_sec		= this->osTime.tm_sec; 
	ptm->tm_min		= this->osTime.tm_min; 
	ptm->tm_hour	= this->osTime.tm_hour;
	ptm->tm_wday	= this->osTime.tm_wday;
	ptm->tm_yday	= this->osTime.tm_yday;
	ptm->tm_isdst	= this->osTime.tm_isdst;

	return ptm;
}


bool c_DateTime::isValid() 
{
  return 
	  (osTime.tm_year+1900 >= 1970 && osTime.tm_year+1900 < 2050) && 
	  (osTime.tm_mon >= 0 && osTime.tm_mon <= 11) && 
	  (osTime.tm_mday >= 1 && osTime.tm_mday <= getLastDay(osTime.tm_year+1900, osTime.tm_mon+1)) &&
      (osTime.tm_hour >= 0 && osTime.tm_hour <= 23) && 
	  (osTime.tm_min >= 0 && osTime.tm_min <= 59) && 
	  (osTime.tm_sec >= 0 && osTime.tm_sec <= 59);
}


int c_DateTime::getLastDay(int year, int mon) 
{  static int dm[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  return (mon != 2)? dm[mon-1] : isLeapYear(year)? 29 : 28;
}



bool c_DateTime::isLeapYear(int year) 
{ 
	return (year%4) == 0 && (year%100 != 0 || year%400 == 0); 
}



void c_DateTime::reset()
{
	this->m_valid = false;
	this->os_time_t = 0;

	osTime.tm_sec		=   0;    // seconds after the minute -- [0,60] /
	osTime.tm_min		=	0;    // minutes after the hour -- [0,59] /
	osTime.tm_hour		=	0;   // hours since midnight -- [0,23] /
	osTime.tm_mday		=	0;   // day of the month -- [1,31] /
	osTime.tm_mon		=	0;    // months since January -- [0,11] /
	osTime.tm_year		=	0; // years since 1900 /
	osTime.tm_wday		=	0;
	osTime.tm_yday		=	0;
	osTime.tm_isdst		=	-1; // Daylight Savings Time flag /
}

void c_DateTime::tokenize(const string& str,
					  vector<string>& tokens,
					  const string& delimiters )
{
	// Skip delimiters at beginning.
	tokens.clear();
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);

	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));

		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);

		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

void c_DateTime::setMidnight(void)
{
    osTime.tm_hour = 0; 
	osTime.tm_min = 0; 
	osTime.tm_sec = 0;
}

bool c_DateTime::isMidnight(void)
{
  return 
	  (osTime.tm_year+1900 >= 1970 && osTime.tm_year+1900 < 2050) && 
	  (osTime.tm_mon >= 0 && osTime.tm_mon <= 11) && 
	  (osTime.tm_mday >= 1 && osTime.tm_mday <= getLastDay(osTime.tm_year+1900, osTime.tm_mon+1)) &&
      (osTime.tm_hour == 0) && 
	  (osTime.tm_min == 0 ) && 
	  (osTime.tm_sec == 0 );
}

bool c_DateTime::backToMidnight(void)
{
	osTime.tm_hour = 0;
	osTime.tm_min = 0 ;
	osTime.tm_sec = 0 ;
	osTime.tm_isdst = -1;
	this->os_time_t = this->calcUTC();
	return this->m_valid;
}

void c_DateTime::addMonths(int months)
{
	int mm = (this->osTime.tm_mon + months);
	this->osTime.tm_mon = (mm % 12);
	this->osTime.tm_year += mm / 12;
	this->os_time_t = this->calcUTC();
}


bool c_DateTime::addSeconds(int seconds)
{
	time_t ltime = this->getUTC();
	ltime += seconds;
	return this->set(ltime);
}

int c_DateTime::compareDays(c_DateTime * ptm)
{
	int dif_value=0;

	if ( !ptm->isValid() || !this->isValid() )
		return 0;  // best we can do - if they are both invalid then they are equal

	dif_value = ( osTime.tm_year - ptm->osTime.tm_year ) * 365;
	dif_value += ( osTime.tm_yday - ptm->osTime.tm_yday );
	return dif_value;
}



int c_DateTime::compare(c_DateTime * ptm)
{
	time_t this_time = this->getUTC();
	time_t that_time = ptm->getUTC();
	
	return (int)(this_time - that_time);
}
/*
	osTime.tm_sec		=   0;    // seconds after the minute -- [0,60] /
	osTime.tm_min		=	0;    // minutes after the hour -- [0,59] /
	osTime.tm_hour		=	0;   // hours since midnight -- [0,23] /
	osTime.tm_mday		=	0;   // day of the month -- [1,31] /
	osTime.tm_mon		=	0;    // months since January -- [0,11] /
	osTime.tm_year		=	0; // years since 1900 /
	osTime.tm_wday		=	0;
	osTime.tm_yday		=	0;
	osTime.tm_isdst		=	0; // Daylight Savings Time flag /
*/
string& c_DateTime::formatDateString(string& str, string fmt)
{
	ostringstream os;
	string format(fmt);
	std::transform(format.begin(), format.end(), format.begin(), (int(*)(int))tolower);
	int y;
	int i;
	char ch;
	//first arg

	str.erase();
	if (!m_valid) return str; 

	for (i=0; i<(int)format.size(); )
	{
		switch (format[i]) {
			case 'y':
				for (y=0; format[i] == 'y' && i<(int)format.size(); i++,y++);
				if (y==2)
					os	<< setw(2) << setfill('0') 
						<< std::setiosflags(ios::fixed|ios::right) 
						<< this->osTime.tm_year - 100;
				else
					os	<< setw(4) << setfill('0') 
						<< std::setiosflags(ios::fixed|ios::right) 
						<< this->osTime.tm_year + 1900;
				break;
			case 'm':
				if ( format[i+1] == 'i' ) {
					i+=2;
					os	<< setw(2) << setfill('0') 
						<< std::setiosflags(ios::fixed|ios::right) 
						<< this->osTime.tm_min;
				}
				else {
					os	<< setw(2) << setfill('0') 
						<< std::setiosflags(ios::fixed|ios::right) 
						<< this->osTime.tm_mon+1;
					for (; format[i] == 'm' && i < (int)format.size(); i++);
				}
				break;

			case 's':
					ch = format[i];
					os	<< setw(2) << setfill('0') 
						<< std::setiosflags(ios::fixed|ios::right) 
						<< this->osTime.tm_sec;
					for (; format[i] == ch && i<(int)format.size(); i++);
					break;
			case 'h':
					ch = format[i];
					os	<< setw(2) << setfill('0') 
						<< std::setiosflags(ios::fixed|ios::right) 
						<< this->osTime.tm_hour;
					for (; format[i] == ch && i < (int)format.size(); i++);
					break;
			case 'd':
					ch = format[i];
					os	<< setw(2) << setfill('0') 
						<< std::setiosflags(ios::fixed|ios::right) 
						<< this->osTime.tm_mday;
					for (; format[i] == ch && i < (int)format.size(); i++);
					break;
			default:
				os << format[i];
				i++;
				break;
		}

	}

	//TODO: return statement
	str.assign( os.str() );
	return str;

}
