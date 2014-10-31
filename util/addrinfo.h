#ifndef	__addrinfo_h
#define	__addrinfo_h

/*
 * Everything here really belongs in <netdb.h>.
 * These defines are separate for now, to avoid having to modify the
 * system's header.
 */

			/* following for getaddrinfo() */
#ifndef AI_PASSIVE
#define	AI_PASSIVE		 1	/* socket is intended for bind() + listen() */
#endif

#ifndef AI_CANONNAME
#define	AI_CANONNAME	 2	/* return canonical name */
#endif

			/* following for getnameinfo() */
#define	NI_MAXHOST	  1025	/* max hostname returned */
#define	NI_MAXSERV	    32	/* max service name returned */

#ifndef NI_NOFQDN
#define	NI_NOFQDN	     1	/* do not return FQDN */
#endif

#ifndef NI_NUMERICHOST
#define	NI_NUMERICHOST   2	/* return numeric form of hostname */
#endif

#ifndef NI_NAMEREQD
#define	NI_NAMEREQD	     4	/* return error if hostname not found */
#endif

#ifndef  NI_NUMERICSERV
#define	NI_NUMERICSERV   8	/* return numeric form of service name */
#endif

#ifndef NI_DGRAM
#define	NI_DGRAM	    16	/* datagram service for getservbyname() */
#endif

			/* error returns */
#define	EAI_ADDRFAMILY	 1	/* address family for host not supported */
#define	EAI_AGAIN		 2	/* temporary failure in name resolution */
#define	EAI_BADFLAGS	 3	/* invalid value for ai_flags */
#define	EAI_FAIL		 4	/* non-recoverable failure in name resolution */
#define	EAI_FAMILY		 5	/* ai_family not supported */
#define	EAI_MEMORY		 6	/* memory allocation failure */
#define	EAI_NODATA		 7	/* no address associated with host */
#define	EAI_NONAME		 8	/* host nor service provided, or not known */
#define	EAI_SERVICE		 9	/* service not supported for ai_socktype */
#define	EAI_SOCKTYPE	10	/* ai_socktype not supported */
#define	EAI_SYSTEM		11	/* system error returned in errno */

#endif	/* __addrinfo_h */
