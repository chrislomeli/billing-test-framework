#pragma once
#include <util/EnvironmentAnsiC.h>
#include <util/EnvironmentStd.h>
#include <unit_test/ut_UnitTest.h>
#include <parser/c_ParserDelimited.h>

class ut_ParserDelimited : public ut_UnitTest
{
public:
	ut_ParserDelimited(void);
	~ut_ParserDelimited(void);

	void test_Driver();
	void test_parse(c_ParserDelimited& parser, string record, ofstream& of);

};
