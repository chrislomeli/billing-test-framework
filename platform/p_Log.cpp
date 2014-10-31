//
#include <stdarg.h>
#include "common/p_Log.h"


/* true globals - all threads */
FILE *  fp_err = NULL;
FILE *  fp_rpt = NULL;
int     gDaemon_proc;           /* set nonzero by daemon_init() */
Bool    gLog_ready;
Loglevel        gLevelSetting = logWarning;


/*==================================================
 * Caller sets the logel level 
 ==================================================*/
void 
p_Log::setLogLevel ( Loglevel lv )
{
	gLevelSetting = lv;
}

/*==================================================
 * low level log function.
 * Caller specifies "errnoflag" and "level". 
 ==================================================*/
void 
p_Log::log(FILE *fp, char ecode, const char* file, int line, const char *fmt, va_list ap)
{
	int		n;
	char	buf[MAX_ERRLINE];
	time_t ltime;
	struct tm* thistime;

	//--datetime--
	time(&ltime);
	thistime = localtime( &ltime ); /* Convert to local time. */

	//--put args on the buffer--
	vsnprintf(buf, sizeof(buf), fmt, ap);	/* this is safe */

	//--add file line info---
	n = strlen(buf);
	if (line>0)
		snprintf(buf+n, sizeof(buf)-n, ": %s(%d)", file, line);

	//--write--
	fprintf(fp,"%c: date(%02d.%02d %02d:%02d): %s\n", 
		ecode, 
		thistime->tm_mon+1,
		thistime->tm_mday,
		thistime->tm_hour,
		thistime->tm_min,
		buf );

	fflush(fp);
	return;
}


/*==================================================
 * log wrapper a message and return to caller.
 * Post: returns the same error code passed in
 ==================================================*/
void p_Log::closeLogs ()
{
	if (fp_rpt != NULL && fp_rpt  != stdout && fp_rpt != stderr )
		fclose(fp_rpt);

	if (fp_err != NULL && fp_err  != stdout && fp_err != stderr )
		fclose(fp_err);

	fp_rpt=NULL;
	fp_err=NULL;
	gLog_ready = False;
}

/*==================================================
 * abortLog
 * Pre:  logging message or some system error
 * Post: output to a default log and 
 *    send interrupt to ourselves  
 ==================================================*/
Return_code
p_Log::abortLog (const char* file, int line, const char * fmt, ...)
{
	int syslevel = 1;
	FILE * fp;
	va_list	ap;

	//-- try to open anything---
	if ((fp=fopen(ABORT_FILE,"a")) == NULL )
		fp = stderr;

	//-- send to log ---
	va_start(ap, fmt);

	if (gDaemon_proc) 
	{
		char buf[MAX_ERRLINE];

		vsnprintf(buf, sizeof(buf), fmt, ap);
		syslog(syslevel, buf);
	}
	else
		log(fp, 'A', file, line, fmt, ap);

	va_end(ap);
	kill(getpid(),SIGINT);

	return SysFailure;
}

/*==================================================
 * log wrapper a message and return to caller.
 * Post: returns the same error code passed in
 ==================================================*/
Return_code
p_Log::errorLog (Return_code rc, Loglevel lv , const char* file, int line, const char * fmt, ...)
{
	char c;
	va_list		ap;

	//--check our level--
	if ( gLevelSetting < lv && lv != logReport)
		return rc;

	//--check our streams--
	if (!gLog_ready)
		initLogs(stderr, stdout);

	//--- determine presentation code --
	switch( lv ) {
	case logAbort:
	case logError:
		c = 'E';
		break;
	case logWarning:
		c = 'W';
		break;
	case logTrace:
	case logDebug:
	default:
		c = 'D';
		break;
	}

	//-- send to log ---
	va_start(ap, fmt);

	log(fp_err, c, file, line, fmt, ap);
	if ((fp_err != fp_rpt ) && (lv == logError || lv == logReport))
		log(fp_rpt, c, file, line, fmt, ap);

	va_end(ap);


	//--- if this is an Abort - send ourselves an interrupt ---
	if ( gLevelSetting == logAbort )
		kill(getpid(),SIGINT);

	return rc;
}


