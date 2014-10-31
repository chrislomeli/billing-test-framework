#ifndef C__tess_SignupRec__H
#define C__tess_SignupRec__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>

#define BS_MODE_SIGNUP 0
#define BS_MODE_UPDATE 2
#define BS_MODE_RAW 3
#define BS_MODE_TRANSITION  4
#define BS_MODE_CANCEL  5

class bs_SignupRec
{
public:
	bs_SignupRec(void);
	~bs_SignupRec(void);

	string formatAccountData(string&, int mode = BS_MODE_SIGNUP );
	string formatTransitionData(string&);
	string formatSignupData(string& account_pkg);

	int n_cycleDom; 
	int n_billType; 
	int n_bankType; 
	int n_cancel; 

	string 	col_delim;


	char s_domain [255]; 
	char s_accountId [255]; 
	char s_lastName [255]; 
	char s_firstName [255]; 
	char s_brandName[255];
	char s_caddress [255]; 
	char s_ccity [255]; 
	char s_cstate [60]; 
	char s_czip [60]; 
	char s_baddress [255]; 
	char s_bcity [255]; 
	char s_bstate [60]; 
	char s_bzip [60]; 
	char s_addDeals [255]; 
	char s_dropDeals [255]; 
	char s_debitNum [60]; 
	char s_debitAttr  [60]; 
	char s_addNumber[60];
	char s_referrer[255];
	char s_referralProgram[255];
	char s_action[60];
	char s_requestSource[60];
	char s_clientVersion[60];
	char s_jrpcProto[60];
	char s_dayPhone[35];
	char s_evePhone[35];
	char s_cemail_addr[60];
	char s_topUp[60];
	char s_CVM[60];
	char s_XFuture_Date[255];
	char s_Member_Number[255];
	char s_Requested_Services[255];
	char s_Drop_Services[255];
	char s_Cancel_Member_Number[255];
	char s_vendor[255];


	void reset(void);
};
#endif
