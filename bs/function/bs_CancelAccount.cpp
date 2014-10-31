#include <bs/function/bs_CancelAccount.h>
#include <util/Portability.h>

bool
bs_CancelAccount::cancelAccount(
  pin_Session * pinp,
  bs_NvRecord& nv,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
  c_Poid service_cp;
  c_Status  status_cp;
  poid_t * service_pdp = NULL;
  pin_decimal_t * amountp = NULL;
  bool rc = false;
  char buff[255];

  int64 database = pinp->getDBNumber();
  ostringstream account_descr;

  string title("cancelAccount nvRec");  nv.StringBuffer(title);
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)title.c_str() );

  ///
  /// -- translation ---------------------------
  ///
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"Unpacking..." );
  string str_account_obj_id0 = nv.value(ACCOUNT_OBJ_ID0);
  string account_no   = nv.value(ACCOUNT_NO);
  string str_source = nv.value(SOURCE);
  string str_descr  = nv.value(DESCRIPTION);

  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"Translating status..." );

  //-- translate account poid-----------
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"Locating account..." );

  c_Poid account_cp;
  if ( ! nv.resolve_AccountPoid(pinp, account_cp))
  {
    sprintf(buff, "Cannot locate account %s ...", str_account_obj_id0.c_str() );
    pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
        PIN_ERR_NOT_FOUND,PIN_FLD_ACCOUNT_OBJ, 0, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
        (char*)buff, ebufp );
    return false;
  };


  ///
  /// -- call base function ---------------------------
  ///
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"Calling base function..." );

  PIN_ERR_CLEAR_ERR(ebufp);

  rc = bs_CancelAccount::cancelAccount(
    pinp,
    account_cp.borrow(),
    str_source,
    str_descr,
    return_flistpp, ebufp );

  return rc;
};




//****************************************************
// cancelAccount
// Pre:
// Post:
//
bool bs_CancelAccount::cancelAccount(
  pin_Session * pinp,
  poid_t * account_pdp,
  string source,
  string description,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
  pin_flist_t * x_flistp = NULL;
  pin_flist_t * d_flistp = NULL;
  pin_flist_t * f_flistp = NULL;
  pin_flist_t * f_outflistp = NULL;
  pin_errbuf_t  localebuf;
  poid_t * pdp = NULL;
  void * vp = NULL;

  const string this_program("cancelAccount");
  if ( source.empty() )
  {
    source.assign(this_program);
  }
  if ( description.empty() )
  {
    description.assign(this_program);
  }

  PIN_ERR_CLEAR_ERR(ebufp);

  //--enter function--------------
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)this_program.c_str() );


  /*--- Create set_status ---*/
  x_flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_SET(x_flistp, PIN_FLD_POID, account_pdp, ebufp);
  PIN_FLIST_FLD_SET(x_flistp, PIN_FLD_PROGRAM_NAME, (void*)source.c_str(), ebufp);
  PIN_FLIST_FLD_SET(x_flistp, PIN_FLD_DESCR, (void*)description.c_str(), ebufp);

  bool rc = pinp->call_opcode(
    UOL_OP_CANCEL_ACCOUNT, 0, x_flistp, return_flistpp, ebufp);

  PIN_ERR_LOG_FLIST(
    PIN_ERR_LEVEL_DEBUG,(char*)"leaving bs_CancelAccount::cancelAccount" , *return_flistpp );

  if (x_flistp) PIN_FLIST_DESTROY(x_flistp,NULL);
  return rc;
}

