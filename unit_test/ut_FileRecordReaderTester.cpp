#include <fsys/ut_FileRecordReaderTester.h>
#include <fsys/f_File.h>

ut_FileRecordReaderTester::ut_FileRecordReaderTester(void)
{
}

ut_FileRecordReaderTester::~ut_FileRecordReaderTester(void)
{
}

void ut_FileRecordReaderTester::dump_file(const char * full_path, ostream& ut_out)
{
	fstream file_op;
	file_op.open(full_path, ios::in);
	if (!file_op){
		ut_out << "Can not open file ..." << full_path << endl;
		return;
	}

    char str[250];
	while(!file_op.eof()) 
	{
		file_op.getline(str,250);
		ut_out << str << endl;
	}       

	file_op.close();
	return;
}


void ut_FileRecordReaderTester::runTest(
			 f_FileRecordReader * reader, string fullpath, ostream& ut_out )
{
	//---sanity-----

	//--setup-------
	f_FileName f;
	f.setFullPath(fullpath);

	//---copy the input file ----
	ostringstream os;
	os << f.dir_name << SLASH << f.file_root_name << ".testcopy";
	
	string destfile(os.str());

	if ( 0 != f_File::copyTextFile(fullpath, destfile) )
	{
		ut_out << "Can not copy file {" 
			<< fullpath << "} to {" 
			<< destfile << "}" << endl;
		return;
	}

	ut_out << "\n========================================================\n";
	ut_out << "\nTESTING File " << fullpath << "....\n";
	ut_out << "\n========================================================\n";

	///----set configs------
	ut_out << "\n========================================================\n";
	ut_out << "\nCONFIGS\n";
	ut_out << "\n========================================================\n";
	if (reader->initReader() != Success){
		ut_out << "initReader initialization failed ..." << endl;
		return;
	}
	reader->config.dump(ut_out);

	ut_out << "\n/////////////////INPUT FILE DUMP/////////////////////////////\n";
	dump_file(destfile.c_str(), ut_out);

	ut_out << "\n/////////////////RUN/////////////////////////////\n";
	reader->readFile(destfile.c_str());

	ut_out << "------Stats-------------\n" 
		<< "lines   : " 	<< reader->m_count_lines << "\n"
		<< "records : " 	<< reader->m_count_records << "\n"
		<< "---------------------------------------\n"
		<< "success : " 	<< reader->m_count_success << "\n"
		<< "empty   : " 	<< reader->m_count_empty_lines << "\n"
		<< "bad line: " 	<< reader->m_count_bad_lines << "\n"
		<< "comments: " 	<< reader->m_count_comments << "\n"
		<< "errors  : " 	<< reader->m_count_parse_errors << "\n"
		<< "dups    : " 	<< reader->m_count_duplicates << "\n"
		<< "skips   : " 	<< reader->m_count_skips << "\n"
		<< endl;

	ut_out << "\n/////////////////UPDATED FILE DUMP/////////////////////////////\n";
	dump_file(destfile.c_str(), ut_out);

}