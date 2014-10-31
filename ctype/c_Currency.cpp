// Double.cpp: implementation of the c_Currency class.
//
//////////////////////////////////////////////////////////////////////
#include "c_Currency.h"
#include <errno.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

c_Currency::c_Currency()
{
	this->reset();
}

c_Currency::c_Currency(double dd)
{
	this->set(dd);

}

c_Currency::~c_Currency()
{
	
}

bool c_Currency::set(string numstr, int cc)
{
	char *endp;
	errno=0;
	this->reset();

	if (numstr.empty())
		return false;

	if (numstr.find_first_not_of('0') == string::npos )
		this->value = 0;
	else {
			this->value = ::strtod(numstr.c_str(), &endp);
			this->currency_code = cc;
			this->isgood = this->isset = true;
			if (errno != 0) 
				return false;
	}
	return (this->isgood);
}


bool c_Currency::set(pin_decimal_t *dp, int cc)
{
	pin_errbuf_t ebuf;
	pin_errbuf_t *ebufp = &ebuf;
	PIN_ERR_CLEAR_ERR(ebufp);

	this->reset();

	double dd = pbo_decimal_to_double(dp, ebufp);
	this->currency_code = cc;
	this->isgood = this->isset = true;

	if ( PIN_ERR_IS_ERR(ebufp)) 
		return false;
	this->value = dd;
	return this->isgood;
}

bool c_Currency::set(double dd,  int cc)
{
	this->value = dd;
	this->currency_code = cc;
	this->isgood = this->isset = true;
	return this->isgood;
}

void c_Currency::reset()
{
	memset((char*)nbuffer,0,50);
	isgood=false;
	isset=false;
	value=0;
}

double c_Currency::get()
{
	if ( this->empty() )
		return -9999999;
	else {
		return value;
	}
}

string& c_Currency::get(string& dstr)
{
	char xbuffer[50];
	dstr.erase();
	if ( this->empty() )
		return dstr;
	else {
		sprintf(xbuffer,"%9.3f", this->value );
		dstr = xbuffer;
	}
	return dstr;
}

pin_decimal_t * c_Currency::getDecimal()
{
	pin_errbuf_t ebuf;
	pin_errbuf_t *ebufp = &ebuf;
	PIN_ERR_CLEAR_ERR(ebufp);

	sprintf(nbuffer,"%.2f", this->value );

	pin_decimal_t * pd = 
		  pbo_decimal_from_str(nbuffer,ebufp);
	return pd;
}
