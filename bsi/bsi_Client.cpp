#include "bsi_Client.h"

string UOL_PASSPHRASE;

bsi_Client::bsi_Client(void)
{
	_server = "";
	_port = "";
	_url = "";
        account_poid = "";
        initGPGME(GPGME_PROTOCOL_OpenPGP);
}

bsi_Client::bsi_Client(string server, string port)
{
	_server = server;
	_port = port;
	_url = "http://"+_server+":"+_port+"/bsi.untd?";
        account_poid = "";
        initGPGME(GPGME_PROTOCOL_OpenPGP);
}

bsi_Client::~bsi_Client(void)
{
}

void bsi_Client::add_key(string vendor, string key)
{
        PUTIN_MAP(&vendor_source2key_id, key.c_str(), vendor.c_str());
}

void bsi_Client::set_pass_phrase(string pass_phrase)
{
        UOL_PASSPHRASE = pass_phrase;
}

return_t bsi_Client::cmo_ping(string& response)
{
        response = "";
        string time_stamp;

        time_t    cur_time;
        cur_time=time(NULL);
        ostringstream os;
        os <<cur_time ;
        time_stamp.assign(os.str());

        string plain_request = string("<?xml version=\"1.0\" ?>\n")+
                                "<VENDOR>[DNS resolvable? hostname.]tess.com</VENDOR>\n"+
                                "<TIMESTAMP>"+time_stamp+"</TIMESTAMP>\n"+
                                "<VERSION>1.0</VERSION>\n"+
                                "<SEQUENCE>1</SEQUENCE>\n"+
                                "<REQUEST>Ping</REQUEST>\n"+
                                "<TEMPLATE>\n"+
                                "       <SEQUENCE>1</SEQUENCE>\n"+
                                "</TEMPLATE>\n\0";


        return cmo_process_command(plain_request, response);

}

return_t bsi_Client::cmo_store_card(bs_NvRecord& nv, string& response)
{
        response = "";

        string time_stamp;

        time_t    cur_time;
        cur_time=time(NULL);
        ostringstream os;
        os <<cur_time ;
        time_stamp.assign(os.str());
        string billing_request("");

        string plain_request = string("<?xml version=\"1.0\" ?>\n")+
                                "<VENDOR>[DNS resolvable? hostname.]tess.com</VENDOR>\n"+
                                "<TIMESTAMP>"+time_stamp+"</TIMESTAMP>\n"+
                                "<VERSION>1.0</VERSION>\n"+
                                "<SEQUENCE>1</SEQUENCE>\n"+
                                "<REQUEST>Store_Card</REQUEST>\n"+
                                "<TEMPLATE>\n";

        if( nv.exists("CM_REG_ID") )
        {
                plain_request += "      <CM_REGISTRATION_ID>"+nv.value(CM_REG_ID)+"</CM_REGISTRATION_ID>\n";
        }

        if( nv.exists("DEBIT_NUM") )
        {
                plain_request += "      <CC_NUM>"+nv.value(DEBIT_NUM)+"</CC_NUM>\n";
        }

        if( nv.exists("PAYINFO_POID") )
        {
                plain_request += "      <PAY_INFO_POID>"+nv.value(PAYINFO_POID)+"</PAY_INFO_POID>\n";
        }

        if( nv.exists("DEBIT_ATTR") )
        {
                plain_request += "      <CC_EXPIRY_DATE>"+nv.value(DEBIT_ATTR)+"</CC_EXPIRY_DATE>\n";
        }

        if( nv.exists("BILLING_NAME") )
        {
                plain_request += "      <BILLING_NAME>"+nv.value(BILLING_NAME)+"</BILLING_NAME>\n";
        }

        if( nv.exists("BILLING_ADDRESS") )
        {

                billing_request += "            <BILLING_ADDRESS_LINE1>"+nv.value(BILLING_ADDRESS)+"</BILLING_ADDRESS_LINE1>\n"+
                                 "            <BILLING_ADDRESS_LINE2></BILLING_ADDRESS_LINE2>\n";
        }

        if( nv.exists("BILLING_CITY") )
        {
                billing_request += "      <BILLING_CITY>"+nv.value(BILLING_CITY)+"</BILLING_CITY>\n";

        }

        if( nv.exists("BILLING_STATE") )
        {
                billing_request += "      <BILLING_STATE>"+nv.value(BILLING_STATE)+"</BILLING_STATE>\n";

        }

        if( nv.exists("BILLING_COUNTRY") )
        {
                billing_request += "      <BILLING_COUNTRY>"+nv.value(BILLING_COUNTRY)+"</BILLING_COUNTRY>\n";

        }

        if( nv.exists("BILLING_ZIPCODE") )
        {
                billing_request += "      <BILLING_ZIP>"+nv.value(BILLING_ZIPCODE)+"</BILLING_ZIP>\n";

        }

        if( !billing_request.empty() )
        {
				plain_request += "      <BILLING_ADDRESS>\n";
				plain_request += billing_request;
    		    plain_request += "      </BILLING_ADDRESS>\n";
		}
        plain_request += "</TEMPLATE>\n";

        if( nv.exists("FLAG") )
        {
                plain_request += string("<flags>\n")+
                                 "      <flag name = \""+nv.value(FLAG)+"\"/>\n"+
                                 "</flags>\n";
        }

        plain_request += "\0";

        return cmo_process_command(plain_request, response);

}

