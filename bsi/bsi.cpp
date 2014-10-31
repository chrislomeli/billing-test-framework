// http.cpp : Defines the entry point for the console application.
//

#include <bsi/bsi_Client.h>


int main(int argc, char* argv[])
{
        bsi_Client bsi("cs03.hyd.int.untd.com", "8888");
        bsi.add_key("UOL_PUB", "Satheesan");
        bsi.set_pass_phrase("bsi");

        bs_NvRecord bsi_nv;
        bsi_nv.set(CM_REG_ID, "test");
	string response;
        bsi.cmo_store_card(bsi_nv, response);

        cout<<"\nnResponse: "<<response<<endl;

	return 1;
}

