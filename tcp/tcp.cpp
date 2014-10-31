// tcp.cpp : Defines the entry point for the console application.
//
#include <util/EnvironmentStd.h>
#include "tcp_Client.h"


int main(int argc, char* argv[])
{
	ostringstream os;
	tcp_Client tcp;

	Return_code rc = tcp.Connect("w1.nyc.test.juno.com","1801", 20);
	if ( rc != Success )
		return 0;

	cout << "Connected with fd=" << tcp.fd;
	sleep(4);


	os << "Juno_Username=tess_test:"
		<< "Brand_Code=NZ:"
		<< "Contact_LastName=Teslau1:"
		<< "Contact_FirstName=Tess1:"
		<< "Contact_MiddleInitial=:"
		<< "Contact_Address1=MyAddress:"
		<< "Contact_Address2=:"
		<< "Contact_City=Woodland Hills:"
		<< "Contact_State=CA:"
		<< "Contact_ZipCode=90266:"
		<< "Contact_Country=USA:"
		<< "Billing_FirstName=Tess2:"
		<< "Billing_LastName=Teslau2:"
		<< "Billing_MiddleInitial=:"
		<< "Billing_Address1=3012 Towsend:"
		<< "Billing_Address2=:"
		<< "Billing_City=Weslake:"
		<< "Billing_State=CA:"
		<< "Billing_ZipCode=90266:"
		<< "Billing_Country=USA:"
		<< "Bill_Type=ACH_BILLING:"
		<< "Billing_CardNumber=4444111122223333:"
		<< "Billing_CardExpiration=0109:"
		<< "Bill_Type_Args=:"
		<< "Billing_Account_Type=:"
		<< "Requested_Deals=1024:"
		<< "Drop_Deals=:"
		<< "Ad_Number=900264:"
		<< "Referrer=INVALID:"
		<< "Referral_Program=INVALID:"
		<< "Action=:"
		<< "Day_Phone=3107987802:"
		<< "Evening_Phone=3107987802:"
		<< "Request_Source=MPB_POSTP:"
		<< "Client_Version=WEB:"
		<< "JRPC_PROTO_PROC=JRPC_MPB_SIGNUP:"
		<< "Cancel_User=:"
		<< "MPB:"
		<< "Broadband_Key=:"
		<< "MachineID=BW437C4AAA5Q67Q5AAA7HFUAAE2LXB:"
		<< "Serial_Num=123456:"
		<< "SEROBJ_VERS_NUMBER=0:"
		<< "JRPC_PROTO_DIR=0"
		<< "\015\012";

		string tryme ( os.str() );
		cout << "Write:\n" << tryme;

		int n = tcp.Write_buffer( tryme.c_str() );
		cout << "Wrote " << n << " bytes ..." << endl;
		sleep(2);

		tcp.Read_buffer();
		cout << "Read: " << endl;
		cout << tcp.buffer;



	return 1;
}

