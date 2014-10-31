// f_FileName.cpp: implementation of the f_FileName class.
//
//////////////////////////////////////////////////////////////////////

#include <fsys/f_FileName.h>

static char *CVS_Id = (char*)"CVS_VERSION:$Id: f_FileName.cpp,v 1.3 2007/01/24 01:15:07 clomeli Exp $";


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

f_FileName::f_FileName()
{

}

f_FileName::~f_FileName()
{

}


int f_FileName::setFullPath(string fullpath)
{
	reset();

	if (fullpath.empty())
		return -1;

	this->full_path.assign(fullpath);

	//---parse file and dir names ----------------
	string::size_type dpos = fullpath.find_last_of(SLASH);

	if (dpos == string::npos)  // no path
	{
		this->file_name.assign(fullpath);
		this->dir_name.assign(".");
	}
	else
	{
		this->file_name.assign(fullpath.substr(dpos+1));
		this->dir_name.assign(fullpath.substr(0,dpos));
	}

	//---parse root and ext names ----------------
	string::size_type extpos = this->file_name.find_last_of(PERIOD);

	if (extpos == string::npos)  // no path
	{
		this->file_root_name.assign(this->file_name);
	}
	else
	{
		this->file_root_name.assign(this->file_name.substr(0,extpos));
		this->extension.assign(this->file_name.substr(extpos+1));
	}
	return 0;

}

void f_FileName::reset()
{
	this->extension.erase();
	this->file_name.erase();
	this->full_path.erase();
	this->file_root_name.erase();
}

int f_FileName::setFileName(string dirstr, string _file_name)
{
	reset();

	if (dirstr.empty() || _file_name.empty())
		return -1;

	
	this->full_path.append(dirstr);
	this->full_path.append(STRSLASH);
	this->full_path.append(_file_name);

	//---parse file and dir names ----------------
	this->dir_name.assign(dirstr);
	this->file_name.assign(_file_name);

	//---parse root and ext names ----------------
	string::size_type extpos = this->file_name.find_last_of(PERIOD);

	if (extpos == string::npos)  // no path
	{
		this->file_root_name.assign(this->file_name);
	}
	else
	{
		this->file_root_name.assign(this->file_name.substr(0,extpos));
		this->extension.assign(this->file_name.substr(extpos+1));
	}
	return 0;
}
