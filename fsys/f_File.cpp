// f_File.cpp: implementation of the f_File class.
//
//////////////////////////////////////////////////////////////////////

#include <fsys/f_File.h>

static char *CVS_Id = (char*)"CVS_VERSION:$Id: f_File.cpp,v 1.3 2007/01/24 01:15:07 clomeli Exp $";


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

f_File::f_File()
{

}

f_File::~f_File()
{

}


int f_File::createDatestampFile (
				string idir,		// starting dir 
				string iroot,		// root of file name
				string iext,		// optional extension
				string istamp,		//date stamp in file name:: "YYYYMMDD" or "UTC" or ""
				string isubdirs,	//to create yyyy/mm/dd sibdirs:: "YYYY/MM/DD"
				string &newfile )		//OUT:  name of file created
{
	time_t ltime;
	int retries = 5000;
	int count = 0;
	string str_yearly;
	string str_monthly;
	string str_daily;
	string str_date;
	bool sdyyyy=false;
	bool sdmm=false;
	bool sddd=false;
	bool yyyy=false;
	bool mm=false;
	bool dd=false;
	bool hh=false;
	bool mi=false;
	bool ss=false;
	bool add_timestamp = false;		//add add_timestamp (UTC) to file name
	bool add_datestamp = false;		//add datestamp YYYYMMDDHHMISS to file name
	bool add_date_subdirs = false;	//add YYYY/MM/DD/ to dest path
	bool extension = true;


	//--- setup---------------
	string szdir	= c_Strings::lowercase(idir);	
	string szroot	= c_Strings::lowercase(iroot);	
	string szext	= c_Strings::lowercase(iext);	
	string stamp	= c_Strings::lowercase(istamp);	
	string subdirs	= c_Strings::lowercase(isubdirs);	

	newfile.erase();
	int fd = -1;


	//-- extension? ---
	if ( szext.empty() )
		extension = false;
	
	//-- determine stamp mode--
	if ( stamp.find("utc") != string::npos )
		add_timestamp = true;
	else
	{
		yyyy= ( stamp.find("yyyy") != string::npos );
		mm	= ( stamp.find("mm") != string::npos );
		dd	= ( stamp.find("dd") != string::npos );
		hh	= ( stamp.find("hh") != string::npos );
		mi	= ( stamp.find("mi") != string::npos );
		ss	= ( stamp.find("ss") != string::npos );
		add_datestamp = (yyyy || mm || dd ); // no point in suppporting hh+ only
	}

	//-- determine subdirs mode--
	sdyyyy= ( subdirs.find("yyyy") != string::npos );
	sdmm	= ( subdirs.find("mm") != string::npos );
	sddd	= ( subdirs.find("dd") != string::npos );
	add_date_subdirs = (sdyyyy || sdmm || sddd );

	


	//-- get timestamp ------
	time(&ltime);
	tm * ts = localtime(&ltime);

	//---path string-------------
	ostringstream opath;
	opath << szdir;
	if (add_date_subdirs)
	{
		if (sdyyyy)
			opath << SLASH << (ts->tm_year)+1900;
		if (sdmm)
			opath << SLASH << setiosflags( ios::right ) << setw(2) << setfill('0')
			<< (ts->tm_mon)+1;
		if (sddd)
			opath << SLASH << setiosflags( ios::right ) << setw(2) << setfill('0')
			<< ts->tm_mday;
	}


	//---file string-------------
	ostringstream oroot;
	oroot << opath.str() << SLASH << szroot;

	if (add_timestamp)
		oroot << szroot << SLASH << (long)ltime;
	else if (add_datestamp)
	{
		oroot << PERIOD;

		if (yyyy)
			oroot << (ts->tm_year)+1900;
		if (mm)
			oroot << setiosflags( ios::right ) << setw(2) << setfill('0')
				<< (ts->tm_mon)+1;
		if (dd)
			oroot << setiosflags( ios::right ) << setw(2) << setfill('0')
				<< ts->tm_mday;
		if (hh)
			oroot << setiosflags( ios::right ) << setw(2) << setfill('0')
				<< ts->tm_hour;
		if (mi)
			oroot << setiosflags( ios::right ) << setw(2) << setfill('0')
				<< ts->tm_min;
		if (ss)
			oroot << setiosflags( ios::right ) << setw(2) << setfill('0')
				<< ts->tm_sec;
	}
	string root(oroot.str());
	

	//---mkdir-------------
	int md;
	if ((md=f_File::mkdirRecursive( opath.str() )) < 0 )
		return md;


	//--- try to create the file ---.
	while ( fd == -1 && count < retries ) 
	{
		ostringstream os;
		os << root;  
		if (count>1) os << PERIOD << count;
		count++;

		if (extension)
			os << PERIOD << szext;

		fd=open(os.str().c_str(), O_WRONLY | O_CREAT | O_EXCL, fMASK );
		if (fd > 0)
			newfile.assign(os.str());
	}
	return fd;

};

int f_File::mkdirRecursive(string dirstr)
{
	string path;
	vector<string> tokens;
	std::vector<string>::iterator iptr;


	//--- get rid of any 'c:' designators------
	path.assign(dirstr);
	c_Strings::trimAll(path);
	if (path.empty())
		return -1;


	bool relative_path = (path.find_first_of("/")!=0);


	//--tokenize------------------
	c_Strings::tokenize(path,tokens, SLASH );
	if (tokens.size() < 1)
		return -1;

	if (!relative_path) tokens[0].insert(0,"/");

	//--make subdirs------------------
	ostringstream os;

	for (iptr = tokens.begin(); 
		iptr != tokens.end(); iptr++ )
	{
		os <<  *iptr << SLASH ;
		string file(os.str());
		if ( !f_File::Access(os.str().c_str(),F_EXISTS) ) {
			if ( mkdir(file.c_str(), fMASK) == -1 ) {
				return -1;
			}
		}
	}
	return 0;
}

void f_File::dumpFile(const char * full_path, ostream& ut_out)
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

short f_File::copyTextFile(string sourcefile, string destfile)
{
	fstream file_op;
	file_op.open(sourcefile.c_str(), ios::in);
	if (!file_op){
		return -1;
	}

	ofstream ut_out;
	ut_out.open(destfile.c_str());
	if (!ut_out){
		return -2;
	}

    char str[250];
	while(!file_op.eof()) 
	{
		file_op.getline(str,250);
		ut_out << str << endl;
	}       
	
	ut_out.close();
	file_op.close();
	return 0;
}
