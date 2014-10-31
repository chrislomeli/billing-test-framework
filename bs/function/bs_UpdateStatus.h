#ifndef C__bs_UpdateStatus__H
#define C__bs_UpdateStatus__H

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
#define UOL_OP_SET_STATUS PCM_OP_CUST_POL_UOL_SET_STATUS
#else
#define UOL_OP_SET_STATUS PCM_OP_CUST_POL_SET_STATUS
#endif


class bs_UpdateStatus
{
public:

  static bool updateStatus(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );


  static bool updateStatus(
    pin_Session * pinp,
    poid_t * account_pdp,
    poid_t * service_pdp,
    int status,
    int status_flags,
    string source,
    string description,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

};

#endif