return_t bsi_Client::cmo_retrieve_card(bs_NvRecord& nv, string& response)
{

        response = "";

        string time_stamp;

        time_t    cur_time;
        cur_time=time(NULL);
        ostringstream os;
        os <<cur_time ;
        time_stamp.assign(os.str());

        string plain_request = string("<?xml version=\"1.0\" ?>\n")+
                                "<VENDOR>[DNS resolvable? hostname.]tess.com</VENDOR>\n"+
                                "<TIMESTAMP>"+time_stamp+"</TIMESTAMP>\n"+
                                "<VERSION>1.0</VERSION>\n"+
                                "<SEQUENCE>1</SEQUENCE>\n"+
                                "<REQUEST>Retrieve_Card</REQUEST>\n"+
                                "<TEMPLATE>\n";

        if( nv.exists("CM_REG_ID") )
        {
                plain_request += "<CM_REGISTRATION_ID>"+nv.value(CM_REG_ID)+"</CM_REGISTRATION_ID>\n";
        }

        if( nv.exists("PAYINFO_POID") )
        {
                plain_request += "<PAY_INFO_POID>"+nv.value(PAYINFO_POID)+"</PAY_INFO_POID>\n";
        }

        plain_request += "</TEMPLATE>\n";
        if( nv.exists("FLAG") )
        {
                plain_request += string("<flags>\n")+
                                 "      <flag name = \""+nv.value(FLAG)+"\"/>\n"+
                                 "</flags>\n";
        }

        plain_request += "\0";

        return cmo_process_command(plain_request, response);

}

return_t bsi_Client::cmo_remove_card(bs_NvRecord& nv, string& response)
{

        response = "";

        string time_stamp;

        time_t    cur_time;
        cur_time=time(NULL);
        ostringstream os;
        os <<cur_time ;
        time_stamp.assign(os.str());

        string plain_request = string("<?xml version=\"1.0\" ?>\n")+
                                "<VENDOR>[DNS resolvable? hostname.]tess.com</VENDOR>\n"+
                                "<TIMESTAMP>"+time_stamp+"</TIMESTAMP>\n"+
                                "<VERSION>1.0</VERSION>\n"+
                                "<SEQUENCE>1</SEQUENCE>\n"+
                                "<REQUEST>Remove_Card</REQUEST>\n"+
                                "<TEMPLATE>\n";

        if( nv.exists("CM_REG_ID") )
        {
                plain_request += "      <CM_REGISTRATION_ID>"+nv.value(CM_REG_ID)+"</CM_REGISTRATION_ID>\n";
        }

        if( nv.exists("PAYINFO_POID") )
        {
                plain_request += "      <PAY_INFO_POID>"+nv.value(PAYINFO_POID)+"</PAY_INFO_POID>\n";
        }

        plain_request += "</TEMPLATE>\n";
        if( nv.exists("FLAG") )
        {
                plain_request += string("<flags>\n")+
                                 "      <flag name = \""+nv.value(FLAG)+"\"/>\n"+
                                  "</flags>\n";
        }

        plain_request += "\0";

        return cmo_process_command(plain_request, response);

}

