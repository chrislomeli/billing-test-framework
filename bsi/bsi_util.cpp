/**
 * \file bsi_util.cpp
 * Source file for utility funtions used in BSI.
 *
 * $Id: bsi_util.cpp,v 1.1 2007/01/15 14:16:56 sathpv Exp $
 * $Source: /proj/cvs/untd_billing/server/tools/cpptools/bsi/bsi_util.cpp,v $
 *********************************************************************************/

#include "bsi_util.h"
#include <fstream>

static char *CVS_Id = (char *)"CVS_VERSION:$Id: bsi_util.cpp,v 1.1 2007/01/15 14:16:56 sathpv Exp $";
static char *CVS_Source = (char *)"CVS_VERSION:$Source: /proj/cvs/untd_billing/server/tools/cpptools/bsi/bsi_util.cpp,v $";

/// Enters the key value pair into the map
return_t PUTIN_MAP(entity_map *map_block, const char * value, const char * key)
{
  typedef entity_map::value_type value_type;
  if (value==NULL || key == NULL){
    return FAILURE;
  }
  map_block->insert (value_type  (std::string (key),  std::string (value)));
  return SUCCESS;
}

/// Searches for a particular key in the map and returns the value corresponding to it
char * entity_findval(const char * key, entity_map * map_block)
{
  if (key == NULL){
    return NULL;
  }
  entity_map::iterator p = map_block->find(std::string(key));
  if (p != map_block->end())
  {
    char *temp = const_cast <char*> ((p->second).c_str());
    if (strcmp(temp,"")) return temp;
  }
  return NULL;
}

/// Returns whether the key exists in the map or not : by mvamsee
bool isExists(const char *key,entity_map * map_block)
{
  if (key == NULL) {
	return NULL;
  }
  entity_map::iterator p = map_block->find(std::string(key));
  if(p != map_block->end()) return true;
  return false;
}

/// Deletes the key and corresponding value from the map : by mvamsee
return_t eraseFromMap(const char *key,entity_map * map_block)
{
  if( key == NULL) return FAILURE;
  entity_map::iterator p = map_block->find(std::string(key));
  if(p != map_block->end()) {
	  map_block->erase(p);
	  return SUCCESS;
  }
  return FAILURE;
}

/// Enters the key value pair into the map
return_t map_nvinsert(const char *key, const char * value, entity_map *map_block)
{
  typedef entity_map::value_type value_type;
  if (value==NULL || key == NULL){
    return FAILURE;
  }
  map_block->insert (value_type  (std::string (key), std::string (value)));
  return SUCCESS;
}

/// Formats the phone number
return_t DisplayablePhoneFormat( char *input_phone, char *output_phone )
{
  int i=0;
  char *ptr;
  if( input_phone == NULL )
  {
    return FAILURE;
  }
  for(ptr=input_phone, i=0; ( ptr && *ptr && (*ptr != '\0') ); ptr++)
  {
    if( isdigit(*ptr) )
    {
      if(i==0) { output_phone[i++] = '('; }
      else if(i==4) { output_phone[i++] = ')'; output_phone[i++] = ' '; }
      else if(i==9) { output_phone[i++] = '-'; }
      output_phone[i++] = *ptr;
    }
  }
  output_phone[i++] = '\0';
  return SUCCESS;
}

/**
 * converts time_t into a displable time format.
 *   input -- intime
 *   output -- outtime -- which should be preallocated
 *   outputlen is the lenght of space passed in
 */
return_t DisplayableTimeFormat(time_t *intime, char *outtime,int  outtimelen)
{
  if (NULL == intime  || NULL == outtime) return FAILURE;
  struct  tm  tmvar;
  localtime_r(intime, &tmvar);
  const char  *format = "%C";
  strftime(outtime, outtimelen, format, &tmvar);
  return SUCCESS;
}

/**
 * EventViewerDateFormat()
 **************************************************************
 * converts time_t into a displable time format for the Event Viewer
 *   input -- intime
 *   output -- outtime -- which should be preallocated
 *   months offset -- number of months offset w.r.t. the current date
 *   outputlen is the lenght of space passed in
 */
