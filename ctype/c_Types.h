#if !defined(AFX_C__TYPE_H__D2500382_3A76_4DE5_89FA_287B648CD4B9__INCLUDED_)
#define AFX_C__TYPE_H__D2500382_3A76_4DE5_89FA_287B648CD4B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <util/EnvironmentAnsiC.h>
#include <stdarg.h>

typedef	enum Parse_type_tag {
		FixedLength,
		Delimited,
		XML,
		NameValueString,
		Parse_Type_Undefined
} Parse_Type_t;

typedef enum DataType_tag {
	Undef_t = '\0',
	Integer_t = 'I',
	Long_t = 'L',
	Time_t = 'T',
	String_t = 'S',
	Date_t = 'D',
	Ulong_t = 'U',
	Float_t = 'F',
	Double_t = 'N',
	Bool_t = 'B'
} DataType_t;

class c_Nullbuff : public std::streambuf
{
public:
	c_Nullbuff ()
	{
		setbuf (0, 0);
	}
protected:
	int overflow (int)
	{
		setp (pbase (), epptr () );
		return 0;
	}
	int sync ()
	{
		return 0;
	}
};

class c_NullOstream : public std::ostream
{
public:
	c_NullOstream () :
	  std::ostream (& buff) { }
private:
	c_Nullbuff buff;
};


struct c_FormatString 
{
	void fromString (string val, string mask="" ) throw() {
		this->str.assign(val);
	}

	const c_FormatString& operator=(string ss) {
		this->str.assign(ss);
		return *this;
	}
	const c_FormatString& operator=(const char * s) {
		this->str.assign(s);
		return *this;
	}
	operator void*() const 
	{
		const char * sz = this->str.c_str();
		return (void*)sz;
	}
	string str;
};

struct c_FieldSpec
{
	c_FieldSpec()
		: data_type(Undef_t)
	{
	}
	c_FieldSpec(const c_FieldSpec& c){
		name = c.name;
		mask = c.mask;
		data_type = c.data_type;
		ignore = c.ignore;
		key=c.key;
	}
	string name;
	string mask;
	DataType_t data_type;
	bool ignore;
	bool key;
};

struct c_NvPair 
{
	c_NvPair(){	clear(); }

	c_NvPair(const c_NvPair& _c)
		: data_type(Undef_t)
		, keyval(false)
	{
		clear();
		value.assign(_c.value);
		data_type = _c.data_type;
		mask.assign(_c.mask);
		name.assign(_c.name);
		keyval = _c.keyval;
		ignore = _c.ignore;
	}
	c_NvPair(const c_NvPair * _c)
		: data_type(Undef_t)
		, keyval(false)
	{
		clear();
		value.assign(_c->value);
		data_type = _c->data_type;
		mask.assign(_c->mask);
		name.assign(_c->name);
		keyval = _c->keyval;
		ignore = _c->ignore;
	}
	
	
	c_NvPair(c_FieldSpec _f, string _v){
		clear();
		set( _f, _v );
	}
	
	void clear()
	{
		value.erase();
		data_type = Undef_t;
		mask.erase();
		name.erase();
		keyval = false;
		ignore = false;
	}
	void set( c_FieldSpec _f, string _v ){
		name.assign(_f.name);
		value.assign(_v);
		data_type = _f.data_type;
		mask.assign(_f.mask);
		keyval = _f.key;
		ignore = _f.ignore;
	}
	void set( string _name, string _value, bool _key, bool _ignore,  DataType_t _type = Undef_t ){
		this->clear();
		name.assign(_name );
		value.assign( _value );
		keyval = _key;
		ignore = _ignore;
		data_type = _type;
	}

	friend ostream& operator<<(ostream& out, const c_NvPair& c )
	{
		out << c.name << "==" << c.value;
		return out;
	}

	string name;
	string mask;
	DataType_t data_type;
	string value;
	bool keyval;
	bool ignore;
};


template <class T> 
struct c_Field
{
	c_Field() : b_empty(true) {};
	c_Field(T in) : b_empty(false) { set(in); };
	c_Field(string _name, T _data) : b_empty(false) { set(_name, _data); };
	c_Field(c_NvPair * nvpair)  
	{ 
			if (!nvpair)
			{
				clear();			
				return;
			}
			this->data.fromString( nvpair->value, nvpair->mask );
			this->name = nvpair->name;
			this->keyval = nvpair->keyval;
	};

	virtual const c_Field<T>& operator=(c_NvPair * nvpair) {
		if (!nvpair)
		{
			clear();			
		}
		else
		{
			this->data.fromString( nvpair->value, nvpair->mask );
			this->name = nvpair->name;
			this->keyval = nvpair->keyval;
		}
		return *this;
	}
	
	void set( string _name, T _data ) { 
		this->data = _data; 
		this->name = _name;
		b_empty=false; 
	};
	
	void set( T in ) { 
		this->data = in; 
		b_empty=false; 
	};
	
