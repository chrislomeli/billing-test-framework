#include "bs_AccountInfo.h"

bs_AccountInfo::bs_AccountInfo(void) : account_pdp(NULL)
{
  reset();
}

bs_AccountInfo::~bs_AccountInfo(void)
{
  reset();
}

/*******************************************************************
 * uol::get_account_balance():
 *
 *******************************************************************/
pin_decimal_t *
bs_AccountInfo::get_AccountBalance( pin_Session * pinp  )
{
  if ( pin_poid_is_null(this->account_pdp) )
    return NULL;

  return bs_AccountInfo::get_AccountBalance(pinp, this->account_pdp );
}

/*******************************************************************
 * uol::get_account_balance():
 *
 *******************************************************************/
pin_decimal_t *
bs_AccountInfo::get_AccountBalance( pin_Session * pinp, poid_t * a_pdp, int resource  )
{ pin_flist_t*  s_flistp = NULL;
  pin_flist_t * out_flistp = NULL;
  pin_flist_t * t1_flistp = NULL,
              * t2_flistp = NULL,
              * t3_flistp = NULL;
  pin_errbuf_t  ebuf;
  pin_errbuf_t  * ebufp = &ebuf;
  pin_decimal_t *amountp = NULL,*tmp_amountp = NULL;
  pin_flist_t*  bal_flistp = NULL;
  poid_t *search_pdp = NULL;
  int32 s_flags = SRCH_DISTINCT;
  const char* sql_stmt = "select X from /balance_group where F1 = V1 ";

  /*-- housekeeping ---*/
  PIN_ERR_CLEAR_ERR(ebufp);

  if ( pin_poid_is_null(a_pdp) )
    return NULL;

  amountp = pbo_decimal_from_str("0.0",ebufp);
  /*** Searching ***/

  s_flistp = pin_flist_create(ebufp);
  search_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/search", (int64)-1, ebufp);
  PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID,(void *)search_pdp, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, &s_flags, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void*)sql_stmt, ebufp);

  /*** Arguments ***/
  t1_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ACCOUNT_OBJ, (void*)a_pdp, ebufp);

  /*** Results ***/
  t1_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_RESULTS, 0, ebufp);
  t2_flistp = PIN_FLIST_ELEM_ADD(t1_flistp, PIN_FLD_BALANCES,resource, ebufp);
  t3_flistp = PIN_FLIST_ELEM_ADD(t2_flistp, PIN_FLD_SUB_BALANCES,PIN_ELEMID_ANY, ebufp);
  PIN_FLIST_FLD_SET(t3_flistp, PIN_FLD_CURRENT_BAL, (void*)NULL, ebufp);

  pinp->call_opcode( PCM_OP_SEARCH,0,s_flistp, &out_flistp, ebufp );
  if(s_flistp) PIN_FLIST_DESTROY(s_flistp,ebufp);

  pin_cookie_t cookie1=NULL, cookie2=NULL;
  int32 elem_id1 =0, elem_id2 = 0;
  /*** Iterating over RESULTS Array ***/
  while((t1_flistp = PIN_FLIST_ELEM_GET_NEXT(out_flistp, PIN_FLD_RESULTS,
            &elem_id1, 1, &cookie1, ebufp)) != NULL) {

     bal_flistp = (pin_flist_t *) PIN_FLIST_ELEM_GET(t1_flistp, PIN_FLD_BALANCES, resource, 1, ebufp);

     cookie2 = NULL;
     elem_id2 = 0;
     while((t2_flistp = PIN_FLIST_ELEM_GET_NEXT(bal_flistp, PIN_FLD_SUB_BALANCES,
            &elem_id2, 1, &cookie2, ebufp)) != NULL) {
         tmp_amountp = (pin_decimal_t *) PIN_FLIST_FLD_GET( t2_flistp, PIN_FLD_CURRENT_BAL, 1, &ebuf );

         if(!pbo_decimal_is_null(tmp_amountp,ebufp) && !pbo_decimal_is_zero(tmp_amountp,ebufp)) {
            pbo_decimal_add_assign(amountp, tmp_amountp,ebufp);
         }
     }
  }

  if ( PIN_ERR_IS_ERR(ebufp) )
    return NULL;

  if(out_flistp) PIN_FLIST_DESTROY_EX(&out_flistp,ebufp);
  return amountp;
}


