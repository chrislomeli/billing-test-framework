#include <fsys/f_FileReader.h>

static char *CVS_Id = (char*)"CVS_VERSION:$Id: f_FileReader.cpp,v 1.3 2007/01/24 01:15:07 clomeli Exp $";


f_FileReader::f_FileReader(int buflen)
: m_buffer(NULL)
, m_fp(NULL)
{
	this->clear();
	this->m_buffer_len = buflen;
}

f_FileReader::~f_FileReader(void)
{
	clear();
}


Return_code f_FileReader::openFile(const char * file_name)
{
	//--- reset -----------
	if (m_count_lines != 0 || this->errorRec.isError())
		this->reset();  // just to be safe - this should always start new counters and error

	//--- open -----------
	errno=0;
	if ((this->m_fp = fopen(file_name, "r+")) == NULL )
	{
			return errorRec.set(BadFileOpen,0,file_name,errno);
	}
	this->filename.setFullPath(file_name);
	this->m_is_open =true;
	return Success;
}

Return_code f_FileReader::closeFile(void)
{
	if (this->m_fp)
		fclose(this->m_fp);
	this->m_fp = NULL;
	this->m_is_open = false;
	return Success;
}

Return_code f_FileReader::getNextRecord(vector<string>& record)
{
	return errorRec.set(NotSupported,0,"getNextRecord vector",errno);;
}

Return_code f_FileReader::getNextRecord(string& record)
{
	//-- pre-conditions --
	if ( ! this->m_is_open )
	{
		return errorRec.set(BadFileStream,0,"getNextRecord",errno);
	}
	
	if (! this->m_buffer)
	{
		if (this->m_buffer_len < 1024)
			this->m_buffer_len =1024;
		m_buffer = new char[this->m_buffer_len];
	}

	//--setup-----
	record.erase();

	bool looking = true;
	bool in_record = false; //state=we are in the middle of a record
	while (looking)
	{
		//-- get one line --
		this->m_pos_begin = ftell( this->m_fp );

		if ( fgets(m_buffer, this->m_buffer_len, this->m_fp ) == NULL )
		{
			return EndOfFile;
		}
		this->m_pos_next = ftell( this->m_fp );
		this->m_count_lines++;

		//---validate and filter----
		if (!m_buffer || strlen(m_buffer)<=0 || m_buffer[0] == '\n' )
		{
			this->m_count_empty_lines++;
			continue;
		}
		if ( ! config.isAnnotated(m_buffer) )
		{
			this->m_count_bad_lines++;
			continue;
		}
		if (config.isComment(m_buffer))
		{
			this->m_count_comments++;
			continue;
		}


		if (config.isDone(m_buffer))  // never re-do dones
		{
			this->m_count_already_done++;
			this->m_count_records++;
			continue;
		}
		if (this->config.filterIsSet()  )// a specific target is set: see if this is it
		{
			if (! this->config.isTarget(m_buffer))  
			{
				this->m_count_skips++;
				this->m_count_records++;
				continue;
			}
		}
		else if (config.isDuplicate(m_buffer))  // already marked as dup
		{
			this->m_count_already_duplicate++;
			this->m_count_records++;
			continue;
		}
		else if (
			! config.isOpen(m_buffer) && 
			! config.isError(m_buffer) &&
			! config.isLoadError(m_buffer))  // target is Not set: only do opens and errors
		{
			this->m_count_skips++;
			this->m_count_records++;
			continue;
		}

		//----
		record.assign(m_buffer);
		break;
	}
	return Success;
}

short
f_FileReader::gotoCurrentRecStart(void)
{
	errno=0;
	if (0 == fseek(this->m_fp, this->m_pos_begin, SEEK_SET))
		return 0;
	return errno;
}

Return_code
f_FileReader::annotateCurrentRecord(RecordAnnotation annotation)
{
	errno = 0;

	//---get current pos---------
	long currpos = ftell( this->m_fp );
	if ( errno != 0 )
	{
		return errorRec.set(BadFilePosition,0,"annotateCurrentRecord",errno);
	}

	//---goto record start---------
	gotoCurrentRecStart();

	if ( fputc(annotation, this->m_fp) == -1 )
	{
		return errorRec.set(BadFileWrite,0,"annotateCurrentRecord",errno);
	}

	//---goback to current pos---------
	fseek(this->m_fp, currpos, SEEK_SET);
	if ( errno != 0 )
	{
		return errorRec.set(BadFilePosition,0,"annotateCurrentRecord",errno);
	}
/*
		<< "success : " 	<< m_count_success<< "\n"
		<< "empty   : " 	<< m_count_empty_lines<< "\n"
		<< "bad line: " 	<< m_count_bad_lines<< "\n"
		<< "comments: " 	<< m_count_comments<< "\n"
		<< "errors  : " 	<< m_count_parse_errors<< "\n"
		<< "dups    : " 	<< m_count_duplicates<< "\n"
		<< "skips   : " 	<< m_count_skips<< "\n"
		<< endl;

*/
	if(config.isDone(annotation))
		this->m_count_success++;
	else if (config.isError(annotation))
		this->m_count_parse_errors++;
	else if ( config.isLoadError(annotation) ) 
		this->m_count_load_errors++;
	else if (config.isDuplicate(annotation))
		this->m_count_duplicates++;

	return Success;
}


void f_FileReader::clear(void)
{
	this->config.reset();
	this->reset();
}

void f_FileReader::reset(void)
{
	//-- error state --
	this->errorRec.clear();

	//-- line and rec counts --
	m_count_lines = 0;
	m_count_records = 0;

	//-- status counts --
	m_count_success = 0;
	m_count_comments = 0;
	m_count_parse_errors = 0;
	m_count_load_errors = 0;
	m_count_duplicates = 0;
	m_count_empty_lines = 0;
	m_count_bad_lines = 0;
	m_count_skips = 0;
	m_count_already_done = 0;
	m_count_already_duplicate=0;
	m_count_header_footers=0;
	m_count_attempted=0;

	//-- file pointers --
	m_pos_begin = 0;
	m_pos_next = 0;
	//-- file states --
	m_is_open = false;
	filename.reset(); ;

	if (m_fp)
		this->closeFile();

	if (m_buffer)
		delete[] m_buffer;
	m_buffer = NULL;
}
