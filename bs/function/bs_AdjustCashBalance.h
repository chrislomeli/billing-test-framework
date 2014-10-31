#ifndef C__bs_AdjustCashBalance__H
#define C__bs_AdjustCashBalance__H

#include <util/EnvironmentStd.h>
#include <bs/obj/bs_AccountInfo.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ctype/c_Decimal.h>


#define DEFAULT_CASH_ADJUSTMENT_VER 205
#define DEFAULT_CASH_ADJUSTMENT_ID  6000
#define DEFAULT_CASH_ADJUSTMENT_DESCR "Adjustment of cash balance"
#define GENERIC_SERVICE_ADJUSTMENT_CODE	"100"

class bs_AdjustCashBalance
{
public:

  static bool adjust_CashBalance(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

  static bool adjust_CashBalance(
    pin_Session * pinp,
    poid_t * account_pdp,
    pin_decimal_t * amountp,
    string source,
    string descr,
    string service_str,
    string reason_id,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp );

};

#endif

