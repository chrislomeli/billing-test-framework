#pragma once
#include <util/EnvironmentAnsiC.h>
#include <util/EnvironmentStd.h>
#include <util/ut_UnitTest.h>
#include <fsys/f_FileRecordReader.h>
#include <parser/c_ParserFactory.h>
#include <parser/c_ParserDelimited.h>
#include <parser/c_ParserFixedLength.h>
#include <unit_test/ut_FileRecordReaderTester.h>

class ut_FileRecordReader : public ut_UnitTest, public f_FileRecordReader
{
public:
	ut_FileRecordReader(void);
	~ut_FileRecordReader(void);

	ut_FileRecordReaderTester tester;

	//--unit test funcs---------------
	void test_Driver();
	void test_one();
	void test_two();
	void test_three();
	void test_four();
	void test_five();
	void test_six();
	void test_seven();
	void test_eight();
	ofstream ut_out;
	string root;

	//--parsers used---------------
	c_ParserDelimited m_HeaderParser;
	c_ParserDelimited m_TrailerParser;
	c_ParserFixedLength m_DetailParser;

	Return_code initParsers(void)
	{
		//-define header--
		m_HeaderParser.m_delimiters.assign("|\n");
		m_HeaderParser.defineColumn("ann",1);  // trim all
		m_HeaderParser.defineColumn("rectype",2); //trim all
		m_HeaderParser.defineColumn("date",3,false,true);  //left spaces left
		if (m_HeaderParser.errorRec.isError())
		{
			ut_out << "Definition Error = ";
			m_HeaderParser.errorRec.dump(ut_out);
			return ConfigError;
		}	
		else 
			m_HeaderParser.dumpDefinitions(ut_out);


		//-define trailer--
		m_TrailerParser.m_delimiters.assign("|\n");
		m_TrailerParser.defineColumn("ann",1);  // trim all
		m_TrailerParser.defineColumn("rectype",2); //trim all
		m_TrailerParser.defineColumn("count",3,false,true);  //left spaces left
		if (m_TrailerParser.errorRec.isError())
		{
			ut_out << "Definition Error = ";
			m_TrailerParser.errorRec.dump(ut_out);
			return ConfigError;
		}	
		else 
			m_TrailerParser.dumpDefinitions(ut_out);

		//-define detail--
		m_DetailParser.defineColumn("ann",1,2);  // trim all
		m_DetailParser.defineColumn("bank_account",3,18); //trim all
		m_DetailParser.defineColumn("date",19,28,false,true);  //left spaces left
		m_DetailParser.defineColumn("last_name",29,38,true,false);  //right spaces left
		m_DetailParser.defineColumn("first_name",39,48,false,false);  // all spaces left
		if (m_DetailParser.errorRec.isError())
		{
			ut_out << "Definition Error = ";
			m_DetailParser.errorRec.dump(ut_out);
			return ConfigError;
		}	
		else 
			m_DetailParser.dumpDefinitions(ut_out);

		return Success;
	}

	Return_code initReader(void)
	{
		mask_header_prefix.assign("PVNH");
		mask_trailer_prefix.assign("PVNT");
		mask_comment_prefix.assign("#");
		return this->initParsers();
	}

	//--- over-ridable -----
	Return_code onGroupHeader(string line) 
	{
		return Success;
	}
	Return_code onGroupTrailer(string line) 
	{
		return Success;
	}
	Return_code onHeader(string line) 
	{
		map<string,c_NvPair> tokens;
		if (m_HeaderParser.parseRecord(line,tokens) != Success )
		{
			ut_out << "onHeader Error ..." << endl;		
			this->m_HeaderParser.errorRec.dump(ut_out);
			return this->errorRec.set(m_HeaderParser.errorRec);
		}
		printTokens("Header",tokens, ut_out );
		return Success;
	}

	Return_code onTrailer(string line) 
	{
		Return_code rc = NotSet;
		map<string,c_NvPair> tokens;
		if ((rc=m_TrailerParser.parseRecord(line,tokens)) != Success )
		{
			ut_out << "onTrailer Error ..." << endl;		
			this->m_TrailerParser.errorRec.dump(ut_out);
			return this->errorRec.set(m_TrailerParser.errorRec);
		}
		printTokens("Trailer",tokens, ut_out );
		return Success;
	}

	RecordAnnotation onDetail(string line) 
	{
		map<string,c_NvPair> tokens;
		if (m_DetailParser.parseRecord(line,tokens) != Success )
		{
			ut_out << "onDetail Error ..." << endl;		
			this->m_DetailParser.errorRec.dump(ut_out);
			return config.ann_Error;
		}
		printTokens("Detail",tokens, ut_out );
		return config.ann_Done;
	}

	Return_code onFinal(Return_code rc)
	{
		this->ut_out << "------Stats-------------\n" 
				<< "lines   : " 	<< m_count_lines << "\n"
				<< "records : " 	<< m_count_records << "\n"
				<< "---------------------------------------\n"
				<< "success : " 	<< m_count_success << "\n"
				<< "empty   : " 	<< m_count_empty_lines << "\n"
				<< "bad line: " 	<< m_count_bad_lines << "\n"
				<< "comments: " 	<< m_count_comments << "\n"
				<< "errors  : " 	<< m_count_parse_errors << "\n"
				<< "dups    : " 	<< m_count_duplicates << "\n"
				<< "skips   : " 	<< m_count_skips << "\n"
				<< endl;
		return rc;
	}

};
