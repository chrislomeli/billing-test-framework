#ifndef STRING_PIN_UTILITY_MAP_H
#define STRING_PIN_UTILITY_MAP_H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <ctype/c_StringFormat.h>
#include <platform/p_Exception.h>
#include <platform/p_Error_map.h>
#include <errno.h>

enum file_status_t {
	fstrSuccess,
	Bad_parse,
	End_recurse
};

#define STRING_FLIST_MAX_LINE 1024

#define MY_TYPE_ERRNO		2000
#define MY_TYPE_LOCATION	2001
#define MY_TYPE_FIELD_NUM	2002
#define MY_TYPE_RECID		2003
#define MY_TYPE_RESERVED	2004
#define MY_TYPE_CLASS		2005



/*
 *
 * Status : not used - deprecated
 */
class c_StringPinFormat : public c_StringFormat 
{
public:

	c_StringPinFormat() {};
	~c_StringPinFormat() {};

	static map<string,int> ercode_map;
	static int list_ptr;


	static pin_decimal_t * StringToDecimal ( const string numstr )
	{
		pin_errbuf_t ebuf;
		PIN_ERR_CLEAR_ERR(&ebuf);
		string msg = "StringToDecimal: can not convert " + numstr;

		pin_decimal_t * n = pin_decimal(numstr.c_str(), &ebuf);

		if ( PIN_ERR_IS_ERR(&ebuf) ) 
				p_Throw(r_Type_Conversion, numstr);

		return n;
	};



	static pin_flist_t * StringArrayToFlist( vector<std::string>& text_buffer )
	{
		file_status_t rc;
		int nlevel=-1;
		pin_errbuf_t ebuf;
		pin_errbuf_t * ebufp = &ebuf;
		c_StringPinFormat::list_ptr=0;
		

		PIN_ERR_CLEAR_ERR(ebufp);

		//-- create flist----
		pin_flist_t * head_flistp = PIN_FLIST_CREATE(ebufp);;
		pin_flist_t * c_flistp = head_flistp;

		//-- cycle through buffer - appending flist ----
		if ((rc=c_StringPinFormat::_recurse_buffer(
				c_flistp, text_buffer, nlevel )) != fstrSuccess ) {
				string msg = "Failed to convert FLIST at:" + text_buffer[c_StringPinFormat::list_ptr++];
				p_Throw(r_Type_Conversion, msg);
		}
		return c_flistp;
	};


	static char * _next_field(char * cp) {
		for ( ; !isspace(*cp) && *cp!='\0'; cp++ );
		for ( ; isspace(*cp) && *cp!='\0'; cp++ ) *cp='\0';

		return cp;
	}

