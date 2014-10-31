#ifndef C__PHONE__H
#define C__PHONE__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <ctype/c_Phone.h>

/*
typedef enum pin_phone_type {
	PIN_PHONE_TYPE_UNKNOWN =	0,
	PIN_PHONE_TYPE_HOME =		1,
	PIN_PHONE_TYPE_WORK =		2,
	PIN_PHONE_TYPE_FAX =		3,
	PIN_PHONE_TYPE_PAGER =		4,
	PIN_PHONE_TYPE_PORTABLE =	5,
	PIN_PHONE_TYPE_POP =		6,
	PIN_PHONE_TYPE_SUPPORT =	7
} pin_phone_type_t;
*/

class c_Phone
{
public:
	c_Phone(void);
	c_Phone(string raw, bool daytime );
	c_Phone(string raw, pin_phone_type_t type = PIN_PHONE_TYPE_POP );

	~c_Phone(void);

	void set(string raw, bool daytime );
	void set(string raw, pin_phone_type_t type );
	void reset();

	string raw_number;
	pin_phone_type_t  phone_type;

	string& getRaw(string&);
	void setRaw(string);
	bool raw_Equals(string test);
};

#endif


