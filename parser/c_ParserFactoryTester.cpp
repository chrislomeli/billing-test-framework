#include <parser/c_ParserFactoryTester.h>
#include <fsys/f_File.h>

c_ParserFactoryTester::c_ParserFactoryTester(void)
{
}

c_ParserFactoryTester::~c_ParserFactoryTester(void)
{
}

void c_ParserFactoryTester::runTest(const char * xml)
{  
	f_FileName f;
	f.setFullPath(xml);

	ostringstream os;
	os << f.dir_name << SLASH << f.file_root_name << PERIOD << "out";
	string output_file(os.str());

	ofstream out;
	out.open(output_file.c_str());
	if (!out){
		cout << "Failed to open " << output_file << endl;
		return;
	}
	this->runTest(xml,out);

}
void c_ParserFactoryTester::runTest(const char * xml, ostream& ut_out)
{  
///---
	c_ParserFactory factory;


///----------------
	ut_out << "\n==========================\nCONFIG FILE=" << xml << "\n==========================\n";
	f_File::dumpFile(xml,ut_out);

	ut_out << "\n==========================\nREAD CONFIG FILE\n==========================\n";
	if (factory.readParserConfigFile(xml) != Success)
		factory.factoryErrorState.dump(ut_out);

	ut_out << "\n==========================\nDUMP CONFIG FILE\n==========================\n";
	factory.dumpConfigs(ut_out);

	ut_out << "\n==========================\nGET NAMES\n==========================\n";
	map<string,c_Parser*>::iterator pos = factory.parsers.begin();
	for (; pos != factory.parsers.end(); pos++)
		this->names.push_back(pos->first);

	vector<string>::iterator vpos = names.begin();
	for (; vpos != names.end(); vpos++)
	{ string name = *vpos;
		ut_out << "\n\n----------FIND NAME: " << name << "--------------" << endl;
		if ((pos = factory.parsers.find(name)) != factory.parsers.end())
		{	c_Parser * cp = pos->second;
			cp->dumpDefinitions(ut_out);
		}
	}

	ut_out << "\n==========================\nGET BOGUS NAME\n==========================\n";

	string name;
	ut_out << "----------FIND NAME: " << name << "--------------" << endl;
	if ((pos = factory.parsers.find(name)) != factory.parsers.end())
	{	
		c_Parser * cp = pos->second;
		cp->dumpDefinitions(ut_out);
	}
	name ="GARBAGE";
	ut_out << "----------FIND NAME: " << name << "--------------" << endl;
	if ((pos = factory.parsers.find(name)) != factory.parsers.end())
	{	
		c_Parser * cp = pos->second;
		cp->dumpDefinitions(ut_out);
	}
	name = "HEADER";
	ut_out << "----------FIND NAME: " << name << "--------------" << endl;
	if ((pos = factory.parsers.find(name)) != factory.parsers.end())
	{	
		c_Parser * cp = pos->second;
		cp->dumpDefinitions(ut_out);
	}

	name = "    header    ";
	ut_out << "----------FIND NAME: {" << name << "}--------------" << endl;
	if ((pos = factory.parsers.find(name)) != factory.parsers.end())
	{	
		c_Parser * cp = pos->second;
		cp->dumpDefinitions(ut_out);
	}


	return;
}


void c_ParserFactoryTester::findParser(string name)
{
}
