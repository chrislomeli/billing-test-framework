#ifndef _COCIBase
#define _COCIBase

//#ifdef sparc
// used for bool definition
//#include <stl_config.h>
//#define std
//#endif


#ifndef OCI_ORACLE
#include "oci.h"
#endif

class COCIBase
{
public:
  COCIBase();
  ~COCIBase();
  virtual dvoid*& get_instance() {return m_instance;}
  dvoid   set_instance(dvoid*& instance){m_instance = instance;}
  // Don't give a reference to the null struct because OCI has
  // side-effects that changes it!!
  virtual dvoid* get_null_struct() {return m_null_struct;}
  dvoid   set_null_struct(dvoid* null_struct){m_null_struct = null_struct;}

  OCIInd* get_indicator(){return &m_Indicator;}

  bool is_null(){return m_Indicator == OCI_IND_NULL;}
  void set_null(bool null = false){(null) ? m_Indicator = OCI_IND_NULL : m_Indicator = OCI_IND_NOTNULL;}


protected:
  OCIInd m_Indicator;

private:
  dvoid* m_instance;
  dvoid* m_null_struct;
};

#endif // _COCIBase
