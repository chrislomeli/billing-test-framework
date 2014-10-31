#include "c_Name.h"

c_Name::c_Name(void)
{
}

c_Name::~c_Name(void)
{
}

void c_Name::setLastName(string instr)
{
	this->last_name.assign(instr);
}
void c_Name::setFirstName(string instr)
{
	this->first_name.assign(instr);
}
void c_Name::setMiddleName(string instr)
{
	this->middle_name.assign(instr);
}

void c_Name::setFullName(string instr)
{
	this->full_name.assign(instr);
}

string& c_Name::get_Fullname(string &outstr, bool generate_if_null)
{
	outstr.erase();

	if (!full_name.empty()) {
		outstr.assign(full_name);
		return outstr;
	}

	if (! generate_if_null )
		return outstr;
	
	if ( last_name.empty()) {
		full_name.append(DEFAULT_NAME);
		outstr.assign(full_name);
		return outstr;
	}
		
	if (!first_name.empty()) {
		full_name.append(first_name);
		full_name.append(" ");
	}
	if (!last_name.empty())
		full_name.append(last_name);

	outstr.assign(full_name);
	return outstr;

}
