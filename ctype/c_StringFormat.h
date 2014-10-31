#ifndef STRING_UTILITY_MAP_H
#define STRING_UTILITY_MAP_H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <platform/p_Exception.h>
#include <errno.h>

class c_StringFormat {
public:

	c_StringFormat() {};
	~c_StringFormat() {};


	static string& alltrim ( string& str )
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


	static string& lowercase( string& str ) {
		std::transform(str.begin(), str.end(), 
					 str.begin(),  
					 (int(*)(int))tolower ); 
		return str;
	};


	static int StringToInteger(const string numstr) 
	{
		int n;
		if (numstr.empty())
			p_Throw(r_Null_value);

		if (numstr.find_first_not_of('0') == string::npos )
			n = 0;
		else if ((n = atoi(numstr.c_str())) == 0 ) {
				p_Throw(r_Type_Conversion, numstr);
		}

		return n;
	};

	static double StringToDouble( const string s )
	{
		double dd;
		try {
			char *endp;
			errno=0;
			dd = ::strtod(s.c_str(), &endp);
			if (errno != 0) {
				p_Throw(r_Type_Conversion, s);
			}
		} 
		catch (...) {
			p_Throw(r_Type_Conversion, s);
		}
		return dd;
	};

	static long StringToLong ( const string s )
	{
		long long_var;
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
				p_Throw(r_Type_Conversion, s);
			}
			else if ( endp == cp )
			{
				p_Throw(r_Type_Conversion, s);
			}

		} 
		catch (...) {
			p_Throw(r_Type_Conversion, s);
		}
		return true;
	};


	static void tokenize(const string& str,
						  vector<string>& tokens,
						  const string& delimiters = " ")
	{
		// Skip delimiters at beginning.
		string::size_type lastPos = str.find_first_not_of(delimiters, 0);

		// Find first "non-delimiter".
		string::size_type pos     = str.find_first_of(delimiters, lastPos);

		while (string::npos != pos || string::npos != lastPos)
		{
			// Found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));

			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);

			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}
	};


	static time_t StringToUTC(string datestr)
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




#endif //STRING_UTILITY_H

