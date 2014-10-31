#ifndef C__c_AccountBalance__H
#define C__c_AccountBalance__H

#include <util/EnvironmentStd.h>
#include <ctype/c_Currency.h>
#include <ctype/c_Resource.h>


class c_AccountBalance
{
public:
	c_AccountBalance(void);
	~c_AccountBalance(void);
	c_Resource resource;
	c_Currency amount;
	c_Currency  limit;
	c_Currency  floor;
	c_Currency  threshold;
	bool equals(c_Resource resourceName, c_Currency c);
};


struct balLess
{
  bool operator()( c_AccountBalance* a, c_AccountBalance* b) 
  {
		return ( a->resource.get() < b->resource.get() );
  }
};

#endif
