#include "bs_LoadUsage.h"
#include <util/Portability.h>


#define OUTBOUND 0
#define INBOUND 1
#define TARGET_SERVICE_TYPE UNTD_SERVICE_TYPE_VOIP

/*********************
0 PIN_FLD_POID           POID [0] 0.0.0.1 /account -1 literal
0 PIN_FLD_SERVICE_OBJ    POID [0] 0.0.0.1 /service/uol_voip -1  literal
0 PIN_FLD_AAC_SERIAL_NUM  STR [0] "1007"  lookup  member service id of /service/<type> for this MEMBER
0 PIN_FLD_START_T      TSTAMP [0] (1117308253)  calc  based on pin_v_tim and duration
0 PIN_FLD_END_T      TSTAMP [0] (1117308853)  calc  based on pin_v_tim and duration
0 PIN_FLD_PROGRAM_NAME    STR [0] "testnap" literal "Tess"
0 PIN_FLD_OBJ_TYPE        STR [0] "/uol_voip/call"  literal "/uol_voip/call"
0 PIN_FLD_INHERITED_INFO SUBSTRUCT [0] allocated 1, used 1
1     PIN_FLD_CALL         SUBSTRUCT [0] allocated 3, used 3
2         PIN_FLD_DURATION_SECONDS         INT [0] 60 ARG Number
2         PIN_FLD_TERMINATE_CAUSE_STR     ENUM [0] "1" 
2         UNTD_FLD_SIP_SESSION_ID          STR [0] "asdf.asdf.1234.1234.asdf1"
2         PIN_FLD_COUNT                    INT [0] 1  literal for testing?  1
2         UNTD_FLD_CORRELATION_ID          STR [0] "" ??  Unique 'session_id'
2         PIN_FLD_DIRECTION               ENUM [0] 0  ARG In=0 #member is the call originator
2         UNTD_FLD_FROM_MEMBER_SERVICE_ID  STR [0] 1007 looup same as above for input
2         UNTD_FLD_TO_SIP_URL            STR [0] ""   some external phone or a member phone
#--OR--
2         PIN_FLD_DIRECTION               ENUM [0] 1  ARG Out=0 #member is the call receiver
2         UNTD_FLD_TO_MEMBER_SERVICE_ID  STR [0] 1007 looup same as above for output
2         UNTD_FLD_FROM_SIP_URL              STR [0] "19192873546"    some external phone or a member phone of party making the call
----

*/
bool
bs_LoadUsage::loadVoipCDRUsage(
        pin_Session * pinp,
        bs_NvRecord& nv,
        pin_flist_t   **return_flistpp,
        pin_errbuf_t  *ebufp    )
{
        poid_t * account_pdp = NULL;
        bool rc = false;

        ///
        /// -- account ---------------------------
        ///
        string title("loadUsage nvRec");
    nv.StringBuffer(title);
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

        ///
        /// -- translation ---------------------------
        ///
  time_t ltime  = 0;
  time_t  stime = 0;
  time_t  rtime = 0;
  int count   = 1;
  char *term_cause  = (char *)"1";
  int direction = OUTBOUND;

  string service_id;
  string amountstr  = nv.value(AMOUNT);
        string account_no = nv.value(ACCOUNT_NO);
  string sip    = nv.value(CALL_TO);
  string dummy_sip  = "18182873000";
  string call_type  = nv.value(CALL_TYPE);
        string member_no  = nv.value(MEMBER_NO);
  string datestr    = nv.value(DATE);
  string session_id = nv.value(SID);
  string core_id    = nv.value(CID);
  string ccount   = nv.value(COUNT);

  c_DateTime      c_date;
        int duration    = atoi( amountstr.c_str() );

  pin_virtual_time(&rtime);

         //-- get the member service id -----------
        poid_t * service_pdp = PIN_POID_CREATE(
      pinp->getDBNumber(), (char*)TARGET_SERVICE_TYPE, (int64)-1, ebufp);

  bs_LoadUsage::findMemberServiceId(
      pinp, service_pdp, member_no, service_id );


  //-- valid date -----------
  //Changing this to add the duration to get the end_t and set the start_t as given time
  if (datestr.empty())
    stime  = rtime;
  else
  {
    c_date.set(datestr);
    stime  = c_date.getUTC();
  }
  ltime = (stime>duration) ? stime + (long)duration : stime;

  //getting the current time
  time_t    cur_time;
  cur_time=time(NULL);

        //-- valid session_id -----------
  if (session_id.empty() )
  {

    ostringstream os;
    os <<  account_no << "." << service_id << "." << cur_time ;
    session_id.assign(os.str());
  }
  else if ( session_id == "NULL" )
  {
    session_id.erase();
  }

         //-- call type -----------
  int call_type_num = (call_type.empty()) ? 0 : atoi( call_type.c_str());


         //-- valid correlation_id -----------
  if (core_id.empty())
  {
    ostringstream os;
    os << service_id << cur_time;
    core_id.assign(os.str());
  }
  else if ( core_id == "NULL" )
  {
    core_id.erase();
  }

        //-- translate duration -----------
  if ( !ccount.empty() )
    count = atoi(ccount.c_str());

        //-- translate call direction/member -----------
  if (sip.empty())
  {
    sip = nv.value("CALL_FROM");

    if (!sip.empty())
      direction = INBOUND;
    else
      sip.assign("1888.888.8888");
  }



        ///
        /// -- set base flist ---------------------------
        ///
        account_pdp = PIN_POID_CREATE(
      pinp->getDBNumber(), (char*)"/account", (int64)-1, ebufp);

  pin_flist_t * s_flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_POID, account_pdp, ebufp);
  PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_SERVICE_OBJ, service_pdp, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_START_T, (void*)&stime, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_END_T, (void*)&ltime, ebufp);
        PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_AAC_SERIAL_NUM, (void*)service_id.c_str(), ebufp);
        PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_PROGRAM_NAME, (void*)"loadVoipCDRUsage", ebufp);
        PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_OBJ_TYPE, (void*)"/uol_voip/call", ebufp);
        pin_flist_t * t1 = PIN_FLIST_SUBSTR_ADD(s_flistp, PIN_FLD_INHERITED_INFO, ebufp);
        pin_flist_t * t2 = PIN_FLIST_SUBSTR_ADD(t1, PIN_FLD_CALL, ebufp);
        PIN_FLIST_FLD_SET(t2, UNTD_FLD_SIP_SESSION_ID, (void*)session_id.c_str(), ebufp);
        PIN_FLIST_FLD_SET(t2, PIN_FLD_COUNT, (void*)&count, ebufp);
        PIN_FLIST_FLD_SET(t2, UNTD_FLD_CORRELATION_ID, (void*)core_id.c_str(), ebufp);
        PIN_FLIST_FLD_SET(t2, PIN_FLD_DURATION_SECONDS, (void*)&duration, ebufp);
        PIN_FLIST_FLD_SET(t2, PIN_FLD_TERMINATE_CAUSE_STR, (void*)term_cause, ebufp);
        PIN_FLIST_FLD_SET(t2, PIN_FLD_DIRECTION, (void*)&direction, ebufp);
        PIN_FLIST_FLD_SET(t2, PIN_FLD_CALL_TYPE, (void*)&call_type_num, ebufp);
  if (direction == OUTBOUND)
  {
    PIN_FLIST_FLD_SET(t2, UNTD_FLD_FROM_MEMBER_SERVICE_ID, (void*)service_id.c_str(), ebufp);
    PIN_FLIST_FLD_SET(t2, UNTD_FLD_TO_SIP_URL, (void*)sip.c_str(), ebufp);
  }
  else
  {
    PIN_FLIST_FLD_SET(t2, UNTD_FLD_TO_MEMBER_SERVICE_ID, (void*)service_id.c_str(), ebufp);
    PIN_FLIST_FLD_SET(t2, UNTD_FLD_TO_SIP_URL, (void*)dummy_sip.c_str(), ebufp);
    PIN_FLIST_FLD_SET(t2, UNTD_FLD_FROM_SIP_URL, (void*)sip.c_str(), ebufp);
  }

  /*
   * ------------- Poulate load_session flist ------------------
   */
  rc = pinp->call_opcode(
      PCM_OP_UOL_POL_ACT_LOAD_SESSION, 0, s_flistp, return_flistpp, ebufp);

  PIN_FLIST_DESTROY_EX(&s_flistp, NULL);
        return rc;
}


