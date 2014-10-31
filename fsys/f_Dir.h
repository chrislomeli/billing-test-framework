// Dir.h: interface for the f_Dir class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIR_H__D2500382_3A76_4DE5_89FA_287B648CD4B9__INCLUDED_)
#define AFX_DIR_H__D2500382_3A76_4DE5_89FA_287B648CD4B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/Utility.h>
#include <util/EnvironmentStd.h>
#include <util/Portability.h>

enum file_type_t 
{
	FILETYPE_DIR,
	FILETYPE_NORMAL,
	FILETYPE_OTHER 
};



class f_Dir  
{
public:
	f_Dir();
	virtual ~f_Dir();

	void * OpenDir(string dir);
	bool ReadDir( void * dirp, string& name, file_type_t& ft );
	void CloseDir(void* vp);
	void pathWalk ( string dirstr, void (*vist)(string,string)  );

	void setMask( string mask );
	void setExtensionList(string extensions);
	bool goodExtension(string filename);
	static bool makePath(string dirname);

	// for now, calling programs have to reset the dirname on recursion 
	void setDirName( string dirstr ) {
		this->current_dir.assign(dirstr);
	};



protected:
	bool dir_open;
	string current_dir;
	string full_dirpath;
	string dir_mask;

	vector<string> extension_list;
};

#endif // !defined(AFX_DIR_H__D2500382_3A76_4DE5_89FA_287B648CD4B9__INCLUDED_)

