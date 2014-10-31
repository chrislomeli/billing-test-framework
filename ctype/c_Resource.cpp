#include "c_Resource.h"

c_Resource::c_Resource(string c)
: c_EnumMap()
{
	initialize_map();
	set(c);
}

c_Resource::c_Resource()
{
}

c_Resource::~c_Resource(void)
{
}


void c_Resource::initialize_map(void)
{
	this->insert("Cash",840, true);
	this->insert("USD",840);
	this->insert("MINUTES",100002, true);
	this->insert("JW_PREPAID_MONTHLY",100004, true);
	this->insert("US Dollar",840,true );
	this->insert("unsused", 1000001, true );
	this->insert("prepaid JG months", 1000002, true );
	this->insert("prepaid JW hours",1000003,true );
	this->insert("discounted JW 1495 month",1000004,true );
	this->insert("prepaid JW month",1000006,true );
	this->insert("discounted JW 995 month",1000007,true );
	this->insert("free JW months",1000008,true );
	this->insert("free JG months",1000009,true );
	this->insert("remaining contract months",1000010,true );
	this->insert("free JE months",1000011,true );
	this->insert("free WH month",1000012,true );
	this->insert("prepaid WH month",1000013,true );
	this->insert("free Premium Email month",1000014,true );
	this->insert("prepaid Premium Email month",1000015,true );
	this->insert("free PC month",1000016,true );
	this->insert("prepaid PC months",1000017,true );
	this->insert("free VP month",1000018,true );
	this->insert("free SE month",1000019,true );
	this->insert("prepaid SE month",1000020,true );
	this->insert("Intra-Cycle Balance",1000021,true );
	this->insert("free DSL month",1000103,true );
	this->insert("free Game Month",1000104,true );
	this->insert("Prepaid Game Month",1000105,true );
	this->insert("Renew free IP months",1000301,true );
	this->insert("Renew free WH month",1000302,true );
	this->insert("Renew free PC month",1000303,true );
	this->insert("Renew free Premium Email",1000304,true );
	this->insert("Renew free SE month",1000305,true );
	this->insert("Renew free VP month",1000306,true );
	this->insert("Renew free DSL month",1000307,true );
	this->insert("Renew free Game Month",1000308,true );
	this->insert("Renew disc9.95 IP months",1000401,true );
	this->insert("Renew disc14.95 IP months",1000501,true );
}
/*
void c_Resource::set_map( map<string,int>*& external_map )
{
	this->reset();
	if (enum_map != NULL)
		delete enum_map;
	enum_map = external_map;
}
*/