/*******************************************************************
 * deleteByAccountNo():
 *******************************************************************/
bool
bs_AccountInfo::deleteByAccountNo(
  pin_Session * pinp,
  string m_account_no )
{
  pin_errbuf_t ebuf;
  pin_errbuf_t * ebufp = &ebuf;
  PIN_ERR_CLEAR_ERR(ebufp);

  if ( !loadByAccountNo(pinp, m_account_no ))
    return false;

  pin_flist_t * s_flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_POID, this->account_pdp, ebufp);

  pin_flist_t * r_flistp = NULL;
  bool rc =   pinp->call_opcode(
    PCM_OP_CUST_DELETE_ACCT, 0, s_flistp, &r_flistp, ebufp);

  if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
    if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);

  return rc;
}



/*******************************************************************
 * loadByAccountPoid():
 *******************************************************************/
bool
bs_AccountInfo::loadByAccountPoid(
  pin_Session * pinp,
  poid_t * a_pdp )
{
  pin_flist_t *flistp = NULL;
  pin_flist_t *s_flistp = NULL;
  pin_flist_t *r_flistp = NULL;
  bool rc = false;

  pin_errbuf_t    ebuf;
  pin_errbuf_t    * ebufp = &ebuf;

  PIN_ERR_CLEAR_ERR(ebufp);
  reset();

  /***********************************************************
   * Get the account poid
   ***********************************************************/
  /*
   * Search::
   */
  s_flistp = PIN_FLIST_CREATE(ebufp);

  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_POID, a_pdp, ebufp);
  //PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_MERCHANT, NULL,ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_STATUS, NULL,ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_STATUS_FLAGS, NULL,ebufp);
  //PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_BILL_TYPE, NULL,ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_ACCOUNT_NO, NULL,ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_ACCESS_CODE1, NULL,ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_AAC_PROMO_CODE, NULL,ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_CREATED_T, NULL,ebufp);
  //PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_ACTG_LAST_T, NULL,ebufp);
  //PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_ACTG_NEXT_T, NULL,ebufp);
  //PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_ACTG_FUTURE_T, NULL,ebufp);

  this->account_state = entity_DoesNotExist;

  rc = pinp->call_opcode( PCM_OP_READ_FLDS, 0, s_flistp, &r_flistp, ebufp);
  if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);

  if ( rc )
  {
    rc = this->_populate_FromFlist(r_flistp);
    this->account_state = entity_Exists;
  }

    if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
  return rc;
}


/*******************************************************************
 * loadByPromoCode():
 *******************************************************************/
