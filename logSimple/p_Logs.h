#ifndef __HEADER__p_Logs__HH
#define __HEADER__p_Logs__HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <util/EnvironmentStd.h>    
#include <ctype/c_DateTime.h>    
#include <logSimple/p_LogSimpleStream.h>

#define	_errorLog	p_Logs::errLog.getErrorStream() << "<ERROR> "
#define	_debugLog(x)	p_Logs::errLog.getStream(x) 
#define	_userLog(x)	p_Logs::userLog.getStream(x)

class p_Logs
{
public:
	p_Logs(void);
	~p_Logs(void);

	static p_LogSimpleStream errLog;
	static p_LogSimpleStream userLog;

};
#endif

