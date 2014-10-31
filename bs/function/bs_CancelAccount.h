#ifndef C__bs_CancelAccount__H
#define C__bs_CancelAccount__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <bs/obj/bs_AccountInfo.h>
#include <bs/obj/bs_ServiceInfo.h>
#include <bs/obj/bs_SignupRec.h>
#include <pin/PinSession.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ops/extension.h>

#ifndef USE_NATIVE_OPCODES
#define UOL_OP_CANCEL_ACCOUNT PCM_OP_CUST_POL_UOL_CANCEL_ACCOUNT
#else
#define UOL_OP_CANCEL_ACCOUNT PCM_OP_CUST_POL_CANCEL_ACCOUNT
#endif


class bs_CancelAccount
{
public:

  static bool cancelAccount(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );


  static bool cancelAccount(
    pin_Session * pinp,
    poid_t * account_pdp,
    string source,
    string description,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

};

#endif
