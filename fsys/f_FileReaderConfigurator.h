#ifndef __HEADER__f_FileReaderConfigurator__HH
#define __HEADER__f_FileReaderConfigurator__HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <fsys/f_File.h>
#include <fsys/f_PropertiesReader.h>
#include <parser/c_ParserDelimited.h>
#include <parser/c_ParserFixedLength.h>
#include <fsys/f_FileRecordReader.h>
#include <platform/p_Exception.h>

class f_FileReaderConfigurator : public f_PropertiesReader
{
public:
	f_FileReaderConfigurator( f_FileRecordReader * readp ) 
		: readerp(NULL)
		, parser(NULL)
	{
		this->errRec.clear();
		this->readerp = readp;	
	}
	~f_FileReaderConfigurator(void) {}


	//--properties handlers --------------------
	void onGroup(string group, c_NvRecord attribs)
	{
		if (group == "annotation")
		{
			this->ann_Config.clear();
			ann_Config.ann_All = '*';
		}
		else if (group == "parser")
		{
			parse_state.assign(attribs.value("state"));  //header, trailer, etc. 

			// -- select parser --
			string id(attribs.value("class")); // which parser
			if (id == "simple_delimited")
			{
				c_ParserDelimited * p = new c_ParserDelimited();
				string parse_delimiter(attribs.value("delimiter"));
				if (!parse_delimiter.empty())
					p->m_delimiters = parse_delimiter;
				parser = (c_Parser*)p;
			}
			else if (id == "simple_fixed")
			{
				c_ParserFixedLength * p = new c_ParserFixedLength();
				parser = (c_Parser*)p;
			}
			else
			{

			}
		}
	}
	
	void onGroupFinal(string group)
	{
		if (group == "annotation")
		{
			this->readerp->config.set(this->ann_Config);
		}
		else if (group == "parser")
		{
			if (!parser || this->parse_state.empty() || parser->column_formats.size() == 0 )
				this->errRec.set(ConfigError,0,parse_state+" parser not configed",0,not_set);
			else
                this->readerp->factory.addParser(parse_state,parser);

			//DEBUG
			//parser->dumpDefinitions();
		}

	}

	void onElem(string group, string elem, string value, c_NvRecord attribs)
	{
		c_Strings::trimAll(value);

		if (group == "annotation")
		{
			if (value.size() <= 0)
				return;
			char ch = value[0];

			if (elem.compare("open")==0) 
				ann_Config.ann_Open = ch;
			else if (elem.compare("done")==0) 
				ann_Config.ann_Done = ch;
			else if (elem.compare("duplicate")==0)  
				ann_Config.ann_Duplicate = ch;
			else if (elem.compare("error")==0) 
				ann_Config.ann_Error = ch;
			else if (elem.compare("comment")==0) 
				ann_Config.ann_Comment = ch;
			else if (elem.compare("loaderror")==0)  
				ann_Config.ann_LoadError = ch;
			else
				throw(p_Exception(ConfigError,__HERE__,
				" element '%s' - is not a valid annotation ", elem.c_str()));
		}
		else if (group == "parser")
		{
			if (elem.compare("column")==0)  
			{
				c_ColumnDefinition column_def;
				string name(attribs.value("name"));
				int errs = 0;

				column_def.field_spec.name=attribs.value("name");
				column_def.field_spec.mask=attribs.value( "mask"  );

				column_def.field_spec.data_type = 
					c_DataType::dataType(attribs.value( "type"  ));

				column_def.field_spec.key = 
					c_Strings::stringToBool(attribs.value( "key"  ), false);

				column_def.field_spec.ignore = 
					c_Strings::stringToBool(attribs.value( "ignore"  ), false);


				column_def.trim_right= 
					c_Strings::stringToBool(attribs.value( "rtrim"  ), true);

				column_def.trim_left = 
					c_Strings::stringToBool(attribs.value( "ltrim"  ), true);

				column_def.position=
					c_Strings::stringToInteger(attribs.value( "index"  ), &errs);

				column_def.start_column=
					c_Strings::stringToInteger(attribs.value( "begin"  ), &errs);

				column_def.end_column=
					c_Strings::stringToInteger(attribs.value( "end"  ), &errs);


				//Parser is instantiated in the OnGroup() function. If parser is null here, 
				//  it means that we never found a parent element that could 
				//  define a known parser before getting to these column definitions
				//  the properties xml is not correct or the parser id is not 'known'
				if (parser == NULL)
				{
					throw(
						p_Exception(ConfigError,__HERE__,
						"Was not able to find a known parser definition before column definitions.." ));
				}
				else if (parser->defineColumn(column_def)!=Success)
					throw(p_Exception(ConfigError,__HERE__,
					"element '%s' - definition is not valid: %s ", name.c_str(), parser->errorRec.desc.c_str()));
			}
		}
	}

	f_FileRecordReader * readerp;
	f_FileAnnotationConfig ann_Config;

	c_Parser * parser;
	string  parse_state;  //header, trailer, etc. 
	ErrorRecord errRec;
};
#endif
