#include <bs/function/bs_CollectAccount.h>
#include <util/Portability.h>


//****************************************************
// collectAccount
// Pre:
// Post:
//
int
bs_CollectAccount::collect_Account(
  pin_Session * pinp,
  bs_NvRecord& nv,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
  c_Poid account_cp;
  c_Decimal amount_cp;
  poid_t * account_pdp = NULL;
  pin_decimal_t * amountp = NULL;
  ostringstream account_descr;
  int64 database = pinp->getDBNumber();
  bool rc = false;
  int result = -9;

  ///
  /// -- translation ---------------------------
  ///
  string title("enter bs_CollectAccount::collect_Account - nvRec");  nv.StringBuffer(title);
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)title.c_str() );

  //--get the account----------------
  string account_no = nv.value(ACCOUNT_NO);
  string str_account_obj_id0 = nv.value(ACCOUNT_OBJ_ID0);
  string str_amount = nv.value(AMOUNT);
  string source = nv.value(SOURCE);
  string service_type = nv.value(SERVICE_OBJ_TYPE);
  if (source.empty())
    source.assign("TESS");

  //-- translate account poid-----------
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Locating account..." );

  bs_AccountInfo  accountInfo;
  if (! str_account_obj_id0.empty() )
  {
    int64 poid_id0 = atol(str_account_obj_id0.c_str());
    account_cp.create(pinp->getDBNumber(), (char *)"/account", poid_id0 );
  }
  else if (! account_no.empty() )
  {
    if((rc =  accountInfo.loadByAccountNo( pinp, account_no )) == true )
      rc = account_cp.create( accountInfo.account_pdp );
  }
  if (  ! rc || account_cp.isnull() ) {
      pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
            PIN_ERR_NOT_FOUND,PIN_FLD_ACCOUNT_OBJ, 0, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"Can not fimd account ...", ebufp );
    return -1;
  }

  //-- translate others -----------
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Translate amount ..." );
  std::transform(
    str_amount.begin(), str_amount.end(), str_amount.begin(), (int(*)(int))toupper);

  if ( str_amount.compare( FULL_ACCOUNT_BALANCE ) == 0 )
  {
    //
    // Collect full balance
    //
    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Getting full account balance to collect ..." );
    pin_decimal_t * amountp =
      bs_AccountInfo::get_AccountBalance( pinp, account_cp.borrow() );

    if ( !rc || pbo_decimal_is_null(amountp,ebufp)  )
    {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
          (char*)"Null account balance for this account - No collect required", ebufp );
      return -2;
    }
    else if ( pbo_decimal_sign(amountp,ebufp) <= 0 )
    {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
          (char*)"No account balance for this account - No collect required", ebufp );
      return -2;
    }
    amount_cp.put(amountp);
  }
  else
  {
    //
    // Collect amount was sent in
    //
    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Collecting the amount sent in .." );
    if (! amount_cp.create( str_amount.c_str() ) ||  amount_cp.isnull() ) {
      if (! PIN_ERR_IS_ERR(ebufp))
        pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
              PIN_ERR_BAD_VALUE,PIN_FLD_AMOUNT, 0, 0);
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"Can not get a valid amount to collect", ebufp );
      return -3;
    }
  }


  ///
  /// -- call base function ---------------------------
  ///
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Calling base function ..." );
  PIN_ERR_CLEAR_ERR(ebufp);

  result = collectAccount(
    pinp,
    source.c_str(),
    account_cp.borrow(),
    amount_cp.borrow(),
    service_type,
    return_flistpp, ebufp );

  return result;
};


