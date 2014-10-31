#ifndef C__AccountStatus__H
#define C__AccountStatus__H

#include <util/EnvironmentStd.h>
#include <ctype/c_EnumMap.h>

class c_Status : public c_EnumMap
{
public:
	c_Status(string);
	c_Status(void);
	~c_Status(void);

	bool operator == ( c_Status c ) {
		return (this->value == c.value); 
	};

	void initialize_map();
};

#endif
