#if !defined(AFX_DOUBLE_H__c_Decimal_3A6376C60282__INCLUDED_)
#define AFX_DOUBLE_H__c_Decimal_3A6376C60282__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning (disable : 4786 4005)
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>

class c_Decimal  
{
public:
	c_Decimal();
	virtual ~c_Decimal();

	void erase();

	bool create( string );
	bool create( const char* str );
	bool create( double dd );
	bool create( pin_decimal_t* );
	bool put( pin_decimal_t* );
	pin_decimal_t * getDecimal();

	bool isnull();
	pin_decimal_t * copy();
	pin_decimal_t * take();
	pin_decimal_t * borrow();


	pin_decimal_t* valuep;
	int ref;
	pin_errbuf_t ebuf;

};

#endif // !defined(AFX_DOUBLE_H__C8D0A9EE_FF6E_4349_A3B5_3A6376C60282__INCLUDED_)
