#include <unit_test/ut_ParserFactory.h>
#include <fsys/f_File.h>

ut_ParserFactory::ut_ParserFactory(void)
{
}

ut_ParserFactory::~ut_ParserFactory(void)
{
}
void ut_ParserFactory::test_two(string root)
{
	f_FileName f;
	f.setFileName(root,"test2.properties");

	ofstream out;
	out.open(f.full_path.c_str());
	if (!out){
		
		return;
	}
	out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
	out << "<root>" << endl;
		out << "<parser state=header>" << endl;
		out << "	<init class=JUNK delimiter=|></init>" << endl;
		out << "	<column index=1 name=annotation></column>" << endl;
		out << "	<column index=2 name=rec_type></column>" << endl;
		out << "	<column index=3 name=date type=Date mask=yyyymmdd></column>" << endl;
		out << "</parser>" << endl;
	out << "</root>" << endl;
	out.close();

	tester.runTest(f.full_path.c_str());
	return;
}
void ut_ParserFactory::test_three(string root)
{
	f_FileName f;
	f.setFileName(root,"test3.properties");

	ofstream out;
	out.open(f.full_path.c_str());
	if (!out){
		
		return;
	}
	out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
	out << "<root>" << endl;
		out << "<parser state=JUNK>" << endl;
		out << "	<init class=simple_delimited delimiter=|></init>" << endl;
		out << "	<column index=1 name=annotation></column>" << endl;
		out << "	<column index=2 name=rec_type></column>" << endl;
		out << "	<column index=3 name=date type=Date mask=yyyymmdd></column>" << endl;
		out << "</parser>" << endl;
	out << "</root>" << endl;
	out.close();

	tester.runTest(f.full_path.c_str());
	return;
}

void ut_ParserFactory::test_four(string root)
{
	f_FileName f;
	f.setFileName(root,"test4.properties");

	ofstream out;
	out.open(f.full_path.c_str());
	if (!out){
		
		return;
	}
	out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
	out << "<root>" << endl;
		out << "<parser state=detail>" << endl;
		out << "	<init class=simple_delimited delimiter=|></init>" << endl;
		out << "	<column index=3 name=annotation></column>" << endl;
		out << "	<column index=2 name=rec_type></column>" << endl;
		out << "	<column index=1 name=date type=Date mask=yyyymmdd></column>" << endl;
		out << "</parser>" << endl;
	out << "</root>" << endl;
	out.close();

	tester.runTest(f.full_path.c_str());
	return;
}

void ut_ParserFactory::test_five(string root)
{
	f_FileName f;
	f.setFileName(root,"test5.properties");

	ofstream out;
	out.open(f.full_path.c_str());
	if (!out){
		
		return;
	}
	out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
	out << "<root>" << endl;
		out << "<parser state=detail>" << endl;
		out << "</parser>" << endl;
	out << "</root>" << endl;
	out.close();

	tester.runTest(f.full_path.c_str());
	return;
}

void ut_ParserFactory::test_six(string root)
{
	f_FileName f;
	f.setFileName(root,"test6.properties");

	tester.runTest(f.full_path.c_str());
	return;
}

void ut_ParserFactory::test_seven(string root)
{
	f_FileName f;
	f.setFileName(root,"test7.properties");

	ofstream out;
	out.open(f.full_path.c_str());
	if (!out){
		
		return;
	}
	out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
	out << "<root>" << endl;
		out << "<parser state=detail>" << endl;
		out << "	<init class=simple_fixed/>" << endl;
		out << "	<column end=2 begin=1 name=annotation></column>" << endl;
		out << "	<column name=bank_account	begin=  3 end=18></column>" << endl;
		out << "	<column name=date       	begin= 19 end=28 type=date mask=yyyymmdd></column>" << endl;
		out << "	<column name=last_name		begin= 29 end=38 right=false></column>" << endl;
		out << "	<column name=first_name		begin= 39 end=48 left=false></column>" << endl;
		out << "</parser>" << endl;
	out << "</root>" << endl;
	out.close();

	tester.runTest(f.full_path.c_str());
	return;
}

void ut_ParserFactory::test_eight(string root)
{
	f_FileName f;
	f.setFileName(root,"test8.properties");

	ofstream out;
	out.open(f.full_path.c_str());
	if (!out){
		
		return;
	}
	out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
	out << "<root>" << endl;
		out << "<parser state=detail>" << endl;
		out << "	<init class=simple_fixed/>" << endl;
		out << "	<column end=2 begin=1 name=annotation></column>" << endl;
		out << "	<column name=bank_account	begin=  3 end=18></column>" << endl;
		out << "	<column name=date       	begin= 39 end=28 type=date mask=yyyymmdd></column>" << endl;
		out << "	<column name=last_name		begin= 29 end=38 right=false></column>" << endl;
		out << "	<column name=first_name		begin= 39 end=48 left=false></column>" << endl;
		out << "</parser>" << endl;
	out << "</root>" << endl;
	out.close();

	tester.runTest(f.full_path.c_str());
	return;
}



void ut_ParserFactory::test_one(string root)
{
	f_FileName f;
	f.setFileName(root,"test1.properties");

	ofstream out;
	out.open(f.full_path.c_str());
	if (!out){
		
		return;
	}
	out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
	out << "<root>" << endl;
		out << "<parser state=header>" << endl;
		out << "	<init class=simple_delimited delimiter=|></init>" << endl;
		out << "	<column index=1 name=annotation></column>" << endl;
		out << "	<column index=2 name=rec_type></column>" << endl;
		out << "	<column index=3 name=date type=Date mask=yyyymmdd></column>" << endl;
		out << "</parser>" << endl;
		out << "<parser state=trailer>" << endl;
		out << "	<init class=simple_delimited delimiter=|/>" << endl;
		out << "	<column index=1 name=annotation></column>" << endl;
		out << "	<column index=2 name=rec_type></column>" << endl;
		out << "	<column index=3 name=count type=Number></column>" << endl;
		out << "</parser>" << endl;
		out << "<parser state=detail>" << endl;
		out << "	<init class=simple_fixed/>" << endl;
		out << "	<column name=annotation		begin=  1 end=2></column>" << endl;
		out << "	<column name=bank_account	begin=  3 end=18></column>" << endl;
		out << "	<column name=date       	begin= 19 end=28 type=date mask=yyyymmdd></column>" << endl;
		out << "	<column name=last_name		begin= 29 end=38 right=false></column>" << endl;
		out << "	<column name=first_name		begin= 39 end=48 left=false></column>" << endl;
		out << "</parser>" << endl;
	out << "</root>" << endl;
	out.close();

	tester.runTest(f.full_path.c_str());


	///---- get values -------------------

	return;
}


void ut_ParserFactory::test_Driver()
{  
	char * cp=NULL;
	string root = ut_Directory + "/ut_ParserFactory";
	f_FileName f;
	f.setFileName(root,"ut_ParserFactory.out");

///----create the pending dir------
	if (0 != f_File::mkdirRecursive(root))
	{	
		ut_out << "Can not create recursive dir: " << root << endl;
		return;
	};

///----------------
	test_one(root);
	test_two(root);
	test_three(root);
	test_four(root);
	test_five(root);
	test_six(root);
	test_seven(root);
	test_eight(root);
	return;
}

