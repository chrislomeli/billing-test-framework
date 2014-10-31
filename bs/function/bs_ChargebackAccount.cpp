
#include "bs_ChargebackAccount.h"
#include <util/Portability.h>

bool
bs_ChargebackAccount::chargeback_account(
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
	string reason_id = nv.value("REASON_ID");
	string offset = nv.value("OFFSET");
	string bal_grp_name = nv.value("BAL_GRP");

	bool rc = false;
	int64 database = pinp->getDBNumber();
	ostringstream account_descr;

	string title("chargeback_account nvRec");  nv.StringBuffer(title);
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
	rc = chargeback_account(
			pinp,
			account_cp.borrow(),
			amount_cp.borrow(),
			reason_id,
			offset,
			bal_grp_name,
			return_flistpp,
			ebufp );

	if (! rc ) {
		if (! PIN_ERR_IS_ERR(ebufp))
			pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
				PIN_ERR_BAD_VALUE, PIN_FLD_TRANS_ID, 0, 0);
		account_descr << "-Error while executing chargeback opcode";
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)((account_descr.str()).c_str()), ebufp );
		return false;
	}

	return rc;

};

bool bs_ChargebackAccount::chargeback_account(
	pin_Session * pinp,
	poid_t * account_pdp,
	pin_decimal_t * amountp,
	string reason_id,
	string offset,
	string bal_grp_name,
	pin_flist_t   **return_flistpp,
	pin_errbuf_t  *ebufp )
{
	pin_flist_t	*input_flistp = NULL;
	int		offset_int = atoi(offset.c_str());
	char		*trans_id = (char *)NULL;
	time_t		*transaction_date = NULL;
	ostringstream os;
	bool rc = false;
	PIN_ERR_CLEAR_ERR(ebufp);

	pin_decimal_t *amount_from_tablep=NULL;
	trans_id = get_trans_id_for_account(pinp, account_pdp, &amount_from_tablep, &transaction_date, offset_int, (char *)bal_grp_name.c_str(), ebufp );

	if ( trans_id == NULL ) {
		if (! PIN_ERR_IS_ERR(ebufp))
			pin_set_err(ebufp, PIN_ERRLOC_APP,  PIN_ERRCLASS_APPLICATION,
				PIN_ERR_BAD_VALUE, PIN_FLD_TRANS_ID, 0, 0);
		os << "-Can not find transaction id";
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,(char*)((os.str()).c_str()), ebufp );
		return false;
	}

	if(pbo_decimal_is_zero(amountp,ebufp)){
		amountp=pbo_decimal_copy(amount_from_tablep,ebufp);
	}
	pbo_decimal_destroy(&amount_from_tablep);

	//--- adjust------------------------------
	input_flistp = PIN_FLIST_CREATE(ebufp);

	PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_POID,(void *) account_pdp, ebufp);
	PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_AMOUNT, (void *)amountp, ebufp);
	PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_TRANS_ID, (void *)trans_id, ebufp);
	PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_STRING, (void *)reason_id.c_str(), ebufp);
	PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_PURCHASE_END_T, (void *)transaction_date, ebufp);
	PIN_FLIST_FLD_SET(input_flistp, PIN_FLD_DESCR, (void *)DEFAULT_CHARGEBACK_DESCR, ebufp);

	/*
	* Call opcode
	*/
	rc = pinp->call_opcode( PCM_OP_UOL_POL_CHARGEBACK, 0, input_flistp,
		return_flistpp, ebufp);

	PIN_FLIST_DESTROY_EX(&input_flistp,ebufp);
	return rc;

}

/*
* This function will return transaction id of a payment event
* for a particular account based on the offset value.
*/

