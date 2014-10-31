#include <parser/c_ParserFactory.h>
#include <parser/c_ParserDelimited.h>
#include <parser/c_ParserFixedLength.h>

c_ParserFactory::c_ParserFactory(void)
{
}

c_ParserFactory::~c_ParserFactory(void)
{
}

c_Parser * c_ParserFactory::getParser(istream& file_op)
{
    char str[250];
	vector<string> commands;
	vector<string> column;
	vector<string>::iterator cpos;
	vector<string> nvpair;
	string::size_type str_len = 0;
	c_Parser * parser = NULL;


	string parse_delimiter, parse_class;
	bool found_init = false;
	while(!file_op.eof()) 
	{
		file_op.getline(str,250);

		string line(str);
		commands.clear();
		c_Strings::tokenize(line,commands, " =</>\t\n");
		str_len = commands.size(); 

		for ( string::size_type pos = 0; pos < str_len; pos++ )
		{
			string command = commands[pos];
			if ( commands[pos] == "class" && pos+1 <  str_len )
			{	parse_class=commands[pos+1];
				found_init = true;
			}
			else if ( commands[pos] == "delimiter" && pos+1 <  str_len )
				parse_delimiter=commands[pos+1];
		}
		if (found_init)
			break;
	}
	if (parse_class.empty())
	{
		// error class must be defined first 
	}

	// -- select parser --
	if (parse_class == "simple_delimited")
	{
		c_ParserDelimited * p = new c_ParserDelimited();
		if (!parse_delimiter.empty())
			p->m_delimiters = parse_delimiter;
		parser = (c_Parser*)p;
	}
	else if (parse_class == "simple_fixed")
	{
		c_ParserFixedLength * p = new c_ParserFixedLength();
		parser = (c_Parser*)p;
	}
	else
	{
		// error bad class name
		return NULL;
	}

	///---column definitions------------------
	/*
		<parser state=header>
			<init class=simple_delimited delimiter=|/>
			<column index=1 name=annotation></column>
			<column index=2 name=rec_type></column>
			<column index=3 name=date type=date mask=yyyymmdd></column>
		</parser>
	*/
	int i = 0; 
	int errs = 0;
	bool end_of_block = false;
	string s1;
	string s2;

	while(!file_op.eof() && !end_of_block && errs == 0 ) 
	{
		file_op.getline(str,250);
		if (strstr(str,"</parser") != NULL )
			break;

		string line(str);
		commands.clear();
		c_Strings::tokenize(line,commands, " =</>\t\n");
		str_len = commands.size(); 

		c_ColumnDefinition column_def;

		for ( string::size_type pos = 0; 
			errs == 0 && pos+1 < str_len; pos++ )
		{
			string s1(commands[pos]);
			string s2(commands[pos+1]);

			if ( s1 == "parser"  )
			{
				end_of_block=true;
				break;
			}
			if ( s1 == "name"  )
				column_def.field_spec.name=s2;

			else if ( s1 == "type"  )
				column_def.field_spec.data_type = c_DataType::dataType(s2);

			else if ( s1 == "mask"  )
				column_def.field_spec.mask=s2;

			else if ( s1 == "rtrim"  )
				column_def.trim_right= c_Strings::stringToBool(s2, true);

			else if ( s1 == "ltrim"  )
				column_def.trim_left = c_Strings::stringToBool(s2, true);

			else if ( s1 == "index"  )
				column_def.position=c_Strings::stringToInteger(s2, &errs);

			else if ( s1 == "begin"  )
				column_def.start_column=c_Strings::stringToInteger(s2, &errs);

			else if ( s1 == "end"  )
				column_def.end_column=c_Strings::stringToInteger(s2, &errs);

		} // end for

		if (errs != 0)
			parser->errorRec.set(ConversionError,0,s1,0,definition_error);
		else if (parser->defineColumn(column_def)!=Success)
			errs++;
	}       

	if ( errs != 0 )
	{
		return NULL;
	}
	return parser;
}

Return_code
c_ParserFactory::readParserConfigFile(const char * filename)
{
	char * cp = NULL;

	//---
	this->factoryErrorState.clear();

	//---
	fstream file_op;
	file_op.open(filename, ios::in);
	if (!file_op){
		return BadFileOpen;
	}

	//---
    char str[250];
	while(!file_op.eof()) 
	{
		file_op.getline(str,250);
		if ( strstr(str,"<parser") == NULL )
			continue;

		string line(str);
		string::size_type cpos = line.find("state=")+6;
		line.erase(0,cpos);
		c_Strings::trimAll(line,"\t\n <>/");
		c_Parser * parser = this->getParser(file_op);
		// need to know which parser; which col?
		if (parser == NULL )
		{
			this->factoryErrorState.set(ConfigError,0,line,0,definition_error);
			return ConfigError;
		}
		else if (parser->errorRec.isError())
		{
			this->factoryErrorState = parser->errorRec;
			return this->factoryErrorState.errCode;
		}
		else if (line.empty()  )
		{
			this->factoryErrorState.set(NullValue,0,"No state= defined",0,definition_error);
			return NullValue;
		}
		else
			this->parsers.insert(pair<string,c_Parser*>(line,parser));
	}       

	file_op.close();
	return Success;
}
int c_ParserFactory::dumpConfigs(ostream& out)
{

	map<string,c_Parser*>::iterator pos = this->parsers.begin();
	for(; pos != this->parsers.end(); pos++)
	{
		out << "\n\n//////////////////////////\nParser: " << pos->first << "\n///////////////////////////////////////////" << endl;
		c_Parser * parser = pos->second;
		if (parser->errorRec.isError())
			parser->errorRec.dump(out);
		parser->dumpDefinitions(out);
	}
	return 0;
}
c_Parser * c_ParserFactory::getParser(string state)
{

	map<string,c_Parser*>::iterator pos = this->parsers.find(state);
	if(pos != this->parsers.end())
	{
		c_Parser * parser = pos->second;
		return parser;
	}

	//try for a wildcard match
	string test;
	pos = this->parsers.begin();
	for(;pos != this->parsers.end(); pos++ )
	{
		test.assign(pos->first);
		size_t epos;
		if ((epos=test.find('*',0)) != string::npos )
		{
			if (state.compare(0,epos,test.substr(0,epos)) == 0 )
			{
				c_Parser * parser = pos->second;
				return parser;
			}
		}
	}
	return NULL;
}

void
c_ParserFactory::addParser(string name, c_Parser * parser)
{
	this->parsers.insert(pair<string,c_Parser*>(name,parser));
}
