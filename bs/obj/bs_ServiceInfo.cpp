#include "bs_ServiceInfo.h"

bs_ServiceInfo::bs_ServiceInfo(void) : service_pdp(NULL)
{
}

bs_ServiceInfo::~bs_ServiceInfo(void)
{

	if (! pin_poid_is_null(service_pdp) ) {
		pin_errbuf_t ebuf;
		PIN_ERR_CLEAR_ERR(&ebuf);
		pin_poid_destroy(service_pdp, &ebuf);
	}
}


/*******************************************************************
 * loadService ():
 *******************************************************************/
bool
bs_ServiceInfo::loadByAccount_Type(
	pin_Session * pinp,
	poid_t * account_pdp,
	string service_type,
	int& service_count )
{
	poid_t*		pdp=NULL;
	poid_t*		svc_pdp=NULL;
	pin_flist_t	*flistp = NULL;	
	pin_flist_t	*s_flistp = NULL;	
	pin_flist_t	*r_flistp = NULL;	
	pin_flist_t * t1 = NULL;
	int32		s_flags = SRCH_DISTINCT;
	const char* sql_stmt = "select X from /service where F1 = V1 and F2 = V2 ";
	bool rc = false;
	void * vp = NULL;

	pin_errbuf_t    ebuf;
	pin_errbuf_t    * ebufp = &ebuf;

	PIN_ERR_CLEAR_ERR(ebufp);


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
		PIN_FLIST_FLD_SET(t1, PIN_FLD_ACCOUNT_OBJ, (void*)account_pdp, ebufp);

	svc_pdp = PIN_POID_CREATE(pinp->getDBNumber(), 
		(char*)service_type.c_str(), (int64)-1, ebufp);

	t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 2, ebufp);
		PIN_FLIST_FLD_PUT(t1, PIN_FLD_POID, (void*)svc_pdp, ebufp);


	/*
	 * Results::
	 */
    t1 = PIN_FLIST_ELEM_ADD( s_flistp, PIN_FLD_RESULTS, 0, ebufp);
		PIN_FLIST_FLD_SET(t1, PIN_FLD_POID, (void*)NULL, ebufp);
		PIN_FLIST_FLD_SET(t1, PIN_FLD_LOGIN, (void*)NULL, ebufp);
		PIN_FLIST_FLD_SET(t1, PIN_FLD_STATUS, (void*)NULL, ebufp);
		PIN_FLIST_FLD_SET(t1, PIN_FLD_STATUS_FLAGS, (void*)NULL, ebufp);

	/*
	 * Read services or exit 
	 */
	if (! pinp->call_opcode( PCM_OP_SEARCH, 0, s_flistp, &r_flistp, ebufp)) {
		if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
		if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
		service_count = -1;
		return false;
	}


	/*
	 *  Valid for accounts having only one
	 *  of any given service - if we find 0 or more than one of the same service
	 *  for the same account then this method is not 
	 *  valid for isolating a specific service
	 */
	pin_flist_t *	b_flistp = NULL;
	pin_cookie_t	cookie = NULL;
	int32			elemid;
	svc_pdp			= NULL;
	rc = false;

	service_count = PIN_FLIST_ELEM_COUNT(r_flistp, PIN_FLD_RESULTS, ebufp );
	switch ( service_count ) {
		case 1:
			rc = true;
			break;
		default:
			if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
			if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
			return false;
	}

	while ((b_flistp = PIN_FLIST_ELEM_GET_NEXT(r_flistp, PIN_FLD_RESULTS,
                &elemid, 1, &cookie, ebufp)) != (pin_flist_t *)NULL) 
	{ 
		if ((vp = PIN_FLIST_FLD_GET(b_flistp, PIN_FLD_STATUS_FLAGS, 1, ebufp )) != NULL )
			status_flags = *(int*)vp;
		if ((vp = PIN_FLIST_FLD_TAKE(b_flistp, PIN_FLD_POID, 1, ebufp )) != NULL )
			service_pdp = (poid_t*)vp;
		if ((vp = PIN_FLIST_FLD_GET(b_flistp, PIN_FLD_STATUS, 1, ebufp )) != NULL )
			this->status.set((int*)vp);
		if ((vp = PIN_FLIST_FLD_GET(b_flistp, PIN_FLD_LOGIN, 1, ebufp )) != NULL )
			login.assign((char*)vp);
		type.assign(service_type);
		break;
	}

	if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
    if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
	return rc;
}


bool bs_ServiceInfo::Exists(void)
{
	bool good_pdp = (pin_poid_is_null(this->service_pdp) == 0) ? true:false;

	return ( good_pdp &&
				this->status.isValid() &&
				!this->login.empty() );

}
