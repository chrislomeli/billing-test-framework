#include "c_PayType.h"

void  c_PayType::initialize_map () 
{

	pin_pay_type_t pay_type = PIN_PAY_TYPE_CC;
	this->insert( PIN_OBJ_TYPE_PAYINFO_CC, pay_type ); 
	this->insert( "CC",		pay_type, true ); 
	this->insert( "cc",		pay_type ); 
	this->insert( "10003",	pay_type ); 

	pay_type = PIN_PAY_TYPE_DD;
	this->insert( PIN_OBJ_TYPE_PAYINFO_DD, pay_type ); 
	this->insert( "dd",			pay_type ); 
	this->insert( "ACH_BILLING",	pay_type, true ); 
	this->insert( "10005",			pay_type ); 

	pay_type = PIN_PAY_TYPE_INVOICE;
	this->insert( PIN_OBJ_TYPE_PAYINFO_INVOICE, pay_type ); 
	this->insert( "INV",		pay_type, true ); 
	this->insert( "invoice",	pay_type ); 
	this->insert( "10001",		pay_type ); 

	pay_type = PIN_PAY_TYPE_LEC;
	this->insert( PIN_OBJ_TYPE_PAYINFO_LEC, pay_type ); 
	this->insert( "LEC_BILLING",	pay_type, true ); 
	this->insert( "lec",	pay_type ); 
	this->insert( "10016",			pay_type ); 

	pay_type = PIN_PAY_TYPE_CASH;
	this->insert( "/payinfo/cash", pay_type ); 
	this->insert( "CASH_BILLING",	pay_type, true ); 
	this->insert( "cash",	pay_type ); 
	this->insert( "10011",			pay_type ); 

	pay_type = PIN_PAY_TYPE_CHECK;
	this->insert( "/payinfo", pay_type ); 
	this->insert( "CHECK_BILLING",	pay_type, true ); 
	this->insert( "check",	pay_type ); 
	this->insert( "10012",			pay_type ); 

	pay_type = PIN_PAY_TYPE_PREPAID;
	this->insert( "/payinfo/prepaid", pay_type ); 
	this->insert( "PREPAID",	pay_type, true ); 
	this->insert( "prepaid",	pay_type ); 
	this->insert( "10000",		pay_type ); 

	pay_type = PIN_PAY_TYPE_SUBORD;
	this->insert( PIN_OBJ_TYPE_PAYINFO_SUBORD, pay_type ); 
	this->insert( "subord",	pay_type, true ); 
	this->insert( "10007",		pay_type ); 

	pay_type = PIN_PAY_TYPE_INTERNAL;
	this->insert( "/payinfo",  pay_type ); 
	this->insert( "VIP",	pay_type, true ); 
	this->insert( "10009",		pay_type ); 
	this->insert( "10008",		PIN_PAY_TYPE_BETA ); 
	this->insert( "10010",		PIN_PAY_TYPE_GUEST ); 
	this->insert( "10011",		PIN_PAY_TYPE_CASH ); 
	this->insert( "10013",		PIN_PAY_TYPE_WTRANSFER ); 
	this->insert( "10014",		PIN_PAY_TYPE_PAYORDER ); 
	this->insert( "10015",		PIN_PAY_TYPE_POSTALORDER ); 
}

pin_fld_num_t c_PayType::getPayinfoArrayType(void)
{
	switch(this->get()) {
	case PIN_PAY_TYPE_CC :
		return PIN_FLD_CC_INFO;
	case PIN_PAY_TYPE_DD :
		return PIN_FLD_DD_INFO;
	case PIN_PAY_TYPE_INVOICE :
		return PIN_FLD_INV_INFO;
	case PIN_PAY_TYPE_LEC :
		return UNTD_FLD_LEC_INFO;

	case PIN_PAY_TYPE_DDEBIT :
	case PIN_PAY_TYPE_UNDEFINED :
	case PIN_PAY_TYPE_PREPAID :
	case PIN_PAY_TYPE_DEBIT :
	case PIN_PAY_TYPE_SMARTC :
	case PIN_PAY_TYPE_SUBORD :
	case PIN_PAY_TYPE_BETA :
	case PIN_PAY_TYPE_INTERNAL :
	case PIN_PAY_TYPE_GUEST :
	case PIN_PAY_TYPE_CASH  :
	case PIN_PAY_TYPE_CHECK  :
	case PIN_PAY_TYPE_WTRANSFER :
	case PIN_PAY_TYPE_PAYORDER :
	case PIN_PAY_TYPE_POSTALORDER :
	default:
		return 0;
	}
}


