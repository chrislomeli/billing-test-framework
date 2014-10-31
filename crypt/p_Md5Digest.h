#ifndef __HEADER__p_Md5Digest__HH
#define __HEADER__p_Md5Digest__HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000/* continuus file information --- %full_filespec: fm_cust_pol_encrypt_pwdmd5.c~5:csrc:1 % */
/*******************************************************************
 *
 *  @(#) %full_filespec: fm_cust_pol_encrypt_pwdmd5.c~5:csrc:1 %
 *
 * MD5 (message digest algorithm from RSA Data Security, Inc.)
 *
 * The following code is taken from the MD5 Reference Implementation
 * (Internet RFC 1321).  The only change was to make all the routines
 * "static" and add a new entry point (make_digest) which produces a
 * digest for a single null-terminated character string.
 *
 *      unsigned char *make_digest(char *string)
 *              Returns a pointer to a 16-byte MD5 digest of the
 *              input parameter.  The returned value points to
 *              static data that will be overwritten on the next
 *              call to the function.
 *
 * Note: other than renaming the make_digest routine, these changes
 * were actually made by Rich Wales <richw@mks.com>.
 *
 * Any and all portions not Copyright RSA are Copyright Portal.
 *
 *******************************************************************/

/**********************************************************************/
#include <util/EnvironmentStd.h>    

/*******************************************************************
 * MD5 related data-types
 *******************************************************************/
/* PROTOTYPES should be set to one if and only if the compiler supports
  function argument prototyping.
The following makes PROTOTYPES default to 0 if it has not already
  been defined with C compiler flags.
 */
#ifndef PROTOTYPES
#define PROTOTYPES 0
#endif

#include <string.h>

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
  returns an empty list.
 */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif


/*******************************************************************
 * RSA Stuff below...
 *******************************************************************/

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

/* MD5 context. */
typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

/* next four lines commented out by richw@mks.com ...
void MD5Init PROTO_LIST ((MD5_CTX *));
void MD5Update PROTO_LIST
  ((MD5_CTX *, unsigned char *, unsigned int));
void MD5Final PROTO_LIST ((unsigned char [16], MD5_CTX *));
*/


/* Constants for MD5Transform routine.
 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static void MD5Transform PROTO_LIST ((UINT4 [4], unsigned char [64]));
static void Encode PROTO_LIST
  ((unsigned char *, UINT4 *, unsigned int));
static void Decode PROTO_LIST
  ((UINT4 *, unsigned char *, unsigned int));
static void MD5_memcpy PROTO_LIST ((POINTER, POINTER, unsigned int));
static void MD5_memset PROTO_LIST ((POINTER, int, unsigned int));

/* next three lines moved from md5.h and made static by richw@mks.com */
static void MD5Init PROTO_LIST ((MD5_CTX *));
static void MD5Update PROTO_LIST ((MD5_CTX *, unsigned char *, unsigned int));
static void MD5Final PROTO_LIST ((unsigned char [16], MD5_CTX *));

static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

static char UOL_HEXSTR[]="0123456789ABCDEF";


class p_Md5Digest
{
public:
	p_Md5Digest(void);
	~p_Md5Digest(void);

	void p_Md5Digest::MD5Init (MD5_CTX *context);/* added by richw@mks.com */

	void p_Md5Digest::MD5Update (/* added by richw@mks.com */
		MD5_CTX *context,                                        /* context */
		unsigned char *input,                                /* input block */
		unsigned int inputLen );                     /* length of input block */

	void p_Md5Digest::MD5Final (/* added by richw@mks.com */
				unsigned char digest[16],                        /* message digest */
				MD5_CTX *context );                                       /* context */

	void p_Md5Digest::MD5Transform (
							UINT4 state[4],
							unsigned char block[64] );
	void p_Md5Digest::Encode (
						unsigned char *output,
						UINT4 *input,
						unsigned int len );

	void p_Md5Digest::Decode (
						UINT4 *output,
						unsigned char *input,
						unsigned int len );

	void p_Md5Digest::MD5_memcpy (
							POINTER output,
							POINTER input,
							unsigned int len );

	void MD5_memset (
						POINTER output,
						int value,
						unsigned int len );


	void p_Md5Digest::makeDigest(
			char *string, unsigned char   *digest );

	bool compareDigest(
		const unsigned char * source , 
		const unsigned char * 
		compare, int size );

	int hexifyCipher( 
			unsigned char *inbuffer, int inlen ,
			char *outbuffer, int outlen );

	int unhexifyCipher( 
		char *inbuffer, int inlen,
		unsigned char *outbuffer, int* outlenp );

	unsigned int unhexnib(unsigned char hexchar);


	std::string base64_encode(
		const unsigned char * bytes_to_encode, 
		unsigned int in_len );

	std::string base64_decode(
		std::string const& encoded_string );

};
#endif

