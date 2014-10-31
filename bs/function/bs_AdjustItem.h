#ifndef C__bs_AdjustItem__H
#define C__bs_AdjustItem__H

#include <util/EnvironmentStd.h>
#include <bs/obj/bs_AccountInfo.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ctype/c_Decimal.h>

class bs_AdjustItem
{
	public:

	static bool adjust_item(
		pin_Session * pinp,
		bs_NvRecord& nv,
		pin_flist_t   **return_flistpp,
		pin_errbuf_t  *ebufp  );

	static bool adjust_item(
		pin_Session * pinp,
		poid_t * account_pdp,
		pin_decimal_t * amountp,
		string source,
		string service_str,
		string item_type,
		string reason_id,
		pin_flist_t   **return_flistpp,
		pin_errbuf_t  *ebufp );

	static poid_t * get_poid_for_adjust_item(
		pin_Session * pinp,
		poid_t *account_pdp,
		string service_str,
		string item_type,
		pin_errbuf_t *ebufp);
};

#endif
