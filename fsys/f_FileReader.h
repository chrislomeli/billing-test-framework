#ifndef __HEADER__f_FileReader__HH
#define __HEADER__f_FileReader__HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <fsys/f_FileName.h>
#include <ctype/c_Types.h>
#include <fsys/f_FileAnnotationConfig.h>

#define RecordAnnotation char


class f_FileReader
{
public:

	f_FileReader(int buflen = 250);
	~f_FileReader(void);

	///---configs--------------

	//--basic commands------
	Return_code openFile(const char * file_name);
	Return_code closeFile(void);
	Return_code annotateCurrentRecord(RecordAnnotation annotation);
	virtual Return_code getNextRecord(string& text_record);  // for singl line recs
	virtual Return_code getNextRecord(vector<string>& text_records); // for multi-line recs

	//--moving around----
	short gotoCurrentRecStart(void);
	short gotoNextRecStart(void);


	void reset(void);		// close file, null and delete all, leave configs
	void clear(void);		// clear all

	//--config ----
	f_FileAnnotationConfig config;

	//-- line and rec counts --
	int m_count_lines;
	int m_count_records;
	//-- status counts --
	int m_count_success;
	int m_count_empty_lines;
	int m_count_bad_lines;
	int m_count_comments;
	int m_count_load_errors;
	int m_count_parse_errors;
	int m_count_duplicates;
	int m_count_skips;
	int m_count_already_done;
	int m_count_already_duplicate;
	int m_count_header_footers;
	int m_count_attempted;

	//-- file pointers --
	long m_pos_begin;
	long m_pos_next;
	//-- file states --
	bool m_is_open;
	f_FileName filename;

	FILE * m_fp;
	char *m_buffer;
	int m_buffer_len;
	ErrorRecord errorRec;
};
#endif
