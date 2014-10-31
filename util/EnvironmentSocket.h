/* include unph */
/* Our own header.  Tabs are set for 4 spaces, not 8 */

#ifndef	__unp_h
#define	__unp_h

#include "config.unix.h"


/* ***********************************************************************
 *  NORMAL DEFINES NOW
 */



/* If anything changes in the following list of #includes, must change
   acsite.m4 also, for configure's tests. */

#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */

#ifdef	HAVE_SYS_SELECT_H
# include	<sys/select.h>	/* for convenience */
#endif

#ifdef	HAVE_POLL_H
# include	<poll.h>		/* for convenience */
#endif

#ifdef	HAVE_STRINGS_H
# include	<strings.h>		/* for convenience */
#endif

/* Three headers are normally needed for socket/file ioctl's:
 * <sys/ioctl.h>, <sys/filio.h>, and <sys/sockio.h>.
 */
#ifdef	HAVE_SYS_IOCTL_H
# include	<sys/ioctl.h>
#endif
#ifdef	HAVE_SYS_FILIO_H
# include	<sys/filio.h>
#endif
#ifdef	HAVE_SYS_SOCKIO_H
# include	<sys/sockio.h>
#endif

#ifdef	HAVE_PTHREAD_H
# include	<pthread.h>
#endif

/* OSF/1 actually disables recv() and send() in <sys/socket.h> */
#ifdef	__osf__
#undef	recv
#undef	send
#define	recv(a,b,c,d)	recvfrom(a,b,c,d,0,0)
#define	send(a,b,c,d)	sendto(a,b,c,d,0,0)
#endif

#ifndef	INADDR_NONE
/* $$.Ic INADDR_NONE$$ */
#define	INADDR_NONE	0xffffffff	/* should have been in <netinet/in.h> */
#endif

#ifndef	SHUT_RD				/* these three Posix.1g names are quite new */
#define	SHUT_RD		0	/* shutdown for reading */
#define	SHUT_WR		1	/* shutdown for writing */
#define	SHUT_RDWR	2	/* shutdown for reading and writing */
/* $$.Ic SHUT_RD$$ */
/* $$.Ic SHUT_WR$$ */
/* $$.Ic SHUT_RDWR$$ */
#endif

/* *INDENT-OFF* */
#ifndef INET_ADDRSTRLEN
/* $$.Ic INET_ADDRSTRLEN$$ */
#define	INET_ADDRSTRLEN		16	/* "ddd.ddd.ddd.ddd\0"
								    1234567890123456 */
#endif

/* Define following even if IPv6 not supported, so we can always allocate
   an adequately-sized buffer, without #ifdefs in the code. */
#ifndef INET6_ADDRSTRLEN
/* $$.Ic INET6_ADDRSTRLEN$$ */
#define	INET6_ADDRSTRLEN	46	/* max size of IPv6 address string:
				   "xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx" or
				   "xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:ddd.ddd.ddd.ddd\0"
				    1234567890123456789012345678901234567890123456 */
#endif
/* *INDENT-ON* */

/* Define bzero() as a macro if it's not in standard C library. */
#ifndef	HAVE_BZERO
#define	bzero(ptr,n)		memset(ptr, 0, n)
/* $$.If bzero$$ */
/* $$.If memset$$ */
#endif

/* Older resolvers do not have gethostbyname2() */
#ifndef	HAVE_GETHOSTBYNAME2
#define	gethostbyname2(host,family)		gethostbyname((host))
#endif

/* The structure returned by recvfrom_flags() */
struct in_pktinfo {
  struct in_addr	ipi_addr;	/* dst IPv4 address */
  int				ipi_ifindex;/* received interface index */
};
/* $$.It in_pktinfo$$ */
/* $$.Ib ipi_addr$$ */
/* $$.Ib ipi_ifindex$$ */

/* We need the newer CMSG_LEN() and CMSG_SPACE() macros, but few
   implementations support them today.  These two macros really need
    an ALIGN() macro, but each implementation does this differently. */
#ifndef	CMSG_LEN
/* $$.Ic CMSG_LEN$$ */
#define	CMSG_LEN(size)		(sizeof(struct cmsghdr) + (size))
#endif
#ifndef	CMSG_SPACE
/* $$.Ic CMSG_SPACE$$ */
#define	CMSG_SPACE(size)	(sizeof(struct cmsghdr) + (size))
#endif

/* Posix.1g requires the SUN_LEN() macro but not all implementations DefinE
   it (yet).  Note that this 4.4BSD macro works regardless whether there is
   a length field or not. */
