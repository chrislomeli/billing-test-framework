#include <bs/obj/bs_SignupRec.h>

inline void CONDITIONAL_ADD_STR (
        ostringstream& os, const char * name, char * value, string col_delim, int& type ) {
        if ( strcmp( name, "-" ) == 0 )
                return;
        os << name << value << col_delim;
        type++;
}


bs_SignupRec::bs_SignupRec(void)
{
  reset();
}

bs_SignupRec::~bs_SignupRec(void)
{
}

void bs_SignupRec::reset(void)
{
  col_delim = ":";

  n_cycleDom=0;
  n_billType=0;
  n_bankType=0;

  memset (s_accountId ,0,255);
  memset (s_domain ,0,255);
  memset (s_lastName ,0,255);
  memset (s_firstName ,0,255);
  memset (s_brandName,0,255);
  memset (s_caddress ,0,255);
  memset (s_ccity ,0,255);
  memset (s_cstate ,0,60);
  memset (s_czip ,0,60);
  memset (s_baddress ,0,255);
  memset (s_bcity ,0,255);
  memset (s_bstate ,0,60);
  memset (s_bzip ,0,60);
  memset (s_addDeals ,0,255);
  memset (s_dropDeals ,0,255);
  memset (s_debitNum ,0,60);
  memset (s_debitAttr  ,0,60);
  memset( s_addNumber,0,60);
  memset( s_referrer,0,255);
  memset( s_referralProgram,0,255);
  memset( s_action,0,60);
  memset( s_requestSource,0,60);
  memset( s_clientVersion,0,60);
  memset( s_jrpcProto,0,60);
  memset( s_dayPhone,0,35);
  memset( s_evePhone,0,35);
  memset( s_cemail_addr,0,60);
  memset( s_topUp,0,60);
  memset( s_CVM,0,60);
        memset( s_XFuture_Date,0,255);
        memset( s_Member_Number,0,255);
        memset( s_Requested_Services,0,255);
        memset( s_Drop_Services,0,255);
        memset( s_Cancel_Member_Number,0,255);
        memset( s_vendor,0,255);

}


