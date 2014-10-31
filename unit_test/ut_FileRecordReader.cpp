#include <unit_test/ut_FileRecordReader.h>
#include <fsys/f_File.h>

ut_FileRecordReader::ut_FileRecordReader(void)
{
}

ut_FileRecordReader::~ut_FileRecordReader(void)
{
}


void ut_FileRecordReader::test_one()
{  
	char * cp=NULL;


	ut_out << "\n========================================================";
	ut_out << "\nTEST ONE setup";


///----create a pending file------
	fstream pend;
	f_FileName f;
	f.setFileName(root,"test1.in");
	pend.open(f.full_path.c_str(), ios::out);
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

	tester.runTest(this,f.full_path.c_str(),ut_out);
	return;
}

void ut_FileRecordReader::test_two()
{  
	char * cp=NULL;


	ut_out << "\n========================================================";
	ut_out << "\nTEST TWO - setup";

///----create a pending file------
	fstream pend;
	f_FileName f;
	f.setFileName(root,"test2.in");
	pend.open(f.full_path.c_str(), ios::out);
	if (!pend){
		ut_out << "Can not open file ..." << f.full_path << endl;
		return;
	}
	pend << "\n\n\n\n\n";
	pend << "_2bcdefghijklmnop\n";
	pend << "#------------------\n";
	pend << "_3bcdefghijklmnop\n";
	pend << "\n\n\n\n\n\n\n\n\n\n";
	pend << "_4bcdefghijklmnop\n";
	pend << "_JUNK---\n";
	pend << "JUNK\n";
	pend << "\0";
	pend << "MOREJUNK";
	pend << "_4bcdefghijklmnop\n";
	pend.close();

	tester.runTest(this,f.full_path.c_str(),ut_out);
	return;
}



void ut_FileRecordReader::test_three()
{  
	char * cp=NULL;

	ut_out << "\n========================================================";
	ut_out << "\nTEST THREE setup";
	
	f_FileName f;
	f.setFileName(root,"test_NOTHING.in");

	tester.runTest(this,f.full_path.c_str(),ut_out);
	return;
}

void ut_FileRecordReader::test_four()
{  
	char * cp=NULL;


	ut_out << "\n=======================================================";
	ut_out << "\nTEST FOUR - setup";

///----create a pending file------
	fstream pend;
	f_FileName f;
	f.setFileName(root,"test4.in");
	pend.open(f.full_path.c_str(), ios::out);
	if (!pend){
		ut_out << "Can not open file ..." << f.full_path << endl;
		return;
	}

	pend << "_|PVNH|20040101\n";
	pend << "_|1000000000000001|20040101|Doe|John\n";
	pend << "_|2000000000000002|20040101|Doe|John\n";
	pend << "_|3000000000000003|20040101|Doe|John\n";
	pend << "#_|3000000000000003|20040101|Doe|John\n";
	pend << "_||||\n";  // empty fields
	pend << "_||\n";	// not enough
	pend << "_||||||||||||\n";  // too many
	pend << "|3000000000000003|20040101|Doe|John\n";  // half annotation
	pend << "_3000000000000003|20040101|Doe|John\n";  // half annotation
	pend << "3000000000000003|20040101|Doe|John\n";		// no annotation
	pend << "\n\n\n\n\n\n";
	pend << "_|4000000000000004|20040101|Doe|John\n";
	pend << "_|5000000000000005|20040101|Doe|John\n";
	pend << "_|6000000000000006|20040101|Doe|John\n";
	pend << "_|PVNT|18\n";
	pend.close();

	tester.runTest(this,f.full_path.c_str(),ut_out);
	return;
}

void ut_FileRecordReader::test_five()
{  
	char * cp=NULL;


	ut_out << "\n=======================================================";
	ut_out << "\nTEST FIVE - no open recs ...";

///----create a pending file------
	fstream pend;
	f_FileName f;
	f.setFileName(root,"test4.in");
	pend.open(f.full_path.c_str(), ios::out);
	if (!pend){
		ut_out << "Can not open file ..." << f.full_path << endl;
		return;
	}
/*
Name: ann, Start: 1, End: 2, Trim Left : 1, Trim Right : 1
Name: bank_account, Start: 3, End: 18, Trim Left : 1, Trim Right : 1
Name: date, Start: 19, End: 28, Trim Left : 0, Trim Right : 1
Name: first_name, Start: 39, End: 48, Trim Left : 0, Trim Right : 0
Name: last_name, Start: 29, End: 38, Trim Left : 1, Trim Right : 0
*/
	pend << "_|PVNH|20040101\n";
	pend << "_|100000000000000120041010  Doe       John      \n";
	pend << "_|200000000000000220041010  Doe       John      \n";
	pend << "_|                                              \n";  // empty fields
	pend << "_|                           \n";	// not enough
	pend << "_|                                                          \n";  // too many
	pend << "|200000000000000220041010  Doe       John      \n";  // half annotation
	pend << "_200000000000000220041010  Doe       John      \n";  // half annotation
	pend << "200000000000000220041010  Doe       John      \n";		// no annotation
	pend << "  200000000000000220041010  Doe       John      \n";		// no annotation
	pend << " 200000000000000220041010  Doe       John      \n";		// no annotation

	pend << "_|300000000000000320041010  Doe       John      \n";
	pend << "_|400000000000000420041010  Doe       John      \n";
	pend << "_|500000000000000520041010  Doe       John      \n";
	pend << "_|600000000000000620041010  Doe       John      \n";
	pend << "_|PVNT|18\n";
	pend.close();

	tester.runTest(this,f.full_path.c_str(),ut_out);
	return;
}

