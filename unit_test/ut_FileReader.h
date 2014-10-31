#pragma once
#include <util/EnvironmentAnsiC.h>
#include <util/EnvironmentStd.h>
#include <util/ut_UnitTest.h>
#include <fsys/f_FileReader.h>

class ut_FileReader : public ut_UnitTest, public f_FileReader
{
public:
	ut_FileReader(void);
	~ut_FileReader(void);

	void create_file(string dir, string file, string contents );
	void test_Driver();
	void test_one(ostream& ut_out);
	void test_two(ostream& ut_out);
	void test_three(ostream& ut_out);

};