string bs_SignupRec::formatAccountData(string& account_pkg, int mode )
{
	char cancel_chr = (n_cancel) ? 'Y' : 'N';

	ostringstream os;
	ostringstream payinfo;

	//-- needs refinment - bs_signup rules are always in flux
	bool bss_include_deals = false;
	bool bss_include_contact_addr = false;
	bool bss_cancel_account = false;
	bool bss_include_billing_addr = false;
	bool bss_include_payinfo = false;
	bool bss_include_referral = false;
	bool bss_include_voip = false;
	bool bss_include_action = false;

	switch (mode){

  case BS_MODE_RAW:
	  bss_include_deals = true;
	  bss_include_contact_addr = true;
	  bss_include_billing_addr = true;
	  bss_include_payinfo = true;
	  bss_include_referral = true;
	  bss_include_voip = true;
	  os << "JRPC_PROTO_PROC=JRPC_MPB_SIGNUP" << col_delim;
	  break;

  case BS_MODE_SIGNUP:
	  bss_include_deals = true;
	  bss_include_contact_addr = true;
	  bss_include_billing_addr = true;
	  bss_include_payinfo = true;
	  bss_include_referral = true;
	  bss_include_voip = true;
	  os << "JRPC_PROTO_PROC=JRPC_MPB_SIGNUP" << col_delim;
	  break;

  case BS_MODE_CANCEL:
	  bss_cancel_account = true;
	  os << "JRPC_PROTO_PROC=JRPC_MPB_TRANSITION" << col_delim;
	  break;


  case BS_MODE_TRANSITION:
	  bss_include_action = true;
	  bss_include_voip = true;
	  bss_include_deals = true;
	  os << "JRPC_PROTO_PROC=JRPC_MPB_TRANSITION" << col_delim;
	  break;

  case BS_MODE_UPDATE:
  default:
	  bss_include_deals =
		  ( strlen( s_addDeals ) > 1 || strlen(s_dropDeals) > 1 );
	  bss_include_contact_addr = ( strlen(s_czip) > 1  );
	  bss_include_billing_addr = ( strlen(s_bzip) > 1  );
	  bss_include_payinfo = ( n_billType >= 10000 && strlen(s_debitNum) > 3  );
	  bss_include_referral = ( strlen(s_referrer) > 1  );

	  os << "JRPC_PROTO_PROC=JRPC_MPB_SIGNUP" << col_delim;
	  break;
	}

	string login(s_accountId);  // not used for cancels only create and update
	if ( strlen(s_domain) > 0 ) {
		login.append("@");
		login.append( s_domain );
	}



	/////// All types - header data ///////////////////////////
	os  << "Juno_Username="   <<  login << col_delim
		<< "Juno_Password=INVALID"  << col_delim
		<< "Ad_Number=" << s_accountId << col_delim
		<< "Client_Version="  << s_clientVersion << col_delim
		<< "Serial_Num=123456"  << col_delim
		<< "Member_Number=" << s_Member_Number << col_delim
		<< "Cancel_Member_Number=" << s_Cancel_Member_Number << col_delim
		<< "XFuture_Date=" << s_XFuture_Date << col_delim
		<< "Requested_Services=" << s_Requested_Services << col_delim
		<< "Drop_Services=" << s_Drop_Services << col_delim;

	if(strlen(s_vendor) ==0)
		os << "MachineID=BW437C4AAA5Q67Q5AAA7HFUAAE2LXB" << col_delim;
	else
		os << "MachineID="  << s_vendor << col_delim;

	if ( strlen(s_brandName) > 0 )
		os << "Brand_Code="    << s_brandName << col_delim;

	if ( strlen(s_requestSource) == 0 )
		os << "Request_Source=TESS" << col_delim;
	else
		os << "Request_Source="    << s_requestSource << col_delim;


	/////// Action data ///////////////////////////
	if ( bss_include_action )
	{
		if ( strlen(s_action) > 0)
			os  << "Action=" <<  s_action << col_delim;
	}

	/////// Canary version data ///////////////////////////
	if ( bss_include_voip )
	{
		if ( strlen(s_topUp) > 0)
			os  << "TopUpAmount=" <<  s_topUp << col_delim;
		if ( strlen(s_CVM) > 0)
			os  << "CVM=" <<  s_CVM << col_delim;
	}

	/////// Signup data ///////////////////////////
	if ( bss_include_contact_addr )
	{ os
	<< "Contact_LastName="  <<  s_lastName << col_delim
	<< "Contact_FirstName=" <<  s_firstName << col_delim
	<< "Contact_MiddleInitial=" << col_delim
	<< "Contact_Address1="  <<  s_caddress << col_delim
	<< "Contact_Address2=" << col_delim
	<< "Contact_City="    <<  s_ccity << col_delim
	<< "Contact_State="   <<  s_cstate << col_delim
	<< "Contact_ZipCode=" <<  s_czip << col_delim
	<< "Contact_Country=USA" << col_delim
	<< "Day_Phone="     << s_dayPhone << col_delim
	<< "Evening_Phone="   << s_evePhone << col_delim;
	}

	if ( bss_include_payinfo )
	{
		switch(this->n_billType) 
		{

		case 10005:
			bss_include_billing_addr = true;
			os << "Bill_Type=ACH_BILLING" << col_delim
				<< "Billing_CardNumber="  <<  this->s_debitNum << col_delim
				<< "Bill_Type_Args="    <<  this->s_debitAttr << col_delim
				<< "Billing_Account_Type="  << this->n_bankType   << col_delim
				<< "Billing_CardExpiration=" << col_delim;
			break;

		case 10016:
			bss_include_billing_addr = false;
			os << "Bill_Type=LEC_BILLING" << col_delim
				<< "Billing_CardNumber=" << col_delim
				<< "Bill_Type_Args="  <<  this->s_debitNum << col_delim
				<< "Billing_Account_Type=" << col_delim
				<< "Billing_CardExpiration=" << col_delim;
			break;

		case 10000:
			bss_include_billing_addr = true;
			os << "Bill_Type=PREPAID" << col_delim
				<< "Billing_CardNumber=" << col_delim
				<< "Bill_Type_Args=" << col_delim
				<< "Billing_Account_Type=" << col_delim
				<< "Billing_CardExpiration=" << col_delim;
			break;

		case 10001:
			bss_include_billing_addr = true;
			os << "Bill_Type=INV" << col_delim
				<< "Billing_CardNumber=" << col_delim
				<< "Bill_Type_Args=" << col_delim
				<< "Billing_Account_Type=" << col_delim
				<< "Billing_CardExpiration=" << col_delim;
			break;

		case 10011:
			bss_include_billing_addr = false;
			os << "Bill_Type=CASH_BILLING" << col_delim
				<< "Billing_CardNumber=" << col_delim
				<< "Bill_Type_Args=" << col_delim
				<< "Billing_Account_Type=" << col_delim
				<< "Billing_CardExpiration=" << col_delim;
			break;

		case 10012:
			bss_include_billing_addr = false;
			os << "Bill_Type=CHECK_BILLING" << col_delim
				<< "Billing_CardNumber=" << col_delim
				<< "Bill_Type_Args=" << col_delim
				<< "Billing_Account_Type=" << col_delim
				<< "Billing_CardExpiration=" << col_delim;
			break;

		case 10003:
		default:
			bss_include_billing_addr = true;
			os << "Bill_Type=CC" << col_delim
				<< "Billing_CardNumber="        <<  this->s_debitNum << col_delim
				<< "Bill_Type_Args=" << col_delim
				<< "Billing_Account_Type=" << col_delim
				<< "Billing_CardExpiration="
				<< this->s_debitAttr  << col_delim;
			break;

		}
	}

	if ( bss_include_billing_addr )
	{ os
	<< "Billing_FirstName=" <<   s_firstName  << col_delim
	<< "Billing_LastName="  <<   s_lastName  << col_delim
	<< "Billing_MiddleInitial=" << col_delim
	<< "Billing_Address1="  <<  s_baddress << col_delim
	<< "Billing_Address2=" << col_delim
	<< "Billing_City="              <<  s_bcity << col_delim
	<< "Billing_State="             <<  s_bstate << col_delim
	<< "Billing_ZipCode="   <<  s_bzip << col_delim
	<< "Billing_Country=USA" << col_delim;
	}


	if ( bss_include_referral )
	{ 
		os	<< "Referrer="      << s_referrer << col_delim
			<< "Referral_Program="  << s_referralProgram << col_delim;
	}

	/////// Signup,Transition or Cancel data ///////////////////////////
	if ( bss_cancel_account )
	{ 
		os
			<< "Cancel_User="   << s_cemail_addr << col_delim; // a little hack because we set this for cancels
	}
	else if ( bss_include_deals )
	{ 
		os
			<< "Requested_Deals="   <<  s_addDeals << col_delim
			<< "Drop_Deals="    << s_dropDeals << col_delim;

	}

	/////// All types - trailer data ///////////////////////////
	os
		<< "Action="  << s_action << col_delim
		<< "MPB"  << col_delim
		<< "Broadband_Key=" << col_delim
		<< "SEROBJ_VERS_NUMBER=0" << col_delim
		<< "JRPC_PROTO_DIR=0"
		<< "\015\012";


	account_pkg.assign(os.str());
	return account_pkg;

}

