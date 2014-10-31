/**
 * \file bsi_util.h
 * Header file for utility funtions used in BSI.
 *
 * $Id: bsi_util.h,v 1.2 2007/01/23 19:53:37 sathpv Exp $
 * $Source: /proj/cvs/untd_billing/server/tools/cpptools/bsi/bsi_util.h,v $
 *********************************************************************************/

#ifndef __BSI_UTIL_H__
#define __BSI_UTIL_H__

#include <map>
#include <iterator>
#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <vector>

#include "return_t.h"

using namespace std;

/**
 * Utility class for BSI
 */

/// The map that is used throughout for storing data
typedef map<string,string> entity_map;

/// Enters the key value pair into the map
return_t PUTIN_MAP(entity_map *pblock, const char * value, const char * key);

/// Searches for a particular key in the map and returns the value corresponding to it
char * entity_findval(const char * key, entity_map * map_block);

/// Returns whether the key exists in the map or not : by mvamsee
bool isExists(const char *key,entity_map * map_block);

/// Deletes the key and corresponding value from the map : by mvamsee
return_t eraseFromMap(const char *key,entity_map * map_block);

/// Enters the key value pair into the map
return_t map_nvinsert(const char *key, const char * value, entity_map *map_block);

/// Formats the phone number
return_t DisplayablePhoneFormat( char *input_phone, char *output_phone );

/// Formats time
return_t DisplayableTimeFormat(time_t *intime, char *outtime, int outtimelen);

/// Format date for EventViewer
return_t EventViewerDateFormat(time_t *intime, char *outtime,int outtimelen,int num_months);

/// converts the date string in the "MM/DD/YYYY" format to a time_t value
time_t ConvertDateStringToTime_t(char *date_str);

/// Formats date
return_t DisplayableDateFormat(time_t *intime, char *outtime, int  outtimelen);

/// Converts to time format
return_t convert_to_time_format( long total_number_of_seconds, char *time);

/// Inserts error code and error message to the map
void insert_error(entity_map *map_block, const char* err_code, const char * error_string);

/// Appends the value to the existing value for the supplied key in the map
return_t map_append_value_with_comma(const char *key, const char * value, entity_map *map_block);

/// Replaces the existing value with the new one for the supplied key in the map
return_t entity_replace(const char *key, const char * value, entity_map *map_block);

/// compares the given value with the value corresponding to the given key in the map
int entity_compare(entity_map *map_block, const char * key, const char *new_value);

/// parses a string of the format "name1:value1,name2:value2" into a map
int parse_config_str_to_map(entity_map *map_block, const char *config_string);

/// Converts a string to lowercase
char *LowerCase(char *s) ;

/// Converts a string to Uppercase : @ mvamsee
char *toUpperCase(char *s) ;

//Data Encryption and decryption methods
void encryptData(char *data, char * &output);
void decryptData(char *data, char* &output);

//Reverses the given string
void strrev(char * &string);

/// Gets the template file location from template_loc map,
/// Reads the corresponding file contents and stores in template_val map
/// : by mvamsee
bool readTemplateFiles(entity_map *template_loc,entity_map *template_val);
#endif
