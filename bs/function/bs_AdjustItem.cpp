#include "bs_AdjustItem.h"
#include <util/Portability.h>


bool
bs_AdjustItem::adjust_item(
  pin_Session * pinp,
  bs_NvRecord& nv,
  pin_flist_t   **return_flistpp,
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
	string item_type = nv.value("ITEM_TYPE");
  	string reason_id = nv.value("REASON_ID");

  	bool rc = false;
	int64 database = pinp->getDBNumber();
  	ostringstream account_descr;

	///
	/// -- translation ---------------------------
	///
	string title("adjust_item nvRec");  nv.StringBuffer(title);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)title.c_str() );


	//-- translate account poid-----------
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Locating account..." );
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
  	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Translate amount..." );
 	if (! amount_cp.create( str_amount.c_str() ) ||  amount_cp.isnull() ) {
	    	if (! PIN_ERR_IS_ERR(ebufp))
			pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
				PIN_ERR_BAD_VALUE,PIN_FLD_AMOUNT, 0, 0);
		account_descr << "-Can not get a valid amount";
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)((account_descr.str()).c_str()), ebufp );
		return false;
	}

	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Calling base function..." );
	PIN_ERR_CLEAR_ERR(ebufp);

	//--Do item adjustment--
	rc = adjust_item(
	  pinp,
	  account_cp.borrow(),
	  amount_cp.borrow(),
	  str_source,
	  service_str,
	  item_type,
	  reason_id,
	  return_flistpp,
	  ebufp );

	  return rc;

};


bool bs_AdjustItem::adjust_item(
  pin_Session * pinp,
  poid_t * account_pdp,
  pin_decimal_t * amountp,
  string source,
  string service_str,
  string item_type,
  string reason_id,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp )
{
  pin_flist_t   *input_flistp = NULL;
  pin_flist_t	*i_flistp = NULL;
  pin_flist_t	*r_flistp = NULL;

  poid_t 	*item_pdp = NULL;

  pin_decimal_t *adjustedp = NULL;
  pin_decimal_t *item_totalp = NULL;

  ostringstream os;
  bool rc = false;

  PIN_ERR_CLEAR_ERR(ebufp);

  //--- setup --------------------------
  /* source / desc name */
  const string this_program("adjust_item");

  if ( source.empty() )
  {
    source.assign(this_program);
  }

  PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, (char*)"Locating Item poid to adjust..." );

  item_pdp = get_poid_for_adjust_item(pinp, account_pdp, service_str, item_type, ebufp );

  if(PIN_POID_IS_NULL(item_pdp)) {
	pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
		PIN_ERR_NOT_FOUND,PIN_FLD_ITEM_OBJ, 0, 0);
	PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
		(char*)"Couldnt find relevant /item poid", ebufp );
	return false;
  }

  i_flistp = PIN_FLIST_CREATE(ebufp);

  PIN_FLIST_FLD_SET(i_flistp, PIN_FLD_POID, (void *)item_pdp, ebufp);
  PIN_FLIST_FLD_SET(i_flistp, PIN_FLD_ADJUSTED, (void *)NULL, ebufp);
  PIN_FLIST_FLD_SET(i_flistp, PIN_FLD_ITEM_TOTAL, (void *)NULL, ebufp);

  pinp->call_opcode(PCM_OP_READ_FLDS, 0, i_flistp, &r_flistp, ebufp);

  adjustedp = (pin_decimal_t *)PIN_FLIST_FLD_TAKE(r_flistp, PIN_FLD_ADJUSTED, 0, ebufp);
  item_totalp = (pin_decimal_t *)PIN_FLIST_FLD_TAKE(r_flistp, PIN_FLD_ITEM_TOTAL, 0, ebufp);

  PIN_FLIST_DESTROY(i_flistp, ebufp);
  PIN_FLIST_DESTROY(r_flistp, ebufp);

  //--- adjust------------------------------
  input_flistp = PIN_FLIST_CREATE(ebufp);

  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_PROGRAM_NAME, (void*)source.c_str(), ebufp);
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_POID,(void *) item_pdp, ebufp);
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_AMOUNT, (void *)amountp, ebufp);
  PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_REASON_CODE, (void *) reason_id.c_str(), ebufp);

  PIN_FLIST_FLD_PUT(input_flistp, PIN_FLD_ADJUSTED, (void *)adjustedp, ebufp);
  PIN_FLIST_FLD_PUT(input_flistp, PIN_FLD_ITEM_TOTAL, (void *)item_totalp, ebufp);

  /*
  * Call opcode
  */
  rc = pinp->call_opcode( PCM_OP_UOL_POL_ITEM_ADJUSTMENT, 0, input_flistp,
        return_flistpp, ebufp);

  PIN_FLIST_DESTROY(input_flistp, ebufp);
  return rc;
}

