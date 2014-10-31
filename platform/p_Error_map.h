#ifndef map_default_return_codes__H
#define map_default_return_codes__H

#define STRING_ID_TYPE_FLD_ERR "Bad or null type field in test_set"

#define XML_TAG_UNIT_TEST "unit_test"
#define XML_TAG_TEST_SET  "test_set"
#define XML_TAG_TEST_CASE "testcase"
#define XML_TAG_PARAM "param"

#define XML_ATTRIBUTE_ID "id"
#define XML_ATTRIBUTE_TYPE "type"
#define XML_ATTRIBUTE_ORDER "ord"
#define XML_ATTRIBUTE_FUNCTION "function"

#define XML_VALUE_INPUT "input"
#define XML_VALUE_STUB "stub"




typedef enum y_return {

//--Field errors------------------------------
	r_Field_not_found	= -01,
	r_Null_value		= -03,
	r_Duplicate_found	= -04,
	r_Not_found			= -05,
	r_Bad_Type			= -06,
	r_Bad_Value			= -07,

//--Parse errors---------------------------
	r_Found_text		= -100,
	r_Wrong_element		= -101,
	r_Parse_error		= -102,
	r_Type_Conversion	= -103,
	r_End_of_block		= -104,


//--IO Errors -------------------------------
	r_Bad_file_open		= -200,
	r_File_not_open		= -201,
	r_End_of_file		= -202,
	r_End_element		= -203,
	r_Insert_failed		= -204,

//--Execution Flow -------------------------------
	r_Execution_sequence= -06,

//--Success------------------------------------
	r_Success			= 1

} Return_code;


class p_Error_map {
	
public:

	p_Error_map(void)
	{	
	};

	~p_Error_map(void)
	{
	};

	static string& getMsg(Return_code rc, string& msg)
	{	
		msg.assign("Unknown");
		return msg;
	};
};

#endif



