/**
 * \file GPGInterface.cpp
 * Source file for GPG interfacing funtions
 *
 * $Id: GPGInterface.cpp,v 1.2 2007/01/31 14:46:36 sathpv Exp $
 * $Source: /proj/cvs/untd_billing/server/tools/cpptools/bsi/GPGInterface.cpp,v $
 *********************************************************************************/

#include "GPGInterface.h"
#include <string.h>

static char *CVS_Id = (char *)"CVS_VERSION:$Id: GPGInterface.cpp,v 1.2 2007/01/31 14:46:36 sathpv Exp $";
static char *CVS_Source = (char *)"CVS_VERSION:$Source: /proj/cvs/untd_billing/server/tools/cpptools/bsi/GPGInterface.cpp,v $";

extern string UOL_PASSPHRASE;


/*** Initializes the GPGME API ***/
return_t initGPGME(gpgme_protocol_t proto)
{
	gpgme_error_t err;

  	gpgme_check_version (NULL);
  	setlocale (LC_ALL, "");
  	gpgme_set_locale (NULL, LC_CTYPE, setlocale (LC_CTYPE, NULL));
  	gpgme_set_locale (NULL, LC_MESSAGES, setlocale (LC_MESSAGES, NULL));

  	err = gpgme_engine_check_version (proto);
  	if(checkGPGError(err) == FAILURE)
  	{
                cout<<"initGPGME failed"<<endl;
  		return FAILURE;
  	}

  	return SUCCESS;
}

/*** Checks the GPG err value ***/
return_t checkGPGError(gpgme_error_t err)
{
	if(err)
	{
		cout<<"checkGPGError()::Error Source: "<< gpgme_strsource(err)<<" Error: "<<gpgme_strerror(err)<<endl;
		return FAILURE;
    	}

	return SUCCESS;
}


/** Encrypts the plain text using pubkey **/
return_t encrypt(gpgme_key_t pubkey, string &plain, string &cipher)
{

	gpgme_ctx_t ctx;
	gpgme_error_t err;
	gpgme_data_t in,out;
	gpgme_key_t key[] = {pubkey,NULL};
	gpgme_encrypt_result_t result;

	err = gpgme_new (&ctx);
	if(checkGPGError(err) == FAILURE)
	{
   		cout<<"encryptTemplate()::checkGPGError(err) == FAILURE"<<endl;
   	 	return FAILURE;
  	}

	gpgme_set_armor (ctx, 1);
	err = gpgme_data_new_from_mem (&in, plain.c_str(), plain.size(), 0);
	if(checkGPGError(err) == FAILURE)
	{
		cout<<"encryptTemplate()::checkGPGError(err) == FAILURE"<<endl;
   	 	return FAILURE;
  	}

  	err = gpgme_data_new (&out);
  	if(checkGPGError(err) == FAILURE)
  	{
     		cout<<"encryptTemplate()::checkGPGError(err) == FAILURE"<<endl;
     		return FAILURE;
  	}

  	err = gpgme_op_encrypt (ctx, key, GPGME_ENCRYPT_ALWAYS_TRUST, in, out);
  	if(checkGPGError(err) == FAILURE)
  	{
     		cout<<"encryptTemplate()::checkGPGError(err) == FAILURE"<<endl;
     		return FAILURE;
  	}

  	result = gpgme_op_encrypt_result (ctx);
  	if (result->invalid_recipients)
  	{
     		cout<<"encryptTemplate()::Invalid recipient encountered: "<<
                        result->invalid_recipients->fpr<<endl;
     		return FAILURE;
  	}

  	unsigned int size=0;
  	char *p = gpgme_data_release_and_get_mem(out,&size);
  	if(!p || size==0) {
		 cout<<"encryptTemplate()::Error in reading out data object"<<endl;
		 return FAILURE;
  	}

 	// Removing the extra characters that are getting appended to the encrypted text
        cipher = "";
        cipher.assign(p, size);
        free(p);

        gpgme_key_unref(key[0]);
  	gpgme_data_release (in);
  	gpgme_release (ctx);

  	return SUCCESS;
}

/* Populates Public Key fingerprint, the email id it is associated with
 * and the encryption algorithm it supports.
 * Returns the key structure on success
 */
