// LogWriteLine.h: interface for the LogWriteLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGWRITELINE_H__2140E2C3_C095_4D32_9D8E_94D0F0572011__INCLUDED_)
#define AFX_LOGWRITELINE_H__2140E2C3_C095_4D32_9D8E_94D0F0572011__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <stdio.h>
#include <stdlib.h>
#include <logCpp/logCpp.h>
#include <logCpp/EventLogType.h>
#include <logCpp/Layout.h>

class LogWriteLine  : public Layout
{
public:
	LogWriteLine();
	virtual ~LogWriteLine();
	string layoutMessage( EventLogType& e );
};

#endif // !defined(AFX_LOGWRITELINE_H__2140E2C3_C095_4D32_9D8E_94D0F0572011__INCLUDED_)