return_t bsi_Client::cmo_close_account(bs_NvRecord& nv, string& response)
{

        response = "";

        string time_stamp;

        time_t    cur_time;
        cur_time=time(NULL);
        ostringstream os;
        os <<cur_time ;
        time_stamp.assign(os.str());

        string plain_request = string("<?xml version=\"1.0\" ?>\n")+
                                "<VENDOR>[DNS resolvable? hostname.]tess.com</VENDOR>\n"+
                                "<TIMESTAMP>"+time_stamp+"</TIMESTAMP>\n"+
                                "<VERSION>1.0</VERSION>\n"+
                                "<SEQUENCE>1</SEQUENCE>\n"+
                                "<REQUEST>Close_Account</REQUEST>\n"+
                                "<TEMPLATE>\n";

        if( nv.exists("CM_REG_ID") )
        {
                plain_request += "      <CM_REGISTRATION_ID>"+nv.value(CM_REG_ID)+"</CM_REGISTRATION_ID>\n";
        }

        plain_request += "</TEMPLATE>\n";
        if( nv.exists("FLAG") )
        {
                plain_request += string("<flags>\n")+
                                 "      <flag name = \""+nv.value(FLAG)+"\"/>\n"+
                                 "</flags>\n";
        }

        plain_request += "\0";

        return cmo_process_command(plain_request, response);

}

return_t bsi_Client::record_payment(bs_NvRecord& nv, string& response)
{

        response = "";

        string time_stamp;

        time_t    cur_time;
        cur_time=time(NULL);
        ostringstream os;
        os <<cur_time ;
        time_stamp.assign(os.str());

        string plain_request = string("<?xml version=\"1.0\"?>\n");
        plain_request += "<trancd>RECP</trancd>\n";

        if( nv.exists(TYPE) )
        {
                plain_request += "<type>"+nv.value(TYPE)+"</type>\n";
        }

        if( nv.exists(SOURCE) )
        {
                plain_request += "<SOURCE>"+nv.value(SOURCE)+"</SOURCE>\n";
        }

        if( nv.exists(PAYMENT_ID) )
        {
                plain_request += "<PAYMENT_ID>"+nv.value(PAYMENT_ID)+"</PAYMENT_ID>\n";
        }

        if( nv.exists(ORDER_ID) )
        {
                plain_request += "<ORDER_ID>"+nv.value(ORDER_ID)+"</ORDER_ID>\n";
        }

        if( nv.exists(PAY_METHOD) )
        {
                plain_request += "<PAY_METHOD>"+nv.value(PAY_METHOD)+"</PAY_METHOD>\n";
        }

        if( nv.exists(PAID_AMOUNT) )
        {
                plain_request += "<PAID_AMOUNT>"+nv.value(PAID_AMOUNT)+"</PAID_AMOUNT>\n";
        }

        if( nv.exists(PAYMENT_DATE) )
        {
                plain_request += "<PAYMENT_DATE>"+nv.value(PAYMENT_DATE)+"</PAYMENT_DATE>\n";
        }

        if( nv.exists(PAYMENT_TIME) )
        {
                plain_request += "<PAYMENT_TIME>"+nv.value(PAYMENT_TIME)+"</PAYMENT_TIME>\n";
        }

        if( nv.exists("FLAG") )
        {
                plain_request += "<FLAGS>"+nv.value(FLAG)+"</FLAGS>\n";
        }

        plain_request += "\0";

        return process_command(plain_request, response);

}