bool
bs_LoadUsage::loadUsage(
        pin_Session * pinp,
        bs_NvRecord& nv,
        pin_flist_t   **return_flistpp,
        pin_errbuf_t  *ebufp    )
{
        c_Decimal amount_cp;
        poid_t * account_pdp = NULL;
        pin_decimal_t * amountp = NULL;


        string account_no = nv.value(ACCOUNT_NO);
        string str_resource = nv.value("resource_id");

        bool rc = false;
        int64 database = pinp->getDBNumber();
        ostringstream account_descr;


        ///
        /// -- translation ---------------------------
        ///
        string title("loadUsage nvRec");  nv.StringBuffer(title);
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
        int resource_id = atoi( str_resource.c_str() );

        ///
        /// -- call base function ---------------------------
        ///
        PIN_ERR_CLEAR_ERR(ebufp);
        PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,
                                (char*)"Callling base function..." );
        rc = loadUsage( pinp,
      account_cp.borrow(),
      "/service/ip",
      0,
      resource_id,
      return_flistpp, ebufp );

        return rc;
};


bool
bs_LoadUsage::loadUsage(
  pin_Session * pinp,
  poid_t * account_pdp,
  string service_type,
  time_t end_t,
  time_t duration_seconds,
  pin_flist_t **return_flistpp,
  pin_errbuf_t * ebufp )
{
        poid_t*         pdp=NULL;
        pin_flist_t     *flistp = NULL;
        pin_flist_t     *s_flistp = NULL;
        pin_flist_t     *r_flistp = NULL;
        pin_flist_t * t1 = NULL;
        pin_flist_t * t2 = NULL;
        int32           s_flags = SRCH_DISTINCT;

        PIN_ERR_CLEAR_ERR(ebufp);
        char * cp = NULL;
        void * vp = NULL;
        int result=0;
        bool rc = true;

    /*
     * --- calulate times -----
     */
    if ( duration_seconds <= 0 )
    {
      return false;
    }

    if ( end_t <= 0  )
      pin_virtual_time(&end_t);

    time_t start_t = end_t - duration_seconds;
    if ( start_t <= 0 )
    {
      return false;
    }

        /*
         * ------- Search for the /service/xxx object for this account ---------
         */
         const char * sql_stmt =
      "select X from /service where F1 = V1 and F2 = V2 ";

    s_flistp = PIN_FLIST_CREATE(ebufp);
        pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/search", (int64)-1, ebufp);
        PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, pdp, ebufp);
        PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, &s_flags, ebufp);
        PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void*)sql_stmt, ebufp);

        /*
         * Args::
         */
        t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
                PIN_FLIST_FLD_SET(t1, PIN_FLD_ACCOUNT_OBJ, (void*)account_pdp, ebufp);

    char * svc = (char*)service_type.c_str();
    poid_t * svcpdp = PIN_POID_CREATE(pinp->getDBNumber(), svc, (int64)-1, ebufp);
        t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 2, ebufp);
                PIN_FLIST_FLD_PUT(t1, PIN_FLD_POID, (void*)svcpdp, ebufp);

        /*
        /*
         * Results::
         */
        t1 = PIN_FLIST_ELEM_ADD ( s_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
                PIN_FLIST_FLD_SET(t1, PIN_FLD_POID, (void*)NULL, ebufp);
                PIN_FLIST_FLD_SET(t1, PIN_FLD_LOGIN, (void*)NULL, ebufp);

        /*
         * Read or exit
         */
        rc = pinp->call_opcode( PCM_OP_SEARCH, 0, s_flistp, &r_flistp, ebufp);
    PIN_FLIST_DESTROY_EX(&s_flistp, NULL);

    if ( ! rc )
    {
            PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
            return false;
        }


        /*
         * ------------- Get service info ------------------
         */
    pin_flist_t * service_flistp = NULL;
    char * svclogin = NULL;
    svcpdp = NULL;

        if ((service_flistp = (pin_flist_t*)PIN_FLIST_ELEM_GET(
      r_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, 1, ebufp )) == NULL )
    {
                return false;
    }
    else if ((svcpdp = PIN_FLIST_FLD_GET(service_flistp, PIN_FLD_POID, 1, ebufp)) == NULL )
    {
                return false;
    }
    if ((svclogin = (char*)PIN_FLIST_FLD_GET(service_flistp, PIN_FLD_LOGIN, 1, ebufp)) == NULL )
    {
                return false;
    }


        /*
         * ------------- Poulate load_session flist ------------------
         */
    string tess("Tess: load_events");
    pin_flist_t *   load_flistp = PIN_FLIST_CREATE(ebufp);
    PIN_FLIST_FLD_SET(load_flistp, PIN_FLD_POID, (void *)account_pdp, ebufp);
    PIN_FLIST_FLD_SET(load_flistp, PIN_FLD_PROGRAM_NAME, (void *)tess.c_str(), ebufp);
    PIN_FLIST_FLD_SET(load_flistp, PIN_FLD_OBJ_TYPE, (void *) "/dialup", ebufp);
    PIN_FLIST_FLD_SET(load_flistp, PIN_FLD_END_T, (void *) &end_t, ebufp);
    PIN_FLIST_FLD_SET(load_flistp, PIN_FLD_START_T, (void *) &start_t, ebufp);
    PIN_FLIST_FLD_SET(load_flistp, PIN_FLD_SERVICE_OBJ, (void*)svcpdp, ebufp);
    PIN_FLIST_FLD_SET(load_flistp, PIN_FLD_LOGIN, (void*)svclogin, ebufp);

    pin_flist_t * d_flistp = PIN_FLIST_SUBSTR_ADD(load_flistp, PIN_FLD_INHERITED_INFO, ebufp);
      pin_flist_t *  dialup_flistp = PIN_FLIST_SUBSTR_ADD((pin_flist_t *) d_flistp, PIN_FLD_DIALUP, ebufp);
        PIN_FLIST_FLD_SET(dialup_flistp, PIN_FLD_NETWORK, (void *) tess.c_str(), ebufp);
        PIN_FLIST_FLD_SET(dialup_flistp, PIN_FLD_LOGIN, (void*)svclogin, ebufp);

    PIN_FLIST_DESTROY_EX(&r_flistp, NULL);

    /*
     * ------------- Poulate load_session flist ------------------
     */
    pin_flist_t * load_out_flistp = NULL;
    rc = pinp->call_opcode( PCM_OP_ACT_LOAD_SESSION, 0, load_flistp, return_flistpp, ebufp);
    PIN_FLIST_DESTROY_EX(&load_flistp, NULL);
    PIN_FLIST_DESTROY_EX(&load_out_flistp,NULL);

  return rc;
}


