#ifndef C__bs_CreateBillingAccount__H
#define C__bs_CreateBillingAccount__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <bs/obj/bs_DealCache.h>
#include <bs/obj/bs_SignupRec.h>
#include <pin/PinSession.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <util/Utility.h>



#define DEFAULT_ADDRESS "21301 Burbank Blvd."
#define DEFAULT_CITY  "Woodland Hills"
#define DEFAULT_STATE "CA"
#define DEFAULT_ZIP   "91367"

class bs_CreateBillingAccount
{
public:

  static bool createAccount(
    pin_Session * pinp,
    string source,
    bs_SignupRec bsr );

  static bs_DealCache * deal_cache;
};


#endif