return_t getKeyInfo(char *keyPattern, gpgme_key_t &key) {

	if(keyPattern == NULL)
	{
		cout<<"getKeyInfo::No key corresponding to vendor-Source: "<<endl;
		return FAILURE;
	}

	gpgme_ctx_t ctx;
	gpgme_error_t err;
	gpgme_keylist_result_t result;

	err = gpgme_new(&ctx);
	if(checkGPGError(err) == FAILURE) {
                 cout<< "getKeyInfo::gpgme_new failed"<<endl;
		 return FAILURE;
	 }

	err = gpgme_op_keylist_start (ctx, keyPattern, 0);
	if(checkGPGError(err) == FAILURE) {
                 cout<< "getKeyInfo::gpgme_op_keylist_start failed"<<endl;
	  	 return FAILURE;
	}

	err = gpgme_op_keylist_next(ctx,&key);
	if(checkGPGError(err) == FAILURE) {
                cout<< "getKeyInfo::gpgme_op_keylist_next failed"<<endl;
		return FAILURE;
	}

  	err = gpgme_op_keylist_end (ctx);
  	if(checkGPGError(err) == FAILURE) {
                cout<< "getKeyInfo::checkGPGError(err) == FAILURE"<<endl;
                return FAILURE;
        }

        result = gpgme_op_keylist_result(ctx);
        if(result->truncated) {
                cout<< "getKeyInfo::Key listing unexpectedly truncated"<<endl;
                return FAILURE;
        }

        /** Checking the key ***/
        if(key->revoked) {
                cout<< "getKeyInfo::Key of " <<keyPattern <<" is revoked"<<endl;
                return FAILURE;
        }

        if(key->expired) {
                cout<<"getKeyInfo::Key of "<<keyPattern<<" is expired"<<endl;
                return FAILURE;
        }

        if(key->disabled) {
                cout<<"getKeyInfo::Key of "<<keyPattern<<" is disabled"<<endl;
                return FAILURE;
        }

        if(key->invalid) {
                cout<<"getKeyInfo::Key of "<<keyPattern<<" is invalid"<<endl;
                return FAILURE;
        }

        if(!key->can_encrypt) {
                cout<<"getKeyInfo::Key of "<<keyPattern<<" is not usable for encryption"<<endl;
                return FAILURE;
        }

        if(key->protocol != GPGME_PROTOCOL_OpenPGP) {
                cout<<"getKeyInfo::Key of "<<keyPattern<<" has unexpected protocol: "<<gpgme_get_protocol_name (key->protocol)<<endl;
                return FAILURE;
        }

        if(!key->subkeys || !key->subkeys->next || key->subkeys->next->next) {
                cout<<"getKeyInfo::Key of "<<keyPattern<<" has unexpected number of subkeys"<<endl;
                return FAILURE;
        }

        if(!key->subkeys->fpr) {
                cout<<"getKeyInfo::Key Fingerprint of "<<keyPattern<<" is NULL"<<endl;
                return FAILURE;
        }

        /** Checking the Secondary Key, which is used for Encryption ***/
        if(key->subkeys->next->revoked) {
                cout<<"getKeyInfo::Secondary Key of "<<keyPattern<<" is revoked"<<endl;
                return FAILURE;
        }

        if(key->subkeys->next->expired) {
                cout<<"getKeyInfo::Secondary Key of "<<keyPattern<<" is expired"<<endl;
                return FAILURE;
        }

        if(key->subkeys->next->disabled) {
                cout<<"getKeyInfo::Secondary Key of "<<keyPattern<<" is disabled"<<endl;
                return FAILURE;
        }

        if(key->subkeys->next->invalid) {
                cout<<"getKeyInfo::Secondary Key of "<<keyPattern<<" is invalid"<<endl;
                return FAILURE;
        }

        if(!key->subkeys->next->can_encrypt) {
                cout<<"getKeyInfo::Secondary Key of "<<keyPattern<<" is not usable for encryption"<<endl;
                return FAILURE;
        }


        string userId;

        if(!key->uids) {
                cout<<"getKeyInfo::No User Id is available for the Public Key of "<<keyPattern<<endl;
                return FAILURE;
        }

        if(key->uids->revoked) {
                cout<<"getKeyInfo::Primary User Id is revoked for the Public Key of "<<keyPattern<<endl;
                return FAILURE;
        }

        if(key->uids->invalid) {
                cout<<"getKeyInfo::Primary User Id is invalid for the Public Key of "<<keyPattern<<endl;
                return FAILURE;
        }

        if(!key->uids->uid) {
                cout<<"getKeyInfo:: No User Id string is available for the Public Key of "<<keyPattern<<endl;
                return FAILURE;
        }

        gpgme_release(ctx);
        return SUCCESS;
}

/** Decrypts the cipher **/
return_t decrypt(string &cipher,string &plain)
{
        gpgme_ctx_t ctx;
        gpgme_error_t err;
        gpgme_data_t inn, out;
        gpgme_decrypt_result_t result;
        int fd;

        err = gpgme_new (&ctx);
        if(checkGPGError(err) == FAILURE) {
                cout<<"decrypt::checkGPGError(err) == FAILURE in opening new context"<<endl;
                return FAILURE;
        }

        err = gpgme_data_new (&out);
        if(checkGPGError(err) == FAILURE) {
                cout<<"decrypt::checkGPGError(err) == FAILURE in creating new data"<<endl;
                return FAILURE;
        }

        gpgme_set_passphrase_cb (ctx, passphrase_cb, NULL);

        err = gpgme_data_new_from_mem (&inn, cipher.c_str(), cipher.size(), 1);
        if(checkGPGError(err) == FAILURE) {
                cout<<"decrypt::checkGPGError(err) == FAILURE in getting data from memory"<<endl;
                return FAILURE;
        }

        err = gpgme_op_decrypt (ctx, inn, out);
        if(checkGPGError(err) == FAILURE) {
                cout<<"decrypt::checkGPGError(err) == FAILURE in decrypting the input"<<endl;
                return FAILURE;
        }

        size_t size=0;
        char *p = gpgme_data_release_and_get_mem(out,&size);
        if(!p || size==0) {
                cout<<"decrypt::Error in reading out data object"<<endl;
                return FAILURE;
        }

        plain = "";
        plain.assign(p, size);
        free(p);

        gpgme_data_release (inn);
        gpgme_release (ctx);
        return SUCCESS;

}

gpgme_error_t passphrase_cb (void *opaque, const char *uid_hint, const char *passphrase_info, int last_was_bad, int fd)
{
        string uol_psph = UOL_PASSPHRASE + string("\n");
        write(fd, uol_psph.c_str(), 4);
        return 0;
}