bool
bs_AccountInfo::loadByPromoCode(
        pin_Session * pinp,
        string m_account_no )
{
        poid_t*         pdp=NULL;
        pin_flist_t     *flistp = NULL;
        pin_flist_t     *s_flistp = NULL;
        pin_flist_t     *r_flistp = NULL;
        pin_flist_t * t1 = NULL;
        int32           s_flags = SRCH_DISTINCT;
        const char* sp_account_no = m_account_no.c_str();
        const char* sql_stmt = "select X from /account where F1 = V1 ";
        bool rc = false;

        pin_errbuf_t    ebuf;
        pin_errbuf_t    * ebufp = &ebuf;

        PIN_ERR_CLEAR_ERR(ebufp);
        reset();

        /***********************************************************
         * Get the account poid
         ***********************************************************/
        /*
         * Search::
         */
        s_flistp = PIN_FLIST_CREATE(ebufp);

        pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/search", (int64)-1, ebufp);
        PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, pdp, ebufp);
        PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, &s_flags, ebufp);
        PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void*)sql_stmt, ebufp);

        /*
         * Args::
         */
        t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
                PIN_FLIST_FLD_SET(t1, PIN_FLD_AAC_PROMO_CODE, (void*)sp_account_no, ebufp);


        /*
         * Results::
         */
    r_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_RESULTS, 0, ebufp);
        PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_POID, NULL,ebufp);
        //PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_MERCHANT, NULL,ebufp);
        PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_STATUS, NULL,ebufp);
        PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_STATUS_FLAGS, NULL,ebufp);
        //PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_BILL_TYPE, NULL,ebufp);
        PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_ACCOUNT_NO, NULL,ebufp);
        PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_ACCESS_CODE1, NULL,ebufp);
        PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_AAC_PROMO_CODE, NULL,ebufp);
        PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_CREATED_T, NULL,ebufp);
        //PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_ACTG_LAST_T, NULL,ebufp);
        //PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_ACTG_NEXT_T, NULL,ebufp);
        //PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_ACTG_FUTURE_T, NULL,ebufp);

        this->account_state = entity_DoesNotExist;
        r_flistp = NULL;

        rc = pinp->call_opcode( PCM_OP_SEARCH, 0, s_flistp, &r_flistp, ebufp);
        if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);

        if ((flistp = (pin_flist_t*)
                PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, 1, ebufp)) != NULL )
        {
                rc = this->_populate_FromFlist(flistp);
                this->account_state = entity_Exists;
        } else {
                rc = false;
        }

    if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
        return rc;
}


/*******************************************************************
 * loadByAccountNo():
 *******************************************************************/
bool
bs_AccountInfo::loadByAccountNo(
  pin_Session * pinp,
  string m_account_no )
{
  poid_t*   pdp=NULL;
  pin_flist_t *flistp = NULL;
  pin_flist_t *s_flistp = NULL;
  pin_flist_t *r_flistp = NULL;
  pin_flist_t * t1 = NULL;
  int32   s_flags = SRCH_DISTINCT;
  const char* sp_account_no = m_account_no.c_str();
  const char* sql_stmt = "select X from /account where F1 = V1 ";
  bool rc = false;

  pin_errbuf_t    ebuf;
  pin_errbuf_t    * ebufp = &ebuf;

  PIN_ERR_CLEAR_ERR(ebufp);
  reset();

  /***********************************************************
   * Get the account poid
   ***********************************************************/
  /*
   * Search::
   */
  s_flistp = PIN_FLIST_CREATE(ebufp);

  pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/search", (int64)-1, ebufp);
  PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, pdp, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, &s_flags, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void*)sql_stmt, ebufp);

  /*
   * Args::
   */
  t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
    PIN_FLIST_FLD_SET(t1, PIN_FLD_ACCOUNT_NO, (void*)sp_account_no, ebufp);

  /*
   * Results::
   */
    r_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_RESULTS, 0, ebufp);
  PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_POID, NULL,ebufp);
  //PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_MERCHANT, NULL,ebufp);
  PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_STATUS, NULL,ebufp);
  PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_STATUS_FLAGS, NULL,ebufp);
  //PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_BILL_TYPE, NULL,ebufp);
  PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_ACCOUNT_NO, NULL,ebufp);
  PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_ACCESS_CODE1, NULL,ebufp);
  PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_AAC_PROMO_CODE, NULL,ebufp);
  PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_CREATED_T, NULL,ebufp);
  //PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_ACTG_LAST_T, NULL,ebufp);
  //PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_ACTG_NEXT_T, NULL,ebufp);
  //PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_ACTG_FUTURE_T, NULL,ebufp);

  this->account_state = entity_DoesNotExist;
  r_flistp = NULL;

  rc = pinp->call_opcode( PCM_OP_SEARCH, 0, s_flistp, &r_flistp, ebufp);
  if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);

  if ((flistp = (pin_flist_t*)
    PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, 1, ebufp)) != NULL )
  {
    rc = this->_populate_FromFlist(flistp);
    this->account_state = entity_Exists;
  } else {
    rc = false;
  }

    if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
  return rc;
}



