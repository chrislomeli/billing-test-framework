#ifndef C__http_Client_H
#define C__http_Clinet_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <iostream.h>

using namespace std;

struct ResponseStruct {
	char *response;
	size_t size;
};


void* myrealloc(void *ptr, size_t size);

class http_Client
{

private:
	string	response;
	CURL	*curl;
	static 	size_t WriteResponseCallback(void *ptr, size_t size, size_t nmemb, void *data);
	long	sendRequest(void);

public:

	http_Client(void);
	~http_Client(void);

	long sendPostRequest(string url, string postData);
	long sendGetRequest(string url);
	string getHttpResponse(void);
};

#endif
