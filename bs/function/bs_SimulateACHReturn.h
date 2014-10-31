#ifndef C__bs_SimulateACHReturn__H
#define C__bs_SimulateACHReturn__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <util/Utility.h>
#include <ctype/c_DateTime.h>

class bs_SimulateACHReturn
{
public:

	bool simulateACHReturn( 
		string trans_id, 
		double amount,
		string reason_code,		
		string ach_pending_dir, //:  /var/juno/queue/fusa/rfr/pending
		string pending_root,	//:  can be std::string("") default = "rfr" - 
		string pending_ext,		//:  can be std::string("") default = "data" -
		bool refund = false );	// return payment is the default

	string err_msg;

};

#endif


