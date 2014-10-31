#ifndef _COCIString
#define _COCIString

#ifndef _COCISession
#include "ocisess.h"
#endif

#ifndef _COCIBase
#include "ocibase.h"
#endif

#ifndef _STRING_
#include <string>
#endif

class COCISession;

class COCIString : public COCIBase
{
public:
  COCIString(const COCISession& Sess);
  COCIString(const COCISession& Sess, dvoid* instance);
  ~COCIString();
  void assign(COCIString& str);
  void assign_text(text* txt);
  text* get_text();
  void resize(ub4 new_size);
  ub4 get_alloc_size();
  ub4 get_size();
  OCIString*& get_oci_string(){return m_String;}
  virtual dvoid*& get_instance(){return (dvoid*&)m_String;}

  // Casts
  operator char * () const;
  COCIString& operator = (const char * str);
  COCIString& operator = (const std::string& str);


private:
  const COCISession& m_Session;
  OCIString* m_String;
};

#endif // _COCIString
