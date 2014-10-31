#ifndef C__bs_CollectAccount__H
#define C__bs_CollectAccount__H

#include <util/EnvironmentStd.h>
#include <bs/obj/bs_AccountInfo.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ctype/c_Decimal.h>
#include <util/ErrorCode.h>

#define FULL_ACCOUNT_BALANCE "FULL"

class bs_CollectAccount
{
public:

  static int  collect_Account(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

  static int collectAccount(
    pin_Session * pinp,
    const char * program_name,
    poid_t * account_pdp,
    pin_decimal_t * amountp,
    string service_type,
    pin_flist_t    **out_flistpp,
    pin_errbuf_t  * ebufp );

};

#endif