return_t EventViewerDateFormat(time_t *intime,
             char   *outtime,
             int  outtimelen,
             int  num_months)
{
  if (NULL == intime  || NULL == outtime) return FAILURE;
  int months = 0, years = 0;
  struct  tm  tmvar;
  const char  *format = "%m/%d/%Y";
  localtime_r(intime, &tmvar);
  if (num_months > 0) {
    months = tmvar.tm_mon + num_months;
    years = months / 12;
    months = months % 12;
    tmvar.tm_mon = months;    // fyi: months are from 0 to 11
    tmvar.tm_year += years;
  } else if (num_months < 0) {
    num_months *= (-1);  // make it a positive number
    if( num_months > tmvar.tm_mon ){
      /* First go back to Dec of last year */
      tmvar.tm_year -= 1;
      num_months -= (tmvar.tm_mon + 1);  /* tmvar.tm_mon is 0 to 11 */
      tmvar.tm_mon = 11; /* Dec */
      /* Now you are at Dec of last year */
      years = num_months / 12;  // Go back by this many years
      tmvar.tm_year -= years;
      num_months = num_months % 12;
      tmvar.tm_mon -= num_months ;
    } else {
      /* Simply substract the offset_number_of_months from tmvar.tm_mon variable */
       tmvar.tm_mon = tmvar.tm_mon - num_months ;
    }
  }
  strftime(outtime, outtimelen, format, &tmvar);
  return SUCCESS;
}

/*************************************************************
* ConvertDateStringToTime_t()
**************************************************************
* converts the date string in the "MM/DD/YYYY" format to a time_t value
*   input -- input date string in the "MM/DD/YYYY" format
*   output -- outtime in time_t format.
*************************************************************/
time_t ConvertDateStringToTime_t(char *date_str)
{
  const char  *procname = "ConvertDateStringToTime_t()";
  char month[10], day[10], year[10];
  time_t time_t_val = (time_t )NULL;
  char *ptr = NULL;
  int index=0;
  int i=0, j=0, k=0;
  /* If "date_str" is NULL or is not in the "MM/DD/YYYY" format */
  if ( (date_str == NULL) || (strlen(date_str) != 10) ) {
    return (time_t )NULL;
  }
  /* parse the incoming date string and extract date, month and year */
  for(ptr=date_str, index=0, i=0, j=0, k=0; ( ptr && *ptr && (*ptr != '\0') ); ptr++, index++) {
    if( isdigit(*ptr) ) {
      if(index<2) {
        month[i++] = *ptr;
      } else if( index>2 and index<5) {
        day[j++] = *ptr;
      } else if( index>5 and index<10) {
        year[k++] = *ptr;
      }
    }
  }
  month[i] = '\0';
  day[j]   = '\0';
  year[k]  = '\0';
  struct  tm  tmvar;
  tmvar.tm_mday = tmvar.tm_mon = tmvar.tm_year = -1;    /* mandatory */
  tmvar.tm_hour = tmvar.tm_min = tmvar.tm_sec = 0;
  tmvar.tm_isdst = 0;
  tmvar.tm_mon  = atoi(month) - 1;      /* months since January [0, 11] */
  tmvar.tm_year = atoi(year) - 1900;    /* years since 1900 */
  tmvar.tm_mday = atoi(day);        /* day of the month [1, 31] */
  tmvar.tm_sec  = 0;        /* seconds after the minute [0, 61]  */
  tmvar.tm_min  = 0;        /* minutes after the hour [0, 59] */
  tmvar.tm_hour = 0;        /* hour since midnight [0, 23] */
  time_t_val = mktime(&tmvar);
  if (time_t_val == -1) {
    return (time_t )NULL;
  }
  /* mktime assumes that tmvar passed in represents local time, so
     it adds 'timezone' to convert from local to GMT.  (It does
     not mess with DST since I pass tmvar.tm_isdst==0).  But tmvar
     represented GMT time, so I have to subtract 'timezone' to
     undo what mktime did
   */
  return time_t_val;
}

/**
 * converts time_t into a displable date format.
 *   @param intime
 *   @return outtime (should be preallocated)
 *   @param outputlen length of space passed in
 */
return_t DisplayableDateFormat(time_t *intime, char *outtime, int  outtimelen)
{
  if (NULL == intime  || NULL == outtime)
  {
    return FAILURE;
  }
  struct  tm  tmvar;
  localtime_r(intime, &tmvar);
  const char  *format = "%B %e, %Y";
  strftime(outtime, outtimelen, format, &tmvar);
  return SUCCESS;
}

