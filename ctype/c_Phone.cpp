#include "c_Phone.h"

c_Phone::c_Phone(string raw, bool daytime )
{
	this->set(raw,daytime);
}

c_Phone::c_Phone(string raw, pin_phone_type_t type )
{
	this->set(raw,type);
}

void c_Phone::set(string raw, bool daytime )
{
	this->setRaw(raw);
	if (daytime)
		this->phone_type = PIN_PHONE_TYPE_HOME;
	else
		this->phone_type = PIN_PHONE_TYPE_WORK;
}

void c_Phone::set(string raw, pin_phone_type_t type )
{
	this->setRaw(raw);
	this->phone_type = type;
}


c_Phone::c_Phone(void)
{
	reset();
}

c_Phone::~c_Phone(void)
{
}

void c_Phone::reset() 
{
	this->raw_number.erase();
	this->phone_type = PIN_PHONE_TYPE_UNKNOWN;
}

string& c_Phone::getRaw(string& raw)
{
	raw.assign(this->raw_number);
	return raw;
}

void c_Phone::setRaw(string raw)
{
	this->raw_number.assign(raw);
}

bool c_Phone::raw_Equals(string test)
{
	if (this->raw_number.empty())
		return false;

	if (test.empty())
		return false;

	return( this->raw_number.compare(test) == 0);

}


