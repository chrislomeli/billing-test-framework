#if !defined(AFX_Category_H__ECE17CBC_D100_435E_899B_FA6CBB65DB8B__INCLUDED_)
#define AFX_Category_H__ECE17CBC_D100_435E_899B_FA6CBB65DB8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <stdio.h>
#include <stdlib.h>
#include <logCpp/NamedMapContainer.h>
#include <logCpp/AppenderFile.h>
#include <logCpp/logCpp.h>


class Category 
	: public NamedMapContainer<Appender>
{
public:
	Category(threshold_t level = level_Trace);
	~Category(void);

	Appender  *	getAppender(string name); 
	Appender  *	addAppender(string name, string path);
	void setThreshold(threshold_t level);

	void logError(const char* stringFormat, ...);
	void logWarning(const char* stringFormat, ...);
	void logTrace(const char* stringFormat, ...); 
	void logInfo(const char* stringFormat, ...);
	void log(threshold_t level, const char* stringFormat, ...); 

private:
	string	makeOutputString(const char* stringFormat, va_list va );
	void	callAppenders(EventLogType& eventp);
	bool	meetsThreshold(threshold_t level);

	threshold_t t_level;
	size_t nominal_size;

};
#endif
