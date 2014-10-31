#ifndef  f_CommandFileReader_MAIN_INTERFACE___HH
#define  f_CommandFileReader_MAIN_INTERFACE___HH

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <util/Utility.h>


class f_CommandFileReader
{
public:
	f_CommandFileReader();
	~f_CommandFileReader(void);

	bool runCommandFile(
		string filename, 
		string delims, 
		char comment_ch='#', 
		char block_ch = '_');

	virtual bool doCommand(vector<string>& tokens) = 0;

	bool get_return_code()	{ return return_code; };
	int get_empty_lines()	{ return empty_lines; };
	int get_line_count()	{ return line_count; };
	int get_success_count()	{ return success_count; };
	int get_err_count()		{ return err_count; };
	int get_block_count()	{ return block_count; };
	int get_skip_count()	{ return skip_count; };
	void set_trim_columns(bool trm)	{ this->trim_columns = trm; };

protected:
	FILE * fp;
	bool init();
	bool return_code;
	int empty_lines;
	int line_count;
	int success_count;
	int err_count;
	int block_count;
	int skip_count;
	
	string filename;
	string delims;
	bool trim_columns;
	char block_char;
	char comment_char;

public:
	void reset(void);
};

#endif


