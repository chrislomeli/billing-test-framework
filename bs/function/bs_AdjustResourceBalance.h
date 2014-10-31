#ifndef C__adjust_ResourceBalance__H
#define C__adjust_ResourceBalance__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <pin/PinSession.h>
#include <bs/obj/bs_AccountInfo.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ctype/c_Decimal.h>

#define DEFAULT_ADJUSTMENT_VER 8
#define DEFAULT_ADJUSTMENT_ID  4
#define DEFAULT_ADJUSTMENT_DESCR "Adjust Resource"
#define AMOUNT          "Amount"

class bs_AdjustResourceBalance
{
public:

  static bool adjust_ResourceBalance(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    bool    is_limit,
    pin_errbuf_t  *ebufp  );

  static bool adjust_ResourceBalance(
    pin_Session * pinp,
    poid_t * account_pdp,
    pin_decimal_t * amountp,
    int element_id,
    string source,
    string &service_str,
    pin_flist_t   **return_flistpp,
    bool is_limit,
    pin_errbuf_t  *ebufp );

};
#endif

