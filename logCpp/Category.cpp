#include "Category.h"
#include <stdarg.h>

Category::Category(threshold_t level)
: 	nominal_size(1024)
{
	this->t_level = level;
}

Category::~Category(void)
{
	this->clear();
}

Appender  *
Category::addAppender(string name, string path)
{
	Appender  * ap = new Appender(path);
	this->addMember(name,ap);
	return ap;
}

void Category::setThreshold(threshold_t level)
{
	this->t_level = level;
	return;
}

bool Category::meetsThreshold(threshold_t level)
{
	if (this->t_level == level_Output)
		return true;
	if (level == level_Output)
		return true;
	return (level >= this->t_level);
}

void Category::logError(const char* stringFormat, ...) 
{ 
	if (!meetsThreshold(level_Error))
		return;
    va_list va;
    va_start(va,stringFormat);

    EventLogType event(
		 makeOutputString(stringFormat, va), level_Error );

	callAppenders(event);
	va_end(va);
}
void Category::logWarning(const char* stringFormat, ...) 
{ 
	if (!meetsThreshold(level_Warning))
		return;
    va_list va;
    va_start(va,stringFormat);

    EventLogType event(
		 makeOutputString(stringFormat, va), level_Warning );

	callAppenders(event);
	va_end(va);
}

void Category::logTrace(const char* stringFormat, ...) 
{ 
	if (!meetsThreshold(level_Trace))
		return;
    va_list va;
    va_start(va,stringFormat);

    EventLogType event(
		 makeOutputString(stringFormat, va), level_Trace );

	callAppenders(event);
	va_end(va);
}

void Category::logInfo(const char* stringFormat, ...) 
{
    va_list va;
    va_start(va,stringFormat);

    EventLogType event(
		 makeOutputString(stringFormat, va), level_Output );

	callAppenders(event);

	va_end(va);
}

void Category::log(threshold_t level, const char* stringFormat, ...) 
{
	if (!meetsThreshold(level))
		return;

    va_list va;
    va_start(va,stringFormat);

    EventLogType event(
		 makeOutputString(stringFormat, va), level );

	callAppenders(event);

	va_end(va);
}

Appender * Category::getAppender(string name)  
{
	Appender * appenderp = NULL;
	map<string,Appender*>::iterator pos = this->_Map.find(name);
	
	if ( pos != this->_Map.end() )
	{
		appenderp = pos->second;
	}
	return appenderp;
}

void Category::callAppenders(EventLogType& event)  
{
	Appender * appenderp = NULL;
	map<string,Appender*>::iterator pos = this->_Map.begin();
	
	for ( ; pos !=this->_Map.end(); pos++ )
	{
		Appender * appenderp = pos->second;
		appenderp->doAppend(event);
	}
}

string Category::makeOutputString(
	const char* stringFormat, va_list va )
{
	size_t size = nominal_size;
	char * buffer = new char[nominal_size];

	while (1) 
	{
		int n = vsnprintf(buffer, size, stringFormat, va);
		        
		// If that worked, return a string.
		if ((n > -1) && (static_cast<size_t>(n) < size)) 
		{
			string s(buffer);
			delete [] buffer;
			return s;
		}
		        
		// Else try again with more space.
		size = (n > -1) ?
			n + 1 :   // ISO/IEC 9899:1999
			size * 2; // twice the old size
		        
		delete [] buffer;
		buffer = new char[size];
	}
}

