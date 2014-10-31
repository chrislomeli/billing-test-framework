// f_Spooler.cpp: implementation of the f_Spooler class.
//
//////////////////////////////////////////////////////////////////////

#include <fsys/f_Spooler.h>
#include <fsys/f_SpoolerConfigurator.h>

static char *CVS_Id = (char*)"CVS_VERSION:$Id: f_Spooler.cpp,v 1.4 2007/01/24 01:15:07 clomeli Exp $";


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


f_Spooler::f_Spooler()
{
	this->errorRec.clear();
}

f_Spooler::~f_Spooler()
{

}



/*=================================================
 * move_toProcessed
 *================================================*/
Return_code
f_Spooler::copyFile(
			f_FileName fname,
			string dest_directory,
			string dest_extension,
			string datestampmask,
			string dirmask,
			string annotation )  // leading record marker
{
        string processed_file;
        string root;
        string extstr;
        char dbuffer[MAX_BUFSIZE];
        char buffer[MAX_BUFSIZE];
        int fd = 0;
        FILE* pf = NULL;


        //-- try to open output ------
        if ((fd=f_File::createDatestampFile(
					dest_directory,	fname.file_root_name, dest_extension,
					datestampmask,dirmask, 
					processed_file    )) < 0 )
		{
		 	_debugLog(logError) << __SPOOL_PREFIX__
				<< " spool.copyFile: Can not create file: " << fname.file_root_name
				<< " in dir: " <<  dest_directory << endl;
			return BadFileOpen;
		}
		errno = 0;

        //-- try to open input ------
        if ((pf = fopen(fname.full_path.c_str(),"r")) == NULL ) {
                close(fd);
		 		_debugLog(logError) << __SPOOL_PREFIX__
					<< " spool.copyFile: Can not open file" 
					<< fname.full_path  << endl;
				return BadFileOpen;
        }

        //-- copy ------
        int len;
        while (fgets(buffer,MAX_BUFSIZE,pf) != NULL ) 
		{
                len = (int)strlen(buffer);
				if (len+1 >= MAX_BUFSIZE)
	                return this->errorRec.set(
						MemRangeError,0,fname.full_path,errno,not_set);
				
				sprintf(dbuffer,"%s%s\0", annotation.c_str(), buffer );
				len = (int)strlen(dbuffer);

                if ( write(fd,(void*)dbuffer,len) != len )
				{
		 			_debugLog(logError) << __SPOOL_PREFIX__
						<< " copyFile: Can write to " << processed_file
						<< endl;
					return BadFileWrite;
				}
        }
        fclose(pf);
        close(fd);


        //--return---
        return Success;

}

/*=================================================
 * move_toProcessed
 *================================================*/
Return_code
f_Spooler::moveFile(
			f_FileName fname,
			string dest_directory,
			string dest_extension,
			string datestampmask,
			string dirmask,
			string annotation )  // leading record marker
{
		Return_code rc;

		if ((rc=copyFile(
				fname, dest_directory, 
				dest_extension, datestampmask,
				dirmask, annotation)) != Success )
		{
			return rc;
		}


		//-- unlink ------
		errno = 0;
		if ( unlink(fname.full_path.c_str()) == -1 )
		{
		 	_debugLog(logError) << __SPOOL_PREFIX__
					<< " copyFile: Can delete file " << fname.full_path
					<< endl;
			return BadFileWrite;
		}
		//--return---
        return Success;

}

/*=================================================
 * walkPending_dir -recursive dir reader
 * Post: Walk through directory structure
 *		and process files.
 *================================================*/
