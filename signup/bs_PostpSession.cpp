#include "bs_PostpSession.h"

int jrpc_debug_level = 0;
int bs_debug_level = 0;



bs_PostpSession::bs_PostpSession(void)
: port(DEFAULT_BS_SIGNUP_PORT)
{
}

bs_PostpSession::~bs_PostpSession(void)
{
}

PostpVersion
bs_PostpSession::getVersionNumber(string	str_version)
{
	if (strcmp(str_version.c_str(),	INVALID_STR) ==	0) {
		return v1_0;
	} else if (strcmp(str_version.c_str(), VERS_2_0) ==	0) {
		return v2_0;
	} else if (strcmp(str_version.c_str(), VERS_2_1) ==	0) {
		return v2_1;
	} else if (strcmp(str_version.c_str(), VERS_2_2) ==	0) {
		return v2_2;
	} else if (strcmp(str_version.c_str(), VERS_3_0) ==	0) {
		return v3_0;
	} else if (strcmp(str_version.c_str(), VERS_4_0) ==	0) {
		return v4_0;
	} else if (strcmp(str_version.c_str(), VERS_5_0) ==	0) {
		return v5_0;
	} else if (strcmp(str_version.c_str(), VERS_5_5) ==	0) {
		return v5_5;
	} else if (strcmp(str_version.c_str(), VERS_7_0S) == 0)	{
		return v7_S;
	} else if (strcmp(str_version.c_str(), VERS_7_S) ==	0) {
		return v7_S;
	} else if (strcmp(str_version.c_str(), VERS_7_T) ==	0) {
		return v7_T;
	} else if (strcmp(str_version.c_str(), VERS_8_S) ==	0) {
		return v8_S;
	} else if (strcmp(str_version.c_str(), VERS_8_T) ==	0) {
		return v8_T;
	} else {
		return v_undef;
	}
}


void bs_PostpSession::setConnectInfo(string _host, int _port, string _program_name, string log_file)
{
	this->host= _host;
	this->port=_port;
	this->program_name=_program_name;

        char * LogFile = (char *)log_file.c_str();

        if (logopen(_program_name.c_str(), LogFile) != SUCCESS) {
                logfile.error("logopen() failed.");
                exit(1);
        }
}

#define	PUT_PARAM(arg_name)	m->PutVal(#arg_name,	(nvrec.value(#arg_name).c_str()) )

int	bs_PostpSession::callBsSignup(
			 c_NvRecord& nvrec,	string sversion )
{
	CSimpleCallObj	simple_rpcobj;
	JRPCClient		clnt;
	NVMap			*m = simple_rpcobj.GetInputMap();
	CStr			output;
	char *callName = NULL;

	/* preconditions */
	if (host.empty() || port <=0 || program_name.empty())
	{
		fprintf(stderr,	"Connection info needs to be set before using callBsSignup ....");
		return CONFIG_ERROR;
	}

	/* get version */
	PostpVersion		 version = this->getVersionNumber(sversion);
	if (version==v_undef)
	{
		fprintf(stderr,	"This version is unknown: %s (%d)", sversion.c_str(), (int)version);
		return CONFIG_ERROR;
	}

	/* All versions	use	these fields --	even upgrade */
	PUT_PARAM(Juno_Username);
	PUT_PARAM(Ad_Number);
	PUT_PARAM(Request_Source);
	PUT_PARAM(Client_Version);
	PUT_PARAM(MachineID);

	/* Only	signups	use	these fields */
	if (version	== v8_S)
	{
		PUT_PARAM(Contact_LastName);
		PUT_PARAM(Contact_FirstName);
		PUT_PARAM(Contact_MiddleInitial);
		PUT_PARAM(Contact_Address1);
		PUT_PARAM(Contact_Address2);
		PUT_PARAM(Contact_City);
		PUT_PARAM(Contact_State);
		PUT_PARAM(Contact_ZipCode);
		PUT_PARAM(Billing_CardNumber);
		PUT_PARAM(Billing_CardExpiration);
		PUT_PARAM(Day_Phone);
		PUT_PARAM(Evening_Phone);
		PUT_PARAM(Billing_LastName);
		PUT_PARAM(Billing_FirstName);
		PUT_PARAM(Billing_MiddleInitial);
		PUT_PARAM(Billing_Address1);
		PUT_PARAM(Billing_Address2);
		PUT_PARAM(Billing_City);
		PUT_PARAM(Billing_State);
		PUT_PARAM(Billing_ZipCode);
		PUT_PARAM(Broadband_Key);
		PUT_PARAM(Contact_Country);
		PUT_PARAM(Billing_Country);
		PUT_PARAM(Brand_Code);
		PUT_PARAM(Bill_Type);
		PUT_PARAM(Bill_Type_Args);
		PUT_PARAM(Billing_Account_Type);
		PUT_PARAM(Action);
		PUT_PARAM(Serial_Num);
		PUT_PARAM(Referrer);
		PUT_PARAM(Referral_Program);
		PUT_PARAM(Requested_Deals);
		PUT_PARAM(Drop_Deals);
		PUT_PARAM(Cancel_User);
		PUT_PARAM(XFuture_Date);
		PUT_PARAM(Member_Number);
		PUT_PARAM(Requested_Services);
		PUT_PARAM(Drop_Services);
		PUT_PARAM(Cancel_Member_Number);
		PUT_PARAM(CVM);
		PUT_PARAM(TopUpAmount);
		callName   = (char *)MPB_SIGNUP_CALL_NAME;

	}
	else if	(version ==	v8_T)	
	{
		m->PutVal("Juno_Password", INVALID_STR);
		PUT_PARAM(Serial_Num);
		PUT_PARAM(Requested_Deals);
		PUT_PARAM(Drop_Deals);
		PUT_PARAM(Action);
		PUT_PARAM(Cancel_User);
		PUT_PARAM(XFuture_Date);
		PUT_PARAM(Member_Number);
		PUT_PARAM(Requested_Services);
		PUT_PARAM(Drop_Services);
		PUT_PARAM(Cancel_Member_Number);
		PUT_PARAM(CVM);
		PUT_PARAM(TopUpAmount);
		callName   = (char *)MPB_TRANSITION_CALL_NAME;
	} else {
		logfile.printf("This version is no longer supported: %s", sversion.c_str());
		return CONFIG_ERROR;
	}

	/* Basically, this will	never fail */
	if (clnt.Init(this->host.c_str(),	port, CALL_TIMEOUT)	!= SUCCESS)	{
		logfile.printf("s:	JRPCClient::Init() failed.", "bs_PostpSession");
		logfile.printf("%s:	host = %s, port	= %u", program_name,
			host, port);
		logfile.printf("%d||", (int) BS_SIGNUP_REQUEUE_FAILURE);
		return CONNECT_ERROR;
	}

	if (clnt.Call(callName,	&simple_rpcobj)	!= SUCCESS)	{
		logfile.printf("%s:	rpc	failed.\n",	"bs_PostpSession");
		logfile.printf("%d||", (int) BS_SIGNUP_REQUEUE_FAILURE);
		return CONNECT_ERROR;
	}

	output.Format("%s|%s|%s", simple_rpcobj.GetCode(),	simple_rpcobj.GetRequestedMpv(), 
		simple_rpcobj.GetInitialMpv());

	logfile.Close(FALSE);

	int rc = 999;
	const char * rs = simple_rpcobj.GetCode();

	if (rs)
		rc=atoi(rs);
	else
		logfile.printf("Can not get a return code from: %s\n", rs );

	return rc;
}


