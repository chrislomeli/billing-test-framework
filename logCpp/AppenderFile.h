#if !defined(AFX_Appender_H__ECE17CBC_D100_435E_899B_FA6CBB65DB8B__INCLUDED_)
#define AFX_Appender_H__ECE17CBC_D100_435E_899B_FA6CBB65DB8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <stdio.h>
#include <stdlib.h>
#include <logCpp/EventLogType.h>
#include <logCpp/Layout.h>


class Appender
{
public:
	Appender();
	Appender(FILE * ofp);
	Appender(string path);
	~Appender(void);

	void putLayout ( Layout * this_layoutp );
	bool doAppend(EventLogType& event);
	bool open(string path);
	void close(void);
	void setKeepClosed(bool close_mode);
	bool add_newline;

private:
	bool open(FILE * ofp);
	string _appender_path;
	FILE * fp;
	bool isExternal;
	bool keep_closed;
	Layout * layout;
};
#endif