/*******************************************************************
 * loadByPoidID():
 *******************************************************************/
bool bs_AccountInfo::loadByPoidID(
  pin_Session * pinp,
  string poid_id0_str )
{
  pin_errbuf_t ebuf;
  int64 poid_id0 = atol( poid_id0_str.c_str() );

  PIN_ERR_CLEAR_ERR(&ebuf);

  /*-- housekeeping ---*/
  poid_t * pdp = PIN_POID_CREATE( pinp->getDBNumber(), (char*)"/account", poid_id0, &ebuf );
  bool rc = this->loadByAccountPoid(pinp,pdp);

  PIN_POID_DESTROY( pdp, &ebuf );
  return rc;
}



/*******************************************************************
 * loadByPoidID():
 *******************************************************************/
bool bs_AccountInfo::loadByPoidString(
  pin_Session * pinp,
  string poid_Str )
{
  pin_flist_t*    account_flistp = NULL;
  pin_flist_t *   r_flistp = NULL;
  poid_t      * local_pdp = NULL;
  pin_errbuf_t    ebuf;
  pin_errbuf_t    * ebufp = &ebuf;
  bool      rc;
  char      * endcpp = NULL;
  ostringstream os;

  /*-- housekeeping ---*/
  PIN_ERR_CLEAR_ERR(ebufp);
  reset();

  local_pdp = PIN_POID_FROM_STR((char*)poid_Str.c_str(), &endcpp, &ebuf);

  if ( PIN_ERR_IS_ERR(&ebuf) ) {
    return false;
  }

  account_flistp = pin_flist_create(ebufp);
  PIN_FLIST_FLD_SET(account_flistp, PIN_FLD_POID,(void *)local_pdp, ebufp);
  //PIN_FLIST_FLD_SET(account_flistp, PIN_FLD_BILL_TYPE, NULL, ebufp);
  PIN_FLIST_FLD_SET(account_flistp, PIN_FLD_STATUS, (void *) NULL, ebufp);
  PIN_FLIST_FLD_SET(account_flistp, PIN_FLD_STATUS_FLAGS, (void*)NULL,ebufp);
  PIN_FLIST_FLD_SET(account_flistp, PIN_FLD_ACCOUNT_NO, (void *) NULL, ebufp);
  PIN_FLIST_FLD_SET(account_flistp, PIN_FLD_ACCESS_CODE1, NULL,ebufp);
  PIN_FLIST_FLD_SET(account_flistp, PIN_FLD_AAC_PROMO_CODE, NULL,ebufp);
  //PIN_FLIST_FLD_SET(account_flistp, PIN_FLD_MERCHANT, (void *) NULL, ebufp);

  this->account_state = entity_DoesNotExist;
  if (( rc= pinp->call_opcode(
    PCM_OP_READ_FLDS,0,account_flistp, &r_flistp, ebufp ))== true )
  {
    rc = this->_populate_FromFlist(r_flistp);
    this->account_state = entity_Exists;
  }

  if(account_flistp) PIN_FLIST_DESTROY(account_flistp,ebufp);
    if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
  pin_poid_destroy(local_pdp, ebufp);

    return rc;

}


