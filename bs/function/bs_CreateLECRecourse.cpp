#include "bs_CreateLECRecourse.h"

bool
bs_CreateLECRecourse::createRecourse(
  pin_Session * pinp,
  c_NvRecord& nv,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  )
{
	const char * file_name = NULL;

  string title("LEC Recourse nvRec");  nv.StringBuffer(title);
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)title.c_str() );

  ///
  /// -- translation ---------------------------
  ///
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"Unpacking..." );
  string sBtn =  nv.value("BTN");
  string sAmount =  nv.value("AMOUNT");
  string sRecordType =  nv.value("TYPE");
  string sAuthDate =  nv.value("DATE");
  string sReasonCode =  nv.value("RESULT");
  string fn = nv.value("FILENAME");
  string sVersion = nv.value("VER");

  if (! fn.empty() )
		file_name = fn.c_str();

	//---e default values 
	if (sReasonCode.empty())sReasonCode.assign("00");
	if (sAuthDate.empty())sAuthDate.assign("080228");



  ///
  /// -- call base function ---------------------------
  ///
  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,(char*)"Calling base function..." );

  PIN_ERR_CLEAR_ERR(ebufp);

  bs_CreateLECRecourse::createRecourse(
	pinp,
	sRecordType.c_str(),
	sBtn.c_str(),
	sAuthDate.c_str(),
	sReasonCode.c_str(),
	sAmount.c_str(),
	sVersion.c_str(),
	file_name,
    return_flistpp, ebufp );

  if (PIN_ERR_IS_ERR(ebufp))
	  return false;
  return true;
};




//****************************************************
// createRecourse
// Pre:
// Post:
//
void bs_CreateLECRecourse::createRecourse(
	pin_Session * pinp,
	const char * sRecordType,
	const char * sBtn,
	const char * sAuthDate,
	const char * sReasonCode,
	const char * sAmount,
	const char * sVersion,
	const char * file_name,
	pin_flist_t   **return_flistpp,
	pin_errbuf_t  *ebufp  )
{
	pin_flist_t * x_flistp = NULL;
	pin_flist_t * c_flistp = NULL;
	int    lec_db = 5;  //WIP:  hard coded
	int    command = 9;
	poid_t * pdp = NULL;
	pin_decimal_t * amtp = NULL;
	void * vp = NULL;
	int impact_type = 0;
	int version = 0; 

	PIN_ERR_CLEAR_ERR(ebufp);

	//--enter function--------------
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "bs_CreateLECRecourse::createRecourse: enter" );

	/*--- Convert ---*/
	if (sVersion && strlen(sVersion)> 0 )
		version = atoi(sVersion);

	amtp = pbo_decimal_from_str(sAmount,ebufp);
	if(amtp && !PIN_ERR_IS_ERR(ebufp) 
		&& pbo_decimal_sign(amtp, ebufp) == -1 )
	{
		pbo_decimal_abs_assign(amtp, ebufp);
		impact_type = 1;
	}


	/*--- Create set_status ---*/
	x_flistp = PIN_FLIST_CREATE(ebufp);
	pdp = pin_poid_create((int64)lec_db,"/_lec_db", (int64)1, ebufp);
	PIN_FLIST_FLD_PUT(x_flistp, PIN_FLD_POID, pdp, ebufp);
	PIN_FLIST_FLD_SET(x_flistp, PIN_FLD_COMMAND, (void*)&command, ebufp);
	c_flistp = PIN_FLIST_ELEM_ADD( x_flistp, PIN_FLD_CHARGES, PIN_ELEMID_ASSIGN, ebufp );
		PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_PHONE, (void*)sBtn, ebufp);
		PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_AUTH_DATE, (void*)sAuthDate, ebufp);
		PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_RECORD_TYPE, (void*)sRecordType, ebufp);
		PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_REASON_CODE, (void*)sReasonCode, ebufp);
		PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_IMPACT_TYPE, (void*)&impact_type, ebufp);
		PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_VERSION_ID, (void*)&version, ebufp);
		PIN_FLIST_FLD_PUT(c_flistp, PIN_FLD_AMOUNT_ADJUSTED, (void*)amtp, ebufp);
	
	if (file_name != NULL ) {
		c_flistp = PIN_FLIST_ELEM_ADD( x_flistp, PIN_FLD_BATCH_INFO, PIN_ELEMID_ASSIGN, ebufp );
			PIN_FLIST_FLD_SET(c_flistp, PIN_FLD_FILENAME, (void*)file_name, ebufp);
	}


	/*----* Connect to the LEC DM *---*/
//	pcm_context_t	*my_ctxp = NULL;
//	PCM_CONTEXT_OPEN(&my_ctxp, (pin_flist_t *)0, ebufp);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,"dm search input flist", x_flistp);
	pinp->call_opcode(
         PCM_OP_SEARCH, 0, x_flistp, return_flistpp, ebufp);
	if (return_flistpp) PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, "dm search output flist", *return_flistpp);

//	if ( PIN_ERR_IS_ERR(ebufp) )
//	{
//		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,"Failed connecting to DM_LEC...", ebufp );
//	}
//	PCM_CONTEXT_CLOSE(my_ctxp, 0, NULL);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,(char*)"leaving bs_CreateLECRecourse::createRecourse" , *return_flistpp );

	PIN_FLIST_DESTROY_EX(&x_flistp,NULL);
	return;
}

