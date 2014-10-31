#ifndef UTILITY_MAP_H
#define UTILITY_MAP_H

#include "EnvironmentStd.h"
#include "Portability.h"
#include <errno.h>




class util
{
public:

	static string& datestamp(string& tstr)
	{
		time_t ltime;

		time(&ltime);
		tm * ts = localtime(&ltime);

		ostringstream ti;
		ti  << (ts->tm_year)+1900
			<< setiosflags( ios::right ) << setw(2) << setfill('0')
			<< (ts->tm_mon)+1
			<< setiosflags( ios::right ) << setw(2) << setfill('0')
			<< ts->tm_mday;

		tstr = ti.str();
		return tstr;
	};

	//**************************************************************
	// Function clean_pathname
	//**************************************************************
	// Function:	Clean up \ chars in path name by changing to \\ 
	// Operation:	Walks string looking for \ and users string::replace
	//				do do the replace.
	static string& clean_pathname(string& str, char from, string to)
	{
			// Skip delimiters at beginning.
			string::size_type lastPos, pos;
			for (lastPos=0, pos = str.find_first_of(from, lastPos);
				pos != string::npos;
				lastPos = pos + to.length() , pos = str.find_first_of(from, lastPos) )
			{
				str.replace(pos,1,to);
			}
			return str;
	};


	static string& datestampYYMMDD(string& tstr)
	{
		time_t ltime;

		time(&ltime);
		tm * ts = localtime(&ltime);

		ostringstream ti;
		ti  << setiosflags( ios::right ) << setw(2) << setfill('0') 
			<< (ts->tm_year - 100)
			<< setiosflags( ios::right ) << setw(2) << setfill('0')
			<< (ts->tm_mon)+1
			<< setw(2) << setfill('0') << ts->tm_mday;

		tstr = ti.str();
		return tstr;
	};


	static string& timestamp(string& tstr)
	{
		time_t ltime;

		time(&ltime);
		tm * ts = localtime(&ltime);

		ostringstream ti;
		ti  << (ts->tm_year)+1900
			<< setiosflags( ios::right ) << setw(2) << setfill('0')
			<< (ts->tm_mon)+1
			<< ts->tm_mday
			<< ts->tm_hour << ts->tm_min << ts->tm_sec;

		tstr = ti.str();
		return tstr;
	};

	static string& alltrim (string& str )
	{
		if ( str.empty() )
			return str;

		 // trim leading whitespace
		   string::size_type  notwhite = str.find_first_not_of(" \t\n");
		   str.erase(0,notwhite);

		   // trim trailing whitespace
		   notwhite = str.find_last_not_of(" \t\n"); 
		   str.erase(notwhite+1); 
		   return str;
	};

	static string& uppercase( string& str ) {
		std::transform(str.begin(), str.end(), 
					 str.begin(),  
					 (int(*)(int))toupper ); 
		return str;
	};

	static string& lowercase( string& str ) {
		std::transform(str.begin(), str.end(), 
					 str.begin(),  
					 (int(*)(int))tolower ); 
		return str;
	};

	static void
	pathtok (string pathname, string& dir, string& filename, string& root, string& ext )
	{
		string::size_type slash = pathname.find_last_of(SLASH);
		string::size_type dot = pathname.rfind('.');

		if (slash == string::npos)  {
			dir.assign("");
			filename.assign( pathname );
		}
		else {  
			dir.assign( pathname.substr(0,slash));
			filename.assign( pathname, slash+1, pathname.length() );
		}

		if (dot == string::npos) {
			ext.assign("");
			root.assign(filename);
		} else {
			ext.assign( pathname.substr(dot+1));
			root.assign( filename.substr(0, filename.rfind('.')));
		}
	};


	static bool str_to_double( const string s, double & dd, bool dec = false)
	{
		try {
			char *endp;
			errno=0;
			dd = ::strtod(s.c_str(), &endp);
			if (errno != 0) {
					return false;
			}
			if (dec) dd /= 100;
		} 
		catch (...) {
			return false;
		}
		return true;
	};

	static bool str_to_long( const string s, long& long_var )
	{
		char *cp = NULL;
		char buff[100];
		
		memset(buff,0,100);
		sprintf(buff,s.c_str());
		cp = &buff[0];

		try {
			char *endp;
			errno=0;
			long_var = strtol( cp, &endp, 10);
			if (errno != 0) {
					return false;
			}
			else if ( endp == cp )
			{
				return false;
			}

		} 
		catch (...) {
			return false;
		}
		return true;
	};

