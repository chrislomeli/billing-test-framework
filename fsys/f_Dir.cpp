// Dir.cpp: implementation of the f_Dir class.
//
//////////////////////////////////////////////////////////////////////

#include "f_Dir.h"
#include <sys/types.h>

#ifdef WIN32
	#include "winsock2.h"
	#include "windows.h"
	#include <sys/types.h> 
	#include <sys/stat.h> 
	#define S_ISDIR(m) (m &_S_IFDIR)
#endif
#include <util/Utility.h>

static char *CVS_Id = (char*)"CVS_VERSION:$Id: f_Dir.cpp,v 1.4 2008/03/05 03:24:13 kwillens Exp $";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

f_Dir::f_Dir() : dir_open(false), dir_mask("")
{
#ifdef WIN32
	dir_open = true;
#else

#endif

}

f_Dir::~f_Dir()
{

}


bool f_Dir::makePath(string dirname)
{
	if (dirname.empty())
		return false;

	struct stat statbuf;
	string::size_type pos;
	string path;
	vector<string> tokens;


	if ((pos =  dirname.find_last_of(':')) != string::npos )
		path = dirname.substr(pos+1);
	else
		path.assign(dirname);

	util::tokenize(path,tokens,SLASH);
	
	size_t siz  = tokens.size();
	size_t i = 0;
	string makedir;

	for ( i=0; i<siz; i++) {
		makedir.append("/");
		makedir.append(tokens[i]);
		if ( stat( makedir.c_str(), &statbuf ) >= 0 )
		{
			if (S_ISDIR(statbuf.st_mode) != 0 ) 
				continue;
			else 
				return false;
		} else {
			if ( mkdir(makedir.c_str(),00777) != 0 )
				return false;
		}
	}
	return true;
}


void f_Dir::pathWalk ( string dirstr, void (*vist)(string,string) )
{
	string	filename;
	string	fulldir;
	file_type_t	ft;
	void * vp;

	if( (vp=this->OpenDir( dirstr )) == NULL )
		return;

	while ( this->ReadDir( vp, filename, ft ) )
	{
		if ( ft == FILETYPE_DIR ) {
			ostringstream dirss ;
			dirss << dirstr.c_str() << SLASH << filename.c_str();
			pathWalk( dirss.str(), (*vist) );
		}
		else if ( ft == FILETYPE_NORMAL )
			(*vist)(dirstr, filename);
	}
	this->CloseDir(vp);
}


void * f_Dir::OpenDir(string dir) {
	this->current_dir.assign(dir);
	if ( ! this->dir_mask.empty() )
	{
		ostringstream fss;
		fss << this->current_dir << SLASH << this->dir_mask;
		this->full_dirpath = ( fss.str() );
	} 
	else {
#ifdef WIN32
		string windows_bs("\\*.*");
		this->full_dirpath = (this->current_dir+windows_bs);
#else
		this->full_dirpath = (this->current_dir);
#endif
	}
#ifdef WIN32
	HANDLE hFind = NULL;
	WIN32_FIND_DATA data;

	if ((hFind = FindFirstFile(
			(LPCTSTR)this->full_dirpath.c_str(), &data )) == INVALID_HANDLE_VALUE )
		return NULL;
	else
		return (void*)hFind;
#else

	DIR * dp = NULL;
	dp = opendir( this->full_dirpath.c_str() );
	return (void*)dp;
#endif
}


/*
bool f_Dir::ReadDir( string& name, file_type_t& ft )
{
#ifdef WIN32

	//-- next entry --
	if ( !dir_open ) 
		return false;
	else if (hFind)
		if (!FindNextFile(_hFind, &data))
			return false;

	//-- filename --
	name.assign( _data.cFileName );

	//--file type --
	if ( _data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		ft = FILETYPE_DIR;
	else
		ft = FILETYPE_NORMAL;

	return true;

#else
		
	struct stat statbuf;

	//-- next entry --
	if ( !dir_open )
		return false;
	
	while ((dirp = readdir(dp)) != NULL 
		&& !this->goodExtension(dirp->d_name));

	if (dirp == NULL )
		return false;

	//-- filename --
	name.assign(dirp->d_name);
	ft = FILETYPE_OTHER;

	//--file type --
	ostringstream os;
	os << this->current_dir << "/" << dirp->d_name;

	string fullname( os.str() );
	if ( lstat( fullname.c_str(), &statbuf ) >= 0 )
	{
		if (S_ISDIR(statbuf.st_mode) != 0 ) 
			ft = FILETYPE_DIR;
		else if ( S_IFREG(statbuf.st_mode) != 0 )
			ft = FILETYPE_NORMAL;
	}
	return true;

#endif
}
*/


