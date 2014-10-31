// f_Spooler.h: interface for the f_Spooler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F_SPOOLER_H__95112E8A_E5CC_48D1_BFAF_7DA568C7DD1B__INCLUDED_)
#define AFX_F_SPOOLER_H__95112E8A_E5CC_48D1_BFAF_7DA568C7DD1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///dependencies///////////////////////////////////////////////
/*** all logging stuff **/

#include <util/EnvironmentStd.h>
#include <fsys/f_Dir.h>
#include <fsys/f_File.h>
#include <ctype/c_Types.h>
#include <fsys/f_FileAnnotationConfig.h>
#include <logSimple/p_Logs.h>
#include <platform/p_Exception.h>

//#define _errorLog p_Logs::errLog.getErrorStream() 
//#define _debugLog(x) p_Logs::errLog.getStream(x)
#define __SPOOL_PREFIX__ "spool: " << __NOW__ 
#define _THIS_STATE(x) "   ThisState: " << spoolState(x) 
#define _NEXT_STATE(x) "   NextState: " << spoolState(x) 

//int summary_SpoolSuccess = 0;  IN CPP file
//int summary_failed = 0;

#define MAX_BUFSIZE 2048	 // defined where?

//////////////////////////////////////////////////
typedef enum SpoolState_tag {
	Spool = 1,
	Pending = 2,
	Processed = 3,
	Suspense = 4,
	Backup = 5,
	Error =6,
	NoChange
} SpoolState;


#define SPOOL_STATE "State=(Spool)"
#define PENDING_STATE "State=(Pending)"
#define PROCESSED_STATE "State=(Processed)"
#define SUSPENSE_STATE "State=(Suspense)"
#define ERROR_STATE "State=(Error)"
#define BACKUP_STATE "State=(Backup)"
#define UNDEF_STATE "State=(????)"


class f_Spooler  
{
public:
	f_Spooler();
	virtual ~f_Spooler();

	struct SpoolConfig 
	{
		void set(
			SpoolState dstate, 
			string ddir,
			string dext, 
			string dstamp, 
			string dmask,
			string dannotation = "")
		{
			this->clear();
			this->state = dstate;
			this->dir.assign(ddir);
			this->ext.assign(dext);
			this->stamp_mask.assign(dstamp);
			this->dir_mask.assign(dmask);
			this->annotation.assign(dannotation);
		}
		void clear()
		{
			this->dir.erase();
			this->ext.erase();
			this->annotation.erase();
			this->stamp_mask.erase();
			this->dir_mask.erase();
			this->state = NoChange;
		}

		SpoolState state;
		string dir;
		string ext;
		string stamp_mask;
		string dir_mask;
		string annotation;
	};


	//--- variables -------------
	map<SpoolState,SpoolConfig> m_spool_configs;
	ErrorRecord errorRec;
	f_FileAnnotationConfig annotation_config;


	//--entry functions----
	virtual Return_code runSpooler(void);  //start the process
	Return_code loadSpoolerProperties(const string config_file);

	//--base functions----
	virtual SpoolState doProcessOneFile(f_FileName fname);		// 'move' from pending to spool

	virtual Return_code processDirectory(SpoolState which_state);	// read spool dir and do work

	Return_code 
	processDirectory(
			SpoolState state,
			f_Dir* dirp, string dirstr, bool sub_dirs );


	Return_code doMoveOneFile(f_FileName fname, SpoolState state, bool required = true);
	Return_code doCopyOneFile(f_FileName fname, SpoolState state, bool required = true);

	//--helper functions----
	Return_code moveFile(
			f_FileName fname,
			string dest_directory,
			string dest_extension,
			string datestampmask,
			string dirmask,
			string annotation ); // leading record marker

	Return_code copyFile(
			f_FileName fname,
			string dest_directory,
			string dest_extension,
			string datestampmask,
			string dirmask,
			string annotation ); // leading record marker




	//--configuration----
	SpoolConfig * getSpoolConfig(SpoolState which_state);
	void setSpoolConfig( SpoolConfig conf );
	void f_Spooler::setSpoolConfig(
				SpoolState dstate, 
				string ddir, string dext, 
				string dstamp, string dmask , string annotate = "");

	//--debugging----
	void dumpConfigs(ostream& out);

		///
	const char * spoolState( SpoolState c ) 
	{
		switch (c)
		{
		case Spool: 	return SPOOL_STATE; 
		case Pending: 	return  PENDING_STATE; 
		case Processed:return  PROCESSED_STATE; 
		case Suspense: return  SUSPENSE_STATE; 
		case Backup: return  BACKUP_STATE; 
		case Error: return  ERROR_STATE; 
		case NoChange:
		default:
				return  UNDEF_STATE; 
		}
	};

	friend ostream& operator<<( ostream& of, SpoolState& c ) 
	{
		switch (c)
		{
		case Spool : 
			of << SPOOL_STATE; break;
		case Pending :
			of << PENDING_STATE; break;
		case Processed :
			of << PROCESSED_STATE; break;
		case Suspense :
			of << SUSPENSE_STATE; break;
		case Backup :
			of << BACKUP_STATE; break;
		case Error :
			of << ERROR_STATE; break;
		case NoChange:
		default:
				of << UNDEF_STATE; break;
		}
		return of;
	};

	void 
	f_Spooler::dumpDirectory(
			f_Dir* dirp, 
			string dirstr,
			ostream& out );



};

#endif // !defined(AFX_F_SPOOLER_H__95112E8A_E5CC_48D1_BFAF_7DA568C7DD1B__INCLUDED_)