	static int createfile (
		string& str_fullpath,
		int& fd,
		const char *szdir, 
		const char *szroot, 
		const char *szext,
		const char * mode ) //mode =YYYY/MM/DD/UTC/COPY 
	{
		time_t ltime;
		int retries = 5000;
		int count = 0;
		char buffer[PATH_MAX];
		char tmpbuffer[PATH_MAX];
		string str_yearly;
		string str_monthly;
		string str_daily;
		string str_date;
		bool yyyy=false;
		bool mm=false;
		bool dd=false;
		bool add_timestamp = false;
		bool add_datestamp = false;
		bool copy_file = false;
		bool extension = true;

		fd = -1;

		//-- extension? ---
		if ( szext == (char*)NULL )
			extension = false;

		//-- copy_file? ---
		if (strstr(mode,"COPY") != NULL )
			copy_file = true;

		if ( strstr(mode,"UTC") != NULL )
			add_timestamp = true;

		//-- determine mode--
		if ( strstr(mode,"YYYY") != NULL )
			yyyy = true;

		if ( strstr(mode,"MM") != NULL )
			mm = true;

		if ( strstr(mode,"DD") != NULL )
			dd = true;

		if ( strstr(mode,"DATESTAMP") != NULL )
			add_datestamp = true;

		if ( strstr(mode,"UTC") != NULL )
			add_timestamp = true;

		//--for filename --
		time(&ltime);

		//--for yyy/mm/dd directory --
		util::datestamp(str_date);
		str_yearly.assign(  str_date.substr(0,4) );
		if (yyyy)
			str_monthly.assign( str_date.substr(4,2) );
		else
			str_monthly.assign( str_date.substr(0,6) );

		if ( mm )
			str_daily.assign( str_date.substr(6,2) );
		else if (yyyy)
			str_daily.assign( str_date.substr(4,4) );
		else
			str_daily.assign( str_date );

		//--does base directory exist? ----
		if ( !util::Access(szdir,F_EXISTS) )
			return -1;
		else
			sprintf(buffer,"%s", szdir);

		//--can we find or make yearly? ----
		strcpy(tmpbuffer,szdir);
		if (yyyy) 
		{
			sprintf(buffer,"%s%s%s", 
					tmpbuffer, SLASH, str_yearly.c_str());
			sprintf(tmpbuffer,buffer);

			if ( !util::Access(buffer,F_EXISTS) )
				if ( mkdir(buffer, fMASK) == -1 )
					return -1;
		}

		//--can we find or make monthly? ----
		if (mm) 
		{
			sprintf(buffer,"%s%s%s", 
					tmpbuffer, SLASH, 
					str_monthly.c_str() ); 
			sprintf(tmpbuffer,buffer);

			if ( !util::Access(buffer,F_EXISTS) )
				if ( mkdir(buffer, fMASK) == -1 )
					return -1;
		}

		//--can we find or make daily? ----
		if (dd) 
		{
			sprintf(buffer,"%s%s%s", 
					tmpbuffer, SLASH,
					str_daily.c_str());

			if ( !util::Access(buffer,F_EXISTS) )
				if ( mkdir(buffer,fMASK) == -1 )
					return -1;
		}

		//--- add timestamp --- or not...
		if (add_datestamp)
			sprintf(tmpbuffer,"%s%s%s.%s", buffer, SLASH, szroot, str_date.c_str());
		else if (add_timestamp)
			sprintf(tmpbuffer,"%s%s%s.%d", buffer, SLASH, szroot, (int)ltime );
		else
			sprintf(tmpbuffer,"%s%s%s", buffer, SLASH, szroot );

		//--- try to create the file ---.
		if ( copy_file ) 
		{
			fd=open(tmpbuffer, O_WRONLY | O_CREAT | O_EXCL, fMASK );
		}

		while ( fd == -1 && count < retries ) 
		{
			if (extension)
				sprintf(buffer,"%s.%04d.%s", tmpbuffer, ++count, szext );
			else
				sprintf(buffer,"%s.%04d", tmpbuffer, ++count );

			fd=open(buffer, O_WRONLY | O_CREAT | O_EXCL, fMASK );
		}

		str_fullpath.assign(buffer);
		return fd;
	};


	static bool str_to_int( const string s, int& n )
	{
		const char *cp = s.c_str();
		return str_to_int( cp, n );
	};

	static bool str_to_int( const char* cp, int& n )
	{
		errno=0;
		n = atoi(cp);
		if (errno != 0) {
				return false;
		}
		return true;
	};

