#ifndef PORTABILITY_UTILITY_MAP_H
#define PORTABILITY_UTILITY_MAP_H


#if defined(WIN32)

	#include <fcntl.h>
	#include <sys/stat.h>
	#include <io.h>
	#include <process.h>
	#include <direct.h>
	#include <limits.h>

	#define SLASH "\\\\"
	#define PATH_MAX 4029
	#define fMASK _S_IWRITE
	#define getpid	_getpid
	#define F_EXISTS 00
	#define F_WRITE 02
	#define F_READ 04
	#define mkdir(x,y) mkdir(x)
    #define ENV_USER "USERNAME"
#else
	#include <stdio.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <inttypes.h>
	#include "dirent.h"
	#include <limits.h>
	#define SLASH "/"
        #define F_EXISTS F_OK
        #define F_WRITE W_OK
        #define F_READ R_OK
        #define F_EXECUTE X_OK

	#define fMASK 00777
	#define ENV_USER "USER"

#endif

inline const char * getuser(const char * x) 
{	
	char * usr__name = getenv(x); 
	if (usr__name) 
		return usr__name;
	else
		return "";
};



#endif //UTILITY_H

