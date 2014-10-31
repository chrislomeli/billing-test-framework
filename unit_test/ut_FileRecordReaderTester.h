#ifndef __HEADER__ut_FileRecordReaderTester__HH
#define __HEADER__ut_FileRecordReaderTester__HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentAnsiC.h>
#include <util/EnvironmentStd.h>
#include <fsys/f_FileRecordReader.h>

class ut_FileRecordReaderTester 
{
public:
	ut_FileRecordReaderTester(void);
	~ut_FileRecordReaderTester(void);

	//--test funcs---------------
	void runTest(f_FileRecordReader * readerp, string fullpath, ostream&ut_out );
	void dump_file(const char * full_path, ostream& out);
};
#endif