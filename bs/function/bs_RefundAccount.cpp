#include <bs/function/bs_RefundAccount.h>
#include <util/Portability.h>


//****************************************************
// refundAccount
// Pre:
// Post:
//
Return_code
bs_RefundAccount::refund_Account(
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

  ///
  /// -- translation ---------------------------
  ///
  string title("enter bs_RefundAccount::refund_Account - nvRec");  nv.StringBuffer(title);
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)title.c_str() );

  //--get the account----------------
  string account_no = nv.value(ACCOUNT_NO);
  string str_account_obj_id0 = nv.value(ACCOUNT_OBJ_ID0);
  string str_amount = nv.value(AMOUNT);
  string str_source = nv.value(SOURCE);
  string str_descr = nv.value(DESCRIPTION);
  string str_srvc_type = nv.value(SERVICE_OBJ_TYPE);

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
    return NoDataFound;
  }

  //-- translate others -----------
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Translate amount ..." );
  std::transform( str_amount.begin(), str_amount.end(), str_amount.begin(), (int(*)(int))toupper);

  if ( str_amount.compare( FULL_ACCOUNT_BALANCE ) == 0 )
  {
    //
    // Refund full balance
    //
    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Getting full account balance to refund ..." );
    pin_decimal_t * amountp =
      bs_AccountInfo::get_AccountBalance( pinp, account_cp.borrow() );

    if ( !rc || pbo_decimal_is_null(amountp,ebufp)  )
    {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
          (char*)"Null account balance for this account - No refund required", ebufp );
      return NothingToDo;
    }
    else if ( pbo_decimal_sign(amountp,ebufp) >= 0 )
    {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
          (char*)"No account balance for this account - No refund required", ebufp );
      return NothingToDo;
    }
    pbo_decimal_negate_assign(amountp,ebufp);
    amount_cp.put(amountp);
  }
  else
  {
    //
    // Refund amount was sent in
    //
    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Refunding the amount sent in .." );
    if (! amount_cp.create( str_amount.c_str() ) ||  amount_cp.isnull() ) {
      if (! PIN_ERR_IS_ERR(ebufp))
        pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
              PIN_ERR_BAD_VALUE,PIN_FLD_AMOUNT, 0, 0);
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"Can not get a valid amount to refund", ebufp );
      return ParseFailure;
    }
  }


  ///
  /// -- call base function ---------------------------
  ///
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Calling base function ..." );
  PIN_ERR_CLEAR_ERR(ebufp);

  rc = refundAccount(
    pinp,
    account_cp.borrow(),
    amount_cp.borrow(),
    str_source,
    str_descr,
    str_srvc_type,
    return_flistpp, ebufp );

  if (rc)
    return Success;
  return PinError;
};