	static file_status_t _recurse_buffer(
		pin_flist_t * c_flistp, 
		vector<string>& text_buffer,
		int in_level )
	{
		pin_flist_t * e_flistp = NULL;
		poid_t * pdp = NULL;
		char line_buffer[STRING_FLIST_MAX_LINE];
		char * level = NULL;
		char * fld_name = NULL;
		char * fld_type = NULL;
		char * arrval = NULL;
		char * state_str = NULL;
		char * cp = NULL;
		char * endcpp = NULL;
		int i = 0;
		int intval = 0;
		time_t ltime = 0;
		char my_level[40];
		char my_number[40];
		pin_decimal_t * amtp = NULL;
		int nlevel;
		int array_value;

		string value;

		pin_errbuf_t ebuf;
		pin_errbuf_t * ebufp = &ebuf;

		PIN_ERR_CLEAR_ERR(ebufp);
		in_level++;


		//-- get next record -----
		memset(line_buffer,0,STRING_FLIST_MAX_LINE);

		//-- skip comments -----
		while ( c_StringPinFormat::list_ptr < (int)text_buffer.size() )
		{
			strcpy( line_buffer, text_buffer[c_StringPinFormat::list_ptr++].c_str() ); 

			if ( line_buffer[0] == '#') {
				continue;
			}

			//-- what level are we on? ----
			level = line_buffer;
			for ( level=line_buffer; isspace(*level) && *level!='\0'; level++ );

			for (i=0; isdigit(level[i]); i++)
				my_level[i] = level[i];

			my_level[i] = '\0';
			if ((nlevel = atoi(my_level)) != in_level) {
				c_StringPinFormat::list_ptr--;    
				return End_recurse;
			}

			//--one line---
			fld_name =  c_StringPinFormat::_next_field(level) ;
			fld_type =  c_StringPinFormat::_next_field(fld_name) ;
			arrval =	c_StringPinFormat::_next_field(fld_type) ;
			value =		c_StringPinFormat::_next_field(arrval) ;

			//--pin-values---
			pin_fld_num_t pin_fld_num = pin_field_of_name((const char *)fld_name);
			int pin_fld_type = pin_type_of_field(pin_fld_num);

			for (i=0; *arrval != '\0'; arrval++) {
				if ( isdigit(*arrval) )
						my_number[i++] = *arrval;
			}
			my_number[i] = '\0';
			array_value = atoi(my_number);


			//--translation --
			switch(pin_fld_type) {

			case  PIN_FLDT_INT:
			case  PIN_FLDT_ENUM:
				intval = atoi(value.c_str());
				PIN_FLIST_FLD_SET(c_flistp, pin_fld_num,(void *)&intval, ebufp);
				break;

			case  PIN_FLDT_STR:
				if ( value.compare("NULL") == 0 )
					value.erase();
				if ( value[0]=='"' ) value = value.substr(1,value.size());
				if ( value[value.size()-1]=='"' ) value = value.substr(0,value.size()-1);

				PIN_FLIST_FLD_SET(c_flistp, pin_fld_num,(void *)value.c_str(), ebufp);
				break;


			case  PIN_FLDT_POID	:
				// valid poid format
				pdp = PIN_POID_FROM_STR((char*)value.c_str(), &endcpp, ebufp);
				PIN_FLIST_FLD_PUT(c_flistp, pin_fld_num,(void *)pdp, ebufp);
				break;

			case  PIN_FLDT_ARRAY:
				e_flistp = PIN_FLIST_ELEM_ADD(c_flistp,pin_fld_num, array_value, ebufp);
				_recurse_buffer(e_flistp, text_buffer, in_level);
				break;

			case  PIN_FLDT_SUBSTRUCT:
				e_flistp = PIN_FLIST_SUBSTR_ADD(c_flistp,pin_fld_num, ebufp);
				_recurse_buffer(e_flistp, text_buffer, in_level);
				break;

			case  PIN_FLDT_TSTAMP:
				string::size_type i;
				if (( i = value.find_first_of('(')) == string::npos )
					return Bad_parse;
				value = value.substr(i+1);
				if (( i = value.find_first_of(')')) == string::npos )
					return Bad_parse;
				value = value.substr(0,i);
				

				ltime = atol(value.c_str());
				PIN_FLIST_FLD_SET(c_flistp, pin_fld_num,(void *)&ltime, ebufp);
				break;
				
			case  PIN_FLDT_DECIMAL:
				cp = (char*)value.c_str();
				if ( value.compare("NULL") == 0 )
					amtp = NULL;
				else if (( amtp =pin_decimal( cp, ebufp)) == NULL )
	//			else if (( amtp =pbo_decimal_from_str( cp, ebufp)) == NULL )
					return Bad_parse;
				PIN_FLIST_FLD_SET(c_flistp, pin_fld_num,(void *)amtp, ebufp);
				break;

			case  PIN_FLDT_ERR:
			case  PIN_FLDT_TIME:
			case  PIN_FLDT_BUF:
			case  PIN_FLDT_BINSTR:
			case  PIN_FLDT_TEXTBUF:
			case  PIN_FLDT_UNUSED:
			case  PIN_FLDT_OBJ:
				break;
			}

			if ( PIN_ERR_IS_ERR(ebufp) )
				return Bad_parse;

		}
		return fstrSuccess;
	};


