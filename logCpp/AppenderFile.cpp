#include "AppenderFile.h"

Appender::Appender(string path)
:	fp(NULL)
, layout(NULL)
, isExternal(false)
, keep_closed(false)
{
	this->open( path );
}
Appender::Appender(FILE * ofp)
:	fp(NULL)
, layout(NULL)
, isExternal(false)
, keep_closed(false)
{
	this->open( ofp );
}


Appender::Appender()
:	fp(NULL)
, layout(NULL)
, isExternal(false)
, keep_closed(false)
{
	
}

Appender::~Appender(void)
{
	this->close();
	if ( this->layout != NULL )
		delete this->layout;
}

bool Appender::open(FILE * ofp)
{
	this->close();

	if (ofp == NULL)
		return false;
	this->isExternal = true;
	this->fp = ofp;
	this->_appender_path = "<<external file>>";
    return true;
}


bool Appender::open(string path)
{
	this->close();

	if (path.empty())
		return false;

	if ( path == "stdout" )
	{
		this->isExternal = true;
		this->fp = stdout;
	}
	else if ( path == "stderr" )
	{
		this->isExternal = true;
		this->fp = stderr;
	}
	else if ((this->fp = 
		fopen( path.c_str(), "a")) == NULL )
	{
		return false;
	}
	this->_appender_path = path;
    return true;
}

void Appender::close(void)
{
	if ( this->fp != NULL && ! this->isExternal)
		fclose(fp);

	this->fp=NULL;
	this->_appender_path = "";
}

bool Appender::doAppend(EventLogType& event)
{
	if (fp == NULL)
		return false;

	if ( this->keep_closed )
	{
		if (this->_appender_path.empty())
			return false;
		this->open( this->_appender_path );
	}

	if (this->layout)
		layout->layoutMessage(event);

	fprintf(fp,"%s", event._message.c_str());

	if ( this->keep_closed )
		this->close();

	fflush(fp);
	return true;
}

void Appender::setKeepClosed(bool close_mode)
{
	this->keep_closed = close_mode;
}


void Appender::putLayout(Layout *this_layoutp)
{
	this->layout = this_layoutp;
}


