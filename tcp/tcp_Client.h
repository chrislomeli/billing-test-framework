#ifndef C__tcp_Client__H
#define C__tcp_Client__H

#include <sys/socket.h> 
#include <util/EnvironmentStd.h>
#include "tcp_Session.h"
#ifdef WIN32
#include <io.h>
#endif

class tcp_Client : public tcp_Session
{
public:
	tcp_Client(void);
	~tcp_Client(void);


	bool is_connected(void);

	Return_code Read_buffer();
	size_t Write_buffer(const char * linetowrite);

	ostringstream returnStream;
	char buffer[1024];
	size_t buff_max;
	time_t timeo;

	FILE *sockout_fp;
	FILE *sockin_fp;
	bool initStreams(void);
	string errString;
};

#endif
