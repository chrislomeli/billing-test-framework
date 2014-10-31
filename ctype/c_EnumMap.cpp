#include <ctype/c_EnumMap.h>


c_EnumMap::c_EnumMap(void) : is_initialized(false)
{
	enum_map.clear();
	this->reset();
}


c_EnumMap::~c_EnumMap(void)
{
	enum_map.clear();
}

bool c_EnumMap::isValid(void)
{
	return this->is_valid;
}

void c_EnumMap::reset(void)
{
	this->value = -1;
	this->is_valid = false;
}


void c_EnumMap::insert(string name, int _value, bool is_default)
{
	if (name.empty())
		return;

	string str = name;
	std::transform( str.begin(), str.end(), str.begin(), (int(*)(int))toupper);

	this->enum_map.insert( pair<string,int>( str, _value )); 
	if (is_default)
		this->reverse_map.insert( pair<int,string>( _value, name )); 

	this->keys.push_back(_value);
}

bool c_EnumMap::isValid(int key)
{
	std::vector< int >::const_iterator p1 = this->keys.begin();

	for ( ; p1 != this->keys.end(); p1++ ) {
		int i = *p1;
		if (key == i)
			return true;
	}
	return false;
}


int c_EnumMap::get() {
	if (this->is_valid)
		return this->value;
	return NULL_ENUM;
}

bool c_EnumMap::set(int* ip)
{
	if (ip == NULL) return false;
	int test = *ip;
	return (this->set(test));
}

bool c_EnumMap::set(int test)
{
	reset();

	if (this->enum_map.size()==0)
		this->initialize_map();

	if ( ! this->isValid(test) )
		return false;
	this->value = test;
	this->is_valid = true;
	return true;
}

bool c_EnumMap::equals(int test)
{
	return ( this->value == test );
}

/*

int c_EnumMap::lookUp(string instr)
{
	if (this->enum_map.size()==0)
		return -1;

	string name = instr;
	std::transform( name.begin(), name.end(), name.begin(), (int(*)(int))toupper);

	map<string,int>::iterator p = this->enum_map.find(name);

	//-- if not found return null----
	if ( p == this->enum_map.end() )
		return -1;

	//-- if not found return null----
	int test = p->second;
	return ( test );
}
*/

bool c_EnumMap::equals(string instr)
{
	if (this->enum_map.size()==0)
		return false;

	string name = instr;
	std::transform( name.begin(), name.end(), name.begin(), (int(*)(int))toupper);

	map<string,int>::iterator p = this->enum_map.find(name);

	//-- if not found return null----
	int test = atoi( instr.c_str());
	if ( p != this->enum_map.end() )
		test = p->second;
	else if ( ! this->isValid( test ) )
		return false;

	//-- if not found return null----
	return ( this->value == test );
}

bool c_EnumMap::set(string instr)
{

	if (this->enum_map.size()==0)
		initialize_map();

	if (this->enum_map.size()==0)
		return false;

	string name = instr;
	std::transform( name.begin(), name.end(), name.begin(), (int(*)(int))toupper);

	map<string,int>::iterator p = this->enum_map.find(name);

	//-- if not found, see if this is a string 
	//		representation of one of the existing keys----
	int test = 0;
	if ( p != this->enum_map.end() ) 
		test = p->second;
	else
		test = atoi( instr.c_str() );

	//-- set if valid ----
	return this->set(test);
}

void c_EnumMap::dump_map(void)
{
	map<string,int>::iterator p1 = this->enum_map.begin();
	for ( ; p1 != this->enum_map.end(); p1++ )
		cout << p1->first << " == " << p1->second << endl;
}


string& c_EnumMap::getString(string& outstr)
{
	if ((is_valid == false) || this->enum_map.size()==0) {
		outstr.assign("<<invalid value>>");
		return outstr;
	}

	map<int,string>::iterator p = this->reverse_map.find(this->value);

	//-- if not found return null----
	if ( p == this->reverse_map.end() ) {
		ostringstream os;
		os << this->value;
		outstr.assign(os.str());
		return outstr;
	}

	//-- if not found return null----
	outstr.assign(p->second);
	return outstr;
}
