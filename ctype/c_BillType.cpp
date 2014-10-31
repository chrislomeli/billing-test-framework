#include "c_BillType.h"

void  c_BillType::initialize_map () 
{

	pin_bill_type_t bill_type = PIN_BILL_TYPE_CC;
	this->insert( PIN_OBJ_TYPE_PAYINFO_CC, bill_type ); 
	this->insert( "CC",		bill_type, true ); 
	this->insert( "cc",		bill_type ); 
	this->insert( "10003",	bill_type ); 

	bill_type = PIN_BILL_TYPE_DD;
	this->insert( PIN_OBJ_TYPE_PAYINFO_DD, bill_type ); 
	this->insert( "dd",			bill_type ); 
	this->insert( "ACH_BILLING",	bill_type, true ); 
	this->insert( "10005",			bill_type ); 

	bill_type = PIN_BILL_TYPE_INVOICE;
	this->insert( PIN_OBJ_TYPE_PAYINFO_INVOICE, bill_type ); 
	this->insert( "INV",		bill_type, true ); 
	this->insert( "invoice",	bill_type ); 
	this->insert( "10001",		bill_type ); 

	bill_type = PIN_BILL_TYPE_LEC;
	this->insert( PIN_OBJ_TYPE_PAYINFO_LEC, bill_type ); 
	this->insert( "LEC_BILLING",	bill_type, true ); 
	this->insert( "lec",	bill_type ); 
	this->insert( "10016",			bill_type ); 

	bill_type = PIN_BILL_TYPE_CASH;
	this->insert( "/payinfo/cash", bill_type ); 
	this->insert( "CASH_BILLING",	bill_type, true ); 
	this->insert( "cash",	bill_type ); 
	this->insert( "10011",			bill_type ); 

	bill_type = PIN_BILL_TYPE_CHECK;
	this->insert( "/payinfo", bill_type ); 
	this->insert( "CHECK_BILLING",	bill_type, true ); 
	this->insert( "check",	bill_type ); 
	this->insert( "10012",			bill_type ); 

	bill_type = PIN_BILL_TYPE_PREPAID;
	this->insert( "/payinfo/prepaid", bill_type ); 
	this->insert( "PREPAID",	bill_type, true ); 
	this->insert( "prepaid",	bill_type ); 
	this->insert( "10000",		bill_type ); 

	bill_type = PIN_BILL_TYPE_SUBORD;
	this->insert( PIN_OBJ_TYPE_PAYINFO_SUBORD, bill_type ); 
	this->insert( "subord",	bill_type, true ); 
	this->insert( "10007",		bill_type ); 

	bill_type = PIN_BILL_TYPE_INTERNAL;
	this->insert( "/payinfo",  bill_type ); 
	this->insert( "VIP",	bill_type, true ); 
	this->insert( "10009",		bill_type ); 
	this->insert( "10008",		PIN_BILL_TYPE_BETA ); 
	this->insert( "10010",		PIN_BILL_TYPE_GUEST ); 
	this->insert( "10011",		PIN_BILL_TYPE_CASH ); 
	this->insert( "10013",		PIN_BILL_TYPE_WTRANSFER ); 
	this->insert( "10014",		PIN_BILL_TYPE_PAYORDER ); 
	this->insert( "10015",		PIN_BILL_TYPE_POSTALORDER ); 
}

pin_fld_num_t c_BillType::getPayinfoArrayType(void)
{
	switch(this->get()) {
	case PIN_BILL_TYPE_CC :
		return PIN_FLD_CC_INFO;
	case PIN_BILL_TYPE_DD :
		return PIN_FLD_CC_INFO;
	case PIN_BILL_TYPE_INVOICE :
		return PIN_FLD_INV_INFO;
	case PIN_BILL_TYPE_LEC :
		return UNTD_FLD_LEC_INFO;

	case PIN_BILL_TYPE_DDEBIT :
	case PIN_BILL_TYPE_UNDEFINED :
	case PIN_BILL_TYPE_PREPAID :
	case PIN_BILL_TYPE_DEBIT :
	case PIN_BILL_TYPE_SMARTC :
	case PIN_BILL_TYPE_SUBORD :
	case PIN_BILL_TYPE_BETA :
	case PIN_BILL_TYPE_INTERNAL :
	case PIN_BILL_TYPE_GUEST :
	case PIN_BILL_TYPE_CASH  :
	case PIN_BILL_TYPE_CHECK  :
	case PIN_BILL_TYPE_WTRANSFER :
	case PIN_BILL_TYPE_PAYORDER :
	case PIN_BILL_TYPE_POSTALORDER :
	default:
		return 0;
	}
}


