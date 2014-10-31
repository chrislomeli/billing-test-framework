#ifndef __HEADER__f_FileRecordReader__HH
#define __HEADER__f_FileRecordReader__HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fsys/f_FileReader.h>
#include <parser/c_ParserFactory.h>
#include <logSimple/p_Logs.h>


typedef enum RecordType_tag {
	t_Header = 1,
	t_GroupHeader = 2,
	t_GroupTrailer = 3,
	t_Detail = 4,
	t_Trailer = 5,
	t_Comment = 6,
	t_NotSet = 7
} RecordType_t;

class f_FileRecordReader :
	public f_FileReader
{
public:
	f_FileRecordReader(void);
	virtual ~f_FileRecordReader(void);

	//--parsers used---------------
	c_ParserFactory factory;
	string parser_config_file;

	Return_code setParserConfig(string conf)
	{
		this->parser_config_file.assign(conf);
		return ( factory.readParserConfigFile(parser_config_file.c_str()) );
	}

	//---the only real funcs - the rest is more an interface -----
	Return_code readFile(const char * fullpath);
	void printTokens(string descr, map<string,c_NvPair>& tokens, ostream&out = cout);
	void setDefaults();

	//--- possibly over-ridable -----
	virtual RecordType_t determineRecordType(string line);
	virtual bool compareRectype(const string prefix, const string line);

	//--- over-ridable -----
	virtual Return_code initReader() = 0;  // override the setDefaults() params
	virtual Return_code onHeader(string line) = 0;	// handle the header record
	virtual Return_code onTrailer(string line) = 0; 
	virtual Return_code onGroupHeader(string line) = 0;
	virtual Return_code onGroupTrailer(string line) = 0;
	virtual Return_code onFinal(Return_code rc) = 0;  // final handling before close
	virtual RecordAnnotation onDetail(string line) = 0;	// this has a state change as an return code - not a Return_code

	f_FileName current_file;
	RecordType_t current_state;

	string mask_header_prefix;
	string mask_trailer_prefix;
	string mask_group_header_prefix;
	string mask_group_trailer_prefix;
	string mask_detail_prefix;
	string mask_comment_prefix;
	string::size_type mask_annotation_length;	// usually 2 (the ann char plus delimiter) - for both fixed and delmited
	int mask_max_errors;		// maximum errors before quitting
	Return_code loadReaderProperties(string config_file);
};
#endif
