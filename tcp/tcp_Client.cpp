#include "tcp_Client.h"

tcp_Client::tcp_Client(void)
: sockout_fp(NULL)
, sockin_fp(NULL)
, buff_max(1024)
, timeo(90)
{
}

tcp_Client::~tcp_Client(void)
{
}


bool tcp_Client::is_connected(void)
{
	return false;
}


/*==============================================
  Write "n" bytes to a file descriptor
 ==============================================*/
size_t
tcp_Client::Write_buffer(const char * linetowrite)
{
	if (linetowrite == NULL)
		return 0;

	size_t		nleft = strlen(linetowrite);
	size_t		nwritten = 0;
	const char	*ptr = linetowrite;

	nwritten = send(this->fd, ptr, (int)nleft,0);

	return(nwritten);
}


Return_code
tcp_Client::Read_buffer()
{
    int n; 
    int max_retry=2; 
    int trys=0;
    fd_set  fds;
    struct timeval tv;

	returnStream.flush();
again:
	memset(this->buffer,0,this->buff_max);

	if ( timeo != NULL && timeo > 0 )
	{
            //-- select the fd just to maintain control----
            FD_ZERO (&fds);
            FD_SET (this->fd, &fds);
            tv.tv_sec  =  (long)(this->timeo);
            tv.tv_usec = 0;

            if ((n = select (this->fd+1, &fds, NULL, NULL,  &tv)) < 0 )
            {
		return BadSocket;
            }
            else if ( FD_ISSET(fd, &fds) == 0 )
            {
		return ServerTimedOut;
            }
	}  //timeo

	trys++;
	while (( n = recv(this->fd, this->buffer, (int)this->buff_max, 0)) > 0 ) {
		returnStream << this->buffer;
	}
	
	if (n == 0)
	{
		if ( trys < max_retry )
			goto again;

		return NoMoreInput;
	}
	else if ( n < 0 )
	{
		if (errno == EINTR) 
		{
			goto again;
		}
		return IOReadFailed;
	} 
	return Success;
}



bool tcp_Client::initStreams(void)
{
	this->sockin_fp = fdopen(this->fd,"r");
	this->sockout_fp = fdopen(this->fd,"w");
	return ( this->sockin_fp != NULL && this->sockout_fp != NULL);
	return false;
}
