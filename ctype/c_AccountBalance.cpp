#include "c_AccountBalance.h"

c_AccountBalance::c_AccountBalance(void)
{
}

c_AccountBalance::~c_AccountBalance(void)
{
}

bool c_AccountBalance::equals( c_Resource resourceName, c_Currency c )
{
	if (! this->resource.equals(resourceName.get() ) )
		return false;
	if ( this->amount == c )
		return true;
	else
		return false;
}
