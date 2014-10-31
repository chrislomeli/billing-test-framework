#ifndef _COCIStatement
#define _COCIStatement

#if defined(_WIN32)
#pragma warning(disable : 4786 4800 4666)
#endif

#ifndef OCI_ORACLE
#include "oci.h"
#endif

#ifndef _STRING_
#include <string>
#endif

#ifndef _VECTOR_
#include <vector>
#endif

class COCISession;
class COCIDate;
class COCIString;


class COCIStatement
{
public:
  COCIStatement(const COCISession& Sess);
  ~COCIStatement();

  COCIStatement& operator = (char * str);
  bool execute(void);
  bool fetch(void);
  void clean_up(void);

  ub4  rows_fetched(){return m_RowsFetched;}

  // define are read variables
  // bind are write variables

  // int
  void define(int pos, int& valuep, signed short* is_null = false);
  void define(int pos, std::vector<int>& valuep);
  void bind(int pos, int& valuep);
  void bind(char* name, int& valuep);
  void bind(int pos, std::vector<int>& valuep);
  void bind(char* name, std::vector<int>& valuep);

  // long
  void define(int pos, long& valuep);
  void define(int pos, std::vector<long>& valuep);
  void bind(int pos, long& valuep);
  void bind(char* name, long& valuep);
  void bind(int pos, std::vector<long>& valuep);
  void bind(char* name, std::vector<long>& valuep);

  // float
  void define(int pos, float& valuep);
  void define(int pos, std::vector<float>& valuep);
  void bind(int pos, float& valuep);
  void bind(int pos, std::vector<float>& valuep);

  // double
  void define(int pos, double& valuep);
  void define(int pos, std::vector<double>& valuep);
  void bind(int pos, double& valuep);
  void bind(int pos, std::vector<double>& valuep);

  // long double
  void define(int pos, long double& valuep);
  void define(int pos, std::vector<long double>& valuep);
  void bind(int pos, long double& valuep);
  void bind(int pos, std::vector<long double>& valuep);


// char (cjl)
  void define(int pos, char* buf, ub4 len, signed short* is_null = false);
  // valuep == (elem_len + 1) * no_of_elem; e.g. for varchar2[10] and array fetch of 5 rows
  // create a buffer (10 + 1) * 5 = 55
  // i.e. for above example pass
  // char buffer[(10+1)*5]; define(1,buffer,10,5); 

  void define(int pos, char* valuep, ub4 elem_len, ub4 no_of_elem);
  void bind(int pos, char* buf, ub4 len, signed short* is_null = false);
  void bind(char* name, char* buf, ub4 len, signed short* is_null = false);
  void bind(int pos, char* valuep, ub4 elem_len, ub4 no_of_elem);

  void define(int pos, COCIDate& date);
  void bind(int pos, COCIDate& date);
  void bind(char* name, COCIDate& date);

  void define(int pos, COCIString& str);
  void bind(int pos, COCIString& str);
  void bind(char* name, COCIString& str);

  void define(int pos, std::string& str, signed short* is_null = 0);
  void bind(int pos, std::string& str);

private:
  OCIStmt* stmthp;
  const COCISession& m_Session;
  char* sqlstmt;
  OCIDefine* m_pDefine;
  OCIBind* m_pBind;
  ub4 m_Rows;
  ub4 m_RowsFetched;
  ub4 m_TotalRowsFetched;

//  std::vector<dvoid**> m_indpp_list;
//  std::vector<dvoid**> m_pgvpp_list;
//  std::vector<ub4*>    m_indszp_list;
//  std::vector<ub4*>    m_pvszp_list;

//  ub4 m_Object_Count;
//  ub4 m_Ref_Count;
//  ub4 m_VArray_Number_Count;
//  ub4 m_VArray_String_Count;
//  ub4 m_VArray_Object_Count;

//  ub4 m_NestedTable_Number_Count;
//  ub4 m_NestedTable_String_Count;
//  ub4 m_NestedTable_Object_Count;

};


#endif
