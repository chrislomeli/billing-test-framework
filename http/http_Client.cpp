#include "http_Client.h"

http_Client::http_Client(void)
{
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
}

http_Client::~http_Client(void)
{
	curl_easy_cleanup(this->curl);
	curl_global_cleanup();
}

/***************************************************************
 * Send an Http Post request
 ***************************************************************/
long http_Client::sendPostRequest(string url, string postData)
{
	long	ret_code = 0;
	int	postdata_size = 0;
	struct	curl_slist *headers=NULL;

	postdata_size = postData.length()+1;
	headers = curl_slist_append(headers, "Content-Type: text/xml");

	curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(this->curl, CURLOPT_POST, 1);
	curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, postData.c_str());
	curl_easy_setopt(this->curl, CURLOPT_POSTFIELDSIZE, postdata_size);

	ret_code =sendRequest();

	return ret_code;
}

/********************************************************************************
 * send an Http Get request
 *******************************************************************************/
 long http_Client::sendGetRequest(string url)
 {
	long ret_code = 0;

	curl_easy_setopt(this->curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str());

	ret_code =sendRequest();

	return ret_code;
}

/************************************************************************
 * Send the request
 ************************************************************************/
long http_Client::sendRequest()
{

    	CURLcode res = CURLE_OK;
	long http_code = 200;
	long ret_code = 0;

	struct ResponseStruct chunk;
	chunk.response=NULL; /* we expect realloc(NULL, size) to work */
	chunk.size = 0;    /* no data at this point */

	curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteResponseCallback);
	curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, (void *)&chunk);

 	res = curl_easy_perform(this->curl);

	if (res == CURLE_OK) {
		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		this->response = chunk.response;
		ret_code = http_code;
	} else {

		ret_code = res;
		this->response.assign(curl_easy_strerror(res));
	}

	if(chunk.response)
	{
		free(chunk.response);
	}

	return ret_code;
}


void* myrealloc(void *ptr, size_t size)
{
   /* There might be a realloc() out there that doesn't like reallocing
      NULL pointers, so we take care of it here */
	if(ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}

size_t http_Client::WriteResponseCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size *nmemb;
	struct ResponseStruct *resp = (struct ResponseStruct *)data;

   	resp->response = (char *)myrealloc(resp->response, resp->size + realsize + 1);
   	if (resp->response)
   	{
		memcpy(&(resp->response[resp->size]), ptr, realsize);
		resp->size += realsize;
		resp->response[resp->size] = 0;
	}

   	return realsize;
 }

string http_Client::getHttpResponse()
{
	return this->response;
}
