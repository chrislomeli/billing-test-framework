#ifndef __INCLUDE__bs_PostpSession_HH__
#define __INCLUDE__bs_PostpSession_HH__

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <util/EnvironmentStd.h>    
#include <ctype/c_NvRecord.h>    

#include <jrpc.h>
#include <bs_signupd.h>
#include <simple_callobj.h>
#include "common/globdefs.h"


#define	CALL_TIMEOUT 90
#define	POSTP_VERSION_INDEX	 24

#define INVALID_STR "INVALID"
#define CONFIG_ERROR -998
#define CONNECT_ERROR -997

#define VERS_2_0 "v2.0"
#define VERS_2_1 "v2.1"
#define VERS_2_2 "v2.2"
#define VERS_3_0 "v3.0"
#define VERS_4_0 "v4.0"
#define VERS_5_0 "v5.0"
#define VERS_5_5 "v5.5"
#define VERS_7_0S "v7.0"
#define VERS_7_S "v7.0s"
#define VERS_7_T "v7.0t"
#define VERS_8_S "v8.0s"
#define VERS_8_T "v8.0t"

typedef enum post_version_t {
	v_undef = -1,
    v1_0 = 0,
    v2_0 = 1,
    v2_1 = 2,
    v2_2 = 3,
    v3_0 = 4,
    v4_0 = 5,
    v5_0 = 6,
    v5_5 = 7,
    v7_S = 8,
    v7_T = 9,
    v8_S = 10,
    v8_T = 11
} PostpVersion;

class bs_PostpSession
{
public:
	bs_PostpSession(void);
	~bs_PostpSession(void);
	PostpVersion getVersionNumber(string str_version);
	void setConnectInfo(string _host, int _port, string _program_name, string log_file = "/dev/null");
	int callBsSignup(c_NvRecord& nvrec, string version);
	string program_name;
	string host;
	int port;
};
#endif