//****************************************************
// refundAccount
// Pre:
// Post:
//
bool bs_RefundAccount::refundAccount(
  pin_Session * pinp,
  poid_t * account_pdp,
  pin_decimal_t * amountp,
  string source,
  string description,
  string service_type,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp )
{
  pin_flist_t   * refund_flistp = NULL;
  pin_flist_t   * in_flistp = NULL;
  pin_flist_t   * out_flistp = NULL;
// pin_flist_t   * r_outflistp = NULL;
  pin_flist_t   *res_flistp = NULL,*t1 = NULL;
//  pin_charge_cmd_t cmd = PIN_CHARGE_CMD_REFUND;
  bool      rc = false;
//  int     iversion  = DEFAULT_ADJUSTMENT_VER;
//  int     id  = DEFAULT_ADJUSTMENT_ID;
//  time_t ltime;
//  pin_virtual_time(&ltime);
    poid_t *srvc_pdp = NULL;
    poid_t *bal_grp_pdp = NULL;
    poid_t *billinfo_pdp = NULL;
// int32 *ach_elemId = NULL;

  /* source / desc name */
  const string this_program("bs_RefundAccount::refundAccount");
  if ( description.empty() )
  {
    ostringstream os;
    os << DEFAULT_REFUND_DESCR << "." << getuser(ENV_USER) << "." << this_program;
    description.assign(os.str());
  }
  if ( source.empty() )
  {
    source.assign(this_program);
  }
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)description.c_str() );

  pin_flist_t *result_flistp = NULL;
  result_flistp = bs_AccountInfo::getService_and_BalGrpPoids(pinp, account_pdp, service_type);
  bal_grp_pdp = PIN_FLIST_FLD_TAKE(result_flistp,PIN_FLD_BAL_GRP_OBJ, 1, ebufp);
  if(result_flistp) PIN_FLIST_DESTROY(result_flistp,ebufp);

  in_flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_PUT(in_flistp, PIN_FLD_POID, bal_grp_pdp, ebufp);
  PIN_FLIST_FLD_SET(in_flistp, PIN_FLD_BILLINFO_OBJ, NULL, ebufp);
  pinp->call_opcode( PCM_OP_READ_FLDS, 0, in_flistp, &out_flistp, ebufp);

  PIN_FLIST_DESTROY_EX(&in_flistp, NULL);

  billinfo_pdp = PIN_FLIST_FLD_TAKE(out_flistp,PIN_FLD_BILLINFO_OBJ, 1, ebufp);

  refund_flistp = PIN_FLIST_CREATE(ebufp);

  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Call refund ...." );
  PIN_FLIST_FLD_SET(refund_flistp, PIN_FLD_POID,(void *) account_pdp, ebufp);
  PIN_FLIST_FLD_SET(refund_flistp, PIN_FLD_AMOUNT,(void *) amountp, ebufp);
  PIN_FLIST_FLD_SET(refund_flistp, PIN_FLD_PROGRAM_NAME,(void *) source.c_str(), ebufp);
  PIN_FLIST_FLD_PUT(refund_flistp, PIN_FLD_BILLINFO_OBJ,(void *) billinfo_pdp, ebufp);

  rc = pinp->call_opcode( PCM_OP_UOL_POL_ITEM_REFUND, 0, refund_flistp, &res_flistp, ebufp);

/*
  //--- two part transaction---------- portal maint does not - so we do not either
  // PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "Open transaction ...." );
  // pinp->transaction_open(0);

  int payType = bs_AccountInfo::getPayType(pinp, account_pdp);
  if(payType == NULL_ENUM)
  {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"Invalid payment type", ebufp );
      return false;
  }


  //--- Input flist info ------------
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Call refund ...." );
  refund_flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_SET(refund_flistp, PIN_FLD_POID,(void *) account_pdp, ebufp);
  PIN_FLIST_FLD_SET(refund_flistp, PIN_FLD_PROGRAM_NAME,(void*)source.c_str(),ebufp);
  PIN_FLIST_FLD_SET(refund_flistp, PIN_FLD_DESCR,(void *)description.c_str(), ebufp);
  PIN_FLIST_FLD_SET(refund_flistp, PIN_FLD_START_T, (void *) &ltime, ebufp);
  PIN_FLIST_FLD_SET(refund_flistp, PIN_FLD_END_T, (void *) &ltime, ebufp);
  charges_flistp = PIN_FLIST_ELEM_ADD(refund_flistp,PIN_FLD_CHARGES, 1, ebufp);
    PIN_FLIST_FLD_SET(charges_flistp, PIN_FLD_PAY_TYPE,(void *) &payType, ebufp);
    PIN_FLIST_FLD_SET(charges_flistp, PIN_FLD_ACCOUNT_OBJ, (void *) account_pdp, ebufp);
    //PIN_FLIST_FLD_SET(charges_flistp, PIN_FLD_MERCHANT, (void *) merchant.c_str(), ebufp);
    PIN_FLIST_FLD_SET(charges_flistp, PIN_FLD_COMMAND, (void *) &cmd, ebufp);

    pin_flist_t *result_flistp = NULL;
    result_flistp = bs_AccountInfo::getService_and_BalGrpPoids(pinp, account_pdp, service_type);
    bal_grp_pdp = PIN_FLIST_FLD_TAKE(result_flistp,PIN_FLD_BAL_GRP_OBJ, 1, ebufp);
    if(result_flistp) PIN_FLIST_DESTROY(result_flistp,ebufp);

    if(PIN_POID_IS_NULL(bal_grp_pdp)) {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"/balance_group poid is NULL", ebufp );
      if (refund_flistp) PIN_FLIST_DESTROY(refund_flistp,ebufp);
      return false;
    }

    /*** Getting PIN_FLD_ACH ****/
