#include "bs_SignupClient.h"
#include <tcp/tcp_Client.h>

// Base function for calling bs_signupd - modeled on postp_signupd
bool bs_SignupClient::bsSignup( 
	string bsserver,
	string bsport,
	string signup_stream, string& result_set )
{
	string return_code;
	result_set.erase();

	//---Connect to bs_signupd-------------------
	tcp_Client tcp;

	Return_code rc = tcp.Connect(bsserver.c_str(), bsport.c_str(), 20);

	if ( rc != Success ) {
		result_set.assign("PANIC: can not connect to bs-signupd - exiting ...");
		return false;
	}

	//---Write to bs_signupd -------------------
	cout << string("Write to bs_signupd ...");
        int n = tcp.Write_buffer( signup_stream.c_str() );
	if ( n <= 0 ) {
		result_set.assign("PANIC: can not write to bs-signupd - exiting ...");
		return false;
	}

	//---Read from bs_signupd -------------------
	cout << string("Read from bs_signupd ...");
        rc = tcp.Read_buffer();
	switch (rc) {
	case BadSocket:
		return_code.assign("returned BadSocket");	
		break;	
	case ServerTimedOut:
		return_code.assign("returned ServerTimedOut");	
		break;	
	case NoMoreInput:
		return_code.assign("returned NoMoreInput");	
		break;	
	case IOReadFailed:
		return_code.assign("returned IOReadFailed");	
		break;	
	case Success:
		return_code.assign("returned Success");	
		break;	
	default:
		return_code.assign("returned Unknown return code");	
		break;	
	}
	cout << "bs_signupd return: " << return_code << endl;


	//---return values------------------------
	result_set.assign(tcp.returnStream.str());

	if (result_set.empty()) 
	{
		cout << "No result set from bs_signup!" << endl;	
	}

	//---cleanup and leave----------------------
	return (rc == Success) ;
}
