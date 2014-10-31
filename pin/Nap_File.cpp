#include "Nap_File.h"

#define CNap_File_MAX_LINE 1024

CNap_File::CNap_File()
{
}

CNap_File::~CNap_File(void)
{
}

/***********************************************
 * next_field utility - get next token
 */
char * next_field(char * cp) {
	for ( ; !isspace(*cp) && *cp!='\0'; cp++ );
	for ( ; isspace(*cp) && *cp!='\0'; cp++ ) *cp='\0';

	return cp;
}

/***********************************************
 * recurse_textfile
 *   recursive portion of the file2flist function
 * Pre:
 *		new flist created 
 *		file is open and fp is passed in
 * Post:
 *		all params on a bona-fide flist
 */
file_status_t CNap_File::recurse_textfile(
	FILE * fp,
	pin_flist_t * c_flistp, 
	int in_level )
{
	pin_flist_t * e_flistp = NULL;
	poid_t * pdp = NULL;
	char line_buffer[CNap_File_MAX_LINE];
	char * level = NULL;
	char * fld_name = NULL;
	char * fld_type = NULL;
	char * arrval = NULL;
	char * value = NULL;
	char * state_str = NULL;
	char * cp = NULL;
	char * endcpp = NULL;
	char * unsupported = (char*)"Unsupported";
	int i = 0;
	int intval = 0;
	time_t ltime = 0;
	char my_level[40];
	char my_number[40];
	pin_decimal_t * amtp = NULL;

	in_level++;

	int nlevel;
	int array_value;

    pin_errbuf_t ebuf;
    pin_errbuf_t * ebufp = &ebuf;

    PIN_ERR_CLEAR_ERR(ebufp);

	//-- get next record -----
	memset(line_buffer,0,CNap_File_MAX_LINE);

	//-- skip comments -----
	this->pos = ftell(fp);

	while ( fgets(line_buffer,CNap_File_MAX_LINE,fp) != NULL ) 
	{
		lineno++;
		if ( line_buffer[0] == '#') {
			this->pos = ftell(fp);
			continue;
		}

		//-- chomp ----
		while ((cp=strchr(line_buffer,'\n')) != NULL )
			*cp = '\0';


		//-- what level are we on? ----
		level = line_buffer;
		for ( level=line_buffer; isspace(*level) && *level!='\0'; level++ );

		for (i=0; isdigit(level[i]); i++)
			my_level[i] = level[i];
		my_level[i] = '\0';
		if ((nlevel = atoi(my_level)) != in_level) {
			fseek(fp,this->pos, SEEK_SET);    
			return End_recurse;
		}

		//--one line---
		fld_name =  next_field(level) ;
		fld_type =  next_field(fld_name) ;
		arrval = next_field(fld_type) ;
		value = next_field(arrval) ;

		//--pin-values---
		pin_fld_num_t pin_fld_num = pin_field_of_name((const char *)fld_name);
		int pin_fld_type = pin_type_of_field(pin_fld_num);

		for (i=0; *arrval != '\0'; arrval++) {
			if ( isdigit(*arrval) )
					my_number[i++] = *arrval;
		}
		my_number[i] = '\0';
		array_value = atoi(my_number);

		//-- substitution--


		//--translation --
		switch(pin_fld_type) {

		case  PIN_FLDT_INT:
		case  PIN_FLDT_ENUM:
			intval = atoi(value);
			PIN_FLIST_FLD_SET(c_flistp, pin_fld_num,(void *)&intval, ebufp);
			break;

		case  PIN_FLDT_STR:
			cp = value + strlen(value) -1;
			if ( *value == '\"' ) value++;
			if ( *cp == '\"' ) *cp = '\0';

			PIN_FLIST_FLD_SET(c_flistp, pin_fld_num,(void *)value, ebufp);
			break;


		case  PIN_FLDT_POID	:
			pdp = PIN_POID_FROM_STR(value, &endcpp, ebufp);
			PIN_FLIST_FLD_PUT(c_flistp, pin_fld_num,(void *)pdp, ebufp);
			break;

		case  PIN_FLDT_ARRAY:
			e_flistp = PIN_FLIST_ELEM_ADD(c_flistp,pin_fld_num, array_value, ebufp);
			recurse_textfile(fp, e_flistp, in_level);
			break;

		case  PIN_FLDT_SUBSTRUCT:
			e_flistp = PIN_FLIST_SUBSTR_ADD(c_flistp,pin_fld_num, ebufp);
			recurse_textfile(fp, e_flistp, in_level);
			break;

		case  PIN_FLDT_TSTAMP:
			if ((cp=strchr(value,'(')) == NULL )
				return Bad_parse;
			for (i=0, cp++; isdigit(*cp); cp++)
				my_number[i++] = *cp;
			my_number[i]='\0';

			ltime = atol(my_number);
			PIN_FLIST_FLD_SET(c_flistp, pin_fld_num,(void *)&ltime, ebufp);
			break;
			
		case  PIN_FLDT_DECIMAL:
			while ( *value == '$' && *value != '\0') value++;
			if ( strcmp(value, "NULL")  == 0 )
				amtp = NULL;
			else if (( amtp =pbo_decimal_from_str((const char *)value, ebufp)) == NULL )
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
	
		this->pos = ftell(fp);
	}
	return End_of_file;

}

/***********************************************
 * file2flist
 * Pre:
 *		file containing an Infranet flist 
 * Post:
 *		create a bona-fide flist structure
 */
pin_flist_t * CNap_File::filetoFlist(string fname)
{
	FILE * fp = NULL;
	char * level = NULL;
	char * fld_name = NULL;
	char * fld_type = NULL;
	char * arrval = NULL;
	char * value = NULL;
	char * state_str = NULL;
	char * cp = NULL;
	char * endcpp = NULL;
	char * unsupported = (char*)"Unsupported";

	int nlevel=-1;
	pin_errbuf_t ebuf;
	pin_errbuf_t * ebufp = &ebuf;

	PIN_ERR_CLEAR_ERR(ebufp);
	
	//-------------------
	this->filename.assign( fname );

	//--open file -----
	if ((fp = fopen(filename.c_str(),"r")) == NULL ) {
		cout << "Bad open for " << filename.c_str() << endl;
		return NULL;
	}

	//-- create the flist -----
	pin_flist_t * head_flistp = PIN_FLIST_CREATE(ebufp);;
	pin_flist_t * c_flistp = head_flistp;

	//-- recursively create each array -----
	lineno=0;
	if ( recurse_textfile(fp, c_flistp, nlevel) != Bad_parse )
		return c_flistp;
	return NULL;
}

