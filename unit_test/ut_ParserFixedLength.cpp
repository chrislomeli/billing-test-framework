#include <unit_test/ut_ParserFixedLength.h>

ut_ParserFixedLength::ut_ParserFixedLength(void)
{
}

ut_ParserFixedLength::~ut_ParserFixedLength(void)
{
}


void ut_ParserFixedLength::test_parse(
			c_ParserFixedLength& parser, 
			string record,
			ofstream& of)
{
	map<string,c_NvPair> pairs;
	map<string,c_NvPair>::iterator pos;

	of << "\n------------Rec: " << record << "---------------\n";

	parser.parseRecord(record, pairs);
	if (parser.errorRec.isError())
	{
		if ( parser.errorRec.errType == definition_error )
			of << "Definition Error = " ;
		else
			of << "Parse Error = " ;
			
		of << parser.errorRec.errCode << " : " << parser.errorRec.desc << endl;
	}
	else
	{
		for (pos = pairs.begin(); pos != pairs.end(); pos++ ){
			string name = pos->first;
			string value = pos->second.name.c_str();
			of << "[" << name << "]: {" << value.c_str() << "}" << endl;
		}
	}
	
	return;

}

void ut_ParserFixedLength::test_Driver()
{  
	char * cp=NULL;
	string filename = ut_Directory + "ut_ParserFixedLength.out";
	ofstream ut_out;


	c_ParserFixedLength parser;

	ut_out.open(filename.c_str());
	if (!ut_out){
		cout << "Can not open file ..." << filename << endl;
		return;
	}

///----------------
	ut_out << "\n/////////////////TEST 1/////////////////////////////\n";
	parser.defineColumn("field1",1,5);  // trim all
	parser.defineColumn("field2",6,10); //trim all
	parser.defineColumn("field3",11,20,false,true);  //left spaces left
	parser.defineColumn("field4",21,26,true,false);  //right spaces left
	parser.defineColumn("field5",27,35,false,false);  // all spaces left
	if (parser.errorRec.isError())
	{
		ut_out << "Definition Error = " 
			<< parser.errorRec.errCode <<  ": " << parser.errorRec.desc << endl;
	}	
	parser.dumpDefinitions(ut_out);
	test_parse(parser,"|f1 ||f2 ||     f3 || f4 ||  f5   |", ut_out);
	test_parse(parser," f1   f2        f3    f4     f5    ", ut_out);
	test_parse(parser,"", ut_out);
	test_parse(parser,"                                    ", ut_out);
	test_parse(parser,"vbbb ", ut_out);

///----------------
	ut_out << "\n/////////////////TEST 2 -mix defintion order/////////////////////////////\n";
	parser.clearDefinitions();
	parser.defineColumn("field5",27,35,false,false);  // all spaces left
	parser.defineColumn("field4",21,26,true,false);  //right spaces left
	parser.defineColumn("field3",11,20,false,true);  //left spaces left
	parser.defineColumn("field1",1,5);  // trim all
	parser.defineColumn("field2",6,10); //trim all
	if (parser.errorRec.isError())
	{
		ut_out << "Definition Error = " 
			<< parser.errorRec.errCode <<  ": " << parser.errorRec.desc << endl;
	}	
	parser.dumpDefinitions(ut_out);
	test_parse(parser,"|f1 ||f2 ||     f3 || f4 ||  f5   |", ut_out);
	test_parse(parser," f1   f2        f3    f4     f5    ", ut_out);
	test_parse(parser,"", ut_out);
	test_parse(parser,"                                    ", ut_out);
	test_parse(parser,"vbbb ", ut_out);


///--------------------
	ut_out << "\n/////////////////TEST 3 -m_protect_field_name = true /////////////////////////////\n";
	parser.clearDefinitions();
	parser.m_protect_field_name = true;
	parser.defineColumn("field1",1,5);  // trim all
	parser.defineColumn("field1",6,15); //trim all
	parser.dumpDefinitions(ut_out);
	if (parser.errorRec.isError())
	{
		ut_out << "Definition Error = " 
			<< parser.errorRec.errCode <<  ": " << parser.errorRec.desc << endl;
	}	
	test_parse(parser," f1   f2        f3    f4     f5    ", ut_out);

///--------------------
	ut_out << "\n/////////////////TEST 4 -m_protect_field_name = false /////////////////////////////\n";
	parser.clearDefinitions();
	parser.m_protect_field_name = false;
	parser.defineColumn("field1",1,5);  // trim all
	parser.defineColumn("field1",6,15); //trim all
	parser.dumpDefinitions(ut_out);
	if (parser.errorRec.isError())
	{
		ut_out << "Definition Error = " 
			<< parser.errorRec.errCode <<  ": " << parser.errorRec.desc << endl;
	}	
	test_parse(parser," f1   f2        f3    f4     f5    ", ut_out);


///--------------------
	ut_out << "\n/////////////////TEST 5 -m_reject_overlaps = true/////////////////////////////\n";
	parser.clearDefinitions();
	parser.m_reject_overlaps = true;
	parser.defineColumn("field1",1,5);  // trim all
	parser.defineColumn("field2",1,5); //trim all
	parser.defineColumn("field3",1,5,false,true);  //left spaces left
	parser.defineColumn("field4",1,5,true,false);  //right spaces left
	parser.defineColumn("field5",1,5,false,false);  // all spaces left
	parser.dumpDefinitions(ut_out);
	if (parser.errorRec.isError())
	{
		ut_out << "Definition Error = " 
			<< parser.errorRec.errCode <<  ": " << parser.errorRec.desc << endl;
	}	
	test_parse(parser," f1   f2        f3    f4     f5    ", ut_out);

///--------------------
	ut_out << "\n/////////////////TEST 6 -m_reject_overlaps = false/////////////////////////////\n";
	parser.clearDefinitions();
	parser.m_reject_overlaps = false;
	parser.defineColumn("field1",1,5);  // trim all
	parser.defineColumn("field2",1,5); //trim all
	parser.defineColumn("field3",1,5,false,true);  //left spaces left
	parser.defineColumn("field4",1,5,true,false);  //right spaces left
	parser.defineColumn("field5",1,5,false,false);  // all spaces left
	parser.dumpDefinitions(ut_out);
	if (parser.errorRec.isError())
	{
		ut_out << "Definition Error = " 
			<< parser.errorRec.errCode <<  ": " << parser.errorRec.desc << endl;
	}	
	test_parse(parser," f1   f2        f3    f4     f5    ", ut_out);


///----------------------
	ut_out << "\n/////////////////TEST 7 -empty field name f2/////////////////////////////\n";
	parser.clearDefinitions();
	parser.defineColumn("field1",1,5);  // trim all
	parser.defineColumn("",6,10); //trim all
	parser.dumpDefinitions(ut_out);
	if (parser.errorRec.isError())
	{
		ut_out << "Definition Error = " 
			<< parser.errorRec.errCode <<  ": " << parser.errorRec.desc << endl;
	}	
	test_parse(parser,"|f1 ||f2 ||     f3 || f4 ||  f5   |", ut_out);
	test_parse(parser," f1   f2        f3    f4     f5    ", ut_out);
	test_parse(parser,"", ut_out);
	test_parse(parser,"                                    ", ut_out);

///----------------------
	ut_out << "\n/////////////////TEST 8 - f1-start==0/////////////////////////////\n";
	parser.clearDefinitions();
	parser.defineColumn("FIELD2",6,10); //trim all
	parser.defineColumn("field1",0,5);  // trim all
	parser.dumpDefinitions(ut_out);
	if (parser.errorRec.isError())
	{
		ut_out << "Definition Error = " 
			<< parser.errorRec.errCode <<  ": " << parser.errorRec.desc << endl;
	}	
	test_parse(parser,"|f1 ||f2 ||     f3 || f4 ||  f5   |", ut_out);
	test_parse(parser," f1   f2        f3    f4     f5    ", ut_out);
	test_parse(parser,"", ut_out);
	test_parse(parser,"                                    ", ut_out);



	ut_out.close();

	return;
}

