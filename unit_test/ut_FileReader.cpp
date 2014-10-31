#include <unit_test/ut_FileReader.h>
#include <fsys/f_File.h>

ut_FileReader::ut_FileReader(void)
{
}

ut_FileReader::~ut_FileReader(void)
{
}

void ut_FileReader::create_file(string dir, string file, string contents )
{
	ofstream pend;
	f_FileName f;
	f.setFileName(dir,file);
	pend.open(f.full_path.c_str());
	if (!pend){
		cout << "Can not open file ..." << f.full_path << endl;
		return;
	}
	pend << "Junk|junk|junk" << endl;
	pend.close();

}

void ut_FileReader::test_one(ostream& ut_out)
{  
	char * cp=NULL;


	ut_out << "\n========================================================\n";
	ut_out << "\nTEST ONE\n";
	ut_out << "\n========================================================\n";
	
	string root("c:\\temp\\ut_file_reader1");
	
	///----set configs------
	this->config.setTarget('*');
	ut_out << "\n/////////////////CONFIGS/////////////////////////////\n";
	this->config.dump(ut_out);


///----create the pending dir------
	if (0 != f_File::mkdirRecursive(root))
	{	
		ut_out << "Can not create recursive dir: " << root << endl;
		return;
	};

///----create a pending file------
	ofstream pend;
	f_FileName f;
	f.setFileName(root,"test1.in");
	pend.open(f.full_path.c_str());
	if (!pend){
		ut_out << "Can not open file ..." << f.full_path << endl;
		return;
	}
	pend << "_1bcdefghijklmnop\n";
	pend << "_2bcdefghijklmnop\n";
	pend << "#------------------\n";
	pend << "_3bcdefghijklmnop\n";
	pend << "#-------------------\n";
	pend << "_4bcdefghijklmnop\n";
	pend << "#--------------------\n";
	pend << "#--------------------\n";
	pend << "_5bcdefghijklmnop\n";
	pend.close();

	ut_out << "\n/////////////////PASS 1 - All /////////////////////////////\n";
	if ( this->openFile(f.full_path.c_str()) != Success )
		ut_out << "fopen failed: " << f.full_path << endl;
	else
		ut_out << "fopen success: " << f.full_path << endl;

	string line;
	int count = 0;
	while (this->getNextRecord(line) == Success )
	{
		ut_out << "in : " << line;
		RecordAnnotation c;	

		int d= count++ % 3;
		switch(d)
		{
		case 0:
			c = config.ann_Done; break;
		case 1:
			c = config.ann_Duplicate; break;
		default:
			c = config.ann_Error; break;
		}

		this->annotateCurrentRecord( c );
	}

	ut_out << "------Stats-------------\n" 
		<< "lines   : " 	<< m_count_lines << "\n"
		<< "records : " 	<< m_count_records << "\n"
		<< "---------------------------------------\n"
		<< "success : " 	<< m_count_success<< "\n"
		<< "empty   : " 	<< m_count_empty_lines<< "\n"
		<< "bad line: " 	<< m_count_bad_lines<< "\n"
		<< "comments: " 	<< m_count_comments<< "\n"
		<< "errors  : " 	<< m_count_parse_errors<< "\n"
		<< "dups    : " 	<< m_count_duplicates<< "\n"
		<< "skips   : " 	<< m_count_skips<< "\n"
		<< endl;

	this->reset();


	ut_out << "\n/////////////////PASS 2 - Errors /////////////////////////////\n";
	this->config.setTarget( config.ann_Error );
	if ( this->openFile(f.full_path.c_str()) != Success )
		ut_out << "fopen failed: " << f.full_path << endl;
	else
		ut_out << "fopen success: " << f.full_path << endl;

	count = 0;
	while (this->getNextRecord(line) == Success )
	{
		ut_out << "in: " << line;
		this->annotateCurrentRecord( config.ann_Done );
	}
	ut_out << "------Stats-------------\n" 
		<< "lines   : " 	<< m_count_lines << "\n"
		<< "records : " 	<< m_count_records << "\n"
		<< "---------------------------------------\n"
		<< "success : " 	<< m_count_success<< "\n"
		<< "empty   : " 	<< m_count_empty_lines<< "\n"
		<< "bad line: " 	<< m_count_bad_lines<< "\n"
		<< "comments: " 	<< m_count_comments<< "\n"
		<< "errors  : " 	<< m_count_parse_errors<< "\n"
		<< "dups    : " 	<< m_count_duplicates<< "\n"
		<< "skips   : " 	<< m_count_skips<< "\n"
		<< endl;

	this->reset();

	ut_out << "\n/////////////////PASS 3 - Output Final Dups,Dones and Comments only/////////////////////////////\n";
	this->config.setTarget( config.ann_All );
	if ( this->openFile(f.full_path.c_str()) != Success )
		ut_out << "fopen failed: " << f.full_path << endl;
	else
		ut_out << "fopen success: " << f.full_path << endl;

	count = 0;
	while (this->getNextRecord(line) == Success )
	{
		ut_out << "in: " << line;
		this->m_count_skips++;
	}
	ut_out << "------Stats-------------\n" 
		<< "lines   : " 	<< m_count_lines << "\n"
		<< "records : " 	<< m_count_records << "\n"
		<< "---------------------------------------\n"
		<< "success : " 	<< m_count_success<< "\n"
		<< "empty   : " 	<< m_count_empty_lines<< "\n"
		<< "bad line: " 	<< m_count_bad_lines<< "\n"
		<< "comments: " 	<< m_count_comments<< "\n"
		<< "errors  : " 	<< m_count_parse_errors<< "\n"
		<< "dups    : " 	<< m_count_duplicates<< "\n"
		<< "skips   : " 	<< m_count_skips<< "\n"
		<< endl;

	this->reset();
	return;
}


