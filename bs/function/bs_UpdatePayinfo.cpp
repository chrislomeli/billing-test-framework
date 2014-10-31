#include <bs/function/bs_UpdatePayinfo.h>
#include <bs/obj/bs_SignupRec.h>
#include <util/Portability.h>


bool
bs_UpdatePayinfo::updatePayinfo(
  pin_Session * pinp,
  bs_NvRecord& nv,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
  c_Decimal amount_cp;
  c_PayType paytype_cp;
  poid_t * account_pdp = NULL;
  pin_decimal_t * amountp = NULL;


  string title("enter updatePayinfo with nvRec");  nv.StringBuffer(title);
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)title.c_str() );

  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Unpacking...");
  string account_no = nv.value(ACCOUNT_NO);
  string str_poid_id0 = nv.value(ACCOUNT_OBJ_ID0);
  string str_source = nv.value(SOURCE);
  bool rc = false;


  int64 database = pinp->getDBNumber();
  ostringstream account_descr;

  ///
  /// -- translation ---------------------------
  ///

  //-- translate account poid-----------
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Locate account...");

  c_Poid account_cp;
  if ( ! nv.resolve_AccountPoid(pinp, account_cp))
  {
    pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
        PIN_ERR_NOT_FOUND,PIN_FLD_ACCOUNT_OBJ, 0, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
        (char*)"Can not locate account info", ebufp );
    return false;
  };

  //-- translate others -----------
  bs_SignupRec bsr;
  string str = nv.value(BANK_TYPE);
  paytype_cp.set( nv.value(BILL_TYPE) );
  sprintf(bsr.s_baddress, "%s\0", nv.value(BILLING_ADDRESS).c_str());
  sprintf(bsr.s_bcity,  "%s\0",nv.value(BILLING_CITY).c_str());
  sprintf(bsr.s_bstate, "%s\0",nv.value(BILLING_STATE).c_str());
  sprintf(bsr.s_bzip,   "%s\0",nv.value(BILLING_ZIPCODE).c_str());
  sprintf(bsr.s_debitNum,   "%s\0",nv.value(DEBIT_NUM).c_str());
  sprintf(bsr.s_debitAttr,  "%s\0",nv.value(DEBIT_ATTR).c_str());
  if ( !str.empty() )
    bsr.n_bankType = atoi( str.c_str());

  ///
  /// -- call base function ---------------------------
  ///
  PIN_ERR_CLEAR_ERR(ebufp);

  rc = bs_UpdatePayinfo::updatePayinfo(
    pinp, account_cp.borrow(), &bsr, str_source,
    return_flistpp, ebufp );

  return rc;
};




