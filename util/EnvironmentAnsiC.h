#ifndef ENVIRONMENTANSIC_H
#define ENVIRONMENTANSIC_H

//Gives ANSI version of standard includes
#if _MSC_VER > 1000
#pragma once
#pragma warning (disable : 4786 4005)
#endif // _MSC_VER > 1000

/* win32 diffs */
#if defined(WIN32)
    #define syslog(d,c) printf("%d: %s\n", d, c )
	#define MKDIR(a,b)	mkdir(a) 
    #define kill(x,y) exit(1)
    #define pid_t int
    #define vsnprintf _vsnprintf
    #define snprintf _snprintf
    #include        <process.h>             /* for syslog() */
#else
	#define			MKDIR(a,b)	mkdir(a,b) 

	#include        <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
	#include        <sys/time.h>    /* timeval{} for select() */
	#include        <sys/socket.h>  /* basic socket definitions */
    #include        <syslog.h>              /* for syslog() */
    #include        <unistd.h>              /* for syslog() */
	#include        <arpa/inet.h>   /* inet(3) functions */
	#include        <sys/un.h>              /* for Unix domain sockets */
	#include        <netdb.h>
	#include        <sys/uio.h>             /* for iovec{} and readv/writev */
	#include        <unistd.h>
	#include        <sys/wait.h>
#endif

#include	<sys/types.h>   /* basic system data types */
#include    <sys/stat.h>    /* for S_xxx file mode constants */
#include	<time.h>                /* timespec{} for pselect() */
#include	<errno.h>
#include	<fcntl.h>               /* for nonblocking */
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<limits.h>
#include	<ctype.h>
#include	<memory.h>


#endif //ENVIRONMENTANSIC_H

