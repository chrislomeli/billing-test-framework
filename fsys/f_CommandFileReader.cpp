// Tess.cpp : Defines the entry point for the console application.
//
#include <util/Utility.h>
#include <fsys/f_CommandFileReader.h>
static char *CVS_Id = (char*)"CVS_VERSION:$Id: f_CommandFileReader.cpp,v 1.4 2007/01/24 01:15:07 clomeli Exp $";


bool f_CommandFileReader::runCommandFile(
	string _filename, 
	string _delims, 
	char comment_ch, 
	char block_ch )
{
	char commandline[1024];

	//---reset-----------------------
	this->reset();

	//--- initializize -------------
	this->block_char = block_ch;
	this->comment_char = comment_ch;
	this->delims.assign(_delims);
	this->filename.assign( _filename );

	if ( ! init() )
		return false;


	// --- read -lines ------------ 
	fp = NULL;

	if ((fp=fopen(filename.c_str(), "r+" )) == NULL )
	{
		cout << "TEST_ERROR: can not open FILE " 
			<< filename << endl;
		return this->return_code;
	}
	long begin_line = ftell(fp);
	long next_line = 0;

	while ( fgets(commandline,1024,fp) )
	{	this->line_count++;

		if ( strlen(commandline)==0 || commandline[0]=='\n' )
		{
			this->empty_lines++;
			begin_line = ftell(fp);
			continue;
		}

		if (commandline[0]==this->comment_char ) 
		{
			this->skip_count++;
			begin_line = ftell(fp);
			continue;
		}	

		if (commandline[0]==this->block_char ) 
		{
			this->block_count++;
			begin_line = ftell(fp);
			continue;
		}	

		string rec;
		rec.assign(commandline);

		if ( rec.empty() ){
			begin_line = ftell(fp);
			continue;
		}
		if ( rec[0] == '#' ) {
			begin_line = ftell(fp);
			continue;
		}
		
		vector<string> tokens;
		util::tokenize( rec,tokens,this->delims, this->trim_columns );

		if ( this->doCommand(tokens) )
		{	this->success_count++;
			next_line = ftell(fp);
			fseek(fp,begin_line,0);
			fputc(this->block_char,fp);
			fseek(fp,next_line,0);
		}
		else 
		{ 
			this->err_count++;
		}

		//-- we are sitting at the beginning of the next line---
		begin_line = ftell(fp);
	}	
	fclose(fp);
	this->return_code = ( this->err_count == 0 );
	return this->return_code;
}	


bool f_CommandFileReader::init()
{
	//--valid input-------------
	if ( this->filename.empty() ) {
		cout << "ERROR: file name not set!!! \n";
		return false;
	}
	if (this->delims.empty()) {
		cout << "ERROR: delimiter string not set!!! \n";
		return false;
	}
	if ( this->comment_char == -1 ) {
		cout << "ERROR: comment_char string not set!!! \n";
		return false;
	}
	if ( this->block_char == -1 ) {
		cout << "ERROR: block_char string not set!!! \n";
		return false;
	}

	//-- in file should exist ----------------------
	if ( ! util::Access(this->filename,F_EXISTS) ) {
		cout << "Config file " <<  filename << " does not exist.  Exiting..." << endl;
		return false;
	}
	return true;
}

void f_CommandFileReader::reset()
{
	this->block_char = -1;
	this->comment_char = -1;
	this->filename.erase();
	this->delims.erase();
	this->return_code = false;
	this->empty_lines = 0;
	this->block_count = 0;
	this->err_count = 0;
	this->line_count = 0;
	this->skip_count = 0;
	this->success_count = 0;
	this->trim_columns = true;
}


f_CommandFileReader::f_CommandFileReader() : fp(NULL)
{
	this->block_char = -1;
	this->comment_char = -1;
}

f_CommandFileReader::~f_CommandFileReader(void)
{
	if ( fp )
			fclose(fp);
}