/**
 *  Converts the time in seconds to HH:MM:SS
 *  and copies into the variable 'time'
 *  Currently this function is mainly used in the usage report pages.
 */
return_t convert_to_time_format(long total_number_of_seconds, char *time)
{
  int   hours   = 0, minutes = 0, seconds = 0;
  char  tmp_hours[5], tmp_minutes[5],tmp_seconds[5];
  int   tmp_total = 0;
  if( total_number_of_seconds < 60 ) {
        seconds = total_number_of_seconds;
  } else
  {
    seconds = total_number_of_seconds%60 ;
    tmp_total = total_number_of_seconds/60 ;
    if( tmp_total < 60 )
    {
      minutes = tmp_total;
    } else
    {
      minutes = tmp_total%60;
      hours   = tmp_total/60;
    }
  }
  if( hours < 10 ) {
    sprintf( tmp_hours, "0%d", hours );
  } else {
    sprintf( tmp_hours, "%d", hours );
  }
  if( minutes < 10 ) {
    sprintf( tmp_minutes, "0%d", minutes );
  } else {
    sprintf( tmp_minutes, "%d", minutes );
  }
  if( seconds < 10 ) {
    sprintf( tmp_seconds, "0%d", seconds );
  } else {
    sprintf( tmp_seconds, "%d", seconds );
  }
  sprintf( time, "%s:%s:%s", tmp_hours, tmp_minutes, tmp_seconds );
  return SUCCESS;
}

/// Appends the value to the existing value for the supplied key in the map
return_t map_append_value_with_comma(const char *key, const char * value, entity_map *map_block)
{
  if (entity_findval(key, map_block) == NULL)
  {
    map_nvinsert(key, value,map_block);
  } else
  {
    string temp_value(entity_findval(key, map_block));
    temp_value.append(",");
    temp_value.append(value);
    entity_map::iterator p = map_block->find(std::string(key));
    map_block->erase(p);
    map_nvinsert(key, temp_value.c_str(),map_block);
  }
  return SUCCESS;
}

/// Replaces the existing value with the new one for the supplied key in the map
return_t entity_replace(const char *key, const char * value, entity_map *map_block)
{
  if (key == NULL | value == NULL) {
    return FAILURE;
  }
  entity_map::iterator p = map_block->find(std::string(key));
  if (p == map_block->end()) {
    map_nvinsert(key, value,map_block);
  } else
  {
    entity_map::iterator p = map_block->find(std::string(key));
    map_block->erase(p);
    map_nvinsert(key, value,map_block);
  }
  return SUCCESS;
}

/// compares the given value with the value corresponding to the given key in the map
int entity_compare(entity_map *map_block, const char * key, const char *new_value)
{
    if (entity_findval(key,map_block) == NULL) return -1;
    return strcmp(entity_findval(key,map_block),new_value);

}

/// Inserts error code and error message to the map
void insert_error(entity_map *map_block, const char * err_code, const char * error_string)
{
   PUTIN_MAP(map_block, error_string,"RET_MSG");
   PUTIN_MAP(map_block, err_code,"RET_CODE");
}

/// parses a string of the format "name1:value1,name2:value2" into a map
int parse_config_str_to_map(entity_map *map_block, const char *config_string) {
  int comma_pos=0,prev_comma_pos=0,colon_pos=0;
  string str(config_string);
  while((comma_pos = str.find(",",prev_comma_pos))>0){
    colon_pos = str.find(":",prev_comma_pos);
    if ((colon_pos <= 0) || (colon_pos > comma_pos)) return -1;
    PUTIN_MAP(map_block,(str.substr(colon_pos+1,comma_pos-colon_pos-1)).c_str(),(str.substr(prev_comma_pos,colon_pos-prev_comma_pos)).c_str());
    prev_comma_pos=comma_pos+1;
  }
  if (prev_comma_pos == 0) return -1;
  if(prev_comma_pos < str.size()){
    comma_pos = str.size();
    colon_pos = str.find(":",prev_comma_pos);
    if ((colon_pos <= 0) || (colon_pos > comma_pos)) return -1;
    PUTIN_MAP(map_block,(str.substr(colon_pos+1,comma_pos-colon_pos-1)).c_str(),(str.substr(prev_comma_pos,colon_pos-prev_comma_pos)).c_str());
  }

  return 0;
}

