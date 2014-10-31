#include "bs_AdjustCashBalance.h"
#include <util/Portability.h>


bool
bs_AdjustCashBalance::adjust_CashBalance(
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
  string str_resource = nv.value("resource_id");
  string str_source = nv.value(SOURCE);
  string str_descr = nv.value(DESCRIPTION);  //optional
  string service_str = nv.value(SERVICE_OBJ_TYPE);
  string reason_id = nv.value("REASON_ID");

  bool rc = false;
  int64 database = pinp->getDBNumber();
  ostringstream account_descr;


  ///
  /// -- translation ---------------------------
  ///
  string title("adjust_CashBalance nvRec");  nv.StringBuffer(title);
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)title.c_str() );


  //-- translate account poid-----------
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Locating account..." );
  c_Poid account_cp;
  if ( ! nv.resolve_AccountPoid(pinp, account_cp))
  {
    pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
        PIN_ERR_NOT_FOUND,PIN_FLD_ACCOUNT_OBJ, 0, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
        (char*)"bad account info", ebufp );
    return false;
  };


  //-- translate others -----------
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Translate amount..." );
  if (! amount_cp.create( str_amount.c_str() ) ||  amount_cp.isnull() ) {
    if (! PIN_ERR_IS_ERR(ebufp))
      pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
            PIN_ERR_BAD_VALUE,PIN_FLD_AMOUNT, 0, 0);
    account_descr << "-Can not get a valid amount";
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)((account_descr.str()).c_str()), ebufp );
    return false;
  }

  ///
  /// -- call base function ---------------------------
  ///
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Calling base function..." );
  PIN_ERR_CLEAR_ERR(ebufp);
  rc = adjust_CashBalance(
    pinp,
    account_cp.borrow(),
    amount_cp.borrow(),
    str_source,
    str_descr,
    service_str,
    reason_id,
    return_flistpp,
    ebufp );

  ///
  /// -- cleanup and return ---------------------------
  ///
  return rc;

};





//****************************************************
// adjust_CashBalance
// Pre:
// Post:
//
bool bs_AdjustCashBalance::adjust_CashBalance(
  pin_Session * pinp,
  poid_t * account_pdp,
  pin_decimal_t * amountp,
  string source,
  string description,
  string service_str,
  string reason_id,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp )
{
  pin_flist_t   *input_flistp = NULL;
  poid_t *bal_pdp = NULL;
  int       dollar_currency = 840;
  int       iversion  = DEFAULT_CASH_ADJUSTMENT_VER;
  ostringstream os;
  bool rc = false;

  char 		*service_code = (char *)NULL;
  service_code = (char *)calloc(256, sizeof(char));
  strcpy(service_code,(const char *)GENERIC_SERVICE_ADJUSTMENT_CODE);
  strcat(service_code,reason_id.c_str());

  int	id = atoi(service_code);

  PIN_ERR_CLEAR_ERR(ebufp);

  //--- setup --------------------------
  /* source / desc name */
  const string this_program("adjust_CashBalance");
  if ( description.empty() )
  {
    ostringstream os;
    os << DEFAULT_CASH_ADJUSTMENT_DESCR << "." << getuser(ENV_USER) << "." << this_program;
    description.assign(os.str());
  }
  if ( source.empty() )
  {
    source.assign(this_program);
  }
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)description.c_str() );

  /* retrieving the bal_grp poid associated with a service
   * if the service_str is NULL, then account level bal_grp will be used
   */
  pin_flist_t *r_flistp = NULL;
  r_flistp = bs_AccountInfo::getService_and_BalGrpPoids(pinp, account_pdp, service_str);
  bal_pdp = PIN_FLIST_FLD_TAKE(r_flistp,PIN_FLD_BAL_GRP_OBJ, 1, ebufp);
  if(r_flistp) PIN_FLIST_DESTROY(r_flistp,ebufp);

  if(PIN_POID_IS_NULL(bal_pdp)) {
     PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"/balance_group poid is NULL", ebufp );
     return false;
  }

  //--- adjust------------------------------
  input_flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_PROGRAM_NAME, (void*)source.c_str(), ebufp);
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_POID,(void *) account_pdp, ebufp);
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_DESCR,  (void *)description.c_str(), ebufp);
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_CURRENCY, (void *) &dollar_currency,ebufp);
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_AMOUNT, amountp, ebufp);
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_STR_VERSION, (void *) &iversion, ebufp);
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_STRING_ID, (void *) &id, ebufp);

  /*** Inserting BAL_GRP poid ***/
  PIN_FLIST_FLD_PUT(input_flistp, PIN_FLD_BAL_GRP_OBJ, (void *) bal_pdp, ebufp);

  /*
  * Call opcode
  */
  rc = pinp->call_opcode( PCM_OP_AR_ACCOUNT_ADJUSTMENT, 0, input_flistp,
        return_flistpp, ebufp);

  PIN_FLIST_DESTROY(input_flistp, ebufp);
  return rc;
}

