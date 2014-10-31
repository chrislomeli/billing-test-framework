#ifndef C__RULE_SET__H
#define C__RULE_SET__H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>

class c_RuleSet
{
public:
	c_RuleSet(void);
	~c_RuleSet(void);

	static map<string,string> rule_map;

	static string& getValue(string rule, string& value);

	static bool defined(string rule);
	static void dump_map();

	static void add(string rule, string value = "");
	static void add(map<string,string> rec);
};

#endif
