#include "t_FunctorMap.h"


t_FunctorMap::t_FunctorMap(void) : is_initialized(false)
{
	enum_map.clear();
	this->reset();
}


t_FunctorMap::~t_FunctorMap(void)
{
	enum_map.clear();
}

void t_FunctorMap::insert(string name, T*& _value)
{
	if (name.empty())
		return;

	string str = name;
	std::transform( str.begin(), str.end(), str.begin(), (int(*)(int))toupper);

	this->enum_map.insert( pair<string,T*&>( str, _value )); 
}


T*& t_FunctorMap::getInstance(string instr)
{
	if (this->enum_map.size()==0)
		return NULL;

	string name = instr;
	std::transform( name.begin(), name.end(), name.begin(), (int(*)(int))toupper);

	map<string,int>::iterator p = this->enum_map.find(name);

	//-- if not found return null----
	if ( p == this->enum_map.end() )
		return NULL;

	//-- if not found return null----
	T*& test = p->second;
	return ( test );
}


void t_FunctorMap::dump_map(void)
{
	map<string,int>::iterator p1 = this->enum_map.begin();
	for ( ; p1 != this->enum_map.end(); p1++ )
		cout << p1->first << " == " << p1->second << endl;
}


