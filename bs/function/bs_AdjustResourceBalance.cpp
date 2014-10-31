#include "bs_AdjustResourceBalance.h"
#include <util/Portability.h>


bool
bs_AdjustResourceBalance::adjust_ResourceBalance(
  pin_Session * pinp,
  bs_NvRecord& nv,
  pin_flist_t   **return_flistpp,
  bool    is_limit,
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
  string service_str = nv.value(SERVICE_OBJ_TYPE);

  bool rc = false;
  int64 database = pinp->getDBNumber();
  ostringstream account_descr;


  ///
  /// -- translation ---------------------------
  ///
  string title("adjust_ResourceBalance nvRec");  nv.StringBuffer(title);
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)title.c_str() );

  //-- translate account poid-----------
  c_Poid account_cp;
  if ( ! nv.resolve_AccountPoid(pinp, account_cp))
  {
    pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
        PIN_ERR_NOT_FOUND,PIN_FLD_ACCOUNT_OBJ, 0, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
        (char*)"Can not locate account...", ebufp );
    return false;
  };


  //-- translate others -----------
  if (! amount_cp.create( str_amount.c_str() ) ||  amount_cp.isnull() ) {
    if (! PIN_ERR_IS_ERR(ebufp))
      pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
            PIN_ERR_BAD_VALUE,PIN_FLD_AMOUNT, 0, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)"Can not convert amount ... ", ebufp );
    return false;
  }
  int resource_id = atoi( str_resource.c_str() );

  ///
  /// -- call base function ---------------------------
  ///
  PIN_ERR_CLEAR_ERR(ebufp);
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,
        (char*)"Callling base function..." );
  rc = adjust_ResourceBalance( pinp,
    account_cp.borrow(), amount_cp.borrow(), resource_id,
    str_source,  service_str,return_flistpp, is_limit,ebufp );

  return rc;
};


//****************************************************
// adjust_ResourceBalance
// Pre:
// Post:
//
bool bs_AdjustResourceBalance::adjust_ResourceBalance(
  pin_Session * pinp,
  poid_t * account_pdp,
  pin_decimal_t * amountp,
  int element_id,
  string source,
  string &service_str,
  pin_flist_t   **return_flistpp,
  bool is_limit,
  pin_errbuf_t  *ebufp )
{
  pin_flist_t   *input_flistp = NULL;
  pin_flist_t   *output_flistp = NULL;
  pin_flist_t   *d_flistp = NULL;
  bool rc = false;
  ostringstream os;
  poid_t *bal_pdp = NULL;

  PIN_ERR_CLEAR_ERR(ebufp);

  //--- setup --------------------------
  /* source / desc name */
  const string this_program("adjust_ResourceBalance");
  if ( source.empty() )
  {
    source.assign(this_program);
  }
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)this_program.c_str() );

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
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_POID, account_pdp, ebufp);
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_PROGRAM_NAME, (void*)source.c_str(), ebufp);

  /*** Inserting BAL_GRP poid ***/
  PIN_FLIST_FLD_PUT(input_flistp, PIN_FLD_BAL_GRP_OBJ, bal_pdp, ebufp);

  if(is_limit) {
        d_flistp = PIN_FLIST_ELEM_ADD( input_flistp, PIN_FLD_LIMIT, element_id, ebufp );
                PIN_FLIST_FLD_SET(d_flistp, PIN_FLD_CREDIT_LIMIT, (void*)amountp, ebufp);
    /*--- Update Limit---*/
    pinp->call_opcode(
       PCM_OP_BILL_SET_LIMIT, 0, input_flistp, return_flistpp, ebufp);
  }else {

    d_flistp = PIN_FLIST_ELEM_ADD( input_flistp, PIN_FLD_DEBIT, element_id, ebufp );
      PIN_FLIST_FLD_SET(d_flistp, PIN_FLD_BAL_OPERAND, (void*)amountp, ebufp);

    /*--- Create debits.---*/
    pinp->call_opcode(
      PCM_OP_BILL_DEBIT, 0, input_flistp, return_flistpp, ebufp);
  }

  if (input_flistp) PIN_FLIST_DESTROY(input_flistp,ebufp);

  return rc;
}



