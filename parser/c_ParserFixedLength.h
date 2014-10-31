#ifndef __HEADER__c_ParserFixedLength__HH
#define __HEADER__c_ParserFixedLength__HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <parser/c_Parser.h>
#include <ctype/c_Strings.h>
#include <ctype/c_Types.h>


class c_ParserFixedLength : public c_Parser
{
public:


	void clearState()
	{
		this->errorRec.clear();
		this->column_formats.clear();	
		m_reject_overlaps = true;
		m_protect_field_name = true;
	}
	void clear()
	{
		this->clearState();
		this->clearDefinitions();
	}




	Return_code parseRecord(string data, map<string,c_NvPair>& tokens)
	{
		if (data.empty())
				return this->errorRec.set(NullValue,0,"empty record",0,parse_error);

		int data_len = (int)data.size();
		int last_col = 0;
		this->errorRec.clear();

		vector<c_ColumnDefinition>::iterator pos = this->column_formats.begin();
		for (; pos != this->column_formats.end(); pos++ )
		{
			c_ColumnDefinition cd = *pos;

			if (cd.end_column >  data_len )
				return this->errorRec.set(ParseError,cd.end_column,"data record not long enough",0,parse_error);
            
			if (cd.end_column>last_col) last_col = cd.end_column;

			int zstart = cd.start_column - 1;  /* fixed format starts with one not zero */
			int zend = cd.end_column - 1;
			string value = data.substr(zstart,(zend-zstart+1));

			if (cd.trim_left && cd.trim_right)
				c_Strings::trimAll(value);
			else if (cd.trim_left) 
				c_Strings::trimLeft(value);
			else if (cd.trim_right)
				c_Strings::trimRight(value);

			//-- set c_NvPair here -----------
			c_NvPair cf(cd.field_spec,value);
			tokens.insert( pair<string,c_NvPair>(cd.field_spec.name,cf) );
		}
		if ( !ignore_trailing && last_col != data_len)
			return this->errorRec.set(ParseError,0,"ignore_trailing=true and data record is larger than definition...",0,parse_error);
		else
			return Success;
	}

	c_ParserFixedLength(void);
	~c_ParserFixedLength(void);
};
#endif

