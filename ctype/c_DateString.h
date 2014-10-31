#ifndef __c_DateString_DATE_FORMAT_H
#define __c_DateString_DATE_FORMAT_H
#pragma once

#include <util/EnvironmentAnsiC.h>
#include <util/EnvironmentStd.h>

#define DATE_BUFF_LEN 40
#define MAX_DATE_FIELDS 7


struct c_DateString
{
public:
	struct tag_tm_date{
		short m_mm;
		short m_dd;
		short m_yy;
		short m_hh;
		short m_mi;
		short m_ss;
		short m_ms;
		short m_cc;
		short m_isdst;
		short m_errno;
		char  m_errtype;
		short m_trailing_chars;
		bool  b_valid_date;
	} tm_date;

	struct dtfmt{
		char type;			// MDYHIS  (I=minutes)
		char ends_with;		//delimter
		short min_chars;			
		short max_chars;
	};

	short MIN_YEAR, MAX_YEAR;
	dtfmt configs[MAX_DATE_FIELDS];



	c_DateString(string datestr, string mask) 
		: MIN_YEAR(1900)
		, MAX_YEAR(3000)
	{
		this->reset_date();
		this->reset_format();
		fromString(datestr, mask );
	};

	c_DateString() 
		: MIN_YEAR(1900)
		, MAX_YEAR(3000)
	{
		this->reset_date();
		this->reset_format();
	};

	void dump_date_format() 
	{
		for (int i =0; i < MAX_DATE_FIELDS; i++ )
		{
			cout << configs[i].type << ":: " 
				<< "delim(" << configs[i].ends_with << ") ." 
				<< "  min(" << configs[i].min_chars << ") ." 
				<< "  max(" << configs[i].max_chars << ") ." << endl;
		}
	};

	void dump_date_info() 
	{
		cout << "Daylight Save	:" <<  this->tm_date.m_isdst << endl;
		cout << "Century		:" <<  this->tm_date.m_cc << endl;
		cout << "Year			:" <<  this->tm_date.m_yy << endl;
		cout << "Month			:" <<  this->tm_date.m_mm << endl;
		cout << "Day			:" <<  this->tm_date.m_dd << endl;
		cout << "Hour			:" <<  this->tm_date.m_hh << endl;
		cout << "Min			:" <<  this->tm_date.m_mi << endl;
		cout << "Sec			:" <<  this->tm_date.m_ss << endl;
		cout << "Milli			:" <<  this->tm_date.m_ms << endl;
		cout << "Errno			:" <<  this->tm_date.m_errno << endl;
		cout << "Errtype		:" << this->tm_date.m_errtype << endl;
		cout << "Trailing		:" << this->tm_date.m_trailing_chars << endl;
	};

	void reset_date() {
		this->tm_date.m_isdst = -1;
		this->tm_date.m_cc = 0;
		this->tm_date.m_mm = 0;
		this->tm_date.m_dd = 0;
		this->tm_date.m_yy = 0;
		this->tm_date.m_hh = 0;
		this->tm_date.m_mi = 0;
		this->tm_date.m_ss = 0;
		this->tm_date.m_ms = 0;
		this->tm_date.m_errtype = 0;
		this->tm_date.m_errno=0;
		this->tm_date.m_trailing_chars = 0;
		this->tm_date.b_valid_date =false;
	};


	void validate_date() {

		if ( this->tm_date.m_errno != 0 )
			return;

		if ( this->tm_date.m_errtype == 0 )
		{
			if ( this->tm_date.m_mm < 1 || this->tm_date.m_mm > 12 )
				this->tm_date.m_errtype = 'M';
			else if ( this->tm_date.m_dd < 1 || this->tm_date.m_dd > 31 )
				this->tm_date.m_errtype = 'D';
			else if ( this->tm_date.m_yy < MIN_YEAR || this->tm_date.m_yy > MAX_YEAR )
				this->tm_date.m_errtype = 'Y';
			else if ( this->tm_date.m_hh < 0 || this->tm_date.m_hh > 24 )
				this->tm_date.m_errtype = 'H';
			else if ( this->tm_date.m_mi < 0 || this->tm_date.m_mi > 59 )
				this->tm_date.m_errtype = 'I';
			else if ( this->tm_date.m_ss < 0 || this->tm_date.m_ss > 59 )
				this->tm_date.m_errtype = 'S';
		}
			
		if (this->tm_date.m_errtype != 0 )
		{
			this->tm_date.m_errno = -98;
		}
	};



