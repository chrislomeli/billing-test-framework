#include "c_Status.h"

c_Status::c_Status(string str) : c_EnumMap()
{
	initialize_map();
	set(str);
}

c_Status::c_Status(void)
{
}

c_Status::~c_Status(void)
{
}

void c_Status::initialize_map()
{
	this->insert("Undefined",0, true);
	this->insert("Active",10100, true);
	this->insert("InActive",10102,true);
	this->insert("Closed",10103,true);
}
