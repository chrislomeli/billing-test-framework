#include <bs/function/bs_UpdateStatus.h>
#include <util/Portability.h>

bool
bs_UpdateStatus::updateStatus(
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

  string title("updateStatus nvRec");  nv.StringBuffer(title);
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)title.c_str() );

  ///
  /// -- translation ---------------------------
  ///
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"Unpacking..." );
  string str_account_obj_id0 = nv.value(ACCOUNT_OBJ_ID0);
  string account_no   = nv.value(ACCOUNT_NO);
  string str_service_obj_id0  = nv.value(SERVICE_OBJ_ID0);
  string str_service_obj_type = nv.value(SERVICE_OBJ_TYPE);
  string str_status = nv.value(STATUS);
  string str_status_flags = nv.value(STATUS_FLAGS);
  string str_source = nv.value(SOURCE);
  string str_descr  = nv.value(DESCRIPTION);

  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"Translating status..." );

  if ( ! status_cp.set(str_status) )
  {
    ostringstream os;
    sprintf(buff, "Cannot translate %s to a known status ...", str_status.c_str() );
    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)buff );

    pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
            PIN_ERR_BAD_VALUE, PIN_FLD_STATUS, 0, 0);
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)str_status.c_str(), ebufp );
    return false;
  }

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



  //-- get service poid-----------
  bs_ServiceInfo service;
  ostringstream service_descr;
  int service_count = 0;

  if (! str_service_obj_id0.empty() )
  {
    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"Create service poid from poid_id0..." );
    int64 poid_id0 = atol(str_service_obj_id0.c_str());
    service_descr << "service_obj_id0=" << str_service_obj_id0;
    service_cp.create(database, (char *)"/service", poid_id0 );
  }
  else if (! str_service_obj_type.empty() )
  {
    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"Locate service poid from type and account poid ..." );

    service_descr << "service_type=" << str_service_obj_type;
    if((rc = service.loadByAccount_Type(
        pinp, account_cp.borrow(), str_service_obj_type, service_count )) == true )
      service_pdp =  service.service_pdp;

    rc = ( ! pin_poid_is_null( service_pdp ) );
  } else {
    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"No service info - assume this is an account level change..." );
    rc = true;  // this is an account level change
  }

  if ( PIN_ERR_IS_ERR(ebufp) || ! rc  ) {
    if (! PIN_ERR_IS_ERR(ebufp))
      pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
            PIN_ERR_NOT_FOUND,PIN_FLD_SERVICE_OBJ, 0, 0);
    service_descr << "-Error: Found " << service_count << " services of this type...";
    string str( service_descr.str());
    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)str.c_str(), ebufp );
    return false;
  }

  //-- translate others -----------
  int istatus = status_cp.get();
  int status_flags = atoi( str_status_flags.c_str() );

  ///
  /// -- call base function ---------------------------
  ///
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"Calling base function..." );

  PIN_ERR_CLEAR_ERR(ebufp);

  rc = bs_UpdateStatus::updateStatus(
    pinp,
    account_cp.borrow(),
    service_pdp,
    istatus,
    status_flags,
    str_source,
    str_descr,
    return_flistpp, ebufp );

  return rc;
};




//****************************************************
// updateStatus
// Pre:
// Post:
//
bool bs_UpdateStatus::updateStatus(
  pin_Session * pinp,
  poid_t * account_pdp,
  poid_t * service_pdp,
  int status,
  int status_flags,
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
  int     flag=status_flags;
  poid_t * pdp = NULL;
  void * vp = NULL;
  bool rc;

  const string this_program("updateStatus");
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
  if ( status == PIN_STATUS_ACTIVE && status_flags <= 0 )
  {
    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_ERROR,
      (char*)"This is an activation - get existing flags" );

    PIN_ERR_CLEAR_ERR(&localebuf);
    pdp = ( pin_poid_is_null(service_pdp)) ? account_pdp : service_pdp;

    f_flistp = PIN_FLIST_CREATE(ebufp);
    PIN_FLIST_FLD_SET(f_flistp, PIN_FLD_POID, (void*)pdp, &localebuf);
    PIN_FLIST_FLD_SET(f_flistp, PIN_FLD_STATUS_FLAGS, (void*)NULL, &localebuf);
    if ( pinp->call_opcode( PCM_OP_READ_FLDS, 0, f_flistp, &f_outflistp, &localebuf))
    {
      if ((vp=PIN_FLIST_FLD_GET(
        f_outflistp, PIN_FLD_STATUS_FLAGS, 1, &localebuf )) != NULL )
        status_flags = *(int*)vp;
    }
    if (f_flistp) PIN_FLIST_DESTROY(f_flistp,NULL); f_flistp = NULL;
    if (f_outflistp) PIN_FLIST_DESTROY_EX(&f_outflistp,NULL); f_outflistp = NULL;
  }
  if ( status_flags < 0) {
    status_flags = 0;
  }

  /*--- Create set_status ---*/
  x_flistp = PIN_FLIST_CREATE(ebufp);
  PIN_FLIST_FLD_SET(x_flistp, PIN_FLD_POID, account_pdp, ebufp);
  if ( ! pin_poid_is_null(service_pdp) )
    PIN_FLIST_FLD_SET(x_flistp, PIN_FLD_SERVICE_OBJ, service_pdp, ebufp);
  PIN_FLIST_FLD_SET(x_flistp, PIN_FLD_PROGRAM_NAME, (void*)source.c_str(), ebufp);
  PIN_FLIST_FLD_SET(x_flistp, PIN_FLD_DESCR, (void*)description.c_str(), ebufp);
  d_flistp = PIN_FLIST_ELEM_ADD( x_flistp, PIN_FLD_STATUSES, PIN_ELEMID_ASSIGN, ebufp );
    PIN_FLIST_FLD_SET(d_flistp, PIN_FLD_STATUS, (void*)&status, ebufp);
    PIN_FLIST_FLD_SET(d_flistp, PIN_FLD_STATUS_FLAGS, (void*)&status_flags, ebufp);

  if ( (! pin_poid_is_null(service_pdp)) && (status == PIN_STATUS_CLOSED) ) {
     rc = pinp->call_opcode(
         PCM_OP_CUST_SET_STATUS, 0, x_flistp, return_flistpp, ebufp);
  }
  else {
     rc = pinp->call_opcode(
         UOL_OP_SET_STATUS, 0, x_flistp, return_flistpp, ebufp);
  }

  PIN_ERR_LOG_FLIST(
    PIN_ERR_LEVEL_DEBUG,(char*)"leaving bs_UpdateStatus::updateStatus" , *return_flistpp );

  if (x_flistp) PIN_FLIST_DESTROY(x_flistp,NULL);
  return rc;
}

