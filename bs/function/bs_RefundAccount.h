#ifndef C__bs_RefundAccount__H
#define C__bs_RefundAccount__H

#include <util/EnvironmentStd.h>
#include <bs/obj/bs_AccountInfo.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ctype/c_Decimal.h>
#include <util/ErrorCode.h>

#define DEFAULT_ADJUSTMENT_VER 8
#define DEFAULT_ADJUSTMENT_ID  4
#define DEFAULT_REFUND_DESCR "refund of credit balance"
#define FULL_ACCOUNT_BALANCE "FULL"

class bs_RefundAccount
{
public:

  static Return_code refund_Account(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

  static bool refundAccount(
    pin_Session * pinp,
    poid_t * account_pdp,
    pin_decimal_t * amountp,
    string source,
    string descr,
    string service_type,
    pin_flist_t   **return_flistpp, pin_errbuf_t  *ebufp );

};

#endif
