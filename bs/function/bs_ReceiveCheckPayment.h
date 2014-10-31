#ifndef C__bs_ReceiveCheckPayment__H
#define C__bs_ReceiveCheckPayment__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <bs/obj/bs_AccountInfo.h>
#include <bs/obj/bs_ServiceInfo.h>
#include <bs/obj/bs_SignupRec.h>
#include <pin/PinSession.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ctype/c_Decimal.h>

#define DEFAULT_CHECK_PMT_DESCR "Check Payment"

class bs_ReceiveCheckPayment
{
public:

  static bool receive_CheckPayment(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

  static bool receive_CheckPayment (
    pin_Session * pinp,
    poid_t * account_pdp,
    pin_decimal_t * amountp,
    string source,
    string descr,
    string service_type,
    pin_flist_t **return_flistpp, pin_errbuf_t * ebufp );

};

#endif