string bs_SignupRec::formatSignupData(string& account_pkg)
{
        char cancel_chr = (n_cancel) ? 'Y' : 'N';

        ostringstream os;
        ostringstream payinfo;


        string login(s_accountId);
        if ( strlen(s_domain) > 0 ) {
                login.append("@");
                login.append( s_domain );
        }


        //-- needs refinment - bs_signup rules are always in flux
        os << "Juno_Username="          <<  login << col_delim
                << "Ad_Number=" << s_accountId << col_delim
                << "Request_Source="    << s_requestSource << col_delim
                << "Client_Version="    << s_clientVersion << col_delim
                << "Brand_Code="                <<  s_brandName << col_delim;

  if(strlen(s_vendor) ==0)
                os << "MachineID=BW437C4AAA5Q67Q5AAA7HFUAAE2LXB" << col_delim;
        else
                os << "MachineID="      << s_vendor << col_delim;


  int use_contact = 0;
  int use_billing = 0;
  int use_other = 0;
  CONDITIONAL_ADD_STR( os,  "Contact_LastName="  , s_lastName , col_delim, use_contact );
  CONDITIONAL_ADD_STR( os,  "Contact_FirstName=" , s_firstName , col_delim, use_contact );
  CONDITIONAL_ADD_STR( os,  "Contact_Address1="  , s_caddress , col_delim, use_contact );
  CONDITIONAL_ADD_STR( os,  "Contact_City="      , s_ccity , col_delim,  use_contact );
  CONDITIONAL_ADD_STR( os,  "Contact_State="     , s_cstate , col_delim, use_contact );
  CONDITIONAL_ADD_STR( os,  "Contact_ZipCode="   , s_czip , col_delim, use_contact );
  CONDITIONAL_ADD_STR( os,  "Day_Phone="         , s_dayPhone , col_delim, use_other );
  CONDITIONAL_ADD_STR( os,  "Evening_Phone="     , s_evePhone , col_delim, use_other );
  CONDITIONAL_ADD_STR( os,  "Billing_FirstName=" , s_firstName  , col_delim, use_billing );
  CONDITIONAL_ADD_STR( os,  "Billing_LastName="  , s_lastName  , col_delim, use_billing  );
  CONDITIONAL_ADD_STR( os,  "Billing_Address1="  , s_baddress , col_delim, use_billing  );
  CONDITIONAL_ADD_STR( os,  "Billing_City="      , s_bcity , col_delim, use_billing  );
  CONDITIONAL_ADD_STR( os,  "Billing_State="     , s_bstate , col_delim, use_billing  );
  CONDITIONAL_ADD_STR( os,  "Billing_ZipCode="   , s_bzip , col_delim, use_billing  );
  CONDITIONAL_ADD_STR( os,  "Referrer="          , s_referrer , col_delim, use_other );
  CONDITIONAL_ADD_STR( os,  "Referral_Program="  , s_referralProgram , col_delim, use_other );
  CONDITIONAL_ADD_STR( os,  "Requested_Deals="   , s_addDeals , col_delim, use_other );
  CONDITIONAL_ADD_STR( os,  "Drop_Deals="        , s_dropDeals , col_delim, use_other );
  CONDITIONAL_ADD_STR( os,  "Action="            , s_action , col_delim, use_other );
  CONDITIONAL_ADD_STR( os,  "JRPC_PROTO_PROC="   , s_jrpcProto , col_delim, use_other );

  if (use_billing > 0) {
  os  << "Billing_Address2=" << col_delim
    << "Billing_Country=USA" << col_delim
    << "Billing_MiddleInitial="<< col_delim;
  }
  if (use_contact > 0) {
  os  << "Contact_Address2=" << col_delim
    << "Contact_Country=USA" << col_delim
    << "Contact_MiddleInitial="<< col_delim;
  }

  switch(this->n_billType)
  {
    case 10003:
                        os << "Bill_Type=CC" << col_delim
                           << "Billing_CardNumber="        <<  this->s_debitNum << col_delim
                           << "Bill_Type_Args=" << col_delim
                           << "Billing_Account_Type=" << col_delim
                           << "Billing_CardExpiration="
                           << this->s_debitAttr  << col_delim;
                break;

                case 10005:
                        os << "Bill_Type=ACH_BILLING" << col_delim
                           << "Billing_CardNumber="        <<  this->s_debitNum << col_delim
                           << "Bill_Type_Args="            <<  this->s_debitAttr << col_delim
                           << "Billing_Account_Type="      << this->n_bankType   << col_delim
                           << "Billing_CardExpiration=" << col_delim;
                break;
                case 10016:
                        os << "Bill_Type=LEC_BILLING" << col_delim
                           << "Billing_CardNumber=" << col_delim
                           << "Bill_Type_Args="    <<  this->s_debitNum << col_delim
                           << "Billing_Account_Type=" << col_delim
                           << "Billing_CardExpiration=" << col_delim;
                break;

                default:
                        os << "Bill_Type=CHECK" << col_delim
                           << "Billing_CardNumber=" << col_delim
                           << "Bill_Type_Args=" << col_delim
                           << "Billing_Account_Type=" << col_delim
                           << "Billing_CardExpiration=" << col_delim;
                        break;
        }

  os << "Referrer="       << s_referrer << col_delim
  << "Referral_Program="  << s_referralProgram << col_delim
  << "Requested_Deals="   <<  s_addDeals << col_delim
  << "Drop_Deals="        << s_dropDeals << col_delim
  << "Cancel_User="       << cancel_chr << col_delim
  << "Action="            << s_action << col_delim
  << "JRPC_PROTO_PROC="   << s_jrpcProto << col_delim
  << "Cancel_User="       << cancel_chr << col_delim
  << "MPB"                << col_delim
  << "Broadband_Key="     << col_delim
  << "SEROBJ_VERS_NUMBER=0" << col_delim
  << "JRPC_PROTO_DIR=0"
  << "\015\012";

        account_pkg.assign(os.str());
        return account_pkg;

}



