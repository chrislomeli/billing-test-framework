// PoidOwner.cpp: implementation of the c_Poid class.
//
//////////////////////////////////////////////////////////////////////
#include "c_Poid.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
c_Poid::c_Poid() : poid_pdp(NULL)
{
	erase();
}

c_Poid::~c_Poid()
{
	this->erase();
}

bool c_Poid::create(int64 db, string type, int64 id)
{
	//-- sanity --------------
	if ( type.empty() || id < 1 )
		return false;

	//-- check for existing and delete
	if ( ref > 0 )
		this->erase();

	PIN_ERR_CLEAR_ERR( &ebuf );
	ref=0;

	this->poid_pdp = 
		PIN_POID_CREATE( db, (char*)type.c_str(), id, &ebuf);

	if ( PIN_ERR_IS_ERR( &ebuf ) ) {
		return false;
	}

	ref = 1;
	return true;
}

bool c_Poid::create(poid_t * pdp)
{
	//-- sanity --------------
	if (pin_poid_is_null(pdp))
		return false;

	//-- check for existing and delete
	if ( ref > 0 )
		this->erase();

	PIN_ERR_CLEAR_ERR( &ebuf );
	ref=0;

	this->poid_pdp = PIN_POID_COPY( pdp, &ebuf );

	if ( PIN_ERR_IS_ERR( &ebuf ) ) {
		return false;
	}

	ref = 1;
	return true;
}

bool c_Poid::create(string poidstr)
{
	//-- sanity --------------
	if (poidstr.empty())
		return false;

	//-- check for existing and delete
	if ( ref > 0 )
		this->erase();

	PIN_ERR_CLEAR_ERR( &ebuf );
	ref=0;

	//-- replace it with new
	this->poid_pdp = 
		PIN_POID_FROM_STR(
			(char*)poidstr.c_str(), NULL, &ebuf ); 

	if ( PIN_ERR_IS_ERR( &ebuf ) ) {
		return false;
	}

	ref = 1;
	return true;
}


void c_Poid::erase()
{
	//-- delete
    if (! PIN_POID_IS_NULL(this->poid_pdp) )
		PIN_POID_DESTROY(this->poid_pdp,NULL);

	//-- reset values
	this->ref = 0; 
	this->poid_pdp = (NULL); 
}

int64 c_Poid::getdb()
{
	return  pin_poid_get_db(this->poid_pdp);
}

string& c_Poid::gettype(string &s)
{
	const char *cp = pin_poid_get_type(this->poid_pdp);
	s = cp;
	return s;
}

int64 c_Poid::getid()
{
	return  PIN_POID_GET_ID(this->poid_pdp);
}


bool c_Poid::isnull()
{
	return ( pin_poid_is_null(this->poid_pdp ) == 0 ? false:true    );
}

poid_t * c_Poid::copy()
{
	PIN_ERR_CLEAR_ERR( &ebuf );
	return  PIN_POID_COPY(this->poid_pdp, &ebuf);
}


poid_t * c_Poid::take()
{
	poid_t * newpoid = this->poid_pdp;
	this->ref = 0; 
	this->poid_pdp = (NULL); 
	return newpoid;
}


poid_t * c_Poid::borrow()
{
	return this->poid_pdp;
}

