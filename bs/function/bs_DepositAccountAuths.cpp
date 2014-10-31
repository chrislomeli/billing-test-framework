#include <bs/function/bs_DepositAccountAuths.h>
#include <bs/obj/bs_AccountInfo.h>


bool
bs_DepositAccountAuths::deposit_charges( pin_Session * pinp, poid_t * account_pdp )
{
  poid_t*   pdp=NULL;
  pin_flist_t *flistp = NULL;
  pin_flist_t *s_flistp = NULL;
  pin_flist_t *r_flistp = NULL;
  pin_flist_t * t1 = NULL;
  pin_flist_t * t2 = NULL;
  int32   s_flags = SRCH_DISTINCT;

  pin_errbuf_t    ebuf;
  pin_errbuf_t    * ebufp = &ebuf;

  PIN_ERR_CLEAR_ERR(ebufp);
  ostringstream os;
  char * cp = NULL;
  void * vp = NULL;
  int result=0;
  pin_charge_cmd_t command = PIN_CHARGE_CMD_AUTH_ONLY;
  bool rc = true;


  //------- SQL statement ------//
  const char * sql_stmt = "select X from /event/billing/payment/dd where F1 = V1 and F2 = V2 and F3 = V3 ";

  /*
   * ---------------- Search for all authorization 'payment' records ------------------
   */
  s_flistp = PIN_FLIST_CREATE(ebufp);
  pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/search", (int64)-1, ebufp);
  PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, pdp, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, &s_flags, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void*)sql_stmt, ebufp);

  /*
   * Args::
   */
  pin_charge_result_t charge_res = PIN_CHARGE_RES_PASS;

  t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
    PIN_FLIST_FLD_SET(t1, PIN_FLD_ACCOUNT_OBJ, (void*)account_pdp, ebufp);

  t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 2, ebufp);
    t2 = PIN_FLIST_SUBSTR_ADD(t1, PIN_FLD_PAYMENT,  ebufp);
      PIN_FLIST_FLD_SET(t2, PIN_FLD_COMMAND, (void*)&command, ebufp);

  t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 3, ebufp);
    t2 = PIN_FLIST_ELEM_ADD(t1, PIN_FLD_DD_INFO, PIN_ELEMID_ANY,  ebufp);
      PIN_FLIST_FLD_SET(t2, PIN_FLD_RESULT, (void*)&result, ebufp);

  /*
  /*
   * Results::
   */
    t1 = PIN_FLIST_ELEM_ADD( s_flistp, PIN_FLD_RESULTS, 0, ebufp);
  t1 = NULL;

  /*
   * Read or exit
   */

  if (! pinp->call_opcode( PCM_OP_SEARCH, 0, s_flistp, &r_flistp, ebufp)) {
    if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
    if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
    return false;
  }

  /*** Getting PAY_TYPE ***/
  int payType = bs_AccountInfo::getPayType(pinp,account_pdp);
  if(payType == NULL_ENUM) {
    if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
    if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
    return false;
  }

  /*
   * ------------- Now see if there are deposits and if not
   *    try to create one
   */
  pin_flist_t * deposit_flistp = PIN_FLIST_CREATE(ebufp);
  pin_flist_t * d_flistp = NULL,*tmp_flistp = NULL;
  pin_flist_t * payment_flistp = NULL;
  pin_flist_t * b_flistp = NULL;
  pin_cookie_t  cookie = NULL;
  int32     elemid;
  pin_charge_cmd_t  deposit = PIN_CHARGE_CMD_DEPOSIT;
  string tess("Tess");

  PIN_FLIST_FLD_SET(deposit_flistp, PIN_FLD_POID, (void*)account_pdp, ebufp);
  PIN_FLIST_FLD_SET(deposit_flistp, PIN_FLD_PROGRAM_NAME, (void*)tess.c_str(), ebufp);
  tmp_flistp = PIN_FLIST_ELEM_ADD( deposit_flistp, PIN_FLD_CHARGES, 0, ebufp );
      PIN_FLIST_FLD_SET(tmp_flistp, PIN_FLD_ACCOUNT_OBJ, (void*)account_pdp, ebufp);
      PIN_FLIST_FLD_SET(tmp_flistp, PIN_FLD_PAY_TYPE, (void*)&payType, ebufp);
      PIN_FLIST_FLD_SET(tmp_flistp, PIN_FLD_COMMAND, (void*)&deposit, ebufp);
  d_flistp = PIN_FLIST_ELEM_ADD( tmp_flistp, PIN_FLD_BILLINFO, 0, ebufp );

  while ((t1 = PIN_FLIST_ELEM_GET_NEXT(r_flistp, PIN_FLD_RESULTS,
                &elemid, 1, &cookie, ebufp)) != (pin_flist_t *)NULL)
  {


    if ((payment_flistp = (pin_flist_t*)PIN_FLIST_SUBSTR_GET(
        t1, PIN_FLD_PAYMENT, 1, ebufp)) != (pin_flist_t *)NULL )
    {

      if ((vp = PIN_FLIST_FLD_GET(payment_flistp, PIN_FLD_TRANS_ID, 1, ebufp)) != NULL )
      {

        if ( bs_DepositAccountAuths::transaction_exists( pinp, (char*)vp, deposit))
          continue;
      }

      PIN_FLIST_FLD_SET(d_flistp, PIN_FLD_TRANS_ID, (void*)vp, ebufp);

      if ((vp = PIN_FLIST_FLD_GET(payment_flistp, PIN_FLD_AMOUNT, 1, ebufp)) != NULL )
        PIN_FLIST_FLD_SET(d_flistp, PIN_FLD_AMOUNT, (void*)vp, ebufp);



      if ((t2 = (pin_flist_t*)PIN_FLIST_ELEM_GET(
          t1, PIN_FLD_DD_INFO, PIN_ELEMID_ANY, 1, ebufp)) != (pin_flist_t *)NULL)
      {
        PIN_FLIST_ELEM_SET(d_flistp, t2, PIN_FLD_DD_INFO, 0, ebufp );
      }


      //----- deposit----------------------------
      // pin_flist_print(deposit_flistp,NULL,ebufp);
      {
        pin_flist_t * d_out_flistp = NULL;
        rc = pinp->call_opcode( PCM_OP_PYMT_COLLECT,0,deposit_flistp, &d_out_flistp, ebufp );
        PIN_FLIST_DESTROY_EX(&d_out_flistp,NULL);
      }
    }
  }
  PIN_FLIST_DESTROY_EX(&deposit_flistp,NULL);
  PIN_FLIST_DESTROY_EX(&s_flistp, NULL);
    PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
  return rc;
}


