#ifndef C__bs_ChargebackAccount__H
#define C__bs_ChargebackAccount__H

#include <util/EnvironmentStd.h>
#include <bs/obj/bs_AccountInfo.h>
#include <bs/obj/bs_NvRecord.h>
#include <ctype/c_Poid.h>
#include <ctype/c_Decimal.h>

#define DEFAULT_CHARGEBACK_DESCR "Chargeback "

class bs_ChargebackAccount
{
public:

  static bool chargeback_account(
  pin_Session * pinp,
  bs_NvRecord& nv,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp  );

  static bool chargeback_account(
  pin_Session * pinp,
  poid_t * account_pdp,
  pin_decimal_t * amountp,
  string reason_id,
  string offset,
  string bal_grp_name,
  pin_flist_t   **return_flistpp,
  pin_errbuf_t  *ebufp );

static char * get_trans_id_for_account(
	pin_Session * pinp,
	poid_t* account_pdp,
  	pin_decimal_t **amountdpp,
	time_t   **transaction_date,
	int offset,
	char *bal_grp_name,
	pin_errbuf_t *ebufp);

static poid_t * get_balgrp_poid_from_name(
	pin_Session * pinp,
	poid_t* account_pdp,
	char *bal_grp_name,
	pin_errbuf_t *ebufp);
};

#endif
