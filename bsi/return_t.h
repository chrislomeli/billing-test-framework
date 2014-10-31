/**
 * \file return_t.h
 * The file for storing return values
 *
 * $Id: return_t.h,v 1.1 2007/01/15 14:16:56 sathpv Exp $
 * $Source: /proj/cvs/untd_billing/server/tools/cpptools/bsi/return_t.h,v $
 *********************************************************************************/


#ifndef _RETURN_T_H_
#define _RETURN_T_H_
/**
 * Structure to store the various return types.
 * Apart from SUCCESS and FAILURE, *ALL* return codes MUST start with ERR_.
 * All failure codes must be less than 0.  Further, the trailing comment
 * after each code MUST be a user-readable description; a script reads this
 * file and generates text error messages.
 */
enum return_t {
  SUCCESS                  =  1,
  CONTINUE             =  2,
  MORE                     =  3,
  FAILURE                  = -1,    
  ERR_COMM             = -2,
  ERR_BAD_MSG            = -3,
  ERR_NOCONN             = -4,
  ERR_CONFIG             = -5,
  ERR_PUTMAIL            = -6,
  ERR_HANGUP             = -7,
  ERR_GARBLED            = -8,
  ERR_TIMEOUT            = -9,
  ERR_OVERFLOW             = -10,
  ERR_ESCAPE             = -11,
  ERR_GETMAIL            = -12,
  ERR_ZMODEM             = -13,
  ERR_AUTH                 = -14,
  ERR_INUSE            = -15,
  ERR_NODATA             = -16,
  END_OF_FILE            = -17, /**< end of file reached */
  BAD_FORMAT             = -18, /**< parsing trouble */
  FILE_ACCESS_ERROR        = -19, /**< file trouble */
  FILE_CLOSED              = -20, /**< expected file to be open */
  DUPLICATE_NAME           = -21, /**< each folder must have a unique name */
  RESERVED_FOLDER          = -22, /**< can't rename/delete reserved folders (e.g. Inbox) */
  NO_MORE_FOLDERS          = -23, /**< exceeded limit on number of folders   */
  NAME_TOO_LONG            = -24, /**< exceeded maximum folder name length */
  TRUNCATED            = -25, /**< text truncated due to size limitations  */
  FOLDER_NOT_INIT          = -26, /**< expected folder to be initialized */
  FOLDER_CLOSED            = -27, /**< expected folder to be open */
  DIRECTORY_NOT_INIT       = -28, /**< expected directory to be initialized */
  ERR_NOSUCHFILE           = -29,
  NOT_INIT                 = -30, /**< item not initialized */
  NOT_FOUND                = -31, /**< item not found   */
  NO_NAME                  = -32, /**< a name is required */
  ERR_BAD_TOLIST           = -33,
  ERR_NOT_INITIALIZED      = -34,
  NO_GOOD            = -35, /**< general error */
  ERR_NOSPC            = -36, /**< No space left on device */
  ERR_INTERRUPT            = -37, /**< Interrupt signal */
  ERR_QUIT             = -38, /**< Quit signal */
  ERR_NETWORK_UNAVAILABLE  = -39, /* X.25 network not available */
  ERR_NO_CARRIER     = -40, /**< Carrier was lost */
  ERR_NO_DIALTONE    = -41,
  ERR_BUSY             = -42,
  ERR_INVALID_KEY          = -43, /**< bad encryption key (eg. weak DES key) */
  ERR_PUT_FAILED     = -44, /**< Xmit failed on put */
  ERR_GET_FAILED     = -45, /**< Xmit failed on get */
  BAD_FPRINT               = -46, /**< Some file failed to authenticate */
  AUTHLOG_NOT_CLEAN        = -47, /**< AuthLogFile object not closed cleanly */
  BAD_FPRINT_AND_NOT_CLEAN = -48, /**< Both of the above two */
  ERR_CORRUPT_ARCHIVE      = -49, /**< Archive is corrupt */
  ERR_CONFLICT             = -51, /**< Ad conflicts with already-loaded ad */
  ERR_RSRC                 = -52, /**< Temporary system resource depletion */
  ERR_REBINDING            = -53, /**< user has moved to new server */
  ERR_VERSION              = -54, /**< Ad is of an incompatible version */
  ERR_CSTROVERFLOW         = -55, /**< CStr overflow */
  ERR_COM_MODEMINIT        = -58, /**< Error in initializing modem */  
  ERR_COM_CONNECT          = -59, /**< Error in connecting to Juno machines */ 
  ERR_ACC_CONFLICT         = -60, /**< This account name is already in use */
  ERR_ADDR_NO_DOMAIN       = -61, /**< The address does not contain a domain name */
  ERR_ADDR_NON_822         = -62, /**< The address does not conform to
                                   *   RFC 822 grammar 
                                   */
  ERR_DIALUPD_FAILED       = -63, /**< An attempt to update the dial
                                   *   profile failed 
                                   */
  ERR_REBINDING_NEEDED     = -64, /**< A rebinding is needed. This is 
                                    *  distinct from ERR_REBINDING, 
                                    *  which says that a rebinding has 
                                    *  taken place. 
                                    */
  ERR_COM_STATE            = -65, /**< Wrong state for communications 
                                   *   operation 
                                   */
  END_OF_SCRIPT            = -66, /**< End of line or end of file
                                   * reached in Script 
                                   */
  ERR_FAILED_LOAD          = -67, /**< Dynamic load of library failed */
  ERR_USER_CANCEL          = -68, /**< User has cancelled connection to server */
  ERR_BLOCKED              = -69, /**< A blocking operation is being
                                   *   carried out 
                                   */
  ERR_MAYBE_INUSE          = -70, /**< This device may be in
                                   *   use. Distinct from ERR_INUSE 
                                   */
  ERR_HIGH_LOAD            = -71, /**< Due to high server load, an
                                   *   operation couldn't be performed 
                                   */
  ERR_NOVALUE              = -73, /**< Context manager did not find the value */
  ERR_NO_UPLOADS     = -74, /**< Server blocking client file uploads */
  ERR_OPAQUE_MESSAGE       = -76, /**< Opaque error message received from server*/ 
  ERR_QUOTA                = -77, /**< User exceeded quota*/
  ERR_ACCT_TERMINATED    = -78, /**< User account terminated in UDB */
  ERR_ACCT_NOGOLDWEB     = -79, /**< Core member not a BS member MPV={} || e#0  */
  ERR_ACCT_PCANCELLED    = -80, /**< User has CANCELLED USER DEAL in Portal */
  ERR_ACCT_PNOTEXIST     = -81, /**< User doen not exist in Portal */
  ERR_ACCT_DELETED     = -83, /**< user account DELETED in UDB */
  ERR_ACCT_PPRODNOTEXISTS  = -84, /**< user has no product in a portal account*/
  ERR_ACCT_INTERNAL        = -85, /**< VIP account: BILL_TYPE is internal */
  ERR_ACCT_INACTIVE    = -86, /**< User BS account is INACTIVE */
  ERR_ACCT_CLOSED    = -87, /**< User BS account is CLOSED   */
  ERR_ACCT_NULLTRANSITION  = -88, /**< User did not specify the upgrade transition*/
  ERR_COOKIE_ABSENT    = -89, /**< Cookie not found in the HTTP request*/
  ERR_ACCT_MESA            = -90, /**< user account is MESA */
  END_RETURNCODE      /**< not used */
};

#ifndef __cplusplus
typedef enum return_t return_t;
#endif

#endif /* _RETURN_T_H_ */

