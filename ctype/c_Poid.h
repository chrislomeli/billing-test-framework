// PoidOwner.h: interface for the c_Poid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POIDOWNER_H__B3A1118C_DD57_4DB6_A193_8B6B0DCB98EA__INCLUDED_)
#define AFX_POIDOWNER_H__B3A1118C_DD57_4DB6_A193_8B6B0DCB98EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning (disable : 4786 4005)
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>

class c_Poid  
{
public:

	c_Poid();
	virtual ~c_Poid();

	bool create ( string poidstr );
	bool create( int64 db, string type, int64 id );
	bool create( poid_t * pdp );
	void erase();
	int64 getdb();
	string& gettype(string &s);
	int64 getid();
	bool isnull();
	poid_t * copy();
	poid_t * take();
	poid_t * borrow();

	poid_t * poid_pdp;
	pin_errbuf_t ebuf;
	int ref;

};

#endif // !defined(AFX_POIDOWNER_H__B3A1118C_DD57_4DB6_A193_8B6B0DCB98EA__INCLUDED_)
