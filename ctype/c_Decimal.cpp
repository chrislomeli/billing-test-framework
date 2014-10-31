// Double.cpp: implementation of the c_Decimal class.
//
//////////////////////////////////////////////////////////////////////
#include "c_Decimal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
c_Decimal::c_Decimal() : valuep(NULL)
{
	this->erase();
}

c_Decimal::~c_Decimal()
{
	if (valuep)
		pin_decimal_destroy(this->valuep);

	valuep=NULL;
}

void c_Decimal::erase()
{
	if (valuep)
		pin_decimal_destroy(this->valuep);

	valuep=NULL;
	ref=0;
}


bool c_Decimal::create(string numstr)
{
	return ( create(numstr.c_str()) );
}

bool c_Decimal::create(const char* numstr)
{
	//-- sanity --------------

	if (numstr == NULL)
		return false;

	if (strlen(numstr) <=0 )
		return false;

	//-- check for existing and delete
	if ( ref > 0 )
		this->erase();

	PIN_ERR_CLEAR_ERR( &ebuf );

	this->valuep = pbo_decimal_from_str(numstr, &ebuf);

	//-- results -----------------
	if ( PIN_ERR_IS_ERR( &ebuf ) ) {
		return false;
	}

	ref = 1;
	return true;
}

bool c_Decimal::put(pin_decimal_t *dp)
{
	//-- sanity --------------
	PIN_ERR_CLEAR_ERR( &ebuf );
	if ( pbo_decimal_is_null(dp, &ebuf))
		return false;

	//-- check for existing and delete
	if ( ref > 0 )
		this->erase();

	this->valuep = dp;
	
	//-- results -----------------
	ref = 1;
	return true;
}


bool c_Decimal::create(pin_decimal_t *dp)
{
	//-- sanity --------------
	PIN_ERR_CLEAR_ERR( &ebuf );
	if ( pbo_decimal_is_null(dp, &ebuf))
		return false;

	//-- check for existing and delete
	if ( ref > 0 )
		this->erase();

	PIN_ERR_CLEAR_ERR( &ebuf );
	ref=0;

	this->valuep = pbo_decimal_copy(dp, &ebuf );
	
	//-- results -----------------
	if ( PIN_ERR_IS_ERR( &ebuf ) ) {
		return false;
	}
	ref = 1;
	return true;
}

bool c_Decimal::create(double dd)
{
	ostringstream os;
	os << dd;
	return create( os.str() );
}

bool c_Decimal::isnull()
{
	PIN_ERR_CLEAR_ERR( &ebuf );
	return ((pbo_decimal_is_null(this->valuep, &ebuf )) == 1 );
}

pin_decimal_t * c_Decimal::copy()
{
	PIN_ERR_CLEAR_ERR( &ebuf );
	return  pbo_decimal_copy(this->valuep, &ebuf);
}


pin_decimal_t * c_Decimal::take()
{
	pin_decimal_t * newdec = this->valuep;
	this->ref = 0; 
	this->valuep = (NULL); 
	return newdec;
}


pin_decimal_t * c_Decimal::borrow()
{
	return this->valuep;
}

