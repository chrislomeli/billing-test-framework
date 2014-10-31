#include <parser/c_ParserDelimited.h>

c_ParserDelimited::c_ParserDelimited(void)
{
	clear();
	this->parse_type = Delimited;
}

c_ParserDelimited::~c_ParserDelimited(void)
{
	clear();
}