	T& get() { 
		if (this->empty()) 
			throw range_error("data not set"); 
		return data; 
	};
	
	string& Name() { 
		if (this->empty()) 
			throw range_error("data not set"); 
		return name; 
	};

	bool empty() { 
		return b_empty; 
	}

	void clear() {
		b_empty = true;
	}
	
	string name;
	T data;
	bool b_empty;
	bool keyval;

};


struct c_DataType {


	static DataType_t dataType(string str) 
	{
		if (str.empty())
			return Undef_t;

		char dch = toupper(str[0]);
		return c_DataType::dataType(dch);
	};
	static DataType_t dataType(const char * ds) 
	{
		if (!ds || strlen(ds) < 1)
			return Undef_t;

		char dch = toupper(ds[0]);
		return c_DataType::dataType(dch);
	};
	static DataType_t dataType(char dch) 
	{
		switch(dch)
		{
		case 'I': 
		case 'L': 
		case 'T': 
		case 'S': 
		case 'D': 
		case 'U': 
		case 'F':
		case 'N':
			return (DataType_t)dch;
		default:
			return Undef_t;
		}
	};
	
};


typedef enum Return_code_tag {
//--Field errors------------------------------
	Field_not_found		= -301,
	NullValue			= -303,
	Duplicate_found		= -304,
	Not_found			= -305,
	Bad_Type			= -306,
	BadValue			= -307,
	ConfigError			= -309,
	MemRangeError		= -310,

//--Parse errors---------------------------
	Found_text		= -100,
	Wrong_element		= -101,
	ParseError		= -102,
	ConversionError	= -103,
	End_of_block		= -104,


//--IO Errors -------------------------------
	BadFileOpen		= -200,
	BadFileStream	= -201,
	EndOfFile		= -202,
	End_element		= -203,
	Insert_failed	= -204,
	BadFilePosition	= -205,
	BadFileWrite	= -206,
	BadFileDelete	= -207,

//--Connection------------------------------------
	ConnectionFailed	= -501,
//--Batch processing------------------------------------
	CompleteWithErrors	= -401,
	NothingToDo		= -402,

//--Generic------------------------------------
	NotSet			= 0,
	NotSupported	= -02,
	Failure			= -01,
	Success			= 1

} Return_code;


typedef enum parseErrType_tag {
	not_set = 0,
	definition_error = 1,
	parse_error = 2,
	environment_error = 3,
	application_error = 4,
	input_error =5
} ErrorType;

struct ErrorRecord {
	Return_code errCode;
	int SystemErr;
	int position;
	string desc;
	ErrorType errType;  // 1=definition, 2=parse, 0=not set

	ErrorRecord () { clear(); }
	ErrorRecord (Return_code rc,
			ErrorType et,
			int syserr,
			const char * fmt, ...)
	{
		char	buf[255];
		va_list		ap;
		va_start(ap, fmt);
		vsnprintf(buf, sizeof(buf), fmt, ap);	/* this is safe */
		this->set(rc,0,string(buf),syserr );
		va_end(ap);
	}


	Return_code set(
		Return_code rc,
		int pos,
		string dsc,
		int syserr = 0,
		ErrorType et = not_set )
	{
		clear();
		errCode = rc;
		SystemErr = syserr;
		position = pos,
		desc.assign(dsc);
		errType = et;
		return rc;
	}

	Return_code
	set (	Return_code rc,
			ErrorType et,
			int syserr,
			const char * fmt, ...)
	{
		char	buf[255];

		va_list		ap;
		va_start(ap, fmt);
		vsnprintf(buf, sizeof(buf), fmt, ap);	/* this is safe */
		va_end(ap);
	
		return (this->set(rc,0,string(buf),syserr ));
	}


	friend ostream& operator<<( ostream& of, const ErrorRecord& c ) 
	{
		of << "errCode=" << (int)c.errCode 
			<< ", SystemErr=" << c.SystemErr 
			<< ", pos=" << c.position
			<< ", desc=" << c.desc 
			<< ", ErrorType=" << (int)c.errType
			<< endl;
		return of;
	};

	void dump(ostream& out) {
		out << "errCode=" << (int)errCode 
			<< ", SystemErr=" << SystemErr 
			<< ", pos=" << position
			<< ", desc=" << desc 
			<< ", ErrorType=" << (int)errType
			<< endl;
	}

	Return_code errnoToRC(int err, Return_code default_rc)
	{
		switch(err)
		{
			case EBADF:
				return BadFileStream;
			default:
				return default_rc;
		}
	}

	Return_code set(ErrorRecord er)
	{
		errCode = er.errCode;
		SystemErr = er.SystemErr;
		position = er.position;
		desc.assign(er.desc);
		errType = er.errType;
		return errCode;
	}

	bool isError()
	{
		return (errCode != NotSet && errCode != Success);
	}

	void clear()
	{
		position=0;
		SystemErr=0;
		errCode=NotSet;
		errType = not_set;
		desc.erase();
	}
};


#endif
