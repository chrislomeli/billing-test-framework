#pragma once
#include <util/EnvironmentAnsiC.h>
#include <util/EnvironmentStd.h>
#include <unit_test/ut_UnitTest.h>
#include <fsys/f_Spooler.h>

class ut_Spooler : public ut_UnitTest
{
public:
	ut_Spooler(void);
	~ut_Spooler(void);

	void create_file(string dir, string file, string contents );
	void test_Driver();
	void test_one(ostream& ut_out);
	void test_two(ostream& ut_out);
	void test_three(ostream& ut_out);

};
