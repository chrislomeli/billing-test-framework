#pragma once
#include <util/EnvironmentStd.h>
#include <util/ut_UnitTest.h>
#include <ctype/c_Strings.h>
#include <ctype/c_Types.h>
#include <parser/c_ParserFactoryTester.h>

class ut_ParserFactory : public ut_UnitTest
{
public:
	ut_ParserFactory(void);
	~ut_ParserFactory(void);
	void test_Driver();
//	void test_one(string root, ostream& ut_out);
	void test_one(string root);
	void test_two(string root);
	void test_three(string root);
	void test_four(string root);
	void test_five(string root);
	void test_six(string root);
	void test_seven(string root);
	void test_eight(string root);

	c_ParserFactoryTester tester;

};
