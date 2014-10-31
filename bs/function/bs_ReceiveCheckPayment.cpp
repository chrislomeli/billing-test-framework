#include "bs_ReceiveCheckPayment.h"
#include <util/Portability.h>

bool
bs_ReceiveCheckPayment::receive_CheckPayment(
  pin_Session * pinp,
  bs_NvRecord& nv,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
  c_Decimal amount_cp;
  poid_t * account_pdp = NULL;
  pin_decimal_t * amountp = NULL;

  string str_login = nv.value(LOGIN);
  string account_no = nv.value(ACCOUNT_NO);
  string str_poid_id0 = nv.value(ACCOUNT_OBJ_ID0);
  string str_amount = nv.value(AMOUNT);
  string str_resource = nv.value(RESOURCE_ID);
  string str_descr = nv.value(DESCRIPTION);
  string str_source = nv.value(SOURCE);
  string str_srvctype = nv.value(SERVICE_OBJ_TYPE);
  bool rc = false;
  int64 database = pinp->getDBNumber();
  ostringstream account_descr;

  ///
  /// -- translation ---------------------------
  ///
  string title("enter receive_CheckPayment - nvRec");  nv.StringBuffer(title);
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)title.c_str() );


  //-- translate account poid-----------
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Locating account..." );

  c_Poid account_cp;
  if ( ! nv.resolve_AccountPoid(pinp, account_cp))
  {
    pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
        PIN_ERR_NOT_FOUND,PIN_FLD_ACCOUNT_OBJ, 0, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
        (char*)"Can not find account info", ebufp );
    return false;
  };

  //-- translate others -----------
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Translate input..." );

  if (! amount_cp.create( str_amount.c_str() ) ||  amount_cp.isnull() ) {
    if (! PIN_ERR_IS_ERR(ebufp))
      pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
            PIN_ERR_BAD_VALUE,PIN_FLD_AMOUNT, 0, 0);
    account_descr << "-Can not get a valid amount";
    string str( account_descr.str() );
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)str.c_str(), ebufp );
    return false;
  }

  ///
  /// -- call base function ---------------------------
  ///
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Calling base function ..." );
  PIN_ERR_CLEAR_ERR(ebufp);
  rc = receive_CheckPayment (
    pinp,
    account_cp.borrow(),
    amount_cp.borrow(),
    str_source,
    str_descr,
    str_srvctype,
    return_flistpp,
    ebufp );

  ///
  /// -- cleanup and return ---------------------------
  ///
  return rc;

};


/*******************************************************************
 * uol::get_account_info():
 *
 *******************************************************************/
