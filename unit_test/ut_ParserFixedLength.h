#pragma once
#include <util/EnvironmentAnsiC.h>
#include <util/EnvironmentStd.h>
#include <unit_test/ut_UnitTest.h>
#include <parser/c_ParserFixedLength.h>

class ut_ParserFixedLength : public ut_UnitTest
{
public:
	ut_ParserFixedLength(void);
	~ut_ParserFixedLength(void);

	void test_Driver();
	void test_parse(c_ParserFixedLength& parser, string record, ofstream& of);

};