/*
* This function will return an open item for an account
* If no open item is available, it will return the most
* recently created item which is in closed state.
*/
poid_t *
bs_AdjustItem::get_poid_for_adjust_item(
	pin_Session * pinp,
	poid_t *account_pdp,
	string service_str,
	string item_type,
	pin_errbuf_t *ebufp)
{
	pin_flist_t * results_flistp = (pin_flist_t *)NULL;
	pin_flist_t * results1_flistp = (pin_flist_t *)NULL;
	pin_flist_t * s_flistp = (pin_flist_t *)NULL;
	pin_flist_t	*t1 = (pin_flist_t *)NULL;
	poid_t		*item_pdp = NULL;
	poid_t		*temp_item_pdp = NULL;
	poid_t		*temp_srv_pdp = NULL;
	poid_t		*pdp = NULL;
	int32           flag = 0;
	time_t		*created_t = (time_t *)0;

	// template - if service_type is not given
	const char * template0 = "select X from /item where F1 = V1 and F2 = V2 and F3 in ( V3 , 4) and item_t.item_total != 0 order by item_t.status asc, item_t.created_t desc ";

	// template - if service_type is given
	const char * template1 = "select X from /item where F1 = V1 and F2 = V2 and F3 = V3 and F4 in ( V4 , 4) and item_t.item_total != 0 order by item_t.status asc, item_t.created_t desc ";

	pin_status_t	status_open = (pin_status_t )PIN_ITEM_STATUS_OPEN;

	// get the item type

	if (strcmp(item_type.c_str(),"SETUP_FEE") == 0) {
		item_type.assign("/item/misc");
	}
	else if (strcmp(item_type.c_str(),"CANCEL_FEE") == 0) {
		item_type.assign("/item/misc");
	}
	else if (strcmp(item_type.c_str(),"USAGE_FEE") == 0) {
		item_type.assign("/item/misc");
	}
	else {
		item_type.assign("/item/cycle_forward");
	}

	int64	db = 0;

	temp_item_pdp = PIN_POID_CREATE(db, item_type.c_str(), (int64)-1, ebufp);


	db = PIN_POID_GET_DB(account_pdp);
	s_flistp = PIN_FLIST_CREATE(ebufp);
	pdp = PIN_POID_CREATE(db,(char *)"/search", (int64)-1, ebufp);
	PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, (void *)pdp, ebufp);
	PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, (void *)&flag, ebufp);

	// Both service_type and item_type are not NULL
	if (service_str != " ")
	{
		temp_srv_pdp = PIN_POID_CREATE(db, service_str.c_str(), (int64)-1, ebufp);

		PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void *)template1, ebufp);

		t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
		PIN_FLIST_FLD_SET(t1, PIN_FLD_ACCOUNT_OBJ, (void *)account_pdp, ebufp);

		t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 2, ebufp);
		PIN_FLIST_FLD_SET(t1, PIN_FLD_POID, (void *)temp_item_pdp, ebufp);

		t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 3, ebufp);
		PIN_FLIST_FLD_SET(t1, PIN_FLD_SERVICE_OBJ, (void *)temp_srv_pdp, ebufp);

		t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 4, ebufp);
		PIN_FLIST_FLD_SET(t1, PIN_FLD_STATUS, (void *)&status_open, ebufp);

	}
	else
	{
		PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void *)template0, ebufp);

		t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
		PIN_FLIST_FLD_SET(t1, PIN_FLD_ACCOUNT_OBJ, (void *)account_pdp, ebufp);

		t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 2, ebufp);
		PIN_FLIST_FLD_SET(t1, PIN_FLD_POID, (void *)temp_item_pdp, ebufp);

		t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 3, ebufp);
		PIN_FLIST_FLD_SET(t1, PIN_FLD_STATUS, (void *)&status_open, ebufp);

	}

	t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_RESULTS, 0, ebufp);
	PIN_FLIST_FLD_SET(t1, PIN_FLD_POID, (void *)NULL, ebufp);

	pinp->call_opcode(PCM_OP_SEARCH, 0, s_flistp, &results_flistp, ebufp);

	if( !PIN_ERR_IS_ERR(ebufp) &&
		(PIN_FLIST_ELEM_COUNT(results_flistp,PIN_FLD_RESULTS, ebufp) > 0))
	{
		t1 = (pin_flist_t *)PIN_FLIST_ELEM_GET(results_flistp, PIN_FLD_RESULTS, 0, 1, ebufp);
		item_pdp = (poid_t *)PIN_FLIST_FLD_TAKE(t1, PIN_FLD_POID, 0, ebufp);
	}

	PIN_FLIST_DESTROY_EX(&results_flistp, NULL);
	PIN_FLIST_DESTROY_EX(&s_flistp, NULL);

	return item_pdp;
}
