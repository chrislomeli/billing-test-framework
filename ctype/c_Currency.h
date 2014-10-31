// Double.h: interface for the c_Currency class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOUBLE_H__C8D0A9EE_FF6E_4349_A3B5_3A6376C60282__INCLUDED_)
#define AFX_DOUBLE_H__C8D0A9EE_FF6E_4349_A3B5_3A6376C60282__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning (disable : 4786 4005)
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>

class c_Currency  
{
public:
	c_Currency(double);
	c_Currency();
	virtual ~c_Currency();

	bool set( string, int cc = 840 );
	bool set( double , int cc = 840);
	bool set( pin_decimal_t* , int cc = 840);

	bool operator == ( c_Currency c ) {
		return (this->value == c.value); 
	}
	void operator += ( c_Currency c ) {
		this->value += c.value; 
	}

	void reset();
	string& get( string& );
	pin_decimal_t * getDecimal();

	bool empty() {
		return (!isset || !isgood);
	};

	

	double value;
	int currency_code;

	char nbuffer[50];
	bool isset;
	bool isgood;

	double get(void);
};

#endif // !defined(AFX_DOUBLE_H__C8D0A9EE_FF6E_4349_A3B5_3A6376C60282__INCLUDED_)