bool
bs_AccountInfo::_populate_FromFlist( pin_flist_t * flistp )
{
  //c_BillType  m_bill_type;
  int     m_status;
  int     m_status_flags;
  bool    rc = true;
  char    *cp = NULL;
  void    *vp = NULL;
  poid_t    * local_pdp = NULL;
  pin_errbuf_t  ebuf;
  pin_errbuf_t  *ebufp = &ebuf;
  time_t    local_created_t = 0;
  time_t    local_last_t=0;
  time_t    local_next_t=0;
  time_t    local_future_t=0;

  PIN_ERR_CLEAR_ERR(ebufp);

  if ((vp= PIN_FLIST_FLD_GET( flistp, PIN_FLD_STATUS, 0, ebufp )) != NULL )
    m_status = *( (int*)vp );

  if ((vp= PIN_FLIST_FLD_GET( flistp, PIN_FLD_STATUS_FLAGS, 0, ebufp )) != NULL )
    m_status_flags = *( (int*)vp );

  if ((cp= (char*)PIN_FLIST_FLD_GET( flistp, PIN_FLD_ACCESS_CODE1, 0, ebufp )) != NULL )
    this->member_no.assign(cp);

  if ((cp= (char*)PIN_FLIST_FLD_GET( flistp, PIN_FLD_AAC_PROMO_CODE, 0, ebufp )) != NULL )
    this->promo_code.assign(cp);

  if ((cp= (char*)PIN_FLIST_FLD_GET( flistp, PIN_FLD_ACCOUNT_NO, 0, ebufp )) != NULL )
    this->account_no.assign(cp);

  //if ((cp= (char*)PIN_FLIST_FLD_GET( flistp, PIN_FLD_MERCHANT, 0, ebufp )) != NULL )
  //  this->merchant.assign(cp);

  //if ((vp= PIN_FLIST_FLD_GET( flistp, PIN_FLD_BILL_TYPE, 0, ebufp )) != NULL )
  //  if ( !bill_type.set((int*)vp) )
  //    rc=false;

  if ((vp= PIN_FLIST_FLD_GET( flistp, PIN_FLD_CREATED_T, 0, ebufp )) != NULL )
    local_created_t = *(time_t*)vp;

  //if((vp=PIN_FLIST_FLD_GET(flistp,PIN_FLD_ACTG_LAST_T,0,ebufp))!=NULL)
  //      local_last_t=*(time_t*)vp;

  //if((vp=PIN_FLIST_FLD_GET(flistp,PIN_FLD_ACTG_NEXT_T,0,ebufp))!=NULL)
  //      local_next_t=*(time_t*)vp;

  //if((vp=PIN_FLIST_FLD_GET(flistp,PIN_FLD_ACTG_FUTURE_T,0,ebufp))!=NULL)
  //      local_future_t=*(time_t*)vp;

  if ((vp= PIN_FLIST_FLD_TAKE( flistp, PIN_FLD_POID, 0, ebufp )) != NULL )
    local_pdp = (poid_t*)vp;

  if (! PIN_ERR_IS_ERR( ebufp ) && rc )
  {
    this->account_pdp = local_pdp;
    this->status.set(m_status);
    this->status_flags = m_status_flags;
    this->account_pdp = account_pdp;
    this->created_t = local_created_t;
    //this->last_t=local_last_t;
    //this->next_t=local_next_t;
    //this->future_t=local_future_t;
  } else {
    if ( ! pin_poid_is_null(local_pdp) )
      pin_poid_destroy(local_pdp, ebufp);
  }
  return rc;

}

/*string& bs_AccountInfo::getMerchant(string& m)
{
  m.assign(this->merchant);
  return m;
}*/

string& bs_AccountInfo::getAccountNo(string& m)
{
  m.assign(this->account_no);
  return m;
}

/*** Callers should check for NULL_ENUM ***/
int bs_AccountInfo::getPayType(pin_Session * pinp) {

  int payType = bs_AccountInfo::getPayType(pinp, this->account_pdp);
  if(!this->pay_type.set(payType)) {
    return NULL_ENUM;
  }
  return this->pay_type.get();
}