bool
bs_ReceiveCheckPayment::receive_CheckPayment (
  pin_Session * pinp,
  poid_t * account_pdp,
  pin_decimal_t * amountp,
  string source,
  string description,
  string service_type,
  pin_flist_t **return_flistpp, pin_errbuf_t * ebufp )
{
  //-- make a check payment ------------------
  time_t ltime;
  pin_flist_t * account_flistp = NULL;
  pin_flist_t * outflistp = NULL;
  pin_flist_t * t1_flistp = NULL;
  pin_flist_t * t2_flistp = NULL;
  pin_flist_t * t3_flistp = NULL;
  poid_t *srvc_pdp = NULL, *bal_grp_pdp = NULL;
  poid_t *billinfo_pdp = NULL;
  int32 *ach_elemId = NULL;

  int command         = 0;
  const char * check_no   = "12345";
  const char * bank_account = "12345";
  const char * bank_code    = "12345";

  /* source / desc name */
  const string this_program("receive_CheckPayment");
  if ( description.empty() )
  {
    ostringstream os;
    os << DEFAULT_CHECK_PMT_DESCR << "." << getuser(ENV_USER) << "." << this_program;
    description.assign(os.str());
  }
  if ( source.empty() )
  {
    source.assign(this_program);
  }
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)description.c_str() );


  PIN_ERR_CLEAR_ERR(ebufp);
  pin_virtual_time(&ltime);

  int payType = bs_AccountInfo::getPayType(pinp,account_pdp);
  if(payType == NULL_ENUM)
  {
     PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"Invalid payment type", ebufp );
     return false;
  }


  pin_flist_t * payment_flistp = pin_flist_create(ebufp);
  PIN_FLIST_FLD_SET(payment_flistp, PIN_FLD_POID,(void *)account_pdp, ebufp);
  PIN_FLIST_FLD_SET(payment_flistp, PIN_FLD_PROGRAM_NAME,(void *)source.c_str(), ebufp);
  PIN_FLIST_FLD_SET(payment_flistp, PIN_FLD_DESCR,(void *)description.c_str(), ebufp);
  t1_flistp = PIN_FLIST_ELEM_ADD(payment_flistp, PIN_FLD_CHARGES, 1, ebufp);
    PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ACCOUNT_OBJ,(void *)account_pdp, ebufp);
    PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_COMMAND,(void *)&command, ebufp);
    PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_PAY_TYPE,(void *) &payType, ebufp);

    pin_flist_t *result_flistp = NULL;
    result_flistp = bs_AccountInfo::getService_and_BalGrpPoids(pinp, account_pdp, service_type);
    bal_grp_pdp = PIN_FLIST_FLD_TAKE(result_flistp,PIN_FLD_BAL_GRP_OBJ, 0, ebufp);
    if(result_flistp) PIN_FLIST_DESTROY(result_flistp,ebufp);


    if(PIN_POID_IS_NULL(bal_grp_pdp)) {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"/balance_group poid is NULL", ebufp );
      if (payment_flistp) PIN_FLIST_DESTROY(payment_flistp,ebufp);
      return false;
    }

    /*** Getting BILLINFO_OBJ and PIN_FLD_ACH ***/

    result_flistp = bs_AccountInfo::getBilling_and_PaymentInfo(pinp, bal_grp_pdp);
    if(result_flistp == (pin_flist_t *)NULL) {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"bs_AccountInfo::getBilling_and_PaymentInfo() returning NULL flist", ebufp );
      if (payment_flistp) PIN_FLIST_DESTROY(payment_flistp,ebufp);
      return false;
    }

    ach_elemId = (int32 *)PIN_FLIST_FLD_GET(result_flistp,PIN_FLD_ACH, 1, ebufp);
    billinfo_pdp = (poid_t *) PIN_FLIST_FLD_GET(result_flistp,PIN_FLD_BILLINFO_OBJ, 1, ebufp);

    if(ach_elemId == NULL) {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"PIN_FLD_ACH is NULL", ebufp );
      if(payment_flistp) PIN_FLIST_DESTROY(payment_flistp,ebufp);
      PIN_FLIST_DESTROY_EX(&result_flistp,ebufp);
      return false;
    }
 
    if(PIN_POID_IS_NULL(billinfo_pdp)) {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"PIN_FLD_BILLINFO_OBJ is NULL", ebufp );
      if(payment_flistp) PIN_FLIST_DESTROY(payment_flistp,ebufp);
      PIN_FLIST_DESTROY_EX(&result_flistp,ebufp);
      return false;
    }

    PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ACH, (void *)ach_elemId, ebufp);
    PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_BAL_GRP_OBJ, (void *)bal_grp_pdp, ebufp);

    t2_flistp = (pin_flist_t *)NULL;
    t2_flistp = PIN_FLIST_ELEM_ADD(t1_flistp, PIN_FLD_BILLINFO, 1, ebufp);
      PIN_FLIST_FLD_SET(t2_flistp, PIN_FLD_AMOUNT,(void *)amountp, ebufp);
      PIN_FLIST_FLD_SET(t2_flistp, PIN_FLD_BILLINFO_OBJ, (void *)billinfo_pdp, ebufp);
      PIN_FLIST_DESTROY_EX(&result_flistp,ebufp);
      t3_flistp = PIN_FLIST_SUBSTR_ADD(t2_flistp, PIN_FLD_PAYMENT, ebufp);
        PIN_FLIST_FLD_SET(t3_flistp, PIN_FLD_DESCR,(void *)description.c_str(), ebufp);
        t2_flistp = PIN_FLIST_SUBSTR_ADD(t3_flistp, PIN_FLD_INHERITED_INFO, ebufp);
          t3_flistp = PIN_FLIST_ELEM_ADD(t2_flistp, PIN_FLD_CHECK_INFO, 0, ebufp);
            PIN_FLIST_FLD_SET(t3_flistp, PIN_FLD_EFFECTIVE_T,(void *)&ltime, ebufp);
            PIN_FLIST_FLD_SET(t3_flistp, PIN_FLD_CHECK_NO,(void *)check_no, ebufp);
            PIN_FLIST_FLD_SET(t3_flistp, PIN_FLD_BANK_CODE,(void *)bank_code, ebufp);
            PIN_FLIST_FLD_SET(t3_flistp, PIN_FLD_BANK_ACCOUNT_NO,(void *)bank_account, ebufp);


  bool rc = pinp->call_opcode(PCM_OP_UOL_POL_PYMT_COLLECT,0,payment_flistp, return_flistpp, ebufp);
  if (payment_flistp) PIN_FLIST_DESTROY(payment_flistp,ebufp);

  bool passed = false;
  if ( rc )
  {
     int * cmd_res = NULL;
     pin_flist_t * res_flistp =
       (pin_flist_t *)PIN_FLIST_ELEM_GET(
            *return_flistpp,PIN_FLD_RESULTS, PIN_ELEMID_ANY, 0, ebufp);

     if ( res_flistp != NULL
       && (cmd_res=(int *)PIN_FLIST_FLD_GET(res_flistp,PIN_FLD_RESULT, 0, ebufp)) != NULL
       && (*cmd_res == PIN_RESULT_PASS))
           passed = true;
  }

  if ( !passed  ) {
    if (! PIN_ERR_IS_ERR(ebufp))
      pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
            PIN_ERR_AUTHORIZATION_FAIL,PIN_FLD_RESULT, 0, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"Receive Payment failed", ebufp );
    pinp->transaction_abort();
    return false;
  }
  return rc;
}

