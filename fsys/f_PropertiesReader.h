#ifndef __HEADER__f_PropertiesReader__HH
#define __HEADER__f_PropertiesReader__HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <ctype/c_NvRecord.h>
#include <ctype/c_Strings.h>
#include <ctype/c_Types.h>
#include <ctype/c_RuleSet.h>

class f_PropertiesReader
{
public:
	f_PropertiesReader(void);
	~f_PropertiesReader(void);
	void readPropertiesFile( const string target, const string filename );
	virtual void onGroup(string group, c_NvRecord attribs);
	virtual void onGroupFinal(string group);
	virtual void onElem(string group, string elem, string value, c_NvRecord attribs);
};
#endif