Return_code 
f_Spooler::processDirectory(
		SpoolState state,
		f_Dir* dirp, 
		string dirstr, 
		bool sub_dirs )
{
	//-- Walk through pending dir structure 
	Return_code rc;
	string	filename;
	string	fulldir;
	file_type_t	ft;
	void * vp;

	
	//-- open the directory ---
	_debugLog(logTrace1) << __SPOOL_PREFIX__<< " Process directory: " << dirstr << endl;

	if( (vp=dirp->OpenDir( dirstr )) == NULL )
	{
		_errorLog 
			<< "spool" << __NOW__ << " Can not open dir " << dirstr << endl;
		return BadFileOpen;
	}


	while ( dirp->ReadDir( vp, filename, ft ) )
	{
		if ( ft == FILETYPE_DIR && sub_dirs ) 
		{ //if directory, recurse
			ostringstream dirss ;
			dirss << dirstr.c_str() << SLASH << filename.c_str();
			this->processDirectory( state, dirp, dirss.str(), sub_dirs );
			dirp->setDirName( dirstr ); // reset back to this dir - shortcut for now
		}
		else if ( ft == FILETYPE_NORMAL ) 
		{
			/* process one file */
			SpoolState next_state;
			f_FileName fname;
			fname.setFileName(dirstr,filename);

			_debugLog(logTrace2) << __SPOOL_PREFIX__<< "Process File: " << fname.file_root_name << endl;
			_debugLog(logTrace3) << _THIS_STATE(state) << endl;
			rc = Success;
			switch(state)
			{
				case Pending:
					if ((rc=this->doCopyOneFile(fname,Backup, false))==Success ) 
						rc=this->doMoveOneFile(fname,Spool);
					next_state = Spool;
					break;

				case Spool:
				default:
					next_state = this->doProcessOneFile(fname);
					if (next_state != state)
						rc=this->doMoveOneFile(fname,next_state);
					break;
			}
			if (rc!=Success)
				_debugLog(logError) << __SPOOL_PREFIX__<< "Failed File: " << fname.file_root_name << endl;
			_debugLog(logTrace3) << _NEXT_STATE(next_state) << endl;

		}
	}

	dirp->CloseDir(vp);
	return Success;
}


/*=================================================
 * walkPending_dir -recursive dir reader
 * Post: Walk through directory structure
 *		and process files.
 *================================================*/
void 
f_Spooler::dumpDirectory(
		f_Dir* dirp, 
		string dirstr,
		ostream& out )
{
	//-- Walk through pending dir structure 
	static int level = 0;
	string	filename;
	string	fulldir;
	file_type_t	ft;
	void * vp;

	level++;

	//--good place to count overall ---
	out <<  "---dir- " << dirstr << "-----" << endl;
	
	//-- open the directory ---
	if( (vp=dirp->OpenDir( dirstr )) == NULL )
		return;


	while ( dirp->ReadDir( vp, filename, ft ) )
	{
		if ( ft == FILETYPE_DIR  ) 
		{ //if directory, recurse
			ostringstream dirss ;
			dirss << dirstr.c_str() << SLASH << filename.c_str();
			this->dumpDirectory(  dirp, dirss.str(), out );
		}
		else if ( ft == FILETYPE_NORMAL ) 
		{
			out <<  "	- " << filename  << endl;

		}
	}


	dirp->CloseDir(vp);
	return;
}



Return_code
f_Spooler::runSpooler(void)
{
	try{
		processDirectory(Pending);
		processDirectory(Spool);
	} 
	catch (const p_Exception& e)
	{
		_errorLog << __NOW__ <<  __SPOOL_PREFIX__ << e << endl;
	}
	return Success;
}
/*
Return_code
f_Spooler::processSpoolDirectory(void)
{
	SpoolConfig * confp = this->getSpoolConfig(Spool);
	if (!confp)
		return this->errorRec.set(ConfigError,0,"No spool dir is set",0,not_set);

	//--instatiate dirent class and walk the directory stuct ----
	f_Dir dir;	
	string pending_root;

	if (! confp->ext.empty() ) 
	{
		dir.setExtensionList(confp->ext);
	}
	
	return this->processDirectory(Spool, &dir, confp->dir, true);
}

Return_code
f_Spooler::processPendingDirectory(void)
{
	Return_code rc;

	SpoolConfig * confp = this->getSpoolConfig(Pending);
	if (!confp)
		return this->errorRec.set(ConfigError,0,"No pending dir is set",0,not_set);

	//--instatiate dirent class and walk the directory stuct ----
	f_Dir dir;	
	string pending_root;

	if (! confp->ext.empty() ) 
	{
		dir.setExtensionList(confp->ext);
	}
	
	if ((rc=this->processDirectory(Pending, &dir, confp->dir, true)) != Success)
		return rc;
	return Success;
}

*/

Return_code
f_Spooler::processDirectory(SpoolState which_state)
{
	SpoolConfig * confp = this->getSpoolConfig(which_state);
	if (!confp)
		throw (
			p_Exception(ConfigError,__HERE__,
			(char*)"processDirectory: Can not find config data for %s", this->spoolState(which_state)));

	//--instatiate dirent class and walk the directory stuct ----
	f_Dir dir;	
	string pending_root;

	if (! confp->ext.empty() ) 
	{
		dir.setExtensionList(confp->ext);
	}
	
	this->processDirectory(which_state, &dir, confp->dir, true);
	return Success;
}


