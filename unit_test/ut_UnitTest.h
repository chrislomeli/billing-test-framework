#pragma once
#include <util/EnvironmentAnsiC.h>
#include <util/EnvironmentStd.h>

#define TEST_DIRECTORY "c:\\temp\\"
class ut_UnitTest
{
public:
	ut_UnitTest(void);
	~ut_UnitTest(void);

	virtual void test_Driver() = 0;
	string ut_Directory;
	ofstream ut_out;
};