void ut_FileRecordReader::test_six()
{  
	char * cp=NULL;


	ut_out << "\n=======================================================";
	ut_out << "\nTEST SIX - some of everything";

///----create a pending file------
	fstream pend;
	f_FileName f;
	f.setFileName(root,"test4.in");
	pend.open(f.full_path.c_str(), ios::out);
	if (!pend){
		ut_out << "Can not open file ..." << f.full_path << endl;
		return;
	}

	pend << ">|PVNH|20040101\n";
	pend << ">|100000000000000120041010  Doe       John      \n";
	pend << ">|200000000000000220041010  Doe       John      \n";
	pend << ">|                                              \n";  // empty fields
	pend << ">|                           \n";	// not enough
	pend << ">|                                                          \n";  // too many
	pend << "|200000000000000220041010  Doe       John      \n";  // half annotation
	pend << "_200000000000000220041010  Doe       John      \n";  // half annotation
	pend << "200000000000000220041010  Doe       John      \n";		// no annotation
	pend << "  200000000000000220041010  Doe       John      \n";		// no annotation
	pend << " 200000000000000220041010  Doe       John      \n";		// no annotation
	pend << ">|300000000000000320041010  Doe       John      \n";
	pend << ">|400000000000000420041010  Doe       John      \n";
	pend << ">|500000000000000520041010  Doe       John      \n";
	pend << "+|600000000000000620041010  Doe       John      \n";
	pend << "!|700000000000000620041010  Doe       John      \n";
	pend << "~|800000000000000620041010  Doe       John      \n"; //annotation not defined
	pend << "*|900000000000000620041010  Doe       John      \n";
	pend << "!|PVNT|18\n"; //error
	pend << "+|PVNT|18\n";  //dup
	pend << "_|PVNT|18\n";
	pend << ">|PVNT|18\n";
	pend.close();

	tester.runTest(this,f.full_path.c_str(),ut_out);
	return;
}

void ut_FileRecordReader::test_seven()
{  
	char * cp=NULL;


	ut_out << "\n=======================================================";
	ut_out << "\nTEST SEVEN  - bad trailer";

///----create a pending file------
	fstream pend;
	f_FileName f;
	f.setFileName(root,"test4.in");
	pend.open(f.full_path.c_str(), ios::out);
	if (!pend){
		ut_out << "Can not open file ..." << f.full_path << endl;
		return;
	}

	pend << ">|PVNH|20040101\n";
	pend << ">|100000000000000120041010  Doe       John      \n";
	pend << ">|200000000000000220041010  Doe       John      \n";
	pend << ">|PVNT~18\n";
	pend.close();

	tester.runTest(this,f.full_path.c_str(),ut_out);
	return;
}

void ut_FileRecordReader::test_eight()
{  
	char * cp=NULL;


	ut_out << "\n=======================================================";
	ut_out << "\nTEST EIGHT  - bad header";

///----create a pending file------
	fstream pend;
	f_FileName f;
	f.setFileName(root,"test4.in");
	pend.open(f.full_path.c_str(), ios::out);
	if (!pend){
		ut_out << "Can not open file ..." << f.full_path << endl;
		return;
	}

	pend << ">|PVNH\n";
	pend << ">|100000000000000120041010  Doe       John      \n";
	pend << ">|200000000000000220041010  Doe       John      \n";
	pend << ">|PVNT|18\n";
	pend.close();

	tester.runTest(this,f.full_path.c_str(),ut_out);
	return;
}


void ut_FileRecordReader::test_Driver()
{
	string filename = ut_Directory + "ut_FileRecordReader.out";
	root.assign("c:\\temp\\ut_file_reader_simple");
	
	ut_out.open(filename.c_str());
	if (!ut_out){
		ut_out << "Can not open file ..." << filename << endl;
		return;
	}
///----create the pending dir------
	if (0 != f_File::mkdirRecursive(root))
	{	
		ut_out << "Can not create recursive dir: " << root << endl;
		return;
	};

	///----set configs------
	ut_out << "\n========================================================\n";
	ut_out << "\nCONFIGS\n";
	ut_out << "\n========================================================\n";
	if (this->initReader() != Success){
		ut_out << "initReader initialization failed ..." << endl;
	}
	else
	{
		this->config.dump(ut_out);

		ut_out << "\n========================================================\n";
		ut_out << "\nRunning tests ....\n";
		ut_out << "\n========================================================\n";
		test_one();
		test_two();
		test_three();
		test_four();
		test_five();
		test_six();
		test_seven();
		test_eight();
	}
	ut_out.close();
}