char *
bs_ChargebackAccount::get_trans_id_for_account(
	pin_Session * pinp,
	poid_t* account_pdp,
	pin_decimal_t ** amount_dpp,
	time_t		**transaction_date,
	int offset,
	char *bal_grp_name,
	pin_errbuf_t *ebufp)
{

	pin_flist_t * results_flistp = (pin_flist_t *)NULL;
	pin_flist_t * s_flistp = (pin_flist_t *)NULL;
	pin_flist_t	*t1 = (pin_flist_t *)NULL;
	pin_flist_t	*pymt_flistp = (pin_flist_t *)NULL;
	pin_flist_t	*balgrp_flistp = (pin_flist_t *)NULL;
	poid_t		*balgrp_pdp = NULL;
	poid_t		*event_pdp = NULL;
	poid_t		*pdp = NULL;
	time_t		*created_t = (time_t *)0;
	char		*trans_id = (char *)NULL;
	int32           flag = SRCH_DISTINCT;
	bool		rc = true;
	int		elemid = offset*(-1) ;

	balgrp_pdp = get_balgrp_poid_from_name(pinp, account_pdp, bal_grp_name, ebufp);
	if(PIN_POID_IS_NULL(balgrp_pdp)) {
		return trans_id;
	}

	const char	*template0 = "select X from /event where F1 like V1 and F2 = V2 and F4 = V4 order by F3 desc ";

	int64	db = 0;

	db = PIN_POID_GET_DB(account_pdp);
	s_flistp = PIN_FLIST_CREATE(ebufp);
	pdp = PIN_POID_CREATE(db, (char *)"/search", (int64)-1, ebufp);
	PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, (void *)pdp, ebufp);

	PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, (void *)&flag, ebufp);
	PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void *)template0, ebufp);

	event_pdp = PIN_POID_CREATE(db, "/event/billing/payment%", (int64)-1, ebufp);

	t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
	PIN_FLIST_FLD_SET(t1, PIN_FLD_POID, (void *)event_pdp, ebufp);

	t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 2, ebufp);
	PIN_FLIST_FLD_SET(t1, PIN_FLD_ACCOUNT_OBJ, (void *)account_pdp, ebufp);

	t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 3, ebufp);
	PIN_FLIST_FLD_SET(t1, PIN_FLD_CREATED_T, (void *)created_t, ebufp);

	t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 4, ebufp);
	balgrp_flistp = PIN_FLIST_SUBSTR_ADD(t1, PIN_FLD_BAL_IMPACTS, ebufp);
	PIN_FLIST_FLD_SET(balgrp_flistp, PIN_FLD_BAL_GRP_OBJ, (void *)balgrp_pdp, ebufp);

	t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_RESULTS, 0, ebufp);

	rc = pinp->call_opcode(PCM_OP_SEARCH, 0, s_flistp, &results_flistp, ebufp);
	PIN_FLIST_DESTROY_EX(&s_flistp,ebufp);

	if ( (! rc) || (results_flistp == (pin_flist_t *)NULL) ) {
		return trans_id;
	}

	t1 = (pin_flist_t *)PIN_FLIST_ELEM_GET(results_flistp, PIN_FLD_RESULTS, elemid, 1,ebufp);
	*transaction_date = (time_t *)PIN_FLIST_FLD_TAKE(t1, PIN_FLD_CREATED_T, 0, ebufp);
	pymt_flistp = (pin_flist_t *)PIN_FLIST_SUBSTR_GET(t1, PIN_FLD_PAYMENT, 0, ebufp);

	trans_id = (char *)PIN_FLIST_FLD_GET(pymt_flistp, PIN_FLD_TRANS_ID, 0, ebufp);
	*amount_dpp = (pin_decimal_t *)PIN_FLIST_FLD_TAKE(pymt_flistp, PIN_FLD_AMOUNT, 0, ebufp);
	return trans_id;
}


poid_t *
bs_ChargebackAccount::get_balgrp_poid_from_name(
	pin_Session * pinp,
	poid_t* account_pdp,
	char *bal_grp_name,
	pin_errbuf_t *ebufp)
{

	const char	*template0 = "select X from /balance_group where F1 = V1 and F2 = V2 ";
	pin_flist_t * results_flistp = (pin_flist_t *)NULL;
	pin_flist_t * s_flistp = (pin_flist_t *)NULL;
	pin_flist_t	*t1 = (pin_flist_t *)NULL;
	poid_t		*balgrp_pdp = NULL;
	poid_t		*pdp = NULL;
	bool		rc = true;
	int32		flag = SRCH_DISTINCT;
	int64		db = 0;

	db = PIN_POID_GET_DB(account_pdp);
	s_flistp = PIN_FLIST_CREATE(ebufp);
	pdp = PIN_POID_CREATE(db, (char *)"/search", (int64)-1, ebufp);
	PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, (void *)pdp, ebufp);

	PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, (void *)&flag, ebufp);
	PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void *)template0, ebufp);

	t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
	PIN_FLIST_FLD_SET(t1, PIN_FLD_NAME, (void *)bal_grp_name, ebufp);

	t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 2, ebufp);
	PIN_FLIST_FLD_SET(t1, PIN_FLD_ACCOUNT_OBJ, (void *)account_pdp, ebufp);

	t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_RESULTS, 0, ebufp);

	rc = pinp->call_opcode(PCM_OP_SEARCH, 0, s_flistp, &results_flistp, ebufp);
	PIN_FLIST_DESTROY_EX(&s_flistp,ebufp);

	if ( (! rc) || (results_flistp == (pin_flist_t *)NULL) ) {
		return balgrp_pdp;
	}

	t1 = (pin_flist_t *)PIN_FLIST_ELEM_GET(results_flistp, PIN_FLD_RESULTS, 0, 1,ebufp);
	balgrp_pdp = (poid_t *)PIN_FLIST_FLD_GET(t1, PIN_FLD_POID, 0, ebufp);

	return balgrp_pdp;
}

