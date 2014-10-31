#ifndef C__loadUsage__H
#define C__loadUsage__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <pin/PinSession.h>
#include <bs/obj/bs_AccountInfo.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ctype/c_Decimal.h>
#include <ctype/c_DateTime.h>
#include "pin_juno.h"

class bs_LoadUsage
{
public:


  static bool loadVoipCDRUsage(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

  static bool loadUsage(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

  static bool loadUsage(
    pin_Session * pinp,
    poid_t * account_pdp,
    string service_type,
    time_t end_t,
    time_t duration_seconds,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp );

  static bool findMemberServiceId(
    pin_Session * pinp,
    poid_t  * service_type_pdp, /* a partial service poid */
    const string member_no,  /* a UNTD member no */
    string& service_id );

};
#endif

