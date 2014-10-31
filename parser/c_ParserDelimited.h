#ifndef __HEADER__c_ParserDelimited__HH
#define __HEADER__c_ParserDelimited__HH
#ifdef WIN32
#pragma once
#endif

#include <util/EnvironmentStd.h>
#include <ctype/c_Strings.h>
#include <ctype/c_Types.h>
#include <parser/c_Parser.h>

class c_ParserDelimited : public c_Parser
{
public:


	string m_delimiters;

	

	void clearState()
	{
		this->errorRec.clear();
		this->column_formats.clear();	
		m_reject_overlaps = true;
		m_protect_field_name = true;
		m_delimiters.assign(" ");
	}
	void clear()
	{
		this->clearState();
		this->clearDefinitions();
	}

	Return_code parseRecord(string data, map<string,c_NvPair>& tokens)
	{
		Return_code rc = NotSet;
		this->errorRec.clear();

		//---toeknize string----
		vector<string> token_strings;
		c_Strings::tokenize(data,token_strings, this->m_delimiters);
		int token_count = (int)token_strings.size();


		vector<c_ColumnDefinition>::iterator pos = this->column_formats.begin();
		for (; pos != this->column_formats.end(); pos++ )
		{
			c_ColumnDefinition cd = *pos;
			int zpos = cd.position-1;  

			if (zpos >=  token_count ){
				rc =  this->errorRec.set(ParseError,cd.position,"data record not enough tokens",0,parse_error);
				return rc;
			}
			string value = token_strings[zpos];

			if (cd.trim_left && cd.trim_right)
				c_Strings::trimAll(value);
			else if (cd.trim_left) 
				c_Strings::trimLeft(value);
			else if (cd.trim_right)
				c_Strings::trimRight(value);


			//-- pass configured attributes here -----------
			c_NvPair cf( cd.field_spec, value );
			tokens.insert( pair<string,c_NvPair>(cd.field_spec.name,cf) );
		}
		return Success;
	}

	c_ParserDelimited(void);
	~c_ParserDelimited(void);
};
#endif

