#ifndef C__tcp_Session__H
#define C__tcp_Session__H


#include <sys/socket.h> 
#include <util/EnvironmentStd.h>
#include <util/EnvironmentSocket.h>
#include <util/ErrorCode.h>
#include "tcp_Session.h"
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#else
#include        <sys/types.h>   /* basic system data types */
#include        <sys/socket.h>  /* basic socket definitions */
#include        <sys/time.h>    /* timeval{} for select() */
#include        <time.h>                /* timespec{} for pselect() */
#include        <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include        <arpa/inet.h>   /* inet(3) functions */
#include        <errno.h>
#include        <fcntl.h>               /* for nonblocking */
#include        <netdb.h>
#include        <signal.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <sys/stat.h>    /* for S_xxx file mode constants */
#include        <sys/uio.h>             /* for iovec{} and readv/writev */
#include        <unistd.h>
#include        <sys/wait.h>
#include        <sys/un.h>              /* for Unix domain sockets */
#endif


int	inet_pton(int family, const char *strptr, void *addrptr);

class tcp_Session
{
public:
	tcp_Session(void);
	~tcp_Session(void);

	virtual Return_code Connect( const char *host, const char *serv, int timeo );

	Return_code Connect_block( sockaddr *saptr, socklen_t salen );

		Return_code Connect_nonb(
			int sockfd, sockaddr *saptr, socklen_t salen, int timeout);

	Return_code Getaddrinfo(
		struct sockaddr_in* Address, 
		const char *Hostname, 
		const char *Hostport );
		int fd;
};
#endif
