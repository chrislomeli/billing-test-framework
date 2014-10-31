#ifndef C__bs_SignupClient__H
#define C__bs_SignupClient__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>


class bs_SignupClient
{
public:

	// Base function for calling bs_signupd - modeled on postp_signupd
	static bool bsSignup(string bsserver, string bsport, string signup_stream, string& result_set);
};
#endif

