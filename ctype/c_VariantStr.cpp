#include <ctype/c_VariantStr.h>
#include <ctype/c_StringFormat.h>
#include <ctype/c_StringPinFormat.h>
#include <platform/p_Exception.h>
#include <platform/p_Error_map.h>

c_VariantStr::c_VariantStr() : type(0)
{

}

c_VariantStr::c_VariantStr(std::string val_str) : type(0)
{
	this->set(val_str);
}

c_VariantStr::~c_VariantStr(void)
{
}

bool c_VariantStr::set(std::string input_str)
{
	this->value.push_back(input_str);
	return true;
}

ostream& c_VariantStr::debug(ostream &out)
{
	vector<string>::iterator pos = this->value.begin();
	bool multiline = ((this->value.size() > 1 ) ? true : false);
	string tab = (multiline) ? "\t\t\t\t" : " ";

	out << ((multiline) ? "\n" : "");
    
	for (; pos != this->value.end(); pos++ ) {
		string s=(string)*pos;
		out << tab << s << endl;
	}
	return out;
}

int c_VariantStr::toInteger()
{
	if ( this->value.size() == 0 ) {
		p_Throw(r_Null_value);
	} else if ( this->type != PIN_FLDT_INT ) {
		p_Throw(r_Bad_Type);
	}

	return c_StringFormat::StringToInteger(this->value[0]);
}

double c_VariantStr::toDouble()
{
	if ( this->value.size() == 0 ) {
		p_Throw(r_Null_value);
	} else if ( this->type != PIN_FLDT_DECIMAL ) {
		p_Throw(r_Bad_Type);
	}

	return c_StringFormat::StringToDouble(this->value[0] );
}

long c_VariantStr::toLong()
{
	if ( this->value.size() == 0 ) {
		p_Throw(r_Null_value);
	} else if ( this->type != PIN_FLDT_DECIMAL ) {
		p_Throw(r_Bad_Type);
	}

	return c_StringFormat::StringToLong(this->value[0] );

}

time_t c_VariantStr::toUTC()
{
	if ( this->value.size() == 0 ) {
		p_Throw(r_Null_value);
	} else if ( this->type != PIN_FLDT_TSTAMP ) {
		p_Throw(r_Bad_Type);
	}

	return c_StringFormat::StringToUTC( this->value[0] );

}

pin_flist_t * c_VariantStr::toPinFlist()
{
	if ( this->value.size() == 0 )
		p_Throw(r_Null_value);
	else if ( this->value.size() == 1 ) {
		string str = this->value[0];
		c_StringFormat::lowercase(str);
		if ( str.compare("null") == 0 )
			return (pin_flist_t *)NULL;
	} else if ( this->type != PIN_FLDT_ARRAY ) {
		p_Throw(r_Bad_Type);
	}
	return c_StringPinFormat::StringArrayToFlist( this->value );

}

bool c_VariantStr::toPinErrBuffer(pin_errbuf_t *ebufp)
{
	PIN_ERR_CLEAR_ERR(ebufp);
	if ( this->value.size() == 0 ) {
		p_Throw(r_Null_value);
	} else if ( this->type != PIN_FLDT_ERRBUF ) {
		p_Throw(r_Bad_Type);
	}

	return ( c_StringPinFormat::StringArrayToEbuf(this->value,ebufp) );
}

pin_decimal_t * c_VariantStr::toPinDecimal()
{
	if ( this->value.size() == 0 ) {
		p_Throw(r_Null_value);
	} else if ( this->type != PIN_FLDT_DECIMAL ) {
		p_Throw(r_Bad_Type);
	}
	return c_StringPinFormat::StringToDecimal( this->value[0] );
}

void c_VariantStr::setType(string stype)
/*
#define PIN_FLDT_UNUSED		0
#define PIN_FLDT_INT		1
#define PIN_FLDT_STR		5	
#define PIN_FLDT_POID		7	// need translator
#define PIN_FLDT_TSTAMP		8	// toUTC()	
#define PIN_FLDT_ARRAY		9	// pin_flist_t
#define PIN_FLDT_ERRBUF		13
#define PIN_FLDT_DECIMAL	14
*/
{
	if ( stype.empty() )
		return;

	switch ( tolower(stype[0]) ) {
	case 'i':
		this->type = PIN_FLDT_INT;
		break;
	case 'd':
	case 'f':
		this->type = PIN_FLDT_DECIMAL;
		break;
	case 's':
		this->type = PIN_FLDT_STR;
		break;
	case 't':
		this->type = PIN_FLDT_TSTAMP;
		break;
	case 'p':
		if (stype.compare("pin_flist_t") == 0 ) {
			this->type = PIN_FLDT_ARRAY;
		} else if (stype.compare("pin_errbuf_t") == 0 ) {
			this->type = PIN_FLDT_ERRBUF;
		} else if (stype.compare("pin_decimal_t") == 0 ) {
			this->type = PIN_FLDT_DECIMAL;
		} else if (stype.compare("poid_t") == 0 ) {
			this->type = PIN_FLDT_POID;
		}
		break;
	};


}
