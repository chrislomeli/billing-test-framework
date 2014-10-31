#include "bs_DealCache.h"

bs_DealCache::bs_DealCache(void)
: universal_plan_flistp(NULL)
, deal_flistp(NULL)
{
	reset();
}

bs_DealCache::~bs_DealCache(void)
{
}

void bs_DealCache::createPlanFlist(
	char* login, 
	vector<int> deal_list, 
	pin_flist_t ** return_flist )
{
    pin_flist_t    * r_flistp = NULL;
    pin_flist_t    * s_flistp = NULL;
	pin_cookie_t    my_cookie = NULL;
    int32             element_id=0;
	int			* ip = NULL;
	void		* vp = NULL;
    pin_errbuf_t ebuf;
    pin_errbuf_t * ebufp = &ebuf;
	vector<int>::iterator p;

	const char * aac_source = "TEST";
	const char * aac_promo = "~invalid";
	const char * aac_package = "-";
	const char * aac_serial = "-";
	const char * aac_access = "TEST-HARNESS";
	const char * password = "password";

    PIN_ERR_CLEAR_ERR(ebufp);

	//-- first make a copy of the universal flist --
	r_flistp = PIN_FLIST_COPY(this->universal_plan_flistp, ebufp);
	

	//--  find a service for each deal ---
	for ( p = deal_list.begin();  p != deal_list.end(); p++ )
			this->addDeal( *p, &r_flistp );


	//--foreach service---
	while ((s_flistp = PIN_FLIST_ELEM_GET_NEXT(r_flistp, PIN_FLD_SERVICES,
			&element_id, 1, &my_cookie, ebufp)) != NULL)
	{
		//-- things we add to all services --
		if ((vp = PIN_FLIST_FLD_GET( s_flistp, PIN_FLD_AAC_PACKAGE, 1, ebufp )) == NULL )
			PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_AAC_PACKAGE,(void *)aac_package, ebufp);

		PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_AAC_SOURCE,(void *)aac_source, ebufp);
		PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_AAC_PROMO_CODE,(void *)aac_promo, ebufp);
		PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_AAC_SERIAL_NUM,(void *)aac_serial, ebufp);
		PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_AAC_ACCESS,(void *)aac_access, ebufp);
		PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_LOGIN,(void *)login, ebufp);
		PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_PASSWD_CLEAR,(void *)password, ebufp);
	
	}
	*return_flist = r_flistp;

}