	static void tokenize(const string& str,
							vector<int>& tokens,
							const string& delimiters = " " )
	{
		// Skip delimiters at beginning.
		string::size_type lastPos = str.find_first_not_of(delimiters, 0);

		// Find first "non-delimiter".
		string::size_type pos     = str.find_first_of(delimiters, lastPos);

		while (string::npos != pos || string::npos != lastPos)
		{
			// Found a token, add it to the vector.
			string s;
			s.assign( str.substr(lastPos, pos - lastPos) );

			errno = 0;
			int i = atoi(s.c_str());
			if (errno == 0)
				tokens.push_back(i);

			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);

			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}
	};

	static void tokenize(const string& str,
							vector<string>& tokens,
							const string& delimiters = " ", bool trim = true )
	{
		// Skip delimiters at beginning.
		string::size_type lastPos = str.find_first_not_of(delimiters, 0);

		// Find first "non-delimiter".
		string::size_type pos     = str.find_first_of(delimiters, lastPos);

		while (string::npos != pos || string::npos != lastPos)
		{
			// Found a token, add it to the vector.
			string col = str.substr(lastPos, pos - lastPos);
			if (trim) util::alltrim(col);
			tokens.push_back(col);

			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);

			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}
	};

	static char * shrink ( const char* in, char* buffer )
	{
		string::size_type  pos;
		int j = 0;
		string::size_type  len = (int)strlen(in);

		for ( pos=0; pos < len; pos++ ) {
			if (isalnum( in[pos] ))
				buffer[j++] = in[pos];
		}
		buffer[j] = '\0';
		return buffer;
	}

	static bool Access(string filestr, int flag)
	{
		if ( access( filestr.c_str(), flag ) == 0 )
			return true;
		else
			return false;
	};

	static time_t str_to_utc(string datestr)
	{
		//-- get year first - then assume day is after year
		string utcstr = "";
		string yy,mm,dd,hh,mi,ss;
		bool yearisfirst = false, hastimestr = false;
		size_t len1=0, len2=0, len3=0;

		time_t utc = 0;

		//-- date part
		if ( datestr.length() < 10 )
			return 0;

		string::size_type s1=0,s2=0, s3=0;

		if ((s1 = datestr.find_first_of("./-",0)) <= 0 )
			return utc;
		if ((s2 = datestr.find_first_of("./-",s1+1)) <= 0 )
			return utc;
		if ((s3 = datestr.find_first_of(" ",s2+1)) <= 0 )
			s3 = datestr.length();
		else
			hastimestr = true;

		if (
			((len1 = s1-0) >= 1 && len1 <= 4) &&
			((len2 = s2-s1-1) >= 1 && len2 <= 2) &&
			((len3 = s3-s2-1) >= 1 && len3 <= 4) 
			)
		{
			if ( len1 == 4 )
				yearisfirst = true;
			else if (len3 == 4)
				yearisfirst = false;
			else
				return utc;
				
			if ( yearisfirst ) 
			{
				yy = datestr.substr(0,len1);
				mm = datestr.substr(s1+1, len2);
				dd = datestr.substr(s2+1, len3);
			} else {
				mm = datestr.substr(0,len1);
				dd = datestr.substr(s1+1, len2);
				yy = datestr.substr(s2+1, len3);
	}
		}
		else 
			return utc;
		
		//-- Time part
		hh = "00";
		mi = "00";
		ss = "00";

		if ( hastimestr && s2+1+len3 < datestr.length())
		{
			string timestr = 
				datestr.substr(
					datestr.find_first_of("0123456789",s2+1+len3));


			if ( timestr.length() >= 8  )
			{
				if ((s1 = timestr.find_first_of(":",0)) != 0)
					if ((s2 = timestr.find_first_of(":",s1+1)) != 0 ) 
					{
						if ((s3 = timestr.find_first_of(" ",s2+1)) != 0 )
							s3 = timestr.length();

						if (
							((len1 = s1-0) >= 1 && len1 <= 2) &&
							((len2 = s2-s1-1) >= 1 && len2 <= 2) &&
							((len3 = s3-s2-1) >= 1 && len3 <= 2) )
						{
							hh = timestr.substr(0,len1);
							mi = timestr.substr(s1+1, len2);
							ss = timestr.substr(s2+1, len3);
						}
					}
			}
		}
		//-- check values
		int nYYYY, nMM, nDD, nHH, nMI, nSS;
			nYYYY =  nMM =  nDD =  nHH =  nMI =  nSS = 0;

		if ((nMM=atoi(mm.c_str())) <= 0 || nMM > 12 )
			return utc;

		if ((nDD=atoi(dd.c_str())) < 0 || nDD > 31 )
			return utc;

		if ((nYYYY=atoi(yy.c_str())) < 1970 || nYYYY > 2035 )
			return utc;

		if ((nHH=atoi(hh.c_str())) < 0 || nHH > 24 )
			return utc;

		if ((nMI=atoi(mi.c_str())) < 0 || nMI > 60 )
			return utc;

		if ((nSS=atoi(ss.c_str())) < 0 || nSS > 60 )
			return utc;

		//-- now make the date
		struct tm osTime;
		osTime.tm_sec		=   nSS;    // seconds after the minute -- [0,60] /
		osTime.tm_min		=	nMI;    // minutes after the hour -- [0,59] /
		osTime.tm_hour		=	nHH;   // hours since midnight -- [0,23] /
		osTime.tm_mday		=	nDD;   // day of the month -- [1,31] /
		osTime.tm_mon		=	nMM -1;    // months since January -- [0,11] /
		osTime.tm_year		=	nYYYY - 1900; // years since 1900 /
		osTime.tm_isdst		=	-1; // Daylight Savings Time flag /
		
		utc = mktime(&osTime);
		return utc;
	};



};

#endif //UTILITY_H

