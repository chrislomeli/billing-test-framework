#ifndef C__bs_NvRecord__H
#define C__bs_NvRecord__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <bs/obj/bs_AccountInfo.h>
#include <ctype/c_NvRecord.h>
#include <ctype/c_Poid.h>

#define COMMAND     "Command"
#define RESOURCE_ID   "Resource_id"
#define ACCOUNT_OBJ_ID0   "account_obj_id0"
#define SERVICE_OBJ_ID0   "service_obj_id0"
#define SERVICE_OBJ_TYPE  "service_obj_type"
#define TOPUP     "Topup"
#define ACCOUNT_NO    "account_no"
#define STATUS      "status"
#define STATUS_FLAGS    "status_flags"
#define AMOUNT      "Amount"
#define SOURCE      "Source"
#define LOGIN     "Login"
#define DESCRIPTION   "Description"
#define BILL_TYPE   "Bill_Type"
#define BANK_TYPE   "Bank_Type"
#define DEBIT_NUM   "Debit_Num"
#define DEBIT_ATTR    "Debit_Attr"
#define BILL_TYPE_ARGS    "Bill_Type_Args"
#define BILLING_ACCOUNT_TYPE  "Billing_Account_Type"
#define BILLING_CARDEXPIRATION  "Billing_CardExpiration"
#define BILLING_CARDNUMBER  "Billing_CardNumber"
#define BILLING_ACCOUNT_TYPE  "Billing_Account_Type"
#define JUNO_USERNAME   "Juno_Username"
#define BRAND_CODE    "Brand_Code"
#define CONTACT_LASTNAME  "Contact_LastName"
#define CONTACT_FIRSTNAME "Contact_FirstName"
#define CONTACT_MIDDLEINITIAL "Contact_MiddleInitial"
#define CONTACT_ADDRESS   "Contact_Address"
#define CONTACT_CITY    "Contact_City"
#define CONTACT_STATE   "Contact_State"
#define CONTACT_ZIPCODE   "Contact_ZipCode"
#define CONTACT_EMAIL   "Contact_Email"
#define CONTACT_COUNTRY   "Contact_Country"
#define BILLING_NAME "Billing_Name"
#define BILLING_FIRSTNAME "Billing_FirstName"
#define BILLING_LASTNAME  "Billing_LastName"
#define BILLING_MIDDLEINITIAL "Billing_MiddleInitial"
#define BILLING_ADDRESS   "Billing_Address"
#define BILLING_CITY    "Billing_City"
#define BILLING_STATE   "Billing_State"
#define BILLING_ZIPCODE   "Billing_ZipCode"
#define BILLING_COUNTRYUSA  "Billing_CountryUSA"
#define BILLING_COUNTRY  "Billing_Country"
#define REQUESTED_DEALS   "Requested_Deals"
#define DROP_DEALS    "Drop_Deals"
#define DAY_PHONE   "Day_Phone"
#define EVENING_PHONE   "Evening_Phone"

//---Cash Payment
#define TYPE "TYPE"
#define ORDER_ID "ORDER_ID"
#define PAYMENT_ID "PAYMENT_ID"
#define PAY_METHOD "PAY_METHOD"
#define PAID_AMOUNT "PAID_AMOUNT"
#define PAYMENT_DATE "PAYMENT_DATE"
#define PAYMENT_TIME "PAYMENT_TIME"

//-----------------CMO-----
#define CM_REG_ID "CM_REG_ID"
#define PAYINFO_POID "PAYINFO_POID"
#define FLAG "FLAG"

//--------------
#define CALL_TYPE   "CALL_TYPE"
#define CALL_TO     "CALL_TO"
#define CALL_FROM   "CALL_FROM"
#define DATE      "DATE"
#define MEMBER_NO   "MEMBER_NO"
#define SID     "SID"
#define CID     "CID"
#define COUNT     "COUNT"


class bs_NvRecord : public c_NvRecord
{
public:

  bool resolve_AccountPoid( pin_Session * pinp, c_Poid& account_cp )
  {
    ostringstream account_descr;
    bool rc = false;

    string str_login = this->value(LOGIN);
    string account_no = this->value(ACCOUNT_NO);
    string str_account_obj_id0 =this->value(ACCOUNT_OBJ_ID0);

    if (! str_account_obj_id0.empty() )
    {
      int64 poid_id0 = atol(str_account_obj_id0.c_str());
      account_descr << "account_obj_id0=" << str_account_obj_id0;
      account_cp.create(pinp->getDBNumber(), (char *)"/account", poid_id0 );
    }
    else if (! account_no.empty() )
    {
      bs_AccountInfo  accountInfo;
      account_descr << "account_no=" << account_no;
      if((rc =  accountInfo.loadByAccountNo( pinp, account_no )) == true )
      {
        account_cp.create( accountInfo.account_pdp );
        this->set("MEMBER_NO",accountInfo.member_no);
      }
    }
    if (  ! rc || account_cp.isnull() ) {
      return false;
    }
    return true;
  };

};
#endif
