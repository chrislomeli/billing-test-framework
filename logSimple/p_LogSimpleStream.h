#ifndef __HEADER__p_LogSimpleStream__HH
#define __HEADER__p_LogSimpleStream__HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <util/EnvironmentStd.h>
#include <util/EnvironmentAnsiC.h>
#include <stdarg.h>
#include <ctype/c_DateTime.h>
#include <logSimple/p_Common.h>
#include <ctype/c_Types.h>



class p_LogSimpleStream
{
public:
	p_LogSimpleStream();
	~p_LogSimpleStream(void);

	bool open(string name);
	ostream& getStream(int level);
	ostream& getErrorStream();

	Return_code	errorLog (Return_code rc, const char* file, int line, const char * fmt, ...);
	bool init( Loglevel _level, string filename );

	string filename;
	ofstream of;
	c_NullOstream nullstream;
	int level;
	void close(void);
};
#endif
