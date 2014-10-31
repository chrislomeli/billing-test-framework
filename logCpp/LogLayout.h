// LogLayout.h: interface for the LogLayout class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGLAYOUT_H__9CFB885E_25F9_43F6_A089_CBD2CDC23CD8__INCLUDED_)
#define AFX_LOGLAYOUT_H__9CFB885E_25F9_43F6_A089_CBD2CDC23CD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <stdio.h>
#include <stdlib.h>
#include <logCpp/logCpp.h>
#include <logCpp/EventLogType.h>
#include <logCpp/Layout.h>


class LogLayout  : public Layout
{
public:
	LogLayout();
	virtual ~LogLayout();

	string layoutMessage( EventLogType& e );

};

#endif // !defined(AFX_LOGLAYOUT_H__9CFB885E_25F9_43F6_A089_CBD2CDC23CD8__INCLUDED_)