int bs_DealCache::addDeal(
	int deal_id, pin_flist_t ** service_flistpp)
{
	int			* ip = NULL;
    pin_cookie_t    my_cookie = NULL;
    pin_cookie_t    my_cookie2 = NULL;
    int32             element_id=0;
    int32             element_id2=0;
    pin_errbuf_t ebuf;
    pin_errbuf_t * ebufp = &ebuf;
    pin_flist_t    * flistp = NULL;
    pin_flist_t    * r_flistp = NULL;
    pin_flist_t    * s_flistp = NULL;
	poid_t * exist_pdp = NULL;
	poid_t * deal_pdp = NULL;
	poid_t * svc_pdp = NULL;

	char deal_str[60];
	sprintf(deal_str,"%d", deal_id);

	char *	permitted_str = NULL;
	bool seeking_service = true;

	PIN_ERR_CLEAR_ERR(ebufp);

	//-- foreach deal poid with a deal_id= deal_id----
	while ((flistp = PIN_FLIST_ELEM_GET_NEXT(this->deal_flistp, PIN_FLD_RESULTS,
            &element_id, 1, &my_cookie, ebufp)) != NULL)
	{
		if ((ip= (int*)PIN_FLIST_FLD_GET( flistp, UNTD_FLD_DEAL_ID, 1, ebufp )) == NULL )
			continue;
		
		if (*ip != deal_id) 
			continue;

		//-- found a deal - find the service and add it --
		seeking_service = true;
		my_cookie2 = NULL;
		element_id2=0;
		deal_pdp = PIN_FLIST_FLD_GET( flistp, PIN_FLD_POID, 1, ebufp );
		permitted_str = (char*)PIN_FLIST_FLD_GET( flistp, PIN_FLD_PERMITTED, 1, ebufp );

		while ( seeking_service && (s_flistp = PIN_FLIST_ELEM_GET_NEXT(*service_flistpp, PIN_FLD_SERVICES,
				&element_id2, 1, &my_cookie2, ebufp)) != NULL)
		{
			//--get poid_type ---
			if ((svc_pdp = PIN_FLIST_FLD_GET( s_flistp, PIN_FLD_SERVICE_OBJ, 1, ebufp )) == NULL )
				continue;

			if ( strcmp(permitted_str, pin_poid_get_type(svc_pdp)) == 0 )
			{
				exist_pdp = PIN_FLIST_FLD_GET( s_flistp, PIN_FLD_DEAL_OBJ, 1, ebufp );
				if ( pin_poid_is_null(exist_pdp) ) {
					PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_DEAL_OBJ,(void *)deal_pdp, ebufp);
					PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_AAC_PACKAGE,(char *)deal_str, ebufp);
					seeking_service = false;
					break;
				}
			}
		}
		//-- no 'empty' service slot found - add another ---
		if ( seeking_service ) {
			s_flistp = PIN_FLIST_ELEM_ADD(*service_flistpp,PIN_FLD_SERVICES,PIN_ELEMID_ASSIGN,ebufp);
			svc_pdp = pin_poid_create((int64)1, permitted_str, (int64)-1, ebufp);
			PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_SERVICE_OBJ,(void *)svc_pdp, ebufp);
			PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_DEAL_OBJ,(void *)deal_pdp, ebufp);
			PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_AAC_PACKAGE,(void *)deal_str, ebufp);
		}
	}
	return 0;
}


bool bs_DealCache::cacheDeals(pin_Session * pinp)
{
        poid_t*         pdp=NULL;
        pin_flist_t     *s_flistp = NULL;
        pin_flist_t     *r_flistp = NULL;
        pin_flist_t * t1 = NULL;
        pin_flist_t * t2 = NULL;
        pin_errbuf_t ebuf;
        pin_errbuf_t * ebufp = &ebuf;
		time_t ltime;
        int32           s_flags = SRCH_DISTINCT;
        int             i=1;
		int				zero = 0;
        const char* select = 
			"select X from /deal where F1 <> V1 and F2 <= V2 and " \
			" ( F3 >= V3 OR F4 = V4 ) and " \
			" lower(name) not like '%x-now%' and lower(name) not like '%x-defer%' ";

        PIN_ERR_CLEAR_ERR(ebufp);


        /***********************************************************
         * Search Args
         ***********************************************************/
        /*
         * Search::
         */
        s_flistp = PIN_FLIST_CREATE(ebufp);
		pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/search", (int64)-1, ebufp);
        PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, pdp, ebufp);
        PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, &s_flags, ebufp);
        PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void*)select, ebufp);

        /*
         * Args::
         */
		time(&ltime);
        t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
                PIN_FLIST_FLD_SET(t1, UNTD_FLD_DEAL_ID, (void*)&zero, ebufp);
        t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 2, ebufp);
                PIN_FLIST_FLD_SET(t1, PIN_FLD_START_T, (void*)&ltime, ebufp);
        t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 3, ebufp);
                PIN_FLIST_FLD_SET(t1, PIN_FLD_END_T, (void*)&ltime, ebufp);
        t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 4, ebufp);
                PIN_FLIST_FLD_SET(t1, PIN_FLD_END_T, (void*)&zero, ebufp);

        /*
         * Results::
         */
        r_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_RESULTS, 0, ebufp);
        PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_POID, NULL,ebufp);
        PIN_FLIST_FLD_SET(r_flistp, UNTD_FLD_DEAL_ID, NULL,ebufp);
        PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_PERMITTED, NULL,ebufp);
        PIN_FLIST_FLD_SET(r_flistp, PIN_FLD_NAME, NULL,ebufp);

		pinp->call_opcode( PCM_OP_SEARCH, 0, s_flistp, &(this->deal_flistp), ebufp);

        if ( PIN_ERR_IS_ERR(ebufp) || this->deal_flistp == (pin_flist_t*)NULL ) {
			if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
			if(this->deal_flistp) PIN_FLIST_DESTROY_EX(&this->deal_flistp, NULL);
			this->deal_flistp = NULL;
			return false;
		}

		if ( PIN_FLIST_ELEM_COUNT(r_flistp, PIN_FLD_RESULTS, &ebuf) >= 1 ) {
			if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
			if(this->deal_flistp) PIN_FLIST_DESTROY_EX(&this->deal_flistp, NULL);
			this->deal_flistp = NULL;
 			return false;
       }

	
        /***********************************************************
         * Clean up.
         ***********************************************************/
        if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
        return true;
}