string bs_SignupRec::formatTransitionData(string& account_pkg )
{
        char cancel_chr = (n_cancel) ? 'Y' : 'N';

        ostringstream os;
        ostringstream payinfo;


        string login(s_accountId);
        if ( strlen(s_domain) > 0 ) {
                login.append("@");
                login.append( s_domain );
        }


        //-- needs refinment - bs_signup rules are always in flux
        os << "Juno_Username="          <<  login << col_delim
                << "Ad_Number=" << s_accountId << col_delim
                << "Request_Source="    << s_requestSource << col_delim
                << "Client_Version="    << s_clientVersion << col_delim
                << "Brand_Code="                <<  s_brandName << col_delim;

  if(strlen(s_vendor) ==0)
                os << "MachineID=BW437C4AAA5Q67Q5AAA7HFUAAE2LXB" << col_delim;
        else
                os << "MachineID="      << s_vendor << col_delim;

        os << "Referrer="       << s_referrer << col_delim
        << "Referral_Program="  << s_referralProgram << col_delim
        << "Requested_Deals="   <<  s_addDeals << col_delim
        << "Drop_Deals="        << s_dropDeals << col_delim
        << "Cancel_User="       << cancel_chr << col_delim
        << "Action="            << s_action << col_delim
        << "JRPC_PROTO_PROC="   << s_jrpcProto << col_delim
        << "Cancel_User="       << cancel_chr << col_delim
        << "Serial_Num=123456"  << col_delim
        << "MPB"                << col_delim
        << "Broadband_Key="     << col_delim
        << "SEROBJ_VERS_NUMBER=0" << col_delim
        << "JRPC_PROTO_DIR=0"
        << "\015\012";

        account_pkg.assign(os.str());
        return account_pkg;

}