/*** Callers should check for NULL_ENUM ***/
int bs_AccountInfo::getPayType(pin_Session * pinp, poid_t *a_pdp)
{
  if(a_pdp == NULL || pin_poid_is_null(a_pdp)) {
    return NULL_ENUM;
  }
  c_PayType payType;
  pin_flist_t * s_flistp = NULL;
  pin_flist_t * out_flistp = NULL;
  pin_flist_t * t1_flistp = NULL;
  pin_errbuf_t  ebuf;
  pin_errbuf_t  * ebufp = &ebuf;
  poid_t *search_pdp = NULL,*tmp_pdp = NULL;
  int32 s_flags = SRCH_DISTINCT;
  const char* sql_stmt = "select X from /billinfo where F1 = V1 ";

  /*-- housekeeping ---*/
  PIN_ERR_CLEAR_ERR(ebufp);

  /*** Searching ***/

  s_flistp = pin_flist_create(ebufp);
  search_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/search", (int64)-1, ebufp);
  PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID,(void *)search_pdp, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, &s_flags, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void*)sql_stmt, ebufp);

  /*** Arguments ***/
  t1_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_ACCOUNT_OBJ, (void*)a_pdp, ebufp);

  /*** Results ***/
  t1_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_RESULTS, 0, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_POID, (void*)NULL, ebufp);
  PIN_FLIST_FLD_SET(t1_flistp, PIN_FLD_PAY_TYPE,(void *)NULL, ebufp);

  pinp->call_opcode( PCM_OP_SEARCH,0,s_flistp, &out_flistp, ebufp );
  if(s_flistp) PIN_FLIST_DESTROY(s_flistp,ebufp);

  pin_cookie_t cookie = NULL;
  int32 elem_id =0;
  /*** Iterating over RESULTS Array ***/
  while((t1_flistp = PIN_FLIST_ELEM_GET_NEXT(out_flistp, PIN_FLD_RESULTS,
            &elem_id, 1, &cookie, ebufp)) != NULL) {
     tmp_pdp = (poid_t *) PIN_FLIST_FLD_GET(t1_flistp, PIN_FLD_POID, 1, ebufp);
     if(!pin_poid_is_null(tmp_pdp)) {
       void *vp = NULL;
       if ((vp= PIN_FLIST_FLD_GET( t1_flistp, PIN_FLD_PAY_TYPE, 0, ebufp )) != NULL )
       payType.set((int*)vp);
       break;
     }
  }

  if(out_flistp) PIN_FLIST_DESTROY(out_flistp,ebufp);
  return payType.get();
}

int bs_AccountInfo::getStatus()
{
  return this->status.get();
}

db_entity_state_t bs_AccountInfo::getAccountState()
{
  return this->account_state;
}

/*void bs_AccountInfo::setMerchant(string m)
{
  this->merchant.assign(m);
}*/

void bs_AccountInfo::setAccountNo(string a)
{
  this->account_no.assign(a);
}

void bs_AccountInfo::setPayType(pin_pay_type_t b)
{
  this->pay_type.set(b);
}


void bs_AccountInfo::reset(void)
{
  pin_errbuf_t ebuf;
  if (this->account_pdp != NULL && !pin_poid_is_null(account_pdp)) {
    PIN_ERR_CLEAR_ERR(&ebuf);
    pin_poid_destroy(account_pdp, &ebuf);
  }
  account_pdp = NULL;
  //this->merchant.erase();
  this->promo_code.erase();
  this->member_no.erase();
  this->account_no.erase();
  this->account_state = unknown_State;
  //this->cycle_dom = 0;
  this->status_flags = 0;
  this->created_t = 0;
}



/*******************************************************************
 * get_AccountPayinfo()
 *
 * preconditions:
 *
 *              1) A valid account poid
 *
 * postconditions:
 *
 *              1) Returns the output flist of PCM_OP_CUST_FIND_PAYINFO
 *
 *******************************************************************/
