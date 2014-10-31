#include <bs/function/bs_UpdateContactInfo.h>
#include <bs/obj/bs_SignupRec.h>
#include <util/Portability.h>


bool
bs_UpdateContactInfo::updateContactInfo(
  pin_Session * pinp,
  bs_NvRecord& nv,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
  c_Decimal amount_cp;
  poid_t * account_pdp = NULL;
  pin_decimal_t * amountp = NULL;

  string account_no = nv.value(ACCOUNT_NO);
  string str_poid_id0 = nv.value(ACCOUNT_OBJ_ID0);
  string str_source = nv.value(SOURCE);
  bool rc = false;


  int64 database = pinp->getDBNumber();
  ostringstream account_descr;

  ///
  /// -- translation ---------------------------
  ///
  string title("updateContactInfo nvRec");
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,
    (char*)nv.StringBuffer(title).c_str());

  //-- translate account poid-----------
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
  bs_SignupRec bsr;
  sprintf(bsr.s_caddress, "%s\0", nv.value(CONTACT_ADDRESS).c_str());
  sprintf(bsr.s_ccity,  "%s\0",nv.value(CONTACT_CITY).c_str());
  sprintf(bsr.s_cstate, "%s\0",nv.value(CONTACT_STATE).c_str());
  sprintf(bsr.s_czip,   "%s\0",nv.value(CONTACT_ZIPCODE).c_str());
  sprintf(bsr.s_lastName, "%s\0",nv.value(CONTACT_LASTNAME).c_str());
  sprintf(bsr.s_firstName, "%s\0",  nv.value(CONTACT_FIRSTNAME).c_str());
  sprintf(bsr.s_cemail_addr,  "%s\0",nv.value(CONTACT_EMAIL).c_str());
  sprintf(bsr.s_evePhone, "%s\0", nv.value(EVENING_PHONE).c_str());
  sprintf(bsr.s_dayPhone, "%s\0", nv.value(DAY_PHONE).c_str());

  ///
  /// -- call base function ---------------------------
  ///
  PIN_ERR_CLEAR_ERR(ebufp);

  rc = bs_UpdateContactInfo::updateContactInfo(
    pinp, account_cp.borrow(), &bsr, str_source, return_flistpp, ebufp );

  return rc;
};



//****************************************************
// update_contactinfo
// Pre:
// Post:
//
bool bs_UpdateContactInfo::updateContactInfo(
  pin_Session * pinp,
  poid_t * account_pdp,
  bs_SignupRec * bsrp,
  string source,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
    pin_flist_t * c_outflistp = NULL;
    pin_flist_t * c_flistp = NULL;
    pin_flist_t * t1_flistp = NULL;
    pin_flist_t * t2_flistp = NULL;
    pin_flist_t * outflistp = NULL;
  void *  vp = NULL;
  int32 flags = 0;

  /* source name */
  /* source / desc name */
  const string this_program("bs_UpdateContactInfo::updateContactInfo");
  if ( source.empty() )
  {
    source.assign(this_program);
  }
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)this_program.c_str() );


  //--- get appropriate fields ----------
  char * firstname  = bsrp->s_firstName;
  char * lastname   = bsrp->s_lastName;
  char * address    = bsrp->s_caddress;
  char * city   = bsrp->s_ccity;
  char * state    = bsrp->s_cstate;
  char * zip    = bsrp->s_czip;
  char * email_addr = bsrp->s_cemail_addr;
  int32 contact_type  = PIN_NAMEINFO_BILLING;
  const char * country    = "USA";
  const char * program_name = source.c_str();


  PIN_ERR_CLEAR_ERR(ebufp);

  /* create flist */
  c_flistp = PIN_FLIST_CREATE( ebufp );

  /* top level fields */
  PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_POID,(void *)account_pdp, ebufp);
  PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_PROGRAM_NAME,(void *)program_name, ebufp);

  /* nameinfo */
  t1_flistp = PIN_FLIST_ELEM_ADD(c_flistp, PIN_FLD_NAMEINFO, contact_type, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_FIRST_NAME,(void *)firstname, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_LAST_NAME,(void *)lastname, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ADDRESS,(void *)address, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_CITY,(void *)city, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_STATE,(void *)state, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ZIP,(void *)zip, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_COUNTRY,(void *)country, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_EMAIL_ADDR,(void *)email_addr, ebufp);

  PIN_ERR_CLEAR_ERR(ebufp);

  bool rc = pinp->call_opcode(
    PCM_OP_CUST_POL_UOL_SET_NAMEINFO, flags, c_flistp, &c_outflistp, ebufp);

  if (c_flistp) PIN_FLIST_DESTROY(c_flistp,ebufp);
  if (c_outflistp) PIN_FLIST_DESTROY_EX(&c_outflistp,ebufp);
  return rc;
}