#ifndef	SUN_LEN
/* $$.Im SUN_LEN$$ */
# define	SUN_LEN(su) \
	(sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
#endif

/* Posix.1g renames "Unix domain" as "local IPC".
   But not all systems DefinE AF_LOCAL and AF_LOCAL (yet). */
#ifndef	AF_LOCAL
#define AF_LOCAL	AF_UNIX
#endif
#ifndef	PF_LOCAL
#define PF_LOCAL	PF_UNIX
#endif

/* Posix.1g requires that an #include of <poll.h> DefinE INFTIM, but many
   systems still DefinE it in <sys/stropts.h>.  We don't want to include
   all the streams stuff if it's not needed, so we just DefinE INFTIM here.
   This is the standard value, but there's no guarantee it is -1. */
#ifndef INFTIM
#define INFTIM          (-1)    /* infinite poll timeout */
#ifdef	HAVE_POLL_H
#define	INFTIM_UNPH				/* tell unpxti.h we defined it */
#endif
#endif

/* Following could be derived from SOMAXCONN in <sys/socket.h>, but many
   kernels still #define it as 5, while actually supporting many more */
#define	LISTENQ		1024	/* 2nd argument to listen() */

/* Miscellaneous constants */
#undef MAXLINE
#define	MAXLINE		4096	/* max text line length */
#define	MAXSOCKADDR  128	/* max socket address structure size */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */

/* Define some port number that can be used for client-servers */
#define	SERV_PORT	 9877			/* TCP and UDP client-servers */
#define	SERV_PORT_STR	"9877"			/* TCP and UDP client-servers */
#define	UNIXSTR_PATH	"/tmp/unix.str"	/* Unix domain stream cli-serv */
#define	UNIXDG_PATH	"/tmp/unix.dg"	/* Unix domain datagram cli-serv */

/* Following shortens all the type casts of pointer arguments */
#define	SA	struct sockaddr

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
					/* default file access permissions for new files */
#define	DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
					/* default permissions for new directories */

typedef	void	Sigfunc(int);	/* for signal handlers */

#define	min(a,b)	((a) < (b) ? (a) : (b))
#define	max(a,b)	((a) > (b) ? (a) : (b))

#ifndef	HAVE_ADDRINFO_STRUCT
# include	"addrinfo.h"
#endif

#ifndef	HAVE_IF_NAMEINDEX_STRUCT
struct if_nameindex {
  unsigned int   if_index;  /* 1, 2, ... */
  char          *if_name;   /* null terminated name: "le0", ... */
};
/* $$.It if_nameindex$$ */
/* $$.Ib if_index$$ */
/* $$.Ib if_name$$ */
#endif

#ifndef	HAVE_TIMESPEC_STRUCT
struct timespec {
  time_t	tv_sec;		/* seconds */
  long		tv_nsec;	/* and nanoseconds */
};
/* $$.It timespec$$ */
/* $$.Ib tv_sec$$ */
/* $$.Ib tv_nsec$$ */
#endif
/* end unph */

#ifndef	HAVE_GETADDRINFO_PROTO
int	 getaddrinfo(const char *, const char *, const struct addrinfo *,
					 struct addrinfo **);
void	 freeaddrinfo(struct addrinfo *);
char	*gai_strerror(int);
#endif

#ifndef	HAVE_GETNAMEINFO_PROTO
int		 getnameinfo(const SA *, socklen_t, char *, size_t, char *, size_t, int);
#endif

#ifndef	HAVE_GETHOSTNAME_PROTO
int		 gethostname(char *, int);
#endif

#ifndef	HAVE_HSTRERROR_PROTO
const char	*hstrerror(int);
#endif

#ifndef	HAVE_IF_NAMETOINDEX_PROTO
unsigned int	 if_nametoindex(const char *);
char			*if_indextoname(unsigned int, char *);
void			 if_freenameindex(struct if_nameindex *);
struct if_nameindex *if_nameindex(void);
#endif

#ifndef	HAVE_INET_PTON_PROTO
int			 inet_pton(int, const char *, void *);
const char	*inet_ntop(int, const void *, char *, size_t);
#endif

#ifndef	HAVE_INET_ATON_PROTO
int		 inet_aton(const char *, struct in_addr *);
#endif

#ifndef	HAVE_ISFDTYPE_PROTO
int		 isfdtype(int, int);
#endif

#ifndef	HAVE_PSELECT_PROTO
int		 pselect(int, fd_set *, fd_set *, fd_set *,
				 const struct timespec *, const sigset_t *);
#endif

#ifndef	HAVE_SOCKATMARK_PROTO
int		 sockatmark(int);
#endif

#ifndef	HAVE_SNPRINTF_PROTO
int		 snprintf(char *, size_t, const char *, ...);
#endif

#endif	/* __unp_h */