	/*************************************
        errno=46
        location=PIN_ERRLOC_DM 
		class=PIN_ERRCLASS_SYSTEM_DETERMINATE 
        field num=PIN_FLD_ATTRIBUTE_INFO
		recid=0 
		reserved=0
	*/
	static bool StringArrayToEbuf(vector<string> lines, pin_errbuf_t * ebufp)
	{
		vector<string>::const_iterator pos = lines.begin();
			

		for (; pos != lines.end(); pos++ ) {
			
			string str = *pos;
			string::size_type pos  = str.find_first_of('=', 0);

			if (string::npos == pos ) {
 				string msg = "StringArrayToEbuf: Need an equal sign: " + str;
				p_Throw(r_Type_Conversion, msg);

			}

			string name = str.substr(0, pos);
			string value = str.substr(pos+1);

			alltrim(name);
			alltrim(value);

			int name_id = c_StringPinFormat::StringToEnum(name);
			int val_id=0;

			switch (name_id) 
			{
			case MY_TYPE_ERRNO:
				val_id = c_StringPinFormat::StringToInteger(value);
				ebufp->pin_err = val_id;
				break;
			case MY_TYPE_LOCATION:
				val_id = c_StringPinFormat::StringToEnum(value);
				ebufp->location = val_id;
				break;

			case MY_TYPE_CLASS:
				val_id = c_StringPinFormat::StringToEnum(value);
				ebufp->pin_errclass = val_id;
				break;

			case MY_TYPE_FIELD_NUM:
				val_id = (int)pin_field_of_name(value.c_str());
				ebufp->field = val_id;
				break;

			case MY_TYPE_RECID:
				val_id = c_StringFormat::StringToInteger(value);
				ebufp->rec_id = val_id;
				break;

			case MY_TYPE_RESERVED:
				val_id = c_StringFormat::StringToInteger(value);
				ebufp->reserved = val_id;
				break;
			}

		}
		return true;
	};


	static int StringToEnum(string key) 
	{
		if (c_StringPinFormat::ercode_map.size()==0)
			initialize_enum_map();

		map<string,int>::iterator p = ercode_map.find(key);

		//--if this is a new head node, Add it----
		if ( p != c_StringPinFormat::ercode_map.end() )
			return (p->second);

		return 0;
	};
		
		
	
	static void initialize_enum_map () 
	{
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_APP",		PIN_ERRLOC_APP )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_PCM",		PIN_ERRLOC_PCM )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_PCP",		PIN_ERRLOC_PCP )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_PCMCPP",	PIN_ERRLOC_PCMCPP )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_PDO",		PIN_ERRLOC_PDO )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_POID",	PIN_ERRLOC_POID )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_CM",		PIN_ERRLOC_CM )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_DM",		PIN_ERRLOC_DM )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_FM",		PIN_ERRLOC_FM )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_FLIST",	PIN_ERRLOC_FLIST )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_IM",		PIN_ERRLOC_IM )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_INFMGR",	PIN_ERRLOC_INFMGR )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_JS",		PIN_ERRLOC_JS )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_JSAPP",	PIN_ERRLOC_JSAPP )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_QM",		PIN_ERRLOC_QM )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_QM",		PIN_ERRLOC_QM )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_LDAP",	PIN_ERRLOC_LDAP )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_NMGR",	PIN_ERRLOC_NMGR )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRLOC_UTILS",	PIN_ERRLOC_UTILS )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRCLASS_SYSTEM_DETERMINATE",	PIN_ERRCLASS_SYSTEM_DETERMINATE )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRCLASS_SYSTEM_INDETERMINATE",	PIN_ERRCLASS_SYSTEM_INDETERMINATE )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRCLASS_SYSTEM_RETRYABLE",		PIN_ERRCLASS_SYSTEM_RETRYABLE )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("PIN_ERRCLASS_APPLICATION",			PIN_ERRCLASS_APPLICATION )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("UNKNOWN", 0 )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("errno", MY_TYPE_ERRNO )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("location", MY_TYPE_LOCATION )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("class", MY_TYPE_CLASS )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("field_num", MY_TYPE_FIELD_NUM )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("recid", MY_TYPE_RECID )); 
		c_StringPinFormat::ercode_map.insert (  pair<string,int>("reserved", MY_TYPE_RESERVED )); 
	
	};


};


#endif //STRING_UTILITY_H