return_t bsi_Client::process_command(string plain_request, string& response)
{

        //getting the current time
        string time_stamp;

        time_t    cur_time;
        cur_time=time(NULL);
        ostringstream os;
        os <<cur_time ;
        time_stamp.assign(os.str());

        response = "------------------------------------------------------------\n";
        response += "BSI Request:\n";
        response += "------------------------------------------------------------\n";
        response += plain_request+"\n";

        string bsi_response = "";
        if(sendPostRequest(plain_request, bsi_response) != SUCCESS)
        {
                cout<<"Sending request to BSI failed\n";
                response +="Sending request to BSI failed: "+bsi_response;
                return FAILURE;
        }

        response += "------------------------------------------------------------\n";
        response += "BSI Response:\n";
        response += "------------------------------------------------------------\n";
        response += bsi_response+"\n";
        response += "------------------------------------------------------------\n";

        return SUCCESS;
}

return_t bsi_Client::cmo_process_command(string plain_request, string& response)
{
        response = "============================================================\n";
        response += "BSI Input:\n";
        response += "============================================================\n";

        char *key_pattern = entity_findval("UOL_PUB", &vendor_source2key_id);

        if (key_pattern == NULL || key_pattern == "")
        {
                cout<<"UOL pub key is not set\n";
                response += "UOL pub key is not set\n";
                return FAILURE;
        }

        gpgme_key_t key;
        if(getKeyInfo(key_pattern, key) == FAILURE)
        {
               cout << "Could not find UOL_PUB key "<<key_pattern<<endl;
               response += "Could not find UOL_PUB key: "+string(key_pattern)+"\n";
               return FAILURE;
        }

        string encr_request= "";
        if(encrypt(key, plain_request, encr_request ) != SUCCESS)
        {
                cout<<"Encrypting the request failed\n";
                response += "Encrypting the request failed\n";
                return FAILURE;
        }

        //getting the current time
        string time_stamp;

        time_t    cur_time;
        cur_time=time(NULL);
        ostringstream os;
        os <<cur_time ;
        time_stamp.assign(os.str());

        string request = string("<?xml version=\"1.0\"?>\n")+
                         "<trancd>CMOT</trancd>\n"+
                         "<transaction_id>"+time_stamp+"</transaction_id>\n"+
                         "<VENDOR>[portalgw.]tess.com</VENDOR>\n"+
                         "<ENCR_TEMPLATE>"+encr_request+"</ENCR_TEMPLATE>\n\0";

        response += "BSI Plain Request:\n";
        response += "------------------------------------------------------------\n";
        response += plain_request+"\n";

        response += "------------------------------------------------------------\n";
        response += "BSI Encrypted Request:\n";
        response += "------------------------------------------------------------\n";
        response += request+"\n";

        response += "============================================================\n";
        response += "BSI Output:\n";
        response += "============================================================\n";

        string bsi_response = "";
        if(sendPostRequest(request, bsi_response) != SUCCESS)
        {
                cout<<"Sending request to BSI failed\n";
                response +="Sending request to BSI failed: "+bsi_response;
                return FAILURE;
        }

        response += "------------------------------------------------------------\n";
        response += "BSI Encypted Response:\n";
        response += "------------------------------------------------------------\n";
        response += bsi_response+"\n";
        response += "------------------------------------------------------------\n";

        XmlInterface Object_des('#','&');
        entity_map response_map;

        int ret = Object_des.XmlParse(const_cast<char*>(bsi_response.c_str()));
        if (!ret)
        {
                Object_des.XmltoMap(response_map);
        }

        char * encr_template = entity_findval("ENCR_TEMPLATE", &response_map);
        if(encr_template == NULL)
        {
                cout<<"ENCR_TEMPLATE == NULL"<<endl;
                return FAILURE;
        }

        string plainTemplate = "";
        string encr_template_str = (string)encr_template;
        string encr_template_formatted = format(encr_template_str);

        if(decrypt(encr_template_formatted, plainTemplate) == FAILURE) {
                cout<<"decrypt() == FAILURE"<<endl;
                return FAILURE;
        }

        entity_replace("ENCR_TEMPLATE", plainTemplate.c_str(), &response_map);
        bsi_response ="";
        PrepareResponseFromMap(&response_map, bsi_response);

        response += "BSI Plain Response:\n";
        response += "------------------------------------------------------------\n";
        response += bsi_response+"\n\0";
        response += "------------------------------------------------------------\n";

        entity_map result_map;
        XmlInterface Object_des1('#','&');
        ret = Object_des1.XmlParse(const_cast<char*>(plainTemplate.c_str()));
        if (!ret)
        {
                Object_des1.XmltoMap(result_map);
                char *account_pdp_str = entity_findval("TEMPLATE#ACCOUNT_POID", &result_map);
                if ( account_pdp_str != NULL)
                        account_poid.assign(account_pdp_str);
        }

        return SUCCESS;
}