void ut_FileReader::test_two(ostream& ut_out)
{  
	char * cp=NULL;


	ut_out << "\n========================================================\n";
	ut_out << "\nTEST TWO\n";
	ut_out << "\n========================================================\n";
	
	string root("c:\\temp\\ut_file_reader1");
	
	///----set configs------
	this->config.setTarget('*');
	ut_out << "\n/////////////////CONFIGS/////////////////////////////\n";
	this->config.dump(ut_out);


///----create the pending dir------
	if (0 != f_File::mkdirRecursive(root))
	{	
		ut_out << "Can not create recursive dir: " << root << endl;
		return;
	};

///----create a pending file------
	ofstream pend;
	f_FileName f;
	f.setFileName(root,"test1.in");
	pend.open(f.full_path.c_str());
	if (!pend){
		ut_out << "Can not open file ..." << f.full_path << endl;
		return;
	}
	pend << "_1bcdefghijklmnop\n";
	pend << "_2bcdefghijklmnop\n";
	pend << "#------------------\n";
	pend << "RECIPIENT";
	pend << "\n ";
	pend << " \n";
	pend << "_3bcdefghijklmnop\n";
	pend << "#-------------------\n";
	pend << "\n";
	pend << "_4bcdefghijklmnop\n";
	pend << "#--------------------\n";
	pend << "_\n";
	pend << "#--------------------\n";
	pend << "\n\n\n\n\n\n\n";
	pend << "_5bcdefghijklmnop\n";
	pend.close();

	ut_out << "\n/////////////////PASS 1 - All /////////////////////////////\n";
	if ( this->openFile(f.full_path.c_str()) != Success )
		ut_out << "fopen failed: " << f.full_path << endl;
	else
		ut_out << "fopen success: " << f.full_path << endl;


	string line;
	int count = 0;
	while (this->getNextRecord(line) == Success )
	{
		ut_out << "in : " << line;
		RecordAnnotation c;	

		int d= count++ % 3;
		switch(d)
		{
		case 0:
			c = config.ann_Done; break;
		case 1:
			c = config.ann_Duplicate; break;
		default:
			c = config.ann_Error; break;
		}

		this->annotateCurrentRecord( c );
	}

	ut_out << "------Stats-------------\n" 
		<< "lines   : " 	<< m_count_lines << "\n"
		<< "records : " 	<< m_count_records << "\n"
		<< "---------------------------------------\n"
		<< "success : " 	<< m_count_success<< "\n"
		<< "empty   : " 	<< m_count_empty_lines<< "\n"
		<< "bad line: " 	<< m_count_bad_lines<< "\n"
		<< "comments: " 	<< m_count_comments<< "\n"
		<< "errors  : " 	<< m_count_parse_errors<< "\n"
		<< "dups    : " 	<< m_count_duplicates<< "\n"
		<< "skips   : " 	<< m_count_skips<< "\n"
		<< endl;

	this->reset();


	ut_out << "\n/////////////////PASS 2 - Errors /////////////////////////////\n";
	this->config.setTarget( config.ann_Error );
	if ( this->openFile(f.full_path.c_str()) != Success )
		ut_out << "fopen failed: " << f.full_path << endl;
	else
		ut_out << "fopen success: " << f.full_path << endl;

	count = 0;
	while (this->getNextRecord(line) == Success )
	{
		ut_out << "in: " << line;
		this->annotateCurrentRecord( config.ann_Done );
	}
	ut_out << "------Stats-------------\n" 
		<< "lines   : " 	<< m_count_lines << "\n"
		<< "records : " 	<< m_count_records << "\n"
		<< "---------------------------------------\n"
		<< "success : " 	<< m_count_success<< "\n"
		<< "empty   : " 	<< m_count_empty_lines<< "\n"
		<< "bad line: " 	<< m_count_bad_lines<< "\n"
		<< "comments: " 	<< m_count_comments<< "\n"
		<< "errors  : " 	<< m_count_parse_errors<< "\n"
		<< "dups    : " 	<< m_count_duplicates<< "\n"
		<< "skips   : " 	<< m_count_skips<< "\n"
		<< endl;

	this->reset();

	ut_out << "\n/////////////////PASS 3 - Output Final Dups,Dones and Comments only/////////////////////////////\n";
	this->config.setTarget( config.ann_All );
	if ( this->openFile(f.full_path.c_str()) != Success )
		ut_out << "fopen failed: " << f.full_path << endl;
	else
		ut_out << "fopen success: " << f.full_path << endl;

	count = 0;
	while (this->getNextRecord(line) == Success )
	{
		ut_out << "in: " << line;
		this->m_count_skips++;
	}
	ut_out << "------Stats-------------\n" 
		<< "lines   : " 	<< m_count_lines << "\n"
		<< "records : " 	<< m_count_records << "\n"
		<< "---------------------------------------\n"
		<< "success : " 	<< m_count_success<< "\n"
		<< "empty   : " 	<< m_count_empty_lines<< "\n"
		<< "bad line: " 	<< m_count_bad_lines<< "\n"
		<< "comments: " 	<< m_count_comments<< "\n"
		<< "errors  : " 	<< m_count_parse_errors<< "\n"
		<< "dups    : " 	<< m_count_duplicates<< "\n"
		<< "skips   : " 	<< m_count_skips<< "\n"
		<< endl;

	this->reset();
	return;
}



void ut_FileReader::test_three(ostream& ut_out)
{  
	return;
}


void ut_FileReader::test_Driver()
{
	string filename = ut_Directory + "ut_FileReader.out";
	ofstream ut_out;

	ut_out.open(filename.c_str());
	if (!ut_out){
		cout << "Can not open file ..." << filename << endl;
		return;
	}
	test_one(ut_out);
	test_two(ut_out);
	test_three(ut_out);
	ut_out.close();
}