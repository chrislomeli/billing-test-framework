#include <parser/c_ParserFixedLength.h>

c_ParserFixedLength::c_ParserFixedLength(void)
{
	clear();
	this->parse_type = FixedLength;
}

c_ParserFixedLength::~c_ParserFixedLength(void)
{
}
