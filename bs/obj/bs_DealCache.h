#ifndef HH_bs_DealCache__H
#define HH_bs_DealCache__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <pin/PinSession.h>


class bs_DealCache
{
public:
	bs_DealCache(void);
	~bs_DealCache(void);

	void createPlanFlist(char* login, vector<int> deal_id, pin_flist_t ** return_flist);
	int addDeal( int deal_id, pin_flist_t ** return_flist);
	bool getUniversalPlan(pin_Session * pinp);
	bool cacheDeals(pin_Session * pinp);

	pin_flist_t * universal_plan_flistp;
	pin_flist_t * deal_flistp;
	int last_elem;
	bool m_initialized;

	bool initialize(pin_Session * pinp, bool force = false);
	void reset();

};

#endif

