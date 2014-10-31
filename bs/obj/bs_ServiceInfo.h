#ifndef C__bs_ServiceInfo__H
#define C__bs_ServiceInfo__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <ctype/c_Status.h>
#include <ctype/c_Currency.h>
#include <pin/PinSession.h>


class bs_ServiceInfo
{
public:
	bs_ServiceInfo(void);
	~bs_ServiceInfo(void);

	bool loadByAccount_Type(
		pin_Session * pinp,
		poid_t * account_pdp,
		string service_type,
		int& service_count );

	string login;
	string type;
	c_Status status;
	int status_flags;
	poid_t * service_pdp;

	string bal_grp_name;

	c_Currency balance;
	bool Exists(void);
};
#endif

