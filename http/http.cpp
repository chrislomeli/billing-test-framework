// http.cpp : Defines the entry point for the console application.
//

#include "http_Client.h"


int main(int argc, char* argv[])
{
	http_Client http;

	string get_url = "cs03.hyd.int.untd.com:8888/bsi.untd?trancd=ASUM&member_number=67038105&type=RETRIEVE";
	string post_url = "cs03.hyd.int.untd.com:8888/bsi.untd?";
	string data= "<trancd>ASUM</trancd>\n<member_number>67038105</member_number>\n<type>RETRIEVE</type>";

	cout << "Reqesting (GET) ....\n";
	long rc = http.sendGetRequest(get_url);

	cout << "Returned: " <<rc<< endl;
	cout << http.getHttpResponse()<<endl;

	cout<<" Requesting (POST) ...\n";
	rc = http.sendPostRequest(post_url, data);

	cout << "Returned: " <<rc<< endl;
	cout << http.getHttpResponse()<<endl;



	return 1;
}