	void reset_format() {
		for (int i =0; i < MAX_DATE_FIELDS; i++ )
		{
			configs[i].type = 0;
			configs[i].ends_with = 0;
			configs[i].min_chars = 0;
			configs[i].max_chars = 0;
		}
	};

	bool getTmStruct( struct tm * sys_date ) 
	{
		if (!sys_date)
		{
			return false;
		}
		sys_date->tm_mon = this->tm_date.m_mm - 1;
		sys_date->tm_year = this->tm_date.m_yy - 1900;
		sys_date->tm_mday = this->tm_date.m_dd;
		sys_date->tm_hour = this->tm_date.m_hh;
		sys_date->tm_min = this->tm_date.m_mi;
		sys_date->tm_sec = this->tm_date.m_ss;
		sys_date->tm_wday = 0;
		sys_date->tm_yday = 0;
		sys_date->tm_isdst = this->tm_date.m_isdst;
		return true;
	};

	///----utc conversions-----------------
	time_t toUTC() // throw
	{
		time_t t;
		int err = 0;
		t = this->toUTC( &err );
		if (err != 0)
			throw range_error("Can not convert date to UTC"); 
		return t;
	}

	time_t toUTC( int *err )	//  
	{
		time_t ltime;
		struct tm timer;

		if (err)
			*err = 0;

		if (this->getTmStruct(&timer))
		{
			timer.tm_isdst = -1;  //let the system calc this 'again'
			ltime = mktime(&timer);
			return ltime;
		}
		if (err)
			*err = -97;
		return 0;
		
	}

	///----utc conversions-----------------
	void fromString(string datestr, string mask ) 
	{
		if (datestr.empty() || mask.empty())
			throw range_error("empty string or mask can not be converted to date"); 

		if (! set_date_format(mask.c_str()) )
			throw range_error("Can not read mask value"); 

		if (set_date_string( datestr.c_str() ) != 0 )
			throw range_error("Can not convert date to string"); 
	}
	int fromString(string datestr, string mask, int *err )
	{
		if (err)
			*err=0;

		int rc = 0;
		if (datestr.empty() || mask.empty())
			rc = -98;

		if (! set_date_format(mask.c_str()) )
			rc = -97;
		else 
			rc= set_date_string( datestr.c_str() );

		if (err)
			*err = rc;
		return rc;
	}


