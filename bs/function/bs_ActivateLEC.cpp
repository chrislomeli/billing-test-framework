#include "bs_ActivateLEC.h"

/*#define PCM_OP_CUST_POL_UOL_ACTIVATE_LEC PCM_OP_TEST_LOOPBACK
*/

#define USE_WORKAROUND 1

bool
bs_ActivateLEC::activateLEC(
  pin_Session * pinp,
  bs_NvRecord& nv,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
  bool rc = false;
  int64 database = pinp->getDBNumber();
  ostringstream account_descr;

  string title("activateLEC nvRec");
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,
    (char*)nv.StringBuffer(title).c_str());

  ///
  /// -- translation ---------------------------
  ///
  //-- translate account poid-----------
  c_Poid account_cp;
  if ( ! nv.resolve_AccountPoid( pinp, account_cp ))
  {
    pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
        PIN_ERR_NOT_FOUND,PIN_FLD_ACCOUNT_OBJ, 0, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
        (char*)"bad account info", ebufp );
    return false;
  };

  ///
  /// -- call base function ---------------------------
  ///
  PIN_ERR_CLEAR_ERR(ebufp);

#ifdef USE_WORKAROUND
  rc = bs_ActivateLEC::manual_activateLEC(
    pinp,
    account_cp.borrow(),
    return_flistpp, ebufp );
#else

  rc = bs_ActivateLEC::activateLEC(
    pinp,
    account_cp.borrow(),
    return_flistpp, ebufp );

#endif
  return rc;
};




//****************************************************
// activateLEC
// Pre:
// Post:
//
bool bs_ActivateLEC::activateLEC(
  pin_Session * pinp,
  poid_t * account_pdp,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
  pin_flist_t * x_flistp = NULL;
  char * program_name = (char*)"tess::bs_ActivateLEC\0";

  PIN_ERR_CLEAR_ERR(ebufp);

#ifdef USE_WORKAROUND
        return bs_ActivateLEC::manual_activateLEC(
                pinp,
                account_pdp,
                return_flistpp, ebufp );
#endif


  /*--- Create set_status ---*/
  x_flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_SET(x_flistp, PIN_FLD_POID, account_pdp, ebufp);
  PIN_FLIST_FLD_SET(x_flistp, PIN_FLD_PROGRAM_NAME, program_name, ebufp);

  bool rc = pinp->call_opcode(
    PCM_OP_CUST_POL_UOL_ACTIVATE_LEC, 0, x_flistp, return_flistpp, ebufp);

  if (x_flistp) PIN_FLIST_DESTROY(x_flistp,NULL);
  return rc;
}


/*******************************************************************
 * fm_cust_pol_uol_activate_lec()
 *
 *******************************************************************
*/
bool bs_ActivateLEC::manual_activateLEC(
  pin_Session * pinp,
  poid_t * account_pdp,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
  poid_t      *a_pdp = NULL;
  pin_flist_t   *ai_flistp = NULL;
  u_int32     result = PIN_RESULT_PASS;
  u_int32     dom = 0;
  time_t      now = 0;
  struct tm   time_fields,
          *time_fieldsp = &time_fields;
  pin_status_t  *curr_statusp;
  bool rc=true;


  now = pin_virtual_time(NULL);
  time_fieldsp = localtime(&now);
  dom = time_fieldsp->tm_mday;
  if (dom > 28) dom = 1;

PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"ENTER manual_activateLEC" );
cout << "DEBUG: Open transaction" << endl;

  // Get input account poid from input flist.
//  if (! pinp->transaction_open(0)) {
//    return false;
//  }

cout << "DEBUG: Done Open transaction set_billing_dom to " << dom << endl;

PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"ENTER set_billing_dom " );
  if ((rc= set_billing_dom(pinp,account_pdp, "manual_activateLEC.set_billing_dom", &dom, return_flistpp, ebufp)) == false )
  {
//    pinp->transaction_abort();
    return rc;
  }
