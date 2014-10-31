#include "tcp_Session.h"

tcp_Session::tcp_Session(void)
: fd(0)
{
#ifdef WIN32
	WSADATA wsData;
	WORD wVersion = MAKEWORD(2,2);
	int err = WSAStartup(wVersion,&wsData);

	if (err != 0)
		return;
#endif

}

tcp_Session::~tcp_Session(void)
{
#ifdef WIN32
	WSACleanup();
#endif
	if (fd > 0)
		close(fd);
}

/**********************************************
 * Getaddrinfo
 * Pre: host and port name
 * Post: Active listening socket; or die
 **********************************************/
Return_code 
tcp_Session::Getaddrinfo(
	struct sockaddr_in* Address, 
	const char *Hostname, 
	const char *Hostport )
{
	/* declares */
	struct servent  *srvptr;
	struct hostent	*hostptr;
	struct in_addr	inaddr;
	int Port=0;

	/* keep it simple -  we know our family */
	Address->sin_family = AF_INET;

    /* first resolve the port info */
    if (( Port = atoi(Hostport)) >  0 )
    {
		//messageLog(logTrace,"service is numeric,  Assume literal port...");
        Address->sin_port=htons(Port);
    }
    else if((srvptr = getservbyname(Hostport, "tcp")) != 0 )
    {
		//messageLog(logTrace,"service is non-numeric,  resolve service ...");
        Address->sin_port = srvptr->s_port;
    }
	else {
		return ServerNotFound;
	}

	/* try for ip4 dotted decimal address resolution */
	if ( isdigit((int)*Hostname) && inet_pton(AF_INET, Hostname, &inaddr) == 1 ) 
	{
		//messageLog(logTrace,"Host is a dotted decimal...");
#ifdef WIN32
        Address->sin_addr.s_addr = inet_addr(Hostname);
#else
        memcpy(&Address->sin_addr, &inaddr, sizeof(struct in_addr));
#endif
	} 
	else 
	{
#ifndef WIN32_FUCK_WINDOWS
		if ((hostptr = gethostbyname(Hostname)) != NULL )
		{
			//messageLog(logTrace,"resolve host by name...");
			memcpy(&Address->sin_addr, hostptr->h_addr, hostptr->h_length);

		} else 
		{ /* error */
			switch (h_errno) {
			case HOST_NOT_FOUND:
			case TRY_AGAIN:
			default:
				break;
			}
			return ServerNotFound;
		}
#else
		struct addrinfo addr_in;
		struct addrinfo * addr_out = NULL;

		addr_in.ai_socktype = SOCK_STREAM;
		addr_in.ai_family = AF_INET;
		addr_in.ai_protocol = IPPROTO_TCP;

		if ( getaddrinfo(Hostname,Hostport, &addr_in, &addr_out ) != 0 ) {
				cout << "Fuck: " << WSAGetLastError();
				return ServerNotFound;
		}

#endif

	}
	return Success;
}


/*=========================================================================
 * connect_nonb 
 * Pre:  
 * Post: open connection or die 
 *========================================*/
Return_code
tcp_Session::Connect_nonb(
		int sockfd, sockaddr *saptr, socklen_t salen, int timeout)
{
	int		bflags=0, flags=0, n=0, error=0;
	void*		vp = NULL;
	fd_set		rset, wset;
	struct timeval	ts;
	struct timeval*	tvalp = NULL;


	//--- connect no block-------------------------
#ifndef WIN32
	unsigned int len= sizeof(error);
#else
	int e=0;
	unsigned long on = -1;
#endif

	//--- time the connection------------------
	memset(&ts,0,sizeof(ts));
	if ( timeout >= 0 ) {
		ts.tv_sec = timeout;
		tvalp = &ts;
	}
	// clear descriptor sets
	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;

	// set non blocking
#ifndef WIN32
	if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0 )
		return CanNotSetSocket;
	if ( fcntl(sockfd, F_SETFL, flags | O_NONBLOCK ) < 0 )
		return CanNotSetSocket;
#else
	if ( ioctlsocket(sockfd, FIONBIO, &on ) != 0 )
		return CanNotSetSocket;
#endif

	if ( (n = connect(sockfd, (sockaddr *) saptr, salen)) < 0)
	{	
#ifndef WIN32
		if (errno != EINPROGRESS)
			return ServerUnavailable;
#else
		e=WSAGetLastError();
		switch (e) {
			case WSAEINPROGRESS:
			case WSAEWOULDBLOCK:
				break;
			default:
				break;
		}
		return ServerUnavailable;
#endif
	}
		
	//--- Select the connection------------------
	if ( (n = select(sockfd+1, &rset, &wset, NULL, tvalp )) < 0 ) 
		return ServerUnavailable;
	else if ( n == 0 )
		return ServerTimedOut;


	//--- Check settings ------------------
	if (!FD_ISSET(sockfd, &rset) && !FD_ISSET(sockfd, &wset)) 
		return BadSocket;


	//--- Positve return------------------
#ifndef WIN32
	if ( getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&error, (int*)&len) < 0 || error )
		return BadSocket;
#endif

	//--- cleanup and return------------------------------
#ifndef WIN32
	if ( fcntl(sockfd, F_SETFL, flags) < 0 )
		return CanNotSetSocket;
#else
	on = 0;
	if ( ioctlsocket(sockfd, FIONBIO, &on ) != 0 )
		return CanNotSetSocket;
#endif

	return Success;
}

/*=========================================================================
 * connect_nonb 
 * Pre:  
 * Post: open connection or die 
 *========================================*/
Return_code
tcp_Session::Connect_block( sockaddr *saptr, socklen_t salen )
{
	int		bflags=0, flags=0, n=0, error=0;
	void*		vp = NULL;
	fd_set		rset, wset;
	struct timeval	ts;
	struct timeval*	tvalp = NULL;


#ifndef WIN32
        unsigned int len= sizeof(error);
#else
        int e=0;
        unsigned long on = -1;
#endif

	if ( (n = connect(this->fd, (sockaddr *) saptr, salen)) < 0) {
		return ServerUnavailable;	
	}
		
	//--- Positve return------------------
#ifndef WIN32
	if ( getsockopt(this->fd, SOL_SOCKET, SO_ERROR, (char*)&error, (int*)&len) < 0 || error )
		return BadSocket;
#endif

	//--- cleanup and return------------------------------
	return Success;
}

/**********************************************
 * Tcp_connect
 * Pre: host and port name
 * Post: Active connecting socket; or die
 **********************************************/
Return_code
tcp_Session::Connect( const char *host, const char *serv, int timeo )
{
	struct sockaddr_in Address;
	Return_code rc = Undefined;

	//--posix.1g socket info---

	//--get network address or die----
	if ( Getaddrinfo( &Address, host, serv ) != Success ) 
	{
		return ServerNotFound;
	}

	//-- open socket --
	if (( this->fd = (int)socket( AF_INET, SOCK_STREAM, IPPROTO_TCP )) < 0 ) 
	{
		return CannotCreateSocket;
	}

	//-- connect to socket ---
	if ((rc=Connect_block( (sockaddr*)&Address, sizeof(Address) )) != Success )
	{
		return rc;
	}
	return Success;
}


