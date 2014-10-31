#ifndef C__c_EnumMap__H
#define C__c_EnumMap__H

#include <util/EnvironmentStd.h>

#define NULL_ENUM -99999

class c_EnumMap
{
public:
	c_EnumMap(void);
	~c_EnumMap(void);

	virtual void initialize_map() = 0;

	virtual bool isValid(int key);
	bool isValid(void);
	void reset(void);
	void insert(string rule, int value, bool is_default = false);
	bool set(string);
	bool set(int*);
	bool set(int);
	int get();
	bool equals(int test);
	bool equals(string instr);

	map<int,string> reverse_map;
	map<string,int> enum_map;
	vector<int> keys;
	
	int value;
	bool is_valid;
	bool is_initialized;
	void dump_map(void);
	string& getString(string& outstr);
//	static int lookUp(string);
};

#endif