pin_flist_t *
bs_AccountInfo::get_AccountPayinfo(
  pin_Session * pinp, poid_t * a_pdp  )
{
  pin_flist_t             *flistp = NULL;
  pin_flist_t             *out_flistp = NULL;
  void                    * vp = NULL;
  int32                   elemid = 0;
  pin_cookie_t    cookie = NULL;
  pin_errbuf_t ebuf;
  pin_errbuf_t * ebufp = &ebuf;

  PIN_ERR_CLEAR_ERR(ebufp);

  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"get_account_payinfo: entered");

  /* ---sanity -----*/
  if (! a_pdp )
  {
    return NULL;
  }

  /* ---get payment object -----*/
  flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_SET(flistp, PIN_FLD_POID, a_pdp, ebufp);
  PIN_FLIST_FLD_SET(flistp, PIN_FLD_ACCOUNT_OBJ, a_pdp, ebufp);

  pinp->call_opcode(PCM_OP_CUST_FIND_PAYINFO, PCM_OPFLG_READ_RESULT, flistp, &out_flistp, ebufp);

  PIN_FLIST_DESTROY_EX(&flistp, NULL);

  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"get_account_payinfo: leaving");
  return out_flistp;
}


/********************************************************************
 *
 * getService_and_BalGrpPoids() : @by mvamsee
 *
 * inputs: account poid and service type string(Optional)
 * output: returns the service poid and balance group poid associated
 *         with that service as flist. If service_str is null string
 *         then returns the account level balance_group poid as flist.
 ********************************************************************/

pin_flist_t * bs_AccountInfo::getService_and_BalGrpPoids(
  pin_Session * pinp, poid_t *account_pdp, string &service_str) {

   pin_flist_t *s_flistp = NULL;
   pin_flist_t *r_flistp = NULL, *tmp_flistp = NULL;
   pin_errbuf_t ebuf, *ebufp;
   ebufp = &ebuf;
   PIN_ERR_CLEAR_ERR(ebufp);
   /*** Searching for service object and retrieving bal_grp poid ***/
   if(service_str != "") {
       poid_t *search_pdp = NULL;
       int32 s_flags = SRCH_DISTINCT;
       string sql_stmt = "select X from /service where poid_type like '%"+ service_str + "%' and F1 = V1 ";

       s_flistp = PIN_FLIST_CREATE(ebufp);
       search_pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/search", (int64)-1, ebufp);
       PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID,(void *)search_pdp, ebufp);
       PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, &s_flags, ebufp);
       PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void*)sql_stmt.c_str(), ebufp);

       /*** Arguments ***/
       tmp_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
       PIN_FLIST_FLD_SET(tmp_flistp, PIN_FLD_ACCOUNT_OBJ, (void*)account_pdp, ebufp);

       /*** Results ***/
       tmp_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_RESULTS, 0, ebufp);
       PIN_FLIST_FLD_SET(tmp_flistp, PIN_FLD_POID, (void *)NULL, ebufp);
       PIN_FLIST_FLD_SET(tmp_flistp, PIN_FLD_BAL_GRP_OBJ, (void *)NULL, ebufp);

       pinp->call_opcode(PCM_OP_SEARCH, 0, s_flistp, &r_flistp, ebufp);

       if(s_flistp) PIN_FLIST_DESTROY(s_flistp,ebufp);

       tmp_flistp = (pin_flist_t *)PIN_FLIST_ELEM_TAKE(r_flistp, PIN_FLD_RESULTS, 0, 1, ebufp);
       if(r_flistp) PIN_FLIST_DESTROY(r_flistp,ebufp);
       return tmp_flistp;
   } else {
       s_flistp = PIN_FLIST_CREATE(ebufp);
       PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_POID, (void*)account_pdp, ebufp);
       PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_BAL_GRP_OBJ,(void *)NULL, ebufp);

       pinp->call_opcode(PCM_OP_READ_FLDS,0, s_flistp, &r_flistp, ebufp);
       if(s_flistp) PIN_FLIST_DESTROY(s_flistp,ebufp);
       return r_flistp;
   }
   return NULL;
}