//****************************************************
// updatePayinfo
// Pre:
// Post:
//
bool bs_UpdatePayinfo::updatePayinfo(
  pin_Session * pinp,
  poid_t * account_pdp,
  bs_SignupRec * bsrp,
  string source,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
    pin_flist_t * p_flistp = NULL;
    pin_flist_t * t1_flistp = NULL;
    pin_flist_t * t2_flistp = NULL;
  poid_t    * pay_pdp = NULL;
  void *  vp = NULL;
  int32 flags = 0;
  char name_buffer[255];

  /* source name */
  const string this_program("updatePayinfo");
  if ( source.empty() )
  {
    source.assign(this_program);
  }
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)this_program.c_str() );


  //--- get appropriate fields ----------
  memset(name_buffer,0,255);
  char * firstname  = bsrp->s_firstName;
  char * lastname   = bsrp->s_lastName;
  char * address    = bsrp->s_baddress;
  char * city     = bsrp->s_bcity;
  char * state    = bsrp->s_bstate;
  char * zip      = bsrp->s_bzip;
  char * debit_num  = bsrp->s_debitNum;
  char * debit_attr = bsrp->s_debitAttr;
  int bank_type   = bsrp->n_bankType;
  pin_pay_type_t pay_type   = (pin_pay_type_t)bsrp->n_billType;
  const char * country    = "USA";
  const char * program_name = source.c_str();
  const char * descr  = source.c_str();

  sprintf(name_buffer,"%s %s", firstname, lastname );

  PIN_ERR_CLEAR_ERR(ebufp);

  /* create flist */
  p_flistp = PIN_FLIST_CREATE( ebufp );

  /* top level fields */
  PIN_FLIST_FLD_SET(p_flistp, PIN_FLD_POID,(void *)account_pdp, ebufp);
  PIN_FLIST_FLD_SET(p_flistp, PIN_FLD_PROGRAM_NAME,(void *)program_name, ebufp);
  PIN_FLIST_FLD_SET(p_flistp, PIN_FLD_DESCR,(void *)descr, ebufp);

  /* payinfo */

  t1_flistp = PIN_FLIST_ELEM_ADD(p_flistp, PIN_FLD_PAYINFO, 1, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_PAY_TYPE,(void *)&pay_type, ebufp);


  switch (pay_type) {
  case PIN_PAY_TYPE_DD:
    pay_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)PIN_OBJ_TYPE_PAYINFO_DD, (int64)-1, ebufp);
    PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,(void *)pay_pdp, ebufp);
    t2_flistp = PIN_FLIST_SUBSTR_ADD(t1_flistp, PIN_FLD_INHERITED_INFO, ebufp);
      t1_flistp = PIN_FLIST_ELEM_ADD(t2_flistp, PIN_FLD_DD_INFO, 0, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_DEBIT_NUM,(void *)debit_num, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_BANK_NO,(void *)debit_attr, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_TYPE,(void *)&bank_type, ebufp);
      /* common to cc and dd */
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_NAME,(void *)name_buffer, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ADDRESS,(void *)address, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_CITY,(void *)city, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_STATE,(void *)state, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ZIP,(void *)zip, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_COUNTRY,(void *)country, ebufp);
    break;

  case PIN_PAY_TYPE_CC:
    pay_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)PIN_OBJ_TYPE_PAYINFO_CC, (int64)-1, ebufp);
    PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,(void *)pay_pdp, ebufp);
    t2_flistp = PIN_FLIST_SUBSTR_ADD(t1_flistp, PIN_FLD_INHERITED_INFO, ebufp);
      t1_flistp = PIN_FLIST_ELEM_ADD(t2_flistp, PIN_FLD_CC_INFO, 0, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_DEBIT_NUM,(void *)debit_num, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_DEBIT_EXP,(void *)debit_attr, ebufp);
      /* common to cc and dd */
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_NAME,(void *)name_buffer, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ADDRESS,(void *)address, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_CITY,(void *)city, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_STATE,(void *)state, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ZIP,(void *)zip, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_COUNTRY,(void *)country, ebufp);
    break;

  case PIN_PAY_TYPE_INVOICE:
    pay_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)PIN_OBJ_TYPE_PAYINFO_INVOICE, (int64)-1, ebufp);
    PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,(void *)pay_pdp, ebufp);
    t2_flistp = PIN_FLIST_SUBSTR_ADD(t1_flistp, PIN_FLD_INHERITED_INFO, ebufp);
      t1_flistp = PIN_FLIST_ELEM_ADD(t2_flistp, PIN_FLD_INV_INFO, 0, ebufp);
      /* common to cc and dd and inv */
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_NAME,(void *)name_buffer, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ADDRESS,(void *)address, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_CITY,(void *)city, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_STATE,(void *)state, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ZIP,(void *)zip, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_COUNTRY,(void *)country, ebufp);
    break;

  case PIN_PAY_TYPE_LEC:
    pay_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)PIN_OBJ_TYPE_PAYINFO_LEC, (int64)-1, ebufp);
    PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,(void *)pay_pdp, ebufp);
    t2_flistp = PIN_FLIST_SUBSTR_ADD(t1_flistp, PIN_FLD_INHERITED_INFO, ebufp);
      t1_flistp = PIN_FLIST_ELEM_ADD(t2_flistp, UNTD_FLD_LEC_INFO, 0, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_PHONE,(void *)debit_num, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_COUNTRY,(void *)country, ebufp);
    break;

  case PIN_PAY_TYPE_PREPAID:
  case PIN_PAY_TYPE_DEBIT:
  case PIN_PAY_TYPE_DDEBIT:
  case PIN_PAY_TYPE_SMARTC:
  case PIN_PAY_TYPE_SUBORD:
  case PIN_PAY_TYPE_BETA:
  case PIN_PAY_TYPE_INTERNAL:
  case PIN_PAY_TYPE_GUEST:
  case PIN_PAY_TYPE_CASH:
  case PIN_PAY_TYPE_CHECK:
  case PIN_PAY_TYPE_WTRANSFER:
  case PIN_PAY_TYPE_PAYORDER:
  case PIN_PAY_TYPE_POSTALORDER:
    pay_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/payinfo", (int64)-1, ebufp);
    PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,(void *)pay_pdp, ebufp);
    break;

  case PIN_PAY_TYPE_UNDEFINED:
  default:
    pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
          PIN_ERR_BAD_VALUE,PIN_FLD_PAY_TYPE, pay_type, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
      (char*)"Attempting to set to an unknown bill type" , ebufp );
    return false;
  }

  PIN_ERR_CLEAR_ERR(ebufp);

  bool rc = pinp->call_opcode(
    PCM_OP_CUST_POL_UOL_SET_PAYINFO, flags, p_flistp, return_flistpp, ebufp);

  if (p_flistp) PIN_FLIST_DESTROY(p_flistp,ebufp);


  //----------- Set Billinfo as well ---------------------------------
  /*if (rc && SET_BILLINFO) {
  pin_flist_t * b_flistp = NULL;
  pin_flist_t * b_outflistp = NULL;
  b_flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_SET(b_flistp, PIN_FLD_POID, account_pdp, ebufp);
  PIN_FLIST_FLD_SET(b_flistp, PIN_FLD_PROGRAM_NAME, (void*)program_name, ebufp);
  t1_flistp = PIN_FLIST_ELEM_ADD(b_flistp, PIN_FLD_BILLINFO, 0, ebufp);
    PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_PAY_TYPE, &pay_type, ebufp);

        rc = pinp->call_opcode(
                PCM_OP_CUST_SET_BILLINFO, flags, b_flistp, &b_outflistp, ebufp);

  PIN_FLIST_DESTROY_EX(&b_flistp,NULL);
  PIN_FLIST_DESTROY_EX(&b_outflistp,NULL);
  }*/
  return rc;
}