bool bs_DealCache::getUniversalPlan(pin_Session * pinp)
{
        poid_t*         pdp=NULL;
        pin_flist_t     *s_flistp = NULL;
        pin_flist_t     *r_flistp = NULL;
        pin_flist_t * t1 = NULL;
        pin_flist_t * t2 = NULL;
        pin_errbuf_t ebuf;
        pin_errbuf_t * ebufp = &ebuf;
        int32           s_flags = SRCH_DISTINCT;
        int             i=1;
		int				zero = 0;
        const char* select = "select X from /plan where F1 = V1 ";
		const char * uname = "UOL Universal Plan";

        PIN_ERR_CLEAR_ERR(ebufp);


        /***********************************************************
         * Search Args
         ***********************************************************/
        /*
         * Search::
         */
        s_flistp = PIN_FLIST_CREATE(ebufp);
		pdp = PIN_POID_CREATE(pinp->getDBNumber(), (char*)"/search", (int64)-1, ebufp);
        PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, pdp, ebufp);
        PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, &s_flags, ebufp);
        PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void*)select, ebufp);

        /*
         * Args::
         */
        t1 = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
                PIN_FLIST_FLD_SET(t1, PIN_FLD_NAME, (void*)uname, ebufp);

        /*
         * Results::
         */
        PIN_FLIST_ELEM_SET(s_flistp, NULL, PIN_FLD_RESULTS, 0, ebufp);

		pinp->call_opcode( PCM_OP_SEARCH, 0, s_flistp, &r_flistp, ebufp);

        if ( PIN_ERR_IS_ERR(ebufp) || r_flistp == (pin_flist_t*)NULL ) {
			if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
			if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
			r_flistp = NULL;
			return false;
		}

		if ( PIN_FLIST_ELEM_COUNT(r_flistp, PIN_FLD_RESULTS, &ebuf) < 1 ) {
			if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
			if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
			r_flistp = NULL;
 			return false;
       }
	
		//--asssume only one ----
		this->universal_plan_flistp = 
			(pin_flist_t*)PIN_FLIST_ELEM_TAKE(r_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, 1, ebufp);

        /***********************************************************
         * Clean up.
         ***********************************************************/
        if(s_flistp) PIN_FLIST_DESTROY(s_flistp, NULL);
        if(r_flistp) PIN_FLIST_DESTROY_EX(&r_flistp, NULL);

        return true;
}


void bs_DealCache::reset()
{
	m_initialized = false;
	if ( universal_plan_flistp )
		PIN_FLIST_DESTROY_EX(&universal_plan_flistp,NULL);
	if ( deal_flistp )
		PIN_FLIST_DESTROY_EX(&deal_flistp,NULL);
	this->universal_plan_flistp = NULL;
	this->deal_flistp =NULL;
}


bool bs_DealCache::initialize(pin_Session * pinp, bool force)
{
	if ( m_initialized && ! force )
		return true;

	reset();

	if (! this->getUniversalPlan(pinp))
		return false;
	if (! this->cacheDeals(pinp) )
		return false;
	this->m_initialized = true;
	return true;
}