/*******************************************************************
 * findMemberService()
 *
 * Summary: Given a member_no and a target service (partial) poid
 *   Find the account and service if they already exist.
 *
 * Input:
 * Output: An flist with the existing account poid and the target service poid.
 *
 *******************************************************************/
bool bs_LoadUsage::findMemberServiceId(
  pin_Session * pinp,
  poid_t  * service_type_pdp, /* a partial service poid */
  const string member_no,  /* a UNTD member no */
  string& service_id )
{
  pin_flist_t * s_flistp = NULL;
  pin_flist_t * sr_flistp = NULL;
  pin_flist_t * t1 = NULL;
  poid_t    * account_pdp = NULL;
  poid_t    * service_pdp = NULL;
  poid_t    * pdp = NULL;
  const char    * service_type = NULL; //included const qualifier
  char    * cp = NULL;
  int32   s_flags = SRCH_DISTINCT;
  int32   elemid = 0;
  pin_cookie_t cookie = NULL;
  bool    rc = false;
  pin_errbuf_t ebuf;
  pin_errbuf_t * ebufp = &ebuf;


  const char* select = "select X from /service 1, /account 2 where 1.F1 = 2.F2 AND 2.F3 = V3 AND 1.F4 = V4 ";

  PIN_ERR_CLEAR_ERR(ebufp);


  /*
   * Get required fields
   */
  service_type = PIN_POID_GET_TYPE(service_type_pdp);


  /*
   * Search for the new service obj - we want to return it
   */
  s_flistp = PIN_FLIST_CREATE(ebufp);
  pdp = PIN_POID_CREATE(PIN_POID_GET_DB(service_type_pdp), (char*)"/search", (int64)-1, ebufp);
  PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, pdp, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, &s_flags, ebufp);
  PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void*)select, ebufp);

  /*
  * Args::
  */
  t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
  PIN_FLIST_FLD_SET(t1, PIN_FLD_ACCOUNT_OBJ, (void*)NULL, ebufp);

  t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 2, ebufp);
  PIN_FLIST_FLD_SET(t1, PIN_FLD_POID, (void*)NULL, ebufp);


  t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 3, ebufp);
  PIN_FLIST_FLD_SET(t1, PIN_FLD_ACCESS_CODE1, (void*)member_no.c_str(), ebufp);


  pdp = PIN_POID_CREATE(pinp->getDBNumber(), service_type, (int64)-1, ebufp);
  t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 4, ebufp);
      PIN_FLIST_FLD_PUT(t1, PIN_FLD_POID, (void*)pdp, ebufp);

  /*
  * Results::
  */
  t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_RESULTS, 0, ebufp);
  PIN_FLIST_FLD_SET(t1, PIN_FLD_POID, (void*)NULL, ebufp);
  PIN_FLIST_FLD_SET(t1, PIN_FLD_AAC_SERIAL_NUM, (void*)NULL, ebufp);


  /*
  * Perform the search
  */
  pinp->call_opcode( PCM_OP_SEARCH, 0, s_flistp, &sr_flistp, ebufp);
  PIN_FLIST_DESTROY_EX(&s_flistp,NULL);


  /*
  * Handle search results
  */
  if ( PIN_ERR_IS_ERR(ebufp) )
  {
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
      (char*)"findMemberService: Error searching for existing 'clone' account ", ebufp );
    PIN_FLIST_DESTROY_EX(&sr_flistp,NULL);
    return rc;
  }


  /*
  * get the service poid - assume there is only one - get the 'first' one
  */
  elemid=0;
  while ((t1 = PIN_FLIST_ELEM_GET_NEXT(sr_flistp, PIN_FLD_RESULTS,
     &elemid, 1, &cookie, ebufp )) != NULL )
  {
    cp = (char*)PIN_FLIST_FLD_GET(t1, PIN_FLD_AAC_SERIAL_NUM, 1, ebufp);
    if (cp && strlen(cp)>0) {
      service_id.assign(cp);
      rc = true;
      break;
    }
  }


  /*
   * Clean up.
   */
  PIN_FLIST_DESTROY_EX(&sr_flistp,NULL);
  return rc;
}


