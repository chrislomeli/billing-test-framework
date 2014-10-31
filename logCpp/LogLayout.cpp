// LogLayout.cpp: implementation of the LogLayout class.
//
//////////////////////////////////////////////////////////////////////

#include "LogLayout.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogLayout::LogLayout()
{

}

LogLayout::~LogLayout()
{

}

string LogLayout::layoutMessage( EventLogType& e )
{
	char	ecode;
	char	buf[60];
	time_t ltime;
	struct tm* thistime;

	//--datetime--
	time(&ltime);
	thistime = localtime( &ltime ); /* Convert to local time. */

	switch(e._priority)
	{
	case level_Warning:
		ecode='W';
		break;
	case level_Error:
		ecode='E';
		break;
	case level_Alert:
		ecode='A';
		break;
	case level_Critcal:
		ecode='C';
		break;
	case level_Trace:
		ecode='T';
		break;
	case level_Output:
	default:
		ecode='I';
		break;
	}

	//--write--
	sprintf(buf,"%c:[%02d/%02d %02d:%02d:%02d]:", 
		ecode, 
		thistime->tm_mon+1,
		thistime->tm_mday,
		thistime->tm_hour,
		thistime->tm_min,
		thistime->tm_sec);

	ostringstream os;
	os << buf << e._message;

	 e._message.assign(os.str());
	 return e._message;

};

