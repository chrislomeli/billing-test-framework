#ifndef C__bs_depositAccountAuths__H
#define C__bs_depositAccountAuths__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <util/Utility.h>
#include <ctype/c_DateTime.h>
#include <pin/PinSession.h>


class bs_DepositAccountAuths
{
public:
	static bool deposit_charges( pin_Session * pinp, poid_t * account_pdp );

private:
	static bool transaction_exists( 
		pin_Session * pinp, 
		char * trans_id, 
		pin_charge_cmd_t command );

};

#endif


