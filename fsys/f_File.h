// f_File.h: interface for the f_File class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F_FILE_H__29427592_1BE3_4A0E_86C1_51DD5B0D7620__INCLUDED_)
#define AFX_F_FILE_H__29427592_1BE3_4A0E_86C1_51DD5B0D7620__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <util/EnvironmentAnsiC.h>
#include <platform/Portability.h>
#include <fsys/f_FileName.h>
#include <ctype/c_Strings.h>

class f_File  
{
public:
	f_File();
	virtual ~f_File();

	static bool Access(string filestr, int flag)
	{
		if ( access( filestr.c_str(), flag ) == 0 )
			return true;
		else
			return false;
	};

	static int createDatestampFile (
					string szdir,		// starting dir 
					string szroot,		// root of file name
					string szext,		// optional extension
					string stamp,		//date stamp in file name:: "YYYYMMDD" or "UTC" or ""
					string subdirs,		//to create yyyy/mm/dd sibdirs:: "YYYY/MM/DD"
					string &newfile );	//OUT:  name of file created

	static int mkdirRecursive(string dirstr);

	static short copyTextFile(string sourcefle, string destfile);
	static void dumpFile(const char * full_path, ostream& ut_out);
};

#endif // !defined(AFX_F_FILE_H__29427592_1BE3_4A0E_86C1_51DD5B0D7620__INCLUDED_)
