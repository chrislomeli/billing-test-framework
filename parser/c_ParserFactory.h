#ifndef __HEADER__c_ParserFactory__HH
#define __HEADER__c_ParserFactory__HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <ctype/c_Strings.h>
#include <parser/c_Parser.h>
#include <ctype/c_Types.h>


class c_ParserFactory
{
public:
	c_ParserFactory(void);
	~c_ParserFactory(void);

	c_Parser * getParser(istream& defines);
	c_Parser * getParser(string state);
	Return_code readParserConfigFile(const char * filename);
	int dumpConfigs(ostream& out);

	ErrorRecord factoryErrorState;
	map<string,c_Parser*> parsers;
	void addParser(string name, c_Parser * parser);
};
#endif
