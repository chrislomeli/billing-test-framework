
#ifndef __BSI__H__
#define __BSI__H__

#include <http/http_Client.h>
#include <bs/obj/bs_NvRecord.h>
#include <xml/tinyxmlinterface.h>
#include <xml/xmloutput.h>
#include "bsi_util.h"
#include "GPGInterface.h"


class bsi_Client
{
private:
	string		_server;
	string		_port;
	string		_url;
	entity_map      vendor_source2key_id;
        string          account_poid;


public:
	bsi_Client(void);
	bsi_Client(string server, string port);
	~bsi_Client(void);

        void add_key(string vendor, string key);
        void set_pass_phrase(string pass_phrase);
	return_t cmo_ping(string& response);
        return_t cmo_store_card(bs_NvRecord& nv, string& response);
        return_t cmo_retrieve_card(bs_NvRecord& nv, string& response);
        return_t cmo_remove_card(bs_NvRecord& nv, string& response);
        return_t cmo_close_account(bs_NvRecord& nv, string& response);
        return_t record_payment(bs_NvRecord& nv, string& response);
        return_t process_command(string request, string& response);
        return_t cmo_process_command(string request, string& response);
        return_t sendPostRequest(string request, string& response);
        return_t sendGetRequest(string request, string& response);
        string format(string temp_str);
        string get_account_poid() { return account_poid;}
        int PrepareResponseFromMap(entity_map *response_map, string& response);
};


#endif
