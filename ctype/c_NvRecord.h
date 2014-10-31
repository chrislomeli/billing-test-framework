#ifndef C__c_NvRecord__H
#define C__c_NvRecord__H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>


class c_NvRecord 
{
public:
	int size();
	c_NvRecord(void);
	~c_NvRecord(void);
	void ignoreCase(bool ignore_case);

	int parse_nv_argument_string(
		const string argument_line, 
		const string delim, 
		const string equal_sign );

	void reset(void);
	void set(string _fld, string _value );

	string value( string _fld );
	bool exists( string _fld );
	void dump_map();
	string& StringBuffer(string& title);

	void tokenize(const string& str,
		vector<string>& tokens,
		const string& delimiters = " ")
	{
		// Skip delimiters at beginning.
		string::size_type lastPos = str.find_first_not_of(delimiters, 0);

		// Find first "non-delimiter".
		string::size_type pos     = str.find_first_of(delimiters, lastPos);

		while (string::npos != pos || string::npos != lastPos)
		{
			// Found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));

			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);

			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}
	};


	map<string,string> nv_map;
	bool use_case_sensitive_names;
	int insert_nv_pair(string equality, string nva_equal_sign = "=");
};

#endif

