#ifndef C__bs_SimulateChargeback__H
#define C__bs_SimulateChargeback__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <util/Utility.h>
#include <ctype/c_DateTime.h>

class bs_SimulateChargeback
{
public:

	bool simulateChargeback( 
		string trans_id, 
		double amount,
		string reason_code,		
		string category,		
		time_t *transaction_date,
		string cb_pending_dir,
		string pending_root,
		string pending_ext,
		string usage_code,
		bool cb_reversal = false );
	string err_msg;

};

#endif


