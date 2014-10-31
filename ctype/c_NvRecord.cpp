#include <ctype/c_NvRecord.h>

c_NvRecord::c_NvRecord(void) : use_case_sensitive_names(true)
{
	this->reset();
}

c_NvRecord::~c_NvRecord(void)
{
	this->reset();
}

void c_NvRecord::ignoreCase(bool ignore_case)
{
	this->use_case_sensitive_names = !ignore_case;
}

void c_NvRecord::reset(void)
{
	nv_map.clear();
}


void c_NvRecord::set(string _fld, string _value )
{
	if (_fld.empty())
		return;
//	if (_value.empty())
//		return;

	string str = _fld;
	if (use_case_sensitive_names) std::transform( str.begin(), str.end(), str.begin(), (int(*)(int))toupper);

	this->nv_map[str] = _value;
//	this->nv_map.insert( pair<string,string>( str, _value )); 
}


void c_NvRecord::dump_map(void)
{
	map<string,string>::iterator p1 = this->nv_map.begin();
	for ( ; p1 != this->nv_map.end(); p1++ )
		cout << p1->first << " == " << p1->second << endl;
}

string& c_NvRecord::StringBuffer(string& title)
{
	ostringstream os;

	os << endl
		<< "================================================\n"
		<< title << endl
		<< "================================================\n";

	map<string,string>::iterator p1 = this->nv_map.begin();
	for ( ; p1 != this->nv_map.end(); p1++ ) {
		os << p1->first << " == " << p1->second << endl;
	}

	os	<< "================================================\n";
	title.assign( os.str() );
	return title;
}

bool c_NvRecord::exists(string _fld)
{
	if (this->nv_map.size()==0)
		return false;

	string name = _fld;
	if (use_case_sensitive_names) std::transform( name.begin(), name.end(), name.begin(), (int(*)(int))toupper);

	map<string,string>::iterator p = this->nv_map.find(name);

	//-- if not found return null----
	if ( p == this->nv_map.end() )
		return false;

	//-- if not found return null----
	return true;
}


string c_NvRecord::value(string _fld)
{
	if (this->nv_map.size()==0)
		return "";

	string name = _fld;
	if (use_case_sensitive_names) std::transform( name.begin(), name.end(), name.begin(), (int(*)(int))toupper);

	map<string,string>::iterator p = this->nv_map.find(name);

	//-- if not found return null----
	if ( p == this->nv_map.end() )
		return "";

	//-- if not found return null----
	return ( p->second );
}



int c_NvRecord::size()
{
	return (int)this->nv_map.size();
}

int c_NvRecord::parse_nv_argument_string(
	const string argument_line,
	const string nva_delimiter,
	const string nva_equal_sign
	)
{ 
	// argument_line=DROP=1027|ADD=1034|DEBIT_NUM=4444111122223333| ....
	vector<string> nv_args;
	vector<string> one_argv;

	// this->reset(); -resposibility of calling function

	//-- tokenize by delimiter ---
	this->tokenize( argument_line, nv_args, nva_delimiter );

	vector<string>::iterator pos = nv_args.begin();
	
	// for each arg (ADD=1027), separate by equals sign and add to nvmap
	for ( ; pos != nv_args.end(); pos++ )
	{
		string arg_str =  *pos;  // Name=Value
		insert_nv_pair(arg_str, nva_equal_sign);
	}
	return (int)this->nv_map.size();
}

int c_NvRecord::insert_nv_pair(string equality, string nva_equal_sign)
{
	string::size_type pos;

	string name;
	string value;

	if (equality.size() == 0)
		return 0;
	if ((pos=equality.find(nva_equal_sign)) == string::npos || pos == 0)
		return 0;

	name.assign( equality.substr(0,pos) );
	value.assign( equality.substr(pos+1) );

	this->set(name,value);
	return 1;
}
