#ifndef __HEADER__C_PARSER_HANDLER__
#define __HEADER__C_PARSER_HANDLER__
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <util/EnvironmentStd.h>
#include <ctype/c_Strings.h>
#include <ctype/c_Types.h>

struct c_ColumnDefinition 
{
	c_ColumnDefinition(const c_ColumnDefinition& cd) 
	{
		this->end_column			= cd.end_column;
		this->start_column			= cd.start_column;
		this->trim_left				= cd.trim_left;
		this->trim_right			= cd.trim_right;
		this->position				= cd.position;
		this->field_spec.data_type	= cd.field_spec.data_type;
		this->field_spec.key		= cd.field_spec.key;
		this->field_spec.ignore		= cd.field_spec.ignore;
		this->field_spec.name		= cd.field_spec.name;
		this->field_spec.mask		= cd.field_spec.mask;
	}

	c_ColumnDefinition() {
		this->end_column=0;
		this->field_spec.data_type = Undef_t;
		this->start_column=0;
		this->trim_left=true;
		this->trim_right=true;
		this->position=0;
		this->field_spec.key =false;
		this->field_spec.ignore = false;
	};
	void set(
			const char * _name,
			const char * _mask,
			DataType_t _dt,
			int _p, int _s, int _end, bool ltrim = true, bool rtrim = true)
	{
		this->field_spec.data_type = _dt;
		this->field_spec.mask = _mask;
		this->field_spec.name = _name;
		this->position = _p;
		this->start_column = _s;
		this->end_column = _end;
		this->trim_left = ltrim;
		this->trim_right = rtrim;
	}
	c_FieldSpec field_spec;
	int position;
	int start_column;
	int end_column;
	bool trim_left;
	bool trim_right;
};

class c_Parser
{
public:
	c_Parser();
	~c_Parser(void);

	Parse_Type_t parse_type;
	bool m_reject_overlaps;		// no overlapping fields
	bool m_protect_field_name;	// no overwrite of a field name

	ErrorRecord errorRec;
	bool ignore_trailing;
	vector<c_ColumnDefinition> column_formats;

	virtual Return_code parseRecord(string data, map<string,c_NvPair>& tokens) = 0;

	int defineColumn(string _name, int start, int end, bool ltrim=true, bool rtrim=true)
	{
		c_ColumnDefinition cd;
		cd.field_spec.name = _name;
		cd.field_spec.data_type = String_t;
		cd.position = 0;
		cd.start_column=start;
		cd.end_column=end;
		cd.trim_left=ltrim;
		cd.trim_right=rtrim;
		return defineColumn(cd);
	}

	Return_code defineColumn(string _name, int pos, bool ltrim=true, bool rtrim=true)
	{
		c_ColumnDefinition cd;
		cd.field_spec.name = _name;
		cd.field_spec.data_type = String_t;
		cd.position = pos;
		cd.start_column=0;
		cd.end_column=0;
		cd.trim_left=ltrim;
		cd.trim_right=rtrim;
		return defineColumn(cd);
	}
	Return_code defineColumn(c_ColumnDefinition& cd)
	{
		if ( errorRec.isError() )
			return errorRec.errCode;

		if (cd.field_spec.name.empty())
			return this->errorRec.set(NullValue,0,"Name can not be empty",0,definition_error);

		if (this->parse_type == Delimited && cd.position <= 0)
			return this->errorRec.set(BadValue,0,"Delimited definitions start with 1, not zero",0,definition_error);

		if ( this->parse_type == FixedLength )
		{
			if ( cd.start_column <= 0 )
				return this->errorRec.set(BadValue,cd.start_column,"start column can not == 0",0,definition_error);
			else if ( cd.start_column > cd.end_column )
				return this->errorRec.set(BadValue,cd.end_column,"end can not be greater than start",0,definition_error);
		}

		int ret=0;
		if((ret=this->testOverlap(cd.field_spec.name,cd.position,cd.start_column,cd.end_column)) != 0)
		{
			if (ret=-5)
				return this->errorRec.set(BadValue,0,"ovelapping name definition",0,definition_error);
			else
				return this->errorRec.set(BadValue,0,"ovelapping column definition",0,definition_error);
		}
		this->column_formats.push_back(cd);
		return Success;
	};

	void dumpDefinitions(ostream& out = cout)
	{
		out << "\n------------------\nNew Definitions\n--------------------\n";
		vector<c_ColumnDefinition>::iterator pos = this->column_formats.begin();
		for (; pos != this->column_formats.end(); pos++ )
		{
			out << "Name: " << pos->field_spec.name
				 << ", Pos: " << pos->position
				 << ", Start: " << pos->start_column
				 << ", End: " << pos->end_column
				 << ", Trim Left: " << pos->trim_left
				 << ", Trim Right: " << pos->trim_right
				 << ", Key: " << pos->field_spec.key
				 << ", Ignore: " << pos->field_spec.ignore
				 << endl;
		}
	}

	short testOverlap(string name, int position, int start, int end)
	{
		if (!this->m_protect_field_name && !this->m_reject_overlaps )
			return 0;

		vector<c_ColumnDefinition>::iterator pos = this->column_formats.begin();
		for (; pos != this->column_formats.end(); pos++ )
		{
			if ( this->m_protect_field_name 
					&& pos->field_spec.name ==  name )
				return -5;

			if ( this->m_reject_overlaps )
			{
				if ( this->parse_type == FixedLength )
					if ( start >= pos->start_column 
						&& start <= pos->end_column )
							return -6;
					else if ( end >= pos->start_column 
						&& end <= pos->end_column )
							return -6;
				else if ( this->parse_type == Delimited )
					if ( position == pos->position )
							return -6;
			}
		}
		return 0;
	}

	void clearDefinitions()
	{
		this->errorRec.clear();
		this->column_formats.clear();	
	}
};
#endif

