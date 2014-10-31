#ifndef __BS_ACCOUNT_INFO___
#define __BS_ACCOUNT_INFO___

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <pin/PinSession.h>
#include <ctype/c_PayType.h>
#include <ctype/c_Status.h>

typedef enum e_db_entity_state {
  unknown_State = -1,
  entity_Exists = 1,
  entity_DoesNotExist = 0
} db_entity_state_t;


class bs_AccountInfo
{
public:
  bs_AccountInfo(void);
  ~bs_AccountInfo(void);
  void reset(void);

  bool loadByAccountPoid(
    pin_Session * pinp,
    poid_t * account_pdp );

  bool loadByPoidString(
    pin_Session * pinp,
    string poid_Str );

  bool loadByPoidID(
    pin_Session * pinp,
    string poid_id0 );

  bool loadByPromoCode(
    pin_Session * pinp,
    string m_account_no );

  bool loadByAccountNo(
    pin_Session * pinp,
    string m_account_no );

  bool deleteByAccountNo(
    pin_Session * pinp,
    string m_account_no );

  bool _populate_FromFlist(
      pin_flist_t * flistp );

  string& getAccountNo(string&);
  //string& getMerchant(string&);
  int getPayType(pin_Session * pinp);
  static int getPayType(pin_Session * pinp,poid_t *accnt_pdp);
  int getStatus();
  db_entity_state_t getAccountState();

  void setAccountNo(string);
  //void setMerchant(string);
  void setPayType(pin_pay_type_t);

  static pin_flist_t * get_AccountPayinfo(
          pin_Session * pinp, poid_t * a_pdp );

  static pin_decimal_t * get_AccountBalance(
      pin_Session * pinp, poid_t * a_pdp, int resource = 840 );

  pin_decimal_t * get_AccountBalance( pin_Session * pinp );

  static pin_flist_t * getService_and_BalGrpPoids(
  pin_Session * pinp, poid_t *account_pdp, string &service_str);

  static pin_flist_t * getBilling_and_PaymentInfo(pin_Session *pinp, 
                                          poid_t *balgrp_pdp );

  db_entity_state_t account_state;

  poid_t * account_pdp;
  //poid_t * payinfo_pdp;
  string member_no;
  string promo_code;
  string account_no;
  string gl_segment;
  //string merchant;
  c_PayType pay_type;
  c_Status status;
  int status_flags;
  //int cycle_dom;
  time_t created_t;
  //time_t last_t;
  //time_t next_t;
  //time_t future_t;

};

#endif
