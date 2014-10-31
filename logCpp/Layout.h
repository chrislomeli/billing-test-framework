// Layout.h: interface for the Layout class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LAYOUT_H__ECE17CBC_D100_435E_899B_FA6CBB65DB8B__INCLUDED_)
#define AFX_LAYOUT_H__ECE17CBC_D100_435E_899B_FA6CBB65DB8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <stdio.h>
#include <stdlib.h>
#include <logCpp/logCpp.h>
#include <logCpp/EventLogType.h>

class Layout  
{
public:
	virtual string layoutMessage( EventLogType& e ) = 0;
	Layout();
	virtual ~Layout();

};

#endif // !defined(AFX_LAYOUT_H__ECE17CBC_D100_435E_899B_FA6CBB65DB8B__INCLUDED_)