/*
    result_flistp = bs_AccountInfo::getBilling_and_PaymentInfo(pinp, bal_grp_pdp);
    if(result_flistp == (pin_flist_t *)NULL) {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"bs_AccountInfo::getBilling_and_PaymentInfo() returning NULL flist", ebufp );
      if (refund_flistp) PIN_FLIST_DESTROY(refund_flistp,ebufp);
      return false;
    }

    ach_elemId = (int32 *)PIN_FLIST_FLD_GET(result_flistp,PIN_FLD_ACH, 1, ebufp);

    if(ach_elemId == NULL) {
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"PIN_FLD_ACH is NULL", ebufp );
      if(refund_flistp) PIN_FLIST_DESTROY(refund_flistp,ebufp);
      PIN_FLIST_DESTROY_EX(&result_flistp,ebufp);
      return false;
    }

  PIN_FLIST_FLD_SET(charges_flistp, PIN_FLD_ACH, (void *)ach_elemId, ebufp);

  PIN_FLIST_DESTROY_EX(&result_flistp,ebufp);

    PIN_FLIST_FLD_PUT(charges_flistp, PIN_FLD_BAL_GRP_OBJ, (void *)bal_grp_pdp, ebufp);

    //t1 = PIN_FLIST_ELEM_ADD(charges_flistp, PIN_FLD_BILLINFO, 1, ebufp);
       PIN_FLIST_FLD_SET(charges_flistp, PIN_FLD_AMOUNT, amountp, ebufp);


  bool passed = false;
  if ( pinp->call_opcode(PCM_OP_UOL_POL_PYMT_COLLECT, 0, refund_flistp, return_flistpp, ebufp))
  {
     pin_flist_t * res_flistp = NULL;
     int * cmd_res = NULL;
     if ((
       (res_flistp = (pin_flist_t *)
        PIN_FLIST_ELEM_GET(*return_flistpp,PIN_FLD_RESULTS, PIN_ELEMID_ANY, 0, ebufp)) != NULL )
       && ((cmd_res=(int *)PIN_FLIST_FLD_GET(res_flistp,PIN_FLD_RESULT, 0, ebufp)) != NULL )
       && (*cmd_res == PIN_RESULT_PASS))
           passed = true;
  }

  if ( !passed  ) {
    if (! PIN_ERR_IS_ERR(ebufp))
      pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
        PIN_ERR_AUTHORIZATION_FAIL,PIN_FLD_RESULT, 0, 0);

    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"Refund failed", ebufp );
    if (refund_flistp) PIN_FLIST_DESTROY(refund_flistp,ebufp);
    return false;
  }
*/
  //-- debit now -----
/*

  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "REFUND: Auto adjust account after refund ...." );
  pin_flist_t *output_flistp = NULL;

  PIN_ERR_CLEAR_ERR(ebufp);
  pbo_decimal_negate_assign(amountp, ebufp);

  if ((rc = bs_AdjustCashBalance::adjust_CashBalance(
      pinp,
      account_pdp,
      amountp,
      source,
      std::string("debit on refund of credit balance"),
      &output_flistp,
      ebufp )) == false )
  {
    if (! PIN_ERR_IS_ERR(ebufp))
      pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
            PIN_ERR_UNKNOWN_EXCEPTION,PIN_FLD_ACCOUNT_OBJ, 0, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,"Debit on Refund failed - transaction rolled back", ebufp );
      PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "Refund transaction .abort..." );
    //pinp->transaction_abort();
  }
  else {
    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "Refund transaction .commit..." );
    //pinp->transaction_commit();
  }
  if (output_flistp) PIN_FLIST_DESTROY_EX(&output_flistp,ebufp);
*/
  if (refund_flistp) PIN_FLIST_DESTROY(refund_flistp,ebufp);
  return rc;

}

