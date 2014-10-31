#ifndef C__c_Resource__H
#define C__c_Resource__H

#include <util/EnvironmentStd.h>
#include <ctype/c_EnumMap.h>

class c_Resource :
	public c_EnumMap
{
public:
	c_Resource(string);
	c_Resource(void);
	~c_Resource(void);
	virtual void initialize_map(void);
};

#endif

