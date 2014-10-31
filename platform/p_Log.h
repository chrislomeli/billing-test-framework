
#if !defined(CERROR_H)
#define CERROR_H

#include	<stdio.h>		/* ANSI C header file */
#include	<stdarg.h>		/* ANSI C header file */
#include	<string.h>		/* for syslog() */
#include	<time.h>		/* for syslog() */

#define   MAX_ERRLINE         4096

/* win32 diffs */
#if defined(WIN32)
	#define syslog(d,c) printf("%d: %s\n", d, c )
	#define kill(x,y) exit(1)
	#define pid_t int
	#define	vsnprintf _vsnprintf
	#define	snprintf _snprintf
	#include	<process.h>		/* for syslog() */
#else
	#include	<syslog.h>		/* for syslog() */
	#include	<unistd.h>		/* for syslog() */
#endif

/* cpluspls difs */
#if !defined(__cplusplus)
	#define True 1
	#define False (!True)
	#define Bool int
#else
	#define True true
	#define False false
	#define Bool bool
#endif

typedef enum struc_return {
	Success		= 1,
	Failure		= -1,
	SysFailure	= -2,
	SysMemory	= -3,
	AppConfig	= -4,
	IOFailure	= -5,
	ParseFailure	= -6,
	PinError	= -7,
	DuplicateRec= -8,
	NoDataFound	= -9,
	Ignore		= -10
} Return_code;

typedef enum struc_loglevel {
	logAbort	=0,
	logError	=1,
	logWarning	=2,
	logReport	=3,
	logTrace	=4,
	logDebug	=5
} Loglevel;

/* defines for error processing */
#define ABORT_FILE "default.log"
#define isSuccess(code) (((code == Success) ?	1 : 0 ))
#define isFailure(code) (((code != Success) ?	1 : 0 ))
#define Suceeded isSuccess
#define Failed isFailure
#define _HERE_ __FILE__,__LINE__


/*==========================
 * internal routines
 *==========================*/
class p_Log {
public:

	void log(
			FILE *fp, 
			char ecode, 
			const char* file, 
			int line, 
			const char *fmt, 
			va_list ap);

	void closeLogs ();
	void setLogLevel ( Loglevel lv );

	Return_code abortLog (
		const char* file, int line, const char * fmt, ...);

	Return_code errorLog (
		Return_code rc, Loglevel lv , const char* file, int line, const char * fmt, ...);

	Return_code messageLog (Loglevel lv, const char *, ...);
	Return_code initLogs (FILE *err, FILE * trace);
	Return_code openLogs (const char * errfile_name, const char *tracefile_name);
};

#endif