return_t bsi_Client::sendPostRequest(string request, string& response)
{
        http_Client     http;
        long            ret_code;

        ret_code = http.sendPostRequest(this->_url, request);
        response=http.getHttpResponse();

        if(ret_code == 200)
        {
                return SUCCESS;

        } else {

                return FAILURE;
        }

}

return_t bsi_Client::sendGetRequest(string request, string& response)
{
        http_Client     http;
        long            ret_code;
        string          url;

        url = this->_url+request;

        ret_code = http.sendGetRequest(url);
        response=http.getHttpResponse();

        if(ret_code == 200)
        {
                return SUCCESS;

        } else {

                return FAILURE;
        }

}

string bsi_Client::format(string enct_template_str)
{
        int i = enct_template_str.find(')');
        string initial = enct_template_str.substr(0,i+1);
        int v_i = enct_template_str.find("Version", 0);
        string begin_str = initial.substr(0,v_i-1);
        string inver_str = initial.substr(v_i);
        string after_initial = enct_template_str.substr(i+1);
        int length = after_initial.length();
        string mid_temp = after_initial.substr(0,length-5);
        int j = mid_temp.rfind("-----", mid_temp.length());
        string middle = after_initial.substr(0,j);
        string ending = after_initial.substr(j, after_initial.length());

        for(int m=0;m<middle.length();m++)
        {
                if( middle[m] == ' ')
                middle[m]='\n';
        }

        string final_str = begin_str+(string)"\n"+inver_str+(string)"\n"+middle+ending;

        final_str +="\n\0";
        return final_str;
}

int bsi_Client::PrepareResponseFromMap(entity_map *response_map, string& _response)
{
        if( response_map == NULL) return FAILURE;

        entity_map::iterator trancd_iter = response_map->find("trancd");
        if (trancd_iter != response_map->end())
        {
                _response += "<?xml version=\"1.0\"?>\n";
                _response += "<" + (string)(trancd_iter->second).c_str() + ">\n";

                entity_map::iterator p = response_map->begin();

                while(p != response_map->end()) {
                        if ( p->first != "trancd") {
                                _response += "\t<";
                                _response += (string)(p->first).c_str();
                                _response += ">";
                                _response += (string)(p->second).c_str();
                                _response += "</";
                                _response += (string)(p->first).c_str();
                                _response += ">\n";
                        }
                        p++;
                }

                _response += "</" + (string)(trancd_iter->second).c_str() + ">\n";
                return SUCCESS;
        }

        return FAILURE;
}
