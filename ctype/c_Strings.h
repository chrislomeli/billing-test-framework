// c_String.h: interface for the c_String class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C_STRING_H__4078DA8F_6287_4F27_B069_F8F4B380E9C4__INCLUDED_)
#define AFX_C_STRING_H__4078DA8F_6287_4F27_B069_F8F4B380E9C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <util/EnvironmentAnsiC.h>
#include <platform/Portability.h>
#include <ctype/c_Types.h>

class c_Strings  
{
public:
	c_Strings();
	virtual ~c_Strings();

	static bool stringToBool(string str, bool default_val = false) 
	{
		if (str.empty())
			return default_val;

		return c_Strings::charToBool(str[0],default_val);
	};
	static bool stringToBool(const char * ds, bool default_val = false) 
	{
		if (!ds || strlen(ds) < 1)
			return default_val;

		return c_Strings::charToBool(ds[0],default_val);
	};
	static bool charToBool(const char dc, bool default_val = false) 
	{
		char udc = toupper(dc);
		switch (udc)
		{
		case '1':
		case 'Y':
		case 'T':
			return true;
		case '0':
		case 'F':
		case 'N':
			return false;
		default:
			return default_val;
		}
	};

	////---3 version of intger conversion ---------------------
	static int 
		stringToInteger ( const string s, int * errp, bool ignore_trailing  = true)
	{
		int int_var = 0;
		long long_var = 0;
		char *end_ptr = NULL;
		
		
		long_var = 0;
        errno = 0;
		if (errp) *errp = 0;
        
        long_var = strtol(s.c_str(), &end_ptr, 0);
 		if (errp) *errp = errno;
       
		switch (errno)
		{
		case 0:
			break;
		case ERANGE:
		default:
			return 0;
		}

        if (long_var > INT_MAX)
        {
			if (errp) *errp = ERANGE;
        }
        else if (long_var < INT_MIN)
        {
			if (errp) *errp = ERANGE;
        }
        else if (end_ptr == s  )
        {
			if (errp) *errp =  EBADF;
        }
        else if (end_ptr != '\0' && ! ignore_trailing)
        {
			if (errp) *errp = EBADF;
        }
		else
            int_var = (int)long_var;

		return int_var;
	};

	static int 
		stringToInteger ( const string s,  bool ignore_trailing = true ) 
	{
		int int_var;
		int err;

		int_var = c_Strings::stringToInteger(s,&err,ignore_trailing);
		if (err != 0)
			throw range_error("Can not convert to int: "+s);

		return int_var;
	};



	static int stringToLong ( const string s, long * long_var, bool ignore_trailing )
	{
		long long_long_var = 0;
		char *end_ptr = NULL;
		
		
		long_long_var = 0;
		long_var = 0;
        errno = 0;
        
        long_long_var = strtol(s.c_str(), &end_ptr, 0);
        
		switch (errno)
		{
		case 0:
			break;
		case ERANGE:
		default:
			return errno;
		}

		if (end_ptr == s)
        {
			return EBADF;
        }
        else if (end_ptr != '\0' && ! ignore_trailing)
        {
			return EBADF;
        }
		
		*long_var = (long)long_long_var;
		return 0;
	};


	static int stringToDouble ( const string s, double * double_var, bool ignore_trailing )
	{
		double d_var;
		char *end_ptr = NULL;
		
		
		d_var = 0;
        errno = 0;
        
        *double_var = strtod(s.c_str(), &end_ptr);
        
        if (ERANGE == errno)
        {
            return ERANGE;
        }
		if (end_ptr == s  )
        {
			return EBADF;
        }
        else if (end_ptr != '\0' && ! ignore_trailing)
        {
			return EBADF;
        }

        switch (errno)
		{
		case 0:
			break;
		case ERANGE:
            return errno;
		default:
            return errno;
		}
	
		return 0;
	};



	static string& trimAll (string& str, string delimiters = " \t\n" )
	{
		//--sanity--
		if (str.empty())
			return str;

		// trim leading whitespace
		string::size_type  notwhite = str.find_first_not_of(delimiters);
		if (notwhite > 0)
			str.erase(0,notwhite);

		// trim trailing whitespace
		notwhite = str.find_last_not_of(delimiters); 
		if (notwhite < str.length())
			str.erase(notwhite+1); 
		return str;
	};

	static string& trimLeft (string& str )
	{
		//--sanity--
		if (str.empty())
			return str;

		// trim leading whitespace
		string::size_type  notwhite = str.find_first_not_of(" \t\n");
		if (notwhite > 0)
			str.erase(0,notwhite);
		return str;
	};
	static string& trimRight (string& str )
	{
		//--sanity--
		if (str.empty())
			return str;
		// trim trailing whitespace
		string::size_type notwhite = str.find_last_not_of(" \t\n"); 
		if (notwhite < str.length())
			str.erase(notwhite+1); 
		return str;
	};

	 static inline string& lowercase( string& str ) {
		std::transform(str.begin(), str.end(), 
					 str.begin(),  
					 (int(*)(int))tolower ); 
		return str;
	};

	static inline string& uppercase( string& str ) {
		std::transform(str.begin(), str.end(), 
					 str.begin(),  
					 (int(*)(int))toupper ); 
		return str;
	};

	static void tokenize(const string& str,
						  vector<string>& tokens,
						  const string& delimiters = " \t\n")
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

#endif // !defined(AFX_C_STRING_H__4078DA8F_6287_4F27_B069_F8F4B380E9C4__INCLUDED_)