bool
bs_DepositAccountAuths::transaction_exists(
  pin_Session * pinp,
  char * trans_id,
  pin_charge_cmd_t command )
{
  poid_t*   pdp=NULL;
  pin_flist_t *flistp = NULL;
  pin_flist_t *s_flistp = NULL;
  pin_flist_t *r_flistp = NULL;
  pin_flist_t * t1 = NULL;
  pin_flist_t * t2 = NULL;
  int32   s_flags = SRCH_DISTINCT;
  pin_fld_num_t substr_fld = PIN_FLD_PAYMENT;

  pin_errbuf_t    ebuf;
  pin_errbuf_t    * ebufp = &ebuf;

  PIN_ERR_CLEAR_ERR(ebufp);
  ostringstream os;
  char * cp = NULL;
  void * vp = NULL;
  bool rc = false;

  //--- SQL statement ------------
  string sql_stmt("select X  from /event/billing/payment/dd where F1 = V1 and F2 = V2 and rownum < 2 ");

  /*
   * Search for last payment::
   */
  s_flistp = PIN_FLIST_CREATE(ebufp);
  pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/search", (int64)-1, ebufp);
  PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, pdp, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, &s_flags, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void*)sql_stmt.c_str(), ebufp);

  /*
   * Args::
   */
  pin_charge_result_t charge_res = PIN_CHARGE_RES_PASS;

  t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
    t2 = PIN_FLIST_SUBSTR_ADD(t1, substr_fld, ebufp);
        PIN_FLIST_FLD_SET(t2, PIN_FLD_TRANS_ID, (void*)trans_id, ebufp);

  t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 2, ebufp);
    t2 = PIN_FLIST_SUBSTR_ADD(t1, substr_fld, ebufp);
        PIN_FLIST_FLD_SET(t2, PIN_FLD_COMMAND, (void*)&command, ebufp);
  /*
   * Results::
   */
  t1 = PIN_FLIST_ELEM_ADD ( s_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
    PIN_FLIST_FLD_SET(t1, PIN_FLD_POID, (void*)NULL, ebufp);

  /*
   * If we have at least one result - return true
   */
  if (! pinp->call_opcode( PCM_OP_SEARCH, 0, s_flistp, &r_flistp, ebufp) )
  {
    if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
    if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
    return false;
  }

  if ( PIN_FLIST_ELEM_COUNT(r_flistp, PIN_FLD_RESULTS, ebufp) > 0 )
    rc = true;



  /*
   * Cleanup and exit
   */
  PIN_FLIST_DESTROY_EX(&s_flistp, NULL);
    PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
  return rc;
}