/*==================================================
 * log wrapper a message and return to caller.
 * Post: returns the same error code passed in
 ==================================================*/
Return_code 
p_Log::messageLog (Loglevel lv, const char * fmt, ...)
{
	va_list		ap;
	char c;
	const char *empty_string = "";

	//--check our level--
	if ( gLevelSetting < lv && lv != logReport)
		return Success;

	//--check our streams--
	if (!gLog_ready)
		initLogs(stderr, stdout);

        //--- determine presentation code --
        switch( lv ) {
        case logAbort:
        case logError:
                c = 'E';
                break;
        case logWarning:
                c = 'W';
                break;
        case logTrace:
        case logDebug:
        default:
                c = 'D';
                break;
        }


	//-- send to log ---
	va_start(ap, fmt);

	log(fp_err, c, empty_string, 0, fmt, ap);
	if ((fp_err != fp_rpt ) && (lv == logError || lv == logReport))
		log(fp_rpt, c, empty_string, 0, fmt, ap);

	va_end(ap);

	return Success;
}

/*==================================================
 * initLogs
 * Pre: pass in open file pointers
 * Post: set them on our global pointers
 ==================================================*/
Return_code
p_Log::initLogs (FILE *err, FILE * trace)
{
	//-- declares --
	time_t ltime;
	struct tm* thistime;

	//--sanity on file handles--
	if ( err == NULL ||  (gLevelSetting >= logTrace && trace == NULL) )
		return abortLog(_HERE_,"received null file handles" );

	//--datetime--
	time(&ltime);
    	thistime = localtime( &ltime ); /* Convert to local time. */

	//--check our log stream--
	if ( fprintf(err, "%d: Initialize logging...\n", (int)getpid())  <= 0 )
		return abortLog( _HERE_,"bad err file handle " );

	//--check our trace stream--
	if ( gLevelSetting >= logTrace ) 
	{
		if ( fprintf(trace, "%d: Initialize trace...\n", (int)getpid() )  <= 0 )
			return abortLog( _HERE_,"bad trace file handle " );
	}

	//--assign to global pointers --
	fp_err = err;
	fp_rpt = trace;

	gLog_ready = True;
	return Success;
}

/*==================================================
 * openLogs
 * Pre: pass in filenames
 * Post: open the files (or defaults) and pass to init 
 ==================================================*/
Return_code
p_Log::openLogs ( const char * errfile_name, const char *reportfile_name )
{
	FILE *ferr = NULL;
	FILE *ftrace = NULL;

	closeLogs ();
	
	if ( errfile_name == NULL )
		ferr = stdout;
	else if ( strlen(errfile_name) <= 0 )
		ferr = stdout;
	if ( strcmp(errfile_name,"stdout") == 0 )
		ferr = stdout;
	else if ( strcmp(errfile_name,"stderr") == 0 )
		ferr = stderr;
	else if ( (ferr=fopen(errfile_name,"a")) == NULL )
			return abortLog( _HERE_,"Can not open %s", errfile_name );

	if ( reportfile_name == NULL )
		ftrace = ferr;
	else if ( strlen(reportfile_name) <= 0 )
		ferr = stdout;
	else if ( strcmp(errfile_name,reportfile_name) == 0 )
		ftrace = ferr;
	else if ( strcmp(reportfile_name,"stdout") == 0 )
		ftrace = stdout;
	else if ( strcmp(reportfile_name,"stderr") == 0 )
		ftrace = stderr;
	else if ( (ftrace=fopen(reportfile_name,"a")) == NULL )
			return abortLog( _HERE_,"Can not open %s", reportfile_name );

	return ( initLogs(ferr,ftrace));

}

/*==================================================
 * low level log function.
 ==================================================*/
inline char * 
catstr(char* buf, int buflen, const char *fmt, va_list ap)
{

	//--put args on the buffer--
	vsnprintf(buf, buflen, fmt, ap);	/* this is safe */
	return buf;
}

