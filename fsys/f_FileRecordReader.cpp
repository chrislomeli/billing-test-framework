#include <fsys/f_FileRecordReader.h>
#include <fsys/f_FileReaderConfigurator.h>
#include <platform/p_Exception.h>

static char *CVS_Id = (char*)"CVS_VERSION:$Id: f_FileRecordReader.cpp,v 1.4 2008/03/05 03:24:13 kwillens Exp $";



f_FileRecordReader::f_FileRecordReader(void)
{
	setDefaults();
}

f_FileRecordReader::~f_FileRecordReader(void)
{
}

Return_code f_FileRecordReader::readFile(const char * fullpath)
{
	Return_code rc = Success;
	this->current_state = t_NotSet;
	this->current_file.setFullPath(fullpath);

	//----reset all counters and errors -------------------------
	this->reset();

	//----read file name -------------------------
	if ((rc=this->openFile(fullpath)) != Success )
		throw (p_Exception(BadFileOpen,__HERE__,(char *)"Can not open file %s", (char*)fullpath));

	string line;
	int count = 0;
	bool complete = false;

	while (this->getNextRecord(line) == Success )
	{
		//--determine what kind of record ---
		RecordType_t rec_type = current_state = this->determineRecordType(line);

		//--do work-------------------------
		rc = Success;
		try {

			switch (rec_type)
			{
				case t_Header:
					this->m_count_header_footers++;
					rc = this->onHeader(line);
					break;
				case t_Trailer:
					this->m_count_header_footers++;
					rc = this->onTrailer(line);
					break;
				case t_GroupHeader:
					this->m_count_header_footers++;
					rc = this->onGroupHeader(line);
					break;
				case t_GroupTrailer:
					this->m_count_header_footers++;
					rc = this->onGroupTrailer(line);
					break;
				case t_Detail:
					{	m_count_attempted++;
						m_count_records++;					
						RecordAnnotation ra = this->onDetail(line);
						this->annotateCurrentRecord( ra );
					}
					break;
				case t_Comment:
					this->annotateCurrentRecord( config.ann_Comment );
					break;
			}
		}
		catch (const p_Exception& e)
		{
			_errorLog << __NOW__ <<  __ERR_TAG__ << __NOW__ << "reader.readFile: fatal error on line " 
				<< m_count_lines << ", file:" << fullpath << "\n" << e << endl;
			this->closeFile();
			return Failure;
		}
	}

	//--interpret results---------------------
	this->closeFile();
	complete = true;

	if (this->m_count_bad_lines > 0 || this->m_count_parse_errors > 0 || this->m_count_load_errors > 0)
		rc = CompleteWithErrors;
	else if (this->m_count_success <= 0)
		rc = NothingToDo;

	rc = this->onFinal(rc);
	return rc;
}


void f_FileRecordReader::setDefaults(void)
{
	this->m_buffer_len = 251;  //default is 250
	this->mask_annotation_length = 2;
	this->mask_max_errors = 0;	
	mask_comment_prefix.assign("#");
	mask_header_prefix.erase();
	mask_trailer_prefix.erase();
	mask_group_header_prefix.erase();
	mask_group_trailer_prefix.erase();
	mask_detail_prefix.erase();
	this->current_state = t_NotSet;
	current_file.reset();
	return;
}

RecordType_t f_FileRecordReader::determineRecordType(string line)
{
	//WIP: This is Wamu specific - not harmful but not accurate for lec either
	if ( compareRectype(mask_header_prefix,line) )
			return t_Header;

	if ( compareRectype(mask_trailer_prefix,line) )
			return t_Trailer;

	if ( compareRectype(mask_group_header_prefix,line) )
			return t_GroupHeader;

	if ( compareRectype(mask_group_trailer_prefix,line) )
			return t_GroupTrailer;

	if ( compareRectype(mask_detail_prefix,line) )
			return t_Detail;

	if ( compareRectype(mask_comment_prefix,line) )
			return t_Comment;

	return t_Detail;
}


bool f_FileRecordReader::compareRectype(const string prefix, const string line)
{
	string::size_type len = 0;

	if ((len=prefix.size()) > 0 &&
    line.compare(  mask_annotation_length, len, prefix ) == 0 )
		return true;
	return false;
}


void f_FileRecordReader::printTokens(
			string descr, 
			map<string,c_NvPair>& tokens, 
			ostream& out ) 
{
	out << "\n--" << descr << ":: " << endl;
	map<string,c_NvPair>::iterator pos = tokens.begin();
	for (; pos != tokens.end(); pos++ )
	{
		out << "\t" << pos->first << "=" << pos->second.value << endl;
	}
	out << endl;
}



Return_code
f_FileRecordReader::loadReaderProperties(string config_file)
{
	f_FileReaderConfigurator rconfig(this);

	rconfig.readPropertiesFile("annotation", config_file);
	rconfig.readPropertiesFile("parser", config_file);
	return Success;
}
