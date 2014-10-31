#ifndef C__bs_UpdatePayInfo__H
#define C__bs_UpdatePayInfo__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <bs/obj/bs_AccountInfo.h>
#include <bs/obj/bs_SignupRec.h>
#include <pin/PinSession.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ctype/c_Decimal.h>
#include <ctype/c_PayType.h>
#include <ops/extension.h>

#define SET_BILLINFO false   // whether to do a set_billinfo as well as set_payinfo

#undef USE_NATIVE_OPCODES
#ifndef USE_NATIVE_OPCODES
#define UOL_OP_SET_PAYINFO PCM_OP_CUST_POL_UOL_SET_PAYINFO
#else
#define UOL_OP_SET_PAYINFO PCM_OP_CUST_SET_PAYINFO
#endif


class bs_UpdatePayinfo
{
public:

  static bool updatePayinfo(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );


  static bool bs_UpdatePayinfo::updatePayinfo(
    pin_Session * pinp,
    poid_t * account_pdp,
    bs_SignupRec * bsrp,
    string source,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

};

#endif