/// Converts a string to lowercase
char *LowerCase(char *s)
{
  if (s == NULL) {
    return NULL;
  } else if (! *s) {
    return NULL;
  }
  int len = strlen(s);
    char c;
    for (int i = 0; i < len; ++i) {
    c = tolower(s[i]);
    if (c != s[i] && !(c <= 'z' && c >= 'a')) {
      return NULL;
    } else {
      s[i] = c;
    }
  }
  return s;
}

/// Converts a string to UpperCase : @mvamsee
char *toUpperCase(char *s)
{
  if (s == NULL) {
    return NULL;
  } else if (! *s) {
    return NULL;
  }
  int len = strlen(s);
    char c;
    for (int i = 0; i < len; ++i) {
    c = toupper(s[i]);
    if (c != s[i] && !(c <= 'Z' && c >= 'A')) {
      return NULL;
    } else {
      s[i] = c;
    }
  }
  return s;
}

/**
 * Encrypts the given data.
 * Logic:
 * Convert the chars into hexademical values and form a new
 * string and return the reversal of that string
 *
 * @param data, data to be encrypted
 * @param encryptedData, pointer to the encrypted output
 */
void encryptData(char *data, char * &encryptedData){
	int len = strlen(data);
	int j = 0;
	char *outputData = (char *)malloc(sizeof(char)*len*2);
	char *temp = (char *)malloc(sizeof(char)*2);
	for(int i = 1; i <= len; i++){
		int val = ((unsigned char)data[i - 1]) ^ (i % 256);
		sprintf(temp,"%x", val);
		if(val < 16){
			char *zero = (char *)malloc(sizeof(char)*2);
			strcpy(zero, "0");
			strcat(zero, temp);
			temp = zero;
		}
		j = (i - 1) * 2;
		*(outputData + j) = temp[0];
		*(outputData + j + 1) = temp[1];
	}
	*(outputData + j + 2) = NULL;
	strrev(outputData);
	encryptedData = outputData;
}

/**
 * Decrypts the given data.
 * Logic:
 * reverse the string
 * Get 2 chars from the string,
 * convert that to hexadecimal and then to integer.
 * Copy this as a char into the output string. Loop this till
 * the end of the encrypted string.
 *
 * @param data, data to be encrypted
 * @param encryptedData, pointer to the encrypted output
 */
void decryptData(char *data, char * &decryptedData){
	int len = strlen(data);
	char *outputData = (char *)malloc(sizeof(char)*(len/2));
	strrev(data);
	int j = 0;
	for(int i = 1; i <= len; i += 2){
		char *hexData = (char *)malloc(sizeof(char)*2);
		*hexData = data[i-1];
		*(hexData + 1) = data[i];
		*(hexData + 2) = NULL;
		outputData[j] = strtoull(hexData, NULL, 16) ^ (j + 1 % 256);
		j ++;
	}
	outputData[j] = NULL;
	decryptedData = outputData;
}

/**
 * Reverses the given string.
 * @param string, pointer to the string to be reversed
 */
void strrev(char *&string){
	int len = strlen(string);
	int i;
	char *revString = (char *)malloc(sizeof(char)*len);
	for (i = 0; i < len; i++){
		revString[i] = string[len - (i + 1)];
	}
	revString[i] = '\0';
	string = revString;
}

/// Gets the template file location from template_loc map,
/// Reads the corresponding file contents and stores in template_val map
/// : by mvamsee
bool readTemplateFiles(entity_map *template_loc,entity_map *template_val) {

   entity_map::iterator p;
   for(p=template_loc->begin();p!=template_loc->end();p++) {
	  //Retrieving the template file location
	  const char *template_file = (p->second).c_str();

	  std::ifstream *in = new ifstream(template_file,std::ios::in|std::ios::ate);
	  if(!in || (!*in)){
	  	return false;
	  }

	  int size = in->tellg();

	  if(size==0) {
	  	 PUTIN_MAP(template_val,"",(p->first).c_str());
	  	 in->close();
	  	 delete in;
	  	 continue;
	  }

	  in->seekg(0,std::ios::beg);
	  char *buffer=new char[size];

	  in->read(buffer,size);

	  PUTIN_MAP(template_val,buffer,(p->first).c_str());
	  delete [] buffer;
	  in->close();
	  delete in;
   }
   return true;
}