	int set_date_string(const char * i_date_string)
	{
		short pos = 0;
		size_t slen = 0;
		static char date_buf[DATE_BUFF_LEN];
		char * date_string = NULL;

		//----reset the internal date-----
		this->reset_date();

		//----sanity-----
		if ( i_date_string == (const char *)NULL )
			this->tm_date.m_errno = -70;
		else if ((slen=strlen(i_date_string)) == 0 )
			this->tm_date.m_errno = -71;
		else if (configs[0].type == 0 )
			this->tm_date.m_errno = -60;

		if ( this->tm_date.m_errno != 0 )
			return this->tm_date.m_errno;


		//--trim starting non digits -----
		for (date_string = (char*)i_date_string; 
				*date_string != '\0' && !isdigit(*date_string); date_string++ );

		if (!date_string || *date_string == '\0')
			this->tm_date.m_errno = -72;
		else if ((slen=strlen(date_string)) == 0 )
			this->tm_date.m_errno = -73;

		if ( this->tm_date.m_errno != 0 )
			return this->tm_date.m_errno;

		//-- environment related fields ---
		time_t t = time(0);
		struct tm * nowptr = localtime((const time_t*)&t);
		this->tm_date.m_isdst = nowptr->tm_isdst;
		this->tm_date.m_cc = 1900 + (( nowptr->tm_year / 100 ) * 100); 


		//-- using the config data, peel fields from the date string ---
		for (int i =0; i < MAX_DATE_FIELDS; i++ )
		{	
			if (configs[i].type == 0)
				continue;

			short start_pos = pos;
			memset(date_buf,0,configs[i].max_chars+1);
			
			for (
				int m=0; 
					m < configs[i].max_chars &&  // max chars for this field (e.g.YYYY=4)
					m < DATE_BUFF_LEN &&		// sanity overrun buffer
					(size_t)pos <= slen &&				// sanity overrun date_string
					date_string[pos] != configs[i].ends_with; // hit delimiter before max
				m++, pos++ )
			{
				date_buf[m] = date_string[pos];	// do work
			}
			while (	(size_t)pos < slen &&   // remove non-digits
					date_string[pos] == configs[i].ends_with ) 
			{
						pos++;
			}
			while (	(size_t)pos < slen && !isdigit(date_string[pos]) ) 
			{//separate, because we are making some assumtions here - we are assuming that if we see delimitrs we strip then off 
						pos++;
			}

			int l = (!date_buf) ? 0: (int)strlen(date_buf);

			//--determine errors ----
			short int_value =0;
			this->tm_date.m_errno = 0;

			if ( l == 0 )// empty
				this->tm_date.m_errno = -61;
			else if ( l > configs[i].max_chars )//overrun
				this->tm_date.m_errno = -62;
			else if ( l < configs[i].min_chars )//underrun
				this->tm_date.m_errno = -63;
			else {
				errno = 0;
				int_value = atoi(date_buf);
				if ( errno !=  0 )
					this->tm_date.m_errno = errno;
			}
			if ( this->tm_date.m_errno != 0 ){
				this->tm_date.m_errtype = configs[i].type;
				break;	//--goto final actions
			}
	
			//--determine the integer value ----
			time_t t = time(0);
			struct tm * nowptr = localtime((const time_t*)&t);
			this->tm_date.m_isdst = nowptr->tm_isdst;

			switch (configs[i].type){
				case 'Y': 
					if (int_value  <= 99) 
						this->tm_date.m_yy = int_value + this->tm_date.m_cc; 
					else
						this->tm_date.m_yy = int_value; 
					break;
				case 'M': this->tm_date.m_mm = int_value; break;
				case 'D': this->tm_date.m_dd = int_value; break;
				case 'H': this->tm_date.m_hh = int_value; break;
				case 'I': this->tm_date.m_mi = int_value; break;
				case 'S': this->tm_date.m_ss = int_value; break;
				case 'L': this->tm_date.m_ms = int_value; break;
				default: break;
			}
			if (slen == pos)
				break;
		}

		//--- final:  do validation fields ---------------
		this->tm_date.m_trailing_chars = ((short)slen-pos);
		this->validate_date();
		return 0;
	}	

	bool set_date_format(const char * format_string)
	{
		short l;
		short use_min = -1;
		short arr_count =0;
		short current_count =0;
		char prev_char = '\0';

		//--clear the array----
		reset_format();

		//--check input----
		if (!format_string || (l=(int)strlen(format_string)) == 0 )
			return false;

		//--process the date format string (e.g. MM/DD/YYYY HH:MI:SS )---
		for (int i=0; i<=l; i++ )
		{
			char c = toupper(format_string[i]);
			if (prev_char=='M' && c == 'S') {//ms kludge
				prev_char = 'L';
				c = 'L';
			}else if (prev_char=='M' && c == 'I'){//mi kludge
				prev_char = 'I';
				c = 'I';
			}

			if ( prev_char == c ) // exception ms
			{	
				current_count++;
				prev_char = c;  
				continue;
			}

			if ((i > 0)					// pass first time through
				&& (use_min >= 0))			// pass delimiters
	//			&& !(prev_char=='M' && c == 'I'))  // pass 'MI' minutes
			{
				configs[arr_count].type = prev_char;
				configs[arr_count].ends_with = c;
				configs[arr_count].min_chars = use_min;
				configs[arr_count].max_chars = current_count;
				arr_count++;
			}
	

			switch (format_string[i]){
			case 'Y':
			case 'y':
				use_min = 2;
				break;
			case 'M':
			case 'm':
			case 'D':
			case 'd':
			case 'H':
			case 'h':
			case 'S':
			case 's':
			case 'I':
			case 'i':
			case 'L':
			case 'l':
				use_min = 1;
				break;
			default:
				use_min = -1;
				break;
			}

			current_count = 1;
			prev_char = c;  
		}
		return true;
	}	

	void set_max_year(short max_year)
	{
		this->MAX_YEAR = max_year;
	}
	void set_min_year(short min_year)
	{
		this->MIN_YEAR = min_year;
	}

	int addMonths(int months)
	{

		return 0;
	}
};

#endif
