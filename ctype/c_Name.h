#ifndef C__BS_NAME__H
#define C__BS_NAME__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>

#define DEFAULT_NAME "On File"

class c_Name
{
public:
	c_Name(void);
	~c_Name(void);

	string last_name;
	string first_name;
	string middle_name;
	string full_name;

	string& get_Fullname(string &outstr, bool generate_if_null=true);
	void setLastName(string);
	void setFirstName(string);
	void setMiddleName(string);
	void setFullName(string);
};

#endif
