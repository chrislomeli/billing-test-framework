#if !defined(AFX_EventLogType_H__ECE17CBC_D100_435E_899B_FA6CBB65DB8B__INCLUDED_)
#define AFX_EventLogType_H__ECE17CBC_D100_435E_899B_FA6CBB65DB8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <stdio.h>
#include <stdlib.h>
#include <logCpp/logCpp.h>

struct EventLogType
{
	EventLogType(string message, threshold_t priority)
	{
		this->_message = message;
		this->_priority = priority;
	};

	string _message;
	threshold_t _priority;
};
#endif

