#ifndef c_VariantStr_H
#define c_VariantStr_H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <platform/p_Exception.h>
#include "c_StringFormat.h"

#pragma warning(disable: 4786)

/*
 *
 * Status : not used - deprecated
 */
class c_VariantStr
{
public:
	void setType( string sytpe );
	short type;
	pin_decimal_t * toPinDecimal();
	
	// String format -translation
	bool	toPinErrBuffer(pin_errbuf_t * ebufp);
	long	toLong();
	double	toDouble();
	int		toInteger();
	time_t	toUTC();
	pin_flist_t * toPinFlist();

	//--CTOR/DTOR
	c_VariantStr(std::string);
	c_VariantStr();
	~c_VariantStr(void);

	bool set(std::string);

	//--variables
	vector<string> value;

	//operators 
	ostream& debug(ostream&);

	friend ostream& operator<<(ostream& out, c_VariantStr& vs) {
		return vs.debug(out);
	}

};

#endif