cout << "DEBUG: Done set_billing_dom - clean flist" << endl;
PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, (char*)"back from set_billing_dom", *return_flistpp );


  if (*return_flistpp)
    PIN_FLIST_DESTROY(*return_flistpp,NULL);
  *return_flistpp = NULL;

cout << "DEBUG: set_account_active " << endl;
  if ( (rc=set_account_active(pinp,"manual_activateLEC.set_account_active",account_pdp, return_flistpp, ebufp)) == false ) {

//    pinp->transaction_abort();
  } return rc;
cout << "DEBUG: Done set_account_active - " << endl;
PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, (char*)"back from set_account_active", *return_flistpp );

//  pinp->transaction_commit();
  return rc;
}

bool bs_ActivateLEC::set_billing_dom(
  pin_Session * pinp,
  poid_t * account_pdp,
  string program_name,
  u_int32     *dom ,
        pin_flist_t   **return_flistpp,
        pin_errbuf_t  *ebufp    )
{
  void    *vp = NULL;
  pin_flist_t *sa_flistp = NULL,
        *ai_flistp = NULL,
        *sar_flistp = NULL;
  int     rc = 0;

  int payType = bs_AccountInfo::getPayType(pinp,account_pdp);
  if(payType == NULL_ENUM)
  {
     return false;
  }

  sa_flistp = PIN_FLIST_CREATE(ebufp);

  PIN_FLIST_FLD_SET(sa_flistp, PIN_FLD_POID, account_pdp, ebufp);
  PIN_FLIST_FLD_SET(sa_flistp, PIN_FLD_PROGRAM_NAME,
              (void *)program_name.c_str(), ebufp);

  //ai_flistp = PIN_FLIST_ELEM_ADD(sa_flistp, PIN_FLD_ACTGINFO, 0, ebufp);

  ai_flistp = PIN_FLIST_ELEM_ADD(sa_flistp, PIN_FLD_BILLINFO, 0, ebufp);
    PIN_FLIST_FLD_SET(ai_flistp, PIN_FLD_PAY_TYPE, (void *)&payType, ebufp);
    PIN_FLIST_FLD_SET(ai_flistp, PIN_FLD_ACTG_FUTURE_DOM, (void *)dom, ebufp);

  rc = pinp->call_opcode(PCM_OP_CUST_SET_BILLINFO, 0,
           sa_flistp, &sar_flistp, ebufp);

  if (sa_flistp) PIN_FLIST_DESTROY_EX(&sa_flistp, NULL);
  if (sar_flistp) PIN_FLIST_DESTROY_EX(&sar_flistp, NULL);

  return rc;
}

bool bs_ActivateLEC::set_account_active(
  pin_Session * pinp,
  string program_name,
  poid_t * account_pdp ,
        pin_flist_t   **return_flistpp,
        pin_errbuf_t  *ebufp    )

{
  u_int32       activeStatus = 10100,
            statusFlag = 0;
  int         rc = 0;
  pin_flist_t     *a_flistp = NULL,
            *b_flistp = NULL,
            *r_flistp = NULL;
  poid_t        *a_pdp = NULL;
  char        *programName = NULL;


  /* Get all the data needed for status, adjustment and debit operations... */
  a_flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_SET(a_flistp, PIN_FLD_POID, (void *)account_pdp, ebufp);
  PIN_FLIST_FLD_SET(a_flistp, PIN_FLD_PROGRAM_NAME,
    (void *)program_name.c_str(), ebufp);

  b_flistp = PIN_FLIST_ELEM_ADD(a_flistp, PIN_FLD_STATUSES, 0, ebufp);
  PIN_FLIST_FLD_SET(b_flistp, PIN_FLD_STATUS,
    (void *)&activeStatus, ebufp);
  PIN_FLIST_FLD_SET(b_flistp, PIN_FLD_STATUS_FLAGS,
    (void *)&statusFlag, ebufp);

  rc = pinp->call_opcode(PCM_OP_CUST_POL_UOL_SET_STATUS, 0, a_flistp, return_flistpp, ebufp);

  if (a_flistp) PIN_FLIST_DESTROY(a_flistp, NULL);
  if (r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);

  return rc;
}