Return_code
f_Spooler::doCopyOneFile(f_FileName fname, SpoolState state, bool required)
{
	_debugLog(logTrace2) << __SPOOL_PREFIX__  << " doCopyOneFile: " << fname.file_root_name << "to "  << spoolState(state) << endl;

	if (state == NoChange){
		_debugLog(logTrace3) << __SPOOL_PREFIX__ << " No change required... " << endl;
		return Success;
	}

	SpoolConfig * confp = this->getSpoolConfig(state);
	if (!confp )
		if (!required) 
		{
				_debugLog(logTrace2) << "Skipping state " << spoolState(state) << endl;
				return Success;
		}
		else
			throw (
				p_Exception(ConfigError,__HERE__,
					(char*)"doCopyOneFile: Can not find config data for %s", spoolState(state)));

	Return_code rc =  copyFile(
				fname,
				confp->dir,
				confp->ext,
				confp->stamp_mask,
				confp->dir_mask, 
				confp->annotation );

	if (rc != Success)
		_debugLog(logTrace2) << __SPOOL_PREFIX__ << " Copy to " << confp->dir << " failed ..." << endl;
	return rc;
}

Return_code
f_Spooler::doMoveOneFile(f_FileName fname, SpoolState state, bool required)
{
	_debugLog(logTrace2) << __SPOOL_PREFIX__  << " doMoveOneFile: " << fname.file_root_name << "to "  << spoolState(state) << endl;

	if (state == NoChange){
		_debugLog(logTrace3) << __SPOOL_PREFIX__ << " No change required... " << endl;
		return Success;
	}

	SpoolConfig * confp = this->getSpoolConfig(state);
	if (!confp )
		if (!required) 
		{
				_debugLog(logTrace2) << "Skipping state " << spoolState(state) << endl;
				return Success;
		}
		else
			throw (
				p_Exception(ConfigError,__HERE__,
					(char*)"doMoveOneFile: Can not find config data for %s", spoolState(state)));

	Return_code rc = moveFile(
			fname,
			confp->dir,
			confp->ext,
			confp->stamp_mask,
			confp->dir_mask, 
			confp->annotation );

	if (rc != Success)
		_debugLog(logTrace2) << __SPOOL_PREFIX__ << " Copy to " << confp->dir << " failed ..." << endl;
	return rc;
}

SpoolState 
f_Spooler::doProcessOneFile(f_FileName fname)
{
	this->errorRec.set(NullValue,application_error,errno,
		"doProcessOneFile TEST version called ..." );
	return Error;

	//--do work----
	static int flip=0;

	int d = flip++ % 3;
	if (d == 0)
		return Processed;
	if (d == 1)
		return Suspense;

	return Processed;
}


f_Spooler::SpoolConfig * f_Spooler::getSpoolConfig(SpoolState which_state)
{
	map<SpoolState,SpoolConfig>::iterator pos = m_spool_configs.find(which_state);
	if (pos == m_spool_configs.end())
		return NULL;
	return &pos->second;
}

void f_Spooler::setSpoolConfig( SpoolConfig conf )
{
	SpoolState dstate = conf.state;
	this->m_spool_configs[dstate] = conf;
}

void f_Spooler::setSpoolConfig(
			SpoolState dstate, 
			string ddir, string dext, 
			string dstamp, string dmask, string annotate  )
{
	SpoolConfig conf;
	conf.set(dstate,ddir, dext, dstamp, dmask, annotate);
	this->m_spool_configs[dstate] = conf;
}

void f_Spooler::dumpConfigs(ostream& out)
{
		out << "\n------------------\nConfigs\n--------------------\n";
			map<SpoolState,SpoolConfig>::iterator pos = this->m_spool_configs.begin();
		for (; pos != this->m_spool_configs.end(); pos++ )
		{
			out << "State: " << (int)pos->second.state
				 << ", dir: " << pos->second.dir
				 << ", ext : " << pos->second.ext
				 << ", stamp : " << pos->second.stamp_mask
				 << ", dirmask : " << pos->second.dir_mask
				 << endl;
		}
}


Return_code f_Spooler::loadSpoolerProperties(const string config_file)
{
	f_SpoolerConfigurator spoolconfig(this);

	spoolconfig.readPropertiesFile("spoolstate", config_file);
	if (spoolconfig.errRec.isError())
		return (this->errorRec.set(spoolconfig.errRec));

	return Success;
}
