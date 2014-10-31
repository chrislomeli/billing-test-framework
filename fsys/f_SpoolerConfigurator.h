#ifndef __HEADER__f_SpoolerConfigurator__HH
#define __HEADER__f_SpoolerConfigurator__HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <util/EnvironmentStd.h>
#include <fsys/f_File.h>
#include <fsys/f_Spooler.h>
#include <fsys/f_PropertiesReader.h>

class f_SpoolerConfigurator : public f_PropertiesReader
{
public:
	f_SpoolerConfigurator( f_Spooler * spoolp ) 
		: spoolerp(NULL)
	{
		this->errRec.clear();
		this->spoolerp = spoolp;	
	}
	~f_SpoolerConfigurator(void) {}


	//--properties handlers --------------------
	void onGroup(string group, c_NvRecord attribs)
	{
			this->sc.clear();
			string id(attribs.value("class"));
			if (id =="pending")
					sc.state = Pending; 
			else if (id =="spool")
					sc.state = Spool; 
			else if (id =="backup")
					sc.state = Backup; 
			else if (id =="processed")
					sc.state = Processed; 
			else if (id =="suspense")
					sc.state = Suspense; 
			else if (id =="error")
					sc.state = Error;
			else
				this->errRec.set(ConfigError,definition_error, 0,
					"Unknown group(%s) ",  group.c_str());

	}
	
	void onGroupFinal(string group)
	{
			spoolerp->setSpoolConfig(sc);
	}

	void onElem(string group, string elem, string value, c_NvRecord attribs)
	{
			if (elem.compare("basedir")==0)  sc.dir = value;
			else if (elem.compare("annotate")==0) sc.annotation = value;
			else if (elem.compare("subdirs")==0)  sc.dir_mask = value;
			else if (elem.compare("filestamp")==0) sc.stamp_mask = value;
			else if (elem.compare("ext")==0) sc.ext = value;
			else
				this->errRec.set(ConfigError,definition_error,0,"Unknown element(%s)", elem.c_str() );
	}

	f_Spooler::SpoolConfig sc;
	f_Spooler * spoolerp;
	ErrorRecord errRec;
};
#endif
