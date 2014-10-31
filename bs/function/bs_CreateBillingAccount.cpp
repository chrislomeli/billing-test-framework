#include "bs_CreateBillingAccount.h"
#include <util/Portability.h>


/*******************************************************************
 * intialize deal cache
 *******************************************************************/
bs_DealCache * bs_CreateBillingAccount::deal_cache = new bs_DealCache();


/*******************************************************************
 * create_customer()
 *******************************************************************/
bool bs_CreateBillingAccount::createAccount(
  pin_Session * pinp,
  string source,
  bs_SignupRec bsr )
{
  /* misc decaration */
  pin_flist_t * c_flistp = NULL;
  pin_flist_t * p_flistp = NULL;
    pin_flist_t * t1_flistp = NULL;
    pin_flist_t * t2_flistp = NULL;
    pin_flist_t * outflistp = NULL;
  pin_errbuf_t ebuf;
  pin_errbuf_t * ebufp = &ebuf;
  poid_t *pay_pdp = NULL;

  char name_buffer[255];
  memset(name_buffer,0,255);
  sprintf(name_buffer,"%s %s", bsr.s_firstName, bsr.s_lastName );

  int32 contact_type      = PIN_NAMEINFO_BILLING;
  int bank_type       = bsr.n_bankType;
  int currency        = 840;
  int cycle_dom       = bsr.n_cycleDom;
  int setstatus       = 10100;
  int s_flags         = 0;
  pin_pay_type_t pay_type = (pin_pay_type_t)bsr.n_billType;
  const char * country    = "USA";
  const char * locale     = "en_US";
  ostringstream os;

  char login[255];

  PIN_ERR_CLEAR_ERR(ebufp);

  /* source name */
  os << source << "." << getuser(ENV_USER) << ".bs_CreateBillingAccount";
  source.assign(os.str());


  /* format login */

  if ( strlen( bsr.s_domain ) > 0 ) {
    sprintf(login,"%s@%s\0", bsr.s_accountId, bsr.s_domain  );
  } else {
    sprintf(login,"%s\0",  bsr.s_accountId );
  }

  /* create a plan flist - set deals */
  vector<int> deal_list;
  string deal_str(bsr.s_addDeals);
  util::tokenize(bsr.s_addDeals,deal_list,",;: ");

  if ( !deal_cache->initialize(pinp) )
  {
    cout << "Can not initialize deal cache ..." << endl;
    return false;
  }

  deal_cache->createPlanFlist(
      login, /* root id for login names, etc */
      deal_list,
      &c_flistp);

  PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
        (char*)"DEBUG-100:: flist from create_plan_flist", c_flistp );

  if ( c_flistp == NULL ) {
    cout << "Can not create initial plan flist ..." << endl;
    return false;
  }

  //---cleanup and prep ----------------------
  if (strlen(bsr.s_caddress) == 0 &&
    strlen(bsr.s_cstate) == 0 &&
    strlen(bsr.s_czip) == 0 &&
    strlen(bsr.s_czip) == 0  )
  {
    sprintf(bsr.s_caddress, "%s\0",DEFAULT_ADDRESS);
    sprintf(bsr.s_cstate, "%s\0",DEFAULT_STATE);
    sprintf(bsr.s_czip, "%s\0",DEFAULT_ZIP);
    sprintf(bsr.s_ccity, "%s\0",DEFAULT_CITY);
  }
  if (strlen(bsr.s_baddress) == 0 &&
    strlen(bsr.s_bstate) == 0 &&
    strlen(bsr.s_bzip) == 0 &&
    strlen(bsr.s_bzip) == 0  )
  {
    sprintf(bsr.s_baddress, "%s\0",bsr.s_caddress);
    sprintf(bsr.s_bstate, "%s\0",bsr.s_cstate);
    sprintf(bsr.s_bzip, "%s\0",bsr.s_czip);
    sprintf(bsr.s_bcity, "%s\0",bsr.s_ccity);
  }


  /* drop useless fields */
  PIN_FLIST_FLD_DROP(c_flistp,PIN_FLD_CREATED_T,ebufp);
  PIN_FLIST_FLD_DROP(c_flistp,PIN_FLD_MOD_T,ebufp);

  /* top level fields */
  PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_NAME,(void *)bsr.s_accountId, ebufp);
  PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_ACCOUNT_NO,    (void *)bsr.s_accountId, ebufp);
  PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_AAC_PROMO_CODE,    (void *)bsr.s_accountId, ebufp);
  PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_AAC_SOURCE,(void *)"bs_CreateBillingAccount::createAccount", ebufp);


  /*** Inserting ACCTINFO : by mvamsee ***/
  /*poid_t *account_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/account", (int64)-1, ebufp);
  t1_flistp = PIN_FLIST_ELEM_ADD(c_flistp, PIN_FLD_ACCTINFO, 1, ebufp);
  PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,    (void *)account_pdp, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ACCOUNT_NO,    (void *)bsr.s_accountId, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_AAC_SOURCE,(void *)"bs_CreateBillingAccount::createAccount", ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_CURRENCY,      (void *)&currency, ebufp);*/

  /*** Inserting BAL_INFO: by mvamsee ***/
  /*poid_t *balgrp_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/balance_group", (int64)-1, ebufp);
  t1_flistp = PIN_FLIST_ELEM_ADD(c_flistp, PIN_FLD_BAL_INFO, 1, ebufp);
  PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,    (void *)balgrp_pdp, ebufp);*/

  /* nameinfo */
  t1_flistp = PIN_FLIST_ELEM_ADD(c_flistp, PIN_FLD_NAMEINFO, contact_type, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_FIRST_NAME,    (void *)bsr.s_firstName, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_LAST_NAME,     (void *)bsr.s_lastName, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ADDRESS,     (void *)bsr.s_caddress, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_CITY,        (void *)bsr.s_ccity, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_STATE,       (void *)bsr.s_cstate, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ZIP,       (void *)bsr.s_czip, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_COUNTRY,     (void *)country, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_EMAIL_ADDR,    (void *)bsr.s_cemail_addr, ebufp);

  /* billinfo */
  t1_flistp = PIN_FLIST_ELEM_ADD(c_flistp, PIN_FLD_BILLINFO, 1, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_PAY_TYPE,     (void *)&pay_type, ebufp);  // :mvamsee
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_MERCHANT,      (void *)bsr.s_brandName, ebufp); 
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_CURRENCY,      (void *)&currency, ebufp);

  /*if (cycle_dom > 0 && cycle_dom < 28)
    PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ACTG_FUTURE_DOM,(void *)&cycle_dom, ebufp);*/



  /* actginfo */
  //t1_flistp = PIN_FLIST_ELEM_ADD(c_flistp, PIN_FLD_ACTGINFO, 1, ebufp); // No longer required :mvamsee


  /* statuses */
  /*t1_flistp = PIN_FLIST_ELEM_ADD(c_flistp, PIN_FLD_STATUSES, 1, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_STATUS,        (void *)&setstatus, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_STATUS_FLAGS,      (void *)&s_flags, ebufp);*/

  /* locales */
  t1_flistp = PIN_FLIST_ELEM_ADD(c_flistp, PIN_FLD_LOCALES, 1, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_LOCALE, (void *)locale, ebufp);

  /* payinfo */
  t1_flistp = PIN_FLIST_ELEM_ADD(c_flistp, PIN_FLD_PAYINFO, 1, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_PAY_TYPE,(void *)&pay_type, ebufp);

  switch (pay_type) {
  case PIN_PAY_TYPE_DD:
    pay_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)PIN_OBJ_TYPE_PAYINFO_DD, (int64)-1, ebufp);
    PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,(void *)pay_pdp, ebufp);
    t2_flistp = PIN_FLIST_SUBSTR_ADD(t1_flistp, PIN_FLD_INHERITED_INFO, ebufp);
      t1_flistp = PIN_FLIST_ELEM_ADD(t2_flistp, PIN_FLD_DD_INFO, 0, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_DEBIT_NUM,(void *)bsr.s_debitNum, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_BANK_NO,(void *)bsr.s_debitAttr, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_TYPE,(void *)&bank_type, ebufp);
      /* common to cc and dd */
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_NAME,(void *)name_buffer, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ADDRESS,(void *)bsr.s_caddress, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_CITY,(void *)bsr.s_ccity, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_STATE,(void *)bsr.s_cstate, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ZIP,(void *)bsr.s_czip, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_COUNTRY,(void *)country, ebufp);
    break;

  case PIN_PAY_TYPE_CC:
    pay_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)PIN_OBJ_TYPE_PAYINFO_CC, (int64)-1, ebufp);
    PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,(void *)pay_pdp, ebufp);
    t2_flistp = PIN_FLIST_SUBSTR_ADD(t1_flistp, PIN_FLD_INHERITED_INFO, ebufp);
      t1_flistp = PIN_FLIST_ELEM_ADD(t2_flistp, PIN_FLD_CC_INFO, 0, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_DEBIT_NUM,(void *)bsr.s_debitNum, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_DEBIT_EXP,(void *)bsr.s_debitAttr, ebufp);
      /* common to cc and dd */
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_NAME,(void *)name_buffer, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ADDRESS,(void *)bsr.s_baddress, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_CITY,(void *)bsr.s_bcity, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_STATE,(void *)bsr.s_bstate, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ZIP,(void *)bsr.s_bzip, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_COUNTRY,(void *)country, ebufp);
    break;

  case PIN_PAY_TYPE_INVOICE:
    pay_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)PIN_OBJ_TYPE_PAYINFO_INVOICE, (int64)-1, ebufp);
    PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,(void *)pay_pdp, ebufp);
    t2_flistp = PIN_FLIST_SUBSTR_ADD(t1_flistp, PIN_FLD_INHERITED_INFO, ebufp);
      t1_flistp = PIN_FLIST_ELEM_ADD(t2_flistp, PIN_FLD_INV_INFO, 0, ebufp);
      /* common to cc and dd and inv */
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_NAME,(void *)name_buffer, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ADDRESS,(void *)bsr.s_baddress, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_CITY,(void *)bsr.s_bcity, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_STATE,(void *)bsr.s_bstate, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ZIP,(void *)bsr.s_bzip, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_COUNTRY,(void *)country, ebufp);
    break;

  case PIN_PAY_TYPE_LEC:
    pay_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)PIN_OBJ_TYPE_PAYINFO_LEC, (int64)-1, ebufp);
    PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,(void *)pay_pdp, ebufp);
    t2_flistp = PIN_FLIST_SUBSTR_ADD(t1_flistp, PIN_FLD_INHERITED_INFO, ebufp);
      t1_flistp = PIN_FLIST_ELEM_ADD(t2_flistp, UNTD_FLD_LEC_INFO, 0, ebufp);
      PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_PHONE,(void *)bsr.s_debitNum, ebufp);
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
  case PIN_PAY_TYPE_UNDEFINED:
  default:
    pay_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/payinfo", (int64)-1, ebufp);
    PIN_FLIST_FLD_PUT(t1_flistp, PIN_FLD_POID,(void *)pay_pdp, ebufp);
    break;

  }

  PIN_ERR_CLEAR_ERR(ebufp);

  pinp->call_opcode( PCM_OP_CUST_POL_UOL_COMMIT_CUSTOMER, s_flags, c_flistp, &outflistp, ebufp);

  if (PIN_ERR_IS_ERR(ebufp))
  {
    if (c_flistp) PIN_FLIST_DESTROY(c_flistp,ebufp);
    if (outflistp)  PIN_FLIST_DESTROY_EX(&outflistp,ebufp);
    return false;

  }


  if (c_flistp)
    PIN_FLIST_DESTROY(c_flistp,ebufp);
  if (outflistp)
    PIN_FLIST_DESTROY_EX(&outflistp,ebufp);

  return true;
}