//****************************************************
// collectAccount
// Pre:
// Post:
//
int bs_CollectAccount::collectAccount(
  pin_Session * pinp,
  const char * program_name,
  poid_t * account_pdp,
  pin_decimal_t * amountp,
  string service_type,
  pin_flist_t    **out_flistpp,
  pin_errbuf_t  * ebufp )
{
  pin_flist_t    *out_flistp = NULL;
  pin_flist_t    *q_flistp = NULL;
  pin_flist_t    *t1 = NULL,*t2 = NULL;
  poid_t *srvc_pdp = NULL, *bal_grp_pdp = NULL;
  static char   * descr = (char*)"Tess Collection";
  void      * vp = NULL;
  int cmd = (int)PIN_CHARGE_CMD_AUTH_ONLY;
  bool rc = false;
  int result = -6;
  int32 *ach_elemId = NULL;

  PIN_ERR_CLEAR_ERR(ebufp);

  /*----- debug ----*/
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Enter collectAccount:: ..." );

  int payType = bs_AccountInfo::getPayType(pinp,account_pdp);
  if(payType == NULL_ENUM)
  {
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"Invalid payment type", ebufp );
    return result;
  }

  /*
   * Allocate the flist for the email (parameters).  Set poid.
   */
  q_flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_SET(q_flistp, PIN_FLD_POID, (void *)account_pdp, ebufp);
  PIN_FLIST_FLD_SET(q_flistp, PIN_FLD_PROGRAM_NAME,(void *)program_name, ebufp);
  PIN_FLIST_FLD_SET(q_flistp, PIN_FLD_DESCR,(void *)descr, ebufp);
  t1 = PIN_FLIST_ELEM_ADD(q_flistp, PIN_FLD_CHARGES, 1, ebufp);

  pin_flist_t *result_flistp = NULL;
  result_flistp = bs_AccountInfo::getService_and_BalGrpPoids(pinp, account_pdp, service_type);
  bal_grp_pdp = PIN_FLIST_FLD_TAKE(result_flistp,PIN_FLD_BAL_GRP_OBJ, 1, ebufp);
  if(result_flistp) PIN_FLIST_DESTROY(result_flistp,ebufp);

  if(PIN_POID_IS_NULL(bal_grp_pdp)) {
     PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"/balance_group poid is NULL", ebufp );
     if (q_flistp) PIN_FLIST_DESTROY(q_flistp,ebufp);
     return false;
  }

  result_flistp = bs_AccountInfo::getBilling_and_PaymentInfo(pinp, bal_grp_pdp);
  if(result_flistp == (pin_flist_t *)NULL) {
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"bs_AccountInfo::getBilling_and_PaymentInfo() returning NULL flist", ebufp );
    if (q_flistp) PIN_FLIST_DESTROY(q_flistp,ebufp);
    return false;
  }

  ach_elemId = (int32 *)PIN_FLIST_FLD_GET(result_flistp,PIN_FLD_ACH, 1, ebufp);

  if(ach_elemId == NULL) {
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"PIN_FLD_ACH is NULL", ebufp );
    if(q_flistp) PIN_FLIST_DESTROY(q_flistp,ebufp);
    PIN_FLIST_DESTROY_EX(&result_flistp,ebufp);
    return false;
  }
  
  PIN_FLIST_FLD_SET(t1, PIN_FLD_ACH, (void *)ach_elemId, ebufp);

  PIN_FLIST_DESTROY_EX(&result_flistp,ebufp);

  if(bal_grp_pdp) PIN_FLIST_FLD_PUT(t1, PIN_FLD_BAL_GRP_OBJ, (void *)bal_grp_pdp, ebufp);

  PIN_FLIST_FLD_SET(t1, PIN_FLD_PAY_TYPE,(void *) &payType, ebufp);
  PIN_FLIST_FLD_SET(t1, PIN_FLD_ACCOUNT_OBJ, (void*)account_pdp, ebufp);
  PIN_FLIST_FLD_SET(t1, PIN_FLD_COMMAND, (void*)&cmd, ebufp);

  //t2 = PIN_FLIST_ELEM_ADD(t1, PIN_FLD_BILLINFO, 1, ebufp);
  PIN_FLIST_FLD_SET(t1, PIN_FLD_AMOUNT, (void*)amountp, ebufp);


  /*
   * Call the collection opcode
   */

  rc = pinp->call_opcode(
    PCM_OP_UOL_POL_PYMT_COLLECT, 0, q_flistp, out_flistpp, ebufp);

  if ( !rc )
    result = -4;
  else if (  *out_flistpp == NULL )
    result = -5;
  else
  {
    int32   elemid = 0;
    pin_cookie_t cookie = NULL;
    pin_flist_t * t1 = NULL;

    while ((t1 = PIN_FLIST_ELEM_GET_NEXT(*out_flistpp, PIN_FLD_RESULTS,
      &elemid, 1, &cookie, ebufp )) != NULL )
    {
      if ((vp = PIN_FLIST_FLD_GET(t1, PIN_FLD_RESULT, 1, ebufp)) != NULL )
        result = *(int*)vp;
    }
  }

  /*
   * Clean up.
   */
  PIN_FLIST_DESTROY_EX(&q_flistp, NULL);
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Leaving collectOnThreshold:: ..." );
  return result;
}

