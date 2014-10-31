#include <logSimple/p_LogSimpleStream.h>
#include <fsys/f_File.h>
#include <fsys/f_FileName.h>


p_LogSimpleStream::p_LogSimpleStream( )
: level(0)
{

}

p_LogSimpleStream::~p_LogSimpleStream(void)
{
	if ( of )
		this->of.close();
}

ostream& p_LogSimpleStream::getErrorStream()
{
	if (level > this->level)
		return p_LogSimpleStream::nullstream;

	if ( !of.is_open()  && ! open(this->filename ) )
		return cerr;

	return this->of;
}

ostream& p_LogSimpleStream::getStream(int _level)
{
	if (_level > this->level)
		return p_LogSimpleStream::nullstream;

	if ( !of.is_open()  && ! open(this->filename ) )
		return cout;

	return this->of;
}


bool p_LogSimpleStream::open(string _file)
{
	if (_file.empty())
		return false;

	//--make the spool root dir ----------------------------------
	f_FileName f; f.setFullPath(_file);
	if (0 != f_File::mkdirRecursive(f.dir_name))
	{	
		cout << "Can not create recursive dir: " << f.dir_name << endl;
		return false;
	};

	///----create the config file------
	if ( of.is_open() )
		this->of.close();

	this->of.open(_file.c_str(), ios::app);
	if (!of.is_open()){
		return false;
	}

	this->filename.assign(_file);
	return true;
}


void p_LogSimpleStream::close(void)
{
	if ( of.is_open() )
		this->of.close();
}


Return_code
p_LogSimpleStream::errorLog (
		Return_code rc, const char* file, int line, const char * fmt, ...)
{
	char	buf[MAX_ERRLINE];
	va_list		ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);	/* this is safe */
	va_end(ap);

	this->of << __ERR_TAG__ << __ONOWHERE__ << buf << endl;
	return rc;
}

bool p_LogSimpleStream::init(Loglevel _level, string _filename)
{
	if (!this->open(_filename))
		return false;
	this->level = _level;
	this->filename = _filename;
	return true;
}
