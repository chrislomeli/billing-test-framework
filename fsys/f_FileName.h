// f_FileName.h: interface for the f_FileName class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F_FILENAME_H__23EE107F_3B57_4590_8CC1_3D52110E6D9C__INCLUDED_)
#define AFX_F_FILENAME_H__23EE107F_3B57_4590_8CC1_3D52110E6D9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <util/EnvironmentStd.h>
#include <platform/Portability.h>


struct f_FileName  
{
public:
	void reset();

	f_FileName();
	virtual ~f_FileName();

	string full_path;		/// /usr/local/bin/try.txt
	string file_name;		/// try.txt
	string dir_name;		/// /usr/local/bin
	string file_root_name;	/// try
	string extension;		//  txt

	int setFullPath(string fullpath);
	int setFileName(string dirstr, string file_name);
};

#endif // !defined(AFX_F_FILENAME_H__23EE107F_3B57_4590_8CC1_3D52110E6D9C__INCLUDED_)
