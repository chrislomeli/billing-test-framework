#ifndef C__bs_UpdateContactInfo__H
#define C__bs_UpdateContactInfo__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <bs/obj/bs_AccountInfo.h>
#include <bs/obj/bs_SignupRec.h>
#include <pin/PinSession.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ctype/c_Decimal.h>


class bs_UpdateContactInfo
{
public:

  static bool updateContactInfo(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

  static bool updateContactInfo(
    pin_Session * pinp,
    poid_t * account_pdp,
    bs_SignupRec * bsrp ,
    string source,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

};

#endif