/*
 * getBilling_and_PaymentInfo()
 * input: Balance group poid
 * output: Returns billinfo poid and PIN_FLD_ACH value as flist
 *         Caller is responsible for destroying the returned flist.
 */
pin_flist_t * bs_AccountInfo::getBilling_and_PaymentInfo(pin_Session *pinp,
                                             poid_t *bal_grp_pdp )  {

   if(PIN_POID_IS_NULL(bal_grp_pdp)) return NULL;

   pin_flist_t *result_flistp = NULL;
   pin_flist_t *i_flistp = NULL, *r_flistp = NULL;
   poid_t *billinfo_pdp = NULL, *payinfo_pdp = NULL;
   int32 *ach_elemId = NULL;


   pin_errbuf_t ebuf, *ebufp;
   ebufp = &ebuf;
   PIN_ERR_CLEAR_ERR(ebufp);

   i_flistp = PIN_FLIST_CREATE(ebufp);
   PIN_FLIST_FLD_SET(i_flistp, PIN_FLD_POID, (void*)bal_grp_pdp, ebufp);
   PIN_FLIST_FLD_SET(i_flistp, PIN_FLD_BILLINFO_OBJ,(void *)NULL, ebufp);

   pinp->call_opcode(PCM_OP_READ_FLDS,0,i_flistp, &r_flistp, ebufp);
   PIN_FLIST_DESTROY_EX(&i_flistp,ebufp);

   billinfo_pdp = (poid_t *)PIN_FLIST_FLD_GET(r_flistp,PIN_FLD_BILLINFO_OBJ, 1, ebufp);
   if(!PIN_POID_IS_NULL(billinfo_pdp)) {
      result_flistp = PIN_FLIST_CREATE(ebufp);
      PIN_FLIST_FLD_SET(result_flistp, PIN_FLD_BILLINFO_OBJ, (void*)billinfo_pdp, ebufp);

      pin_flist_t *t1_flistp = NULL;
      i_flistp = PIN_FLIST_CREATE(ebufp);
      PIN_FLIST_FLD_SET(i_flistp, PIN_FLD_POID, (void*)billinfo_pdp, ebufp);
      PIN_FLIST_FLD_SET(i_flistp, PIN_FLD_PAYINFO_OBJ,(void *)NULL, ebufp);

      pinp->call_opcode(PCM_OP_READ_FLDS,0,i_flistp, &t1_flistp, ebufp);
      PIN_FLIST_DESTROY_EX(&i_flistp,ebufp);

      payinfo_pdp = (poid_t *)PIN_FLIST_FLD_GET(t1_flistp,PIN_FLD_PAYINFO_OBJ, 1, ebufp);
      if(!PIN_POID_IS_NULL(payinfo_pdp)) {
         pin_flist_t *t2_flistp = NULL;
         i_flistp = PIN_FLIST_CREATE(ebufp);
         PIN_FLIST_FLD_SET(i_flistp, PIN_FLD_POID, (void*)payinfo_pdp, ebufp);
         PIN_FLIST_FLD_SET(i_flistp, PIN_FLD_ACH,(void *)NULL, ebufp);

         pinp->call_opcode(PCM_OP_READ_FLDS,0,i_flistp, &t2_flistp, ebufp);
         PIN_FLIST_DESTROY_EX(&i_flistp,ebufp);

         ach_elemId = (int32 *)PIN_FLIST_FLD_GET(t2_flistp,PIN_FLD_ACH, 1, ebufp);
         if(ach_elemId) {
            PIN_FLIST_FLD_SET(result_flistp, PIN_FLD_ACH, (void*)ach_elemId, ebufp);
         }
         PIN_FLIST_DESTROY_EX(&t2_flistp,ebufp);
     }
     PIN_FLIST_DESTROY_EX(&t1_flistp,ebufp);
   }
   PIN_FLIST_DESTROY_EX(&r_flistp,ebufp);
   return result_flistp;
}
