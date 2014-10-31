#ifndef _COCIBase
#include "ocibase.h"
#endif

COCIBase::COCIBase()
: m_instance(0)
, m_null_struct(0)
, m_Indicator(OCI_IND_NULL)
{
}

COCIBase::~COCIBase()
{
}