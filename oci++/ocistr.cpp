#ifndef _COCIString
#include "ocistr.h"
#endif

#ifndef _COCISession
#include "ocisess.h"
#endif

COCIString::COCIString(const COCISession& Sess)
: m_Session(Sess)
{
  m_Indicator = OCI_IND_NULL;
  m_String = (OCIString*)0;
}

COCIString::COCIString(const COCISession& Sess, dvoid* instance)
: m_Session(Sess)
{
  m_Indicator = OCI_IND_NOTNULL;
  m_String = (OCIString*)instance;
}

COCIString::~COCIString()
{
}

void COCIString::assign(COCIString& str)
{
  m_Indicator = str.m_Indicator;
  CHECK(m_Session.get_error(),OCIStringAssign(m_Session.get_env(), m_Session.get_error(), str.get_oci_string(), (OCIString**)&get_oci_string()));
}

void COCIString::assign_text(text* txt)
{
  if(txt)
  {
    m_Indicator = OCI_IND_NOTNULL;
    CHECK(m_Session.get_error(), OCIStringAssignText(m_Session.get_env(), 
		m_Session.get_error(), txt, (ub4)strlen((char*)txt), (OCIString**)&get_oci_string()));
  }
  else
  {
    m_Indicator = OCI_IND_NULL;
  }
}

text* COCIString::get_text()
{
  return OCIStringPtr(m_Session.get_env(), get_oci_string());
}

void COCIString::resize(ub4 new_size)
{
  CHECK(
	  m_Session.get_error(), 
	  OCIStringResize(m_Session.get_env(), 
	  m_Session.get_error(), 
	  new_size, 
	  (OCIString**)&get_oci_string()
	  ));
}


ub4 COCIString::get_alloc_size()
{
  ub4 allocsize = 0;
  CHECK(m_Session.get_error(), OCIStringAllocSize(m_Session.get_env(), m_Session.get_error(), get_oci_string(), &allocsize));
  return allocsize;
}

ub4 COCIString::get_size()
{
  return OCIStringSize(m_Session.get_env(), get_oci_string());
}


COCIString::operator char* () const
{
  return (char*)((COCIString&)*this).get_text();
}

COCIString& COCIString::operator = (const char * str)
{
  assign_text((text*)str);
  return *this;
}

COCIString& COCIString::operator = (const std::string& str)
{
  assign_text((text*)str.c_str());
  return *this;
}