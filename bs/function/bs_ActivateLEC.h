#ifndef __bs_ActivateLEC___HH
#define __bs_ActivateLEC___HH

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <pin/PinSession.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ctype/c_Decimal.h>


class bs_ActivateLEC
{
public:


  static bool activateLEC(
    pin_Session * pinp,
    bs_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

  static bool activateLEC(
    pin_Session * pinp,
    poid_t * account_pdp,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );

  static bool manual_activateLEC(
          pin_Session * pinp,
          poid_t * account_pdp,
          pin_flist_t   **return_flistpp,
          pin_errbuf_t  *ebufp    );

  static bool set_account_active(
          pin_Session * pinp,
          string program_name,
          poid_t * account_pdp ,
          pin_flist_t   **return_flistpp,
          pin_errbuf_t  *ebufp    );

  static bool set_billing_dom(
          pin_Session * pinp,
          poid_t * account_pdp,
          string program_name,
          u_int32                 *dom ,
          pin_flist_t   **return_flistpp,
          pin_errbuf_t  *ebufp    );


};
#endif

