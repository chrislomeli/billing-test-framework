#pragma once
#include <util/EnvironmentStd.h>
#include <unit_test/ut_UnitTest.h>
#include <ctype/c_Strings.h>
#include <ctype/c_Types.h>
#include <parser/c_ParserFactory.h>

class c_ParserFactoryTester 
{
public:
	c_ParserFactoryTester(void);
	~c_ParserFactoryTester(void);
	void runTest(const char * config_file, ostream& out);
	void runTest(const char * config_file);
	void findParser(string name);


	vector<string> names;
};
