#ifndef p_Exception__H
#define p_Exception__H

#include <util/EnvironmentStd.h>
#include <ctype/c_Types.h>

#define MAX_ERRLINE 255


class p_Exception
{
public:

	p_Exception(
		Return_code code, 
		const char * _file, 
		int _line, const char * fmt, ...)
	{	
		if (fmt && strlen(fmt) > 0 )
		{
			va_list		ap;
			va_start(ap, fmt);
			vsnprintf(msg, sizeof(msg), fmt, ap);	/* this is safe */
			va_end(ap);
		}
		rc = code;
		file_name = _file;
		line = _line;

	}

	~p_Exception(void)
	{
	}

	friend ostream& operator<<( ostream& of, const p_Exception& c ) 
	{
		of <<  "[" << (int)c.rc << "]:"
			<< c.file_name << ":" << c.line << ": "
			<< c.msg 
			<< endl;
		return of;
	};


	Return_code rc;
	const char * file_name;
	char msg[MAX_ERRLINE];
	int line;
};

/*
inline void
p_Throw(Return_code code, string value="<Null>")
{
	string msg;

	p_Exception * e = new 	p_Exception(
		code, __FILE__, __LINE__, p_Error_map::getMsg(code,msg), value);
	throw e;
}
*/
#endif


