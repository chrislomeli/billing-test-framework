#ifndef __P_COMMON__H__
#define __P_COMMON__H__

const char * root_name(const char * filename);

#define __NOW__ c_DateTime(time(NULL)) 
#define __OHERE__  root_name(__FILE__) << ":" << __LINE__ 
#define __ONOWHERE__ __NOW__ << " " << __OHERE__ 

#define __HERE__  root_name(__FILE__), __LINE__ 

#define __ERR_TAG__ "~E~"
#define MAX_ERRLINE 255

typedef enum struc_loglevel {
	logReport	=0,
	logError	=1,
	logWarning	=2,
	logTrace1	=3,
	logTrace2	=4,
	logTrace	=5,
	logTrace3	=5
} Loglevel;



#endif
