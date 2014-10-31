#include <ctype/c_RuleSet.h>

map<string,string> c_RuleSet::rule_map;

#define EMPTY_STRING ""

c_RuleSet::c_RuleSet(void)
{
}

c_RuleSet::~c_RuleSet(void)
{
	
}

void c_RuleSet::add(map<string,string> rec)
{
	map<string,string>::iterator p1 = rec.begin();
	for ( ; p1 != rec.end(); p1++ )
		c_RuleSet::add( p1->first , p1->second );
}

void c_RuleSet::add(string rule, string value)
{
	if ( rule.empty() ) return;
	if ( value.empty() ) value.assign( "TRUE" );

	std::transform( rule.begin(), rule.end(), rule.begin(), (int(*)(int))toupper);
	map<string,string>::iterator p = c_RuleSet::rule_map.find(rule);

	//--if this is a new head node, Add it----
	if ( p != c_RuleSet::rule_map.end() )
		c_RuleSet::rule_map.erase(p);

	c_RuleSet::rule_map.insert( pair<string,string>( rule, value )); 
}


string& c_RuleSet::getValue(string rule, string& value)
{
	value.erase();
	if (c_RuleSet::rule_map.size()==0)
		return value;

	std::transform( rule.begin(), rule.end(), rule.begin(), (int(*)(int))toupper);
	map<string,string>::iterator p = c_RuleSet::rule_map.find(rule);

	//--if this is a new head node, Add it----
	if ( p != c_RuleSet::rule_map.end() )
		value.assign(p->second);

	return value;
}

bool c_RuleSet::defined(string rule )
{
	if (c_RuleSet::rule_map.size()==0)
		return false;
	if ( rule.empty() )
		return false;

	std::transform( rule.begin(), rule.end(), rule.begin(), (int(*)(int))toupper);
	map<string,string>::iterator p = c_RuleSet::rule_map.find(rule);

	//--if this is a new head node, Add it----
	if ( p != c_RuleSet::rule_map.end() ) {
		string str = p->second;
		std::transform( str.begin(), str.end(), str.begin(), (int(*)(int))toupper);
		if (str.empty())
			return false;
		return true;
	}
	return false;
}

void c_RuleSet::dump_map()
{
	map<string,string>::iterator p = c_RuleSet::rule_map.begin();

	//--if this is a new head node, Add it----
	for ( ; p != c_RuleSet::rule_map.end(); p++ ) {
		string id = p->first;
		string value =  p->second;
		cout << "< " << id << "=" << value << endl;
	}

	return;
}