bool f_Dir::ReadDir( void * vdirp, string& name, file_type_t& ft )
{
#ifdef WIN32
	HANDLE hFind = (HANDLE)vdirp;
	WIN32_FIND_DATA data;
	BOOL ret = FALSE;

	//-- next entry --
	if (!hFind)
		return false;

	while (true)
	{
		if ( !(ret=FindNextFile(hFind, &data)) )
			return false;

		//-- filename --
		name.assign( data.cFileName );

		if ( name.compare(".") == 0 || name.compare("..") == 0 )
			continue;

		//--file type --
		if ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			ft = FILETYPE_DIR;
			return true;
		}
		else {
			ft = FILETYPE_NORMAL;
			if ( this->goodExtension(data.cFileName ))
				return true;
		}
	}
	return false;

#else
//cout << "DEBUG: Read Directory: " << endl;
	struct stat statbuf;
	DIR* dirp = (DIR*)vdirp;
	struct dirent *entryp = NULL;
	
	
	if ( dirp == (DIR*)NULL )
		return false;

	//-- next entry --
	while (true)
	{
		//-- next filename --
		if ((entryp = readdir(dirp)) == NULL )
			return false;

		name.assign(entryp->d_name);
//cout << "	DEBUG: File: " << name.c_str() << endl;
		ft = FILETYPE_OTHER;

		if ( name.compare(".") == 0 || name.compare("..") == 0 )
			continue;

		//--file type --
		ostringstream os;
		os << this->current_dir.c_str() << "/" << entryp->d_name;
//cout << "		DEBUG: Build name dir: " << current_dir << endl;
//cout << "		DEBUG: Build name dname: " << entryp->d_name << endl;

		string fullname( os.str() );
//cout << "		DEBUG: fullname=" << fullname << endl;

		if ( lstat( fullname.c_str(), &statbuf ) >= 0 )
		{
			if (S_ISDIR(statbuf.st_mode) != 0 ) {
//cout << "		DEBUG: filetype=directory" << endl;
				ft = FILETYPE_DIR;
				return true;
			}
			else if ( S_ISREG(statbuf.st_mode) != 0 ) {
//cout << "		DEBUG: filetype=file-" ;
				ft = FILETYPE_NORMAL;
				if ( this->goodExtension( entryp->d_name ) ) {
//cout << "		DEBUG: Good" << endl;
					return true;
				}
				else
				{
//cout << "	DEBUG: Skip" << endl;
				}
			}
		}
		else
		{
//cout << "	DEBUG: Can not lstat file " << endl;

		}
	}
//cout << "	DEBUG: eod" << endl;
	return false;

#endif
}

void f_Dir::CloseDir(void* vp)
{
#ifdef WIN32
	HANDLE hFind = (HANDLE)vp;
	FindClose( hFind );
#else
	DIR * dp  = (DIR*)vp;
	closedir(dp);		
#endif
}

void f_Dir::setExtensionList(string extensions)
{
	util::tokenize( extensions, this->extension_list );
}



bool f_Dir::goodExtension(string filename)
{
//cout << "		DEBUG: check extensions for " << filename << "..." << endl;
	//--first: automatic false --
	if ( filename.compare("..") == 0 || filename.compare(".") == 0 )
		return false;

	//--second: mostly true --
	if ( this->extension_list.empty() )
		return true;

//cout << "		DEBUG: we have  extensions..." << endl;
	//--compare against extension list --
	string extstr;

	string::size_type period  = filename.find_last_of(".");
	if ( period == string::npos )
		extstr.assign(".");
	else
		extstr.assign( filename.substr(period+1, filename.length()) );

	std::vector<string>::iterator iptr;

	for (iptr = this->extension_list.begin(); 
		iptr != this->extension_list.end(); iptr++ )
	{
		string this_ext(*iptr);
//cout << "		DEBUG:  compare (" << extstr << ") to (" << this_ext << ")" << endl;
		if ( extstr.compare( this_ext ) == 0 )
			return true; 
	}

	return false;

}

void f_Dir::setMask(string mask)
{
	this->dir_mask.assign(mask);
}

