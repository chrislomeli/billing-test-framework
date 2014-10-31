/**
 * \file GPGInterface.h
 * Header file for GPG interfacing funtions
 *
 * $Id: GPGInterface.h,v 1.1 2007/01/15 14:16:56 sathpv Exp $
 * $Source: /proj/cvs/untd_billing/server/tools/cpptools/bsi/GPGInterface.h,v $
 *********************************************************************************/
#ifndef __GPGINTERFACE_H__
#define __GPGINTERFACE_H__

#include "return_t.h"
#include "bsi_util.h"
#include <unistd.h>
#include <locale.h>
#include <gpg/gpgme.h>


/*** Initializes the GPGME API ***/
return_t initGPGME(gpgme_protocol_t proto);

/*** Checks the GPG err value ***/
return_t checkGPGError(gpgme_error_t err);

/** Encrypts the template by using vendors public key **/
return_t encrypt(gpgme_key_t pubkey,string &plain,string &cipher);

/* Populates Public Key fingerprint, Algorithm and
 * Returns the key structure on success
 */
return_t getKeyInfo(char *key_pattern,gpgme_key_t &key);

/** Decrypts the cipher **/
return_t decrypt(string &cipher,string &plain);

extern "C" gpgme_error_t passphrase_cb (void *opaque, const char *uid_hint, const char *passphrase_info, int last_was_bad, int fd);

#endif

