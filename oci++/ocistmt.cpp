#ifndef _COCIStatement
#include "ocistmt.h"
#endif

#ifndef _COCISession
#include "ocisess.h"
#endif

#ifndef _COCIDate
#include "ocidate.h"
#endif

#ifndef _COCIString
#include "ocistr.h"
#endif


COCIStatement::COCIStatement(const COCISession& Sess)
: stmthp(0)
, m_Session(Sess)
, sqlstmt(0)
, m_pDefine(0)
, m_pBind(0)
, m_Rows(1)
, m_RowsFetched(0)
, m_TotalRowsFetched(0)
{
}

COCIStatement::~COCIStatement()
{
  if(sqlstmt)
    delete [] sqlstmt;

  if ( stmthp ) {
  CHECK(m_Session.get_error(),OCIHandleFree(stmthp,OCI_HTYPE_STMT));
	stmthp = 0;
  }

  clean_up();
}

void COCIStatement::clean_up(void)
{
  m_Rows = 1;
  m_RowsFetched = 0;
  m_TotalRowsFetched = 0;
}

COCIStatement& COCIStatement::operator = (char * str)
{
  if(!stmthp)
    CHECK(
		m_Session.get_error(),
		OCIHandleAlloc((dvoid *)m_Session.get_env(), (dvoid **)&stmthp, OCI_HTYPE_STMT, 0, 0));

  if(sqlstmt)
  {
    delete [] sqlstmt;
    sqlstmt = (char*)0;
    clean_up();
  }

  ub4 len = (ub4)strlen(str);
  sqlstmt = new char[len + 1];
  sprintf(sqlstmt,str);
  CHECK(m_Session.get_error(),OCIStmtPrepare(stmthp, m_Session.get_error(), 
	  (unsigned char *)sqlstmt, (ub4)strlen(sqlstmt), OCI_NTV_SYNTAX, 0));
  return *this;
}

bool COCIStatement::execute(void)
{
  bool ret_val = true;
  sb4 status = OCIStmtExecute(
	  m_Session.get_svc(), 
	  stmthp, 
	  m_Session.get_error(), 
	  (ub4) m_Rows, 
	  (ub4) 0,
      (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT);


  if(status == OCI_NO_DATA)
  {
    // Check to see if we were bringing back gobbets of data (=> not all slots filled)
    ub4 size = sizeof(ub4);
    CHECK(m_Session.get_error(), OCIAttrGet ( (dvoid *) stmthp,
                      (ub4) OCI_HTYPE_STMT,
                      (dvoid*)&m_RowsFetched, &size, (ub4) OCI_ATTR_ROW_COUNT,
                      m_Session.get_error()));
    if(m_RowsFetched == 0)
    {
      m_TotalRowsFetched = 0;
      ret_val = false;
    }
  }
  else
  {
    CHECK(m_Session.get_error(), status);
    m_RowsFetched = m_Rows;
    m_TotalRowsFetched += m_Rows;

  }

  return ret_val;
}

bool COCIStatement::fetch(void)
{
  bool ret_val = true;

  // Has all data been retrieved? (=> can't fetch any more data)
  if(m_RowsFetched < m_Rows)
  {
    ret_val = false;
  }
  else
  {

    sb4 status = OCIStmtFetch(stmthp, m_Session.get_error(), (ub4) m_Rows, (ub4) OCI_FETCH_NEXT,
                                 (ub4) OCI_DEFAULT);
  
  
    if ( status == OCI_NO_DATA )
    {
      // Check to see if we were bringing back gobbets of data (=> not all slots filled)
      ub4 size = sizeof(ub4);
      m_RowsFetched = 0;
      CHECK(m_Session.get_error(), OCIAttrGet ( (dvoid *) stmthp,
                        (ub4) OCI_HTYPE_STMT,
                        (dvoid*)&m_RowsFetched, &size, (ub4) OCI_ATTR_ROW_COUNT,
                        m_Session.get_error()));
      // If same => no more rows
      if(m_RowsFetched <= m_TotalRowsFetched)
      {
        m_TotalRowsFetched = 0;
        ret_val = false;
      }
      else // Otherwise => more data
      {
        m_RowsFetched = m_RowsFetched - m_TotalRowsFetched;
      }

    }
    else
    {  
      CHECK(m_Session.get_error(), status);
      m_RowsFetched = m_Rows;
      m_TotalRowsFetched += m_Rows;
    }
  }

  return ret_val;
}

void COCIStatement::define(int pos, int& valuep, signed short* is_null)
{
  CHECK(m_Session.get_error(), OCIDefineByPos(stmthp, &m_pDefine, m_Session.get_error(), (ub4)pos, &valuep, (sb4)sizeof(int), (ub2)SQLT_INT, (is_null) ? is_null: 0,0,0,OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::define(int pos, std::vector<int>& valuep)
{
  // Check to see if there is at least one element in the std::vector
  if(valuep.size() == 0)
  {
    // Put one in there
    valuep.push_back(1);
  }
  CHECK(m_Session.get_error(), OCIDefineByPos(stmthp, &m_pDefine, m_Session.get_error(), (ub4)pos, &valuep[0], (sb4)sizeof(int), (ub2)SQLT_INT, 0,0,0,OCI_DEFAULT));
  m_Rows = (ub4)valuep.size();
}

void COCIStatement::bind(int pos, int& valuep)
{
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)&valuep, (sb4)sizeof(int), (ub2)SQLT_INT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::bind(char* name, int& valuep)
{
  CHECK(m_Session.get_error(), 
	  OCIBindByName(
		stmthp, &m_pBind, m_Session.get_error(), 
		(text *)name, (sb4)strlen(name), 
		(dvoid*)&valuep, (sb4)(sizeof (int)), (ub2)SQLT_INT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::bind(int pos, std::vector<int>& valuep)
{
  if(valuep.size() == 0)
  {
    throw CBindException();
  }
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)&valuep[0], (sb4)sizeof(int), (ub2)SQLT_INT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = (ub4)valuep.size();
}

void COCIStatement::bind(char* name, std::vector<int>& valuep)
{
  if(valuep.size() == 0)
  {
    throw CBindException();
  }
  CHECK(m_Session.get_error(), 
	  OCIBindByName(stmthp, &m_pBind, 
	  m_Session.get_error(), (text *)name, 
	  (sb4)strlen(name), (dvoid*)&valuep[0], (sb4)sizeof (int), (ub2)SQLT_INT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = (sb4)valuep.size();
}

void COCIStatement::define(int pos, long& valuep)
{
  CHECK(m_Session.get_error(), OCIDefineByPos(stmthp, &m_pDefine, m_Session.get_error(), (ub4)pos, &valuep, (sb4)sizeof(eb4), (ub2)SQLT_INT, 0,0,0,OCI_DEFAULT));
  m_Rows = 1;
}


void COCIStatement::define(int pos, std::vector<long>& valuep)
{
  // Check to see if there is at least one element in the std::vector
  if(valuep.size() == 0)
  {
    // Put one in there
    valuep.push_back(1);
  }
  CHECK(m_Session.get_error(), OCIDefineByPos(stmthp, &m_pDefine, m_Session.get_error(), (ub4)pos, &valuep[0], (sb4)sizeof(eb4), (ub2)SQLT_INT, 0,0,0,OCI_DEFAULT));
  m_Rows = (ub4)valuep.size();
}

void COCIStatement::bind(int pos, long& valuep)
{
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)&valuep, (sb4)sizeof(eb4), (ub2)SQLT_INT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::bind(char* name, long& valuep)
{
  CHECK(m_Session.get_error(), OCIBindByName(stmthp, &m_pBind, m_Session.get_error(), (text *)name, (sb4)strlen(name), (dvoid*)&valuep, (sb4)sizeof (eb4), (ub2)SQLT_INT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::bind(int pos, std::vector<long>& valuep)
{
  if(valuep.size() == 0)
  {
    throw CBindException();
  }
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)&valuep[0], (sb4)sizeof(eb4), (ub2)SQLT_INT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = (ub4)valuep.size();
}

void COCIStatement::bind(char* name, std::vector<long>& valuep)
{
  if(valuep.size() == 0)
  {
    throw CBindException();
  }
  CHECK(m_Session.get_error(), OCIBindByName(stmthp, &m_pBind, m_Session.get_error(), (text *)name, (sb4)strlen(name), (dvoid*)&valuep[0], (sb4)sizeof (eb4), (ub2)SQLT_INT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = (ub4)valuep.size();

}

void COCIStatement::define(int pos, float& valuep)
{
  CHECK(m_Session.get_error(), OCIDefineByPos(stmthp, &m_pDefine, m_Session.get_error(), (ub4)pos, &valuep, (sb4)sizeof(float), (ub2)SQLT_FLT, 0,0,0,OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::define(int pos, std::vector<float>& valuep)
{
  // Check to see if there is at least one element in the std::vector
  if(valuep.size() == 0)
  {
    // Put one in there
    valuep.push_back(1);
  }
  CHECK(m_Session.get_error(), OCIDefineByPos(stmthp, &m_pDefine, m_Session.get_error(), (ub4)pos, &valuep[0], (sb4)sizeof(float), (ub2)SQLT_FLT, 0,0,0,OCI_DEFAULT));
  m_Rows = (ub4)valuep.size();
}

void COCIStatement::bind(int pos, float& valuep)
{
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)&valuep, (sb4)sizeof(float), (ub2)SQLT_FLT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::bind(int pos, std::vector<float>& valuep)
{
  if(valuep.size() == 0)
  {
    throw CBindException();
  }
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)&valuep[0], (sb4)sizeof(float), (ub2)SQLT_FLT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = (ub4)valuep.size();
}

void COCIStatement::define(int pos, double& valuep)
{
  CHECK(m_Session.get_error(), OCIDefineByPos(stmthp, &m_pDefine, m_Session.get_error(), (ub4)pos, &valuep, (sb4)sizeof(double), (ub2)SQLT_FLT, 0,0,0,OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::define(int pos, std::vector<double>& valuep)
{
  // Check to see if there is at least one element in the std::vector
  if(valuep.size() == 0)
  {
    // Put one in there
    valuep.push_back(1);
  }
  CHECK(m_Session.get_error(), OCIDefineByPos(stmthp, &m_pDefine, m_Session.get_error(), (ub4)pos, &valuep[0], (sb4)sizeof(double), (ub2)SQLT_FLT, 0,0,0,OCI_DEFAULT));
  m_Rows = (ub4)valuep.size();
}

void COCIStatement::bind(int pos, double& valuep)
{
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)&valuep, (sb4)sizeof(double), (ub2)SQLT_FLT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::bind(int pos, std::vector<double>& valuep)
{
  if(valuep.size() == 0)
  {
    throw CBindException();
  }
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)&valuep[0], (sb4)sizeof(double), (ub2)SQLT_FLT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = (ub4)valuep.size();
}

void COCIStatement::define(int pos, long double& valuep)
{
  CHECK(m_Session.get_error(), OCIDefineByPos(stmthp, &m_pDefine, m_Session.get_error(), (ub4)pos, &valuep, (sb4)sizeof(long double), (ub2)SQLT_FLT, 0,0,0,OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::define(int pos, std::vector<long double>& valuep)
{
  // Check to see if there is at least one element in the std::vector
  if(valuep.size() == 0)
  {
    // Put one in there
    valuep.push_back(1);
  }
  CHECK(m_Session.get_error(), OCIDefineByPos(stmthp, &m_pDefine, m_Session.get_error(), (ub4)pos, &valuep[0], (sb4)sizeof(long double), (ub2)SQLT_FLT, 0,0,0,OCI_DEFAULT));
  m_Rows = (ub4)valuep.size();
}

void COCIStatement::bind(int pos, long double& valuep)
{
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)&valuep, (sb4)sizeof(long double), (ub2)SQLT_FLT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::bind(int pos, std::vector<long double>& valuep)
{
  if(valuep.size() == 0)
  {
    throw CBindException();
  }
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)&valuep[0], (sb4)sizeof(long double), (ub2)SQLT_FLT, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = (ub4)valuep.size();
}


void COCIStatement::define(int pos, char* buf, ub4 len, signed short* is_null)
{
  CHECK(m_Session.get_error(), 
	  OCIDefineByPos(
			stmthp, &m_pDefine, m_Session.get_error(), 
			(ub4)pos, buf, len + 1, (ub2)SQLT_STR, (is_null) ? is_null: 0,0,0,OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::define(int pos, char* valuep, ub4 elem_len, ub4 no_of_elem)
{
  CHECK(m_Session.get_error(), 
	  OCIDefineByPos(
			stmthp, &m_pDefine, m_Session.get_error(), (ub4)pos, 
			valuep, (sb4)elem_len + 1, (ub2)SQLT_STR, 0,0,0,OCI_DEFAULT));
  m_Rows = no_of_elem;
}

void COCIStatement::bind(int pos, char* buf, ub4 len, signed short* is_null)
{
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)buf, (sb4)len + 1, (ub2)SQLT_STR, (is_null) ? is_null: 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::bind(char* name, char* buf, ub4 len, signed short* is_null)
{
  CHECK(m_Session.get_error(), OCIBindByName(stmthp, &m_pBind, m_Session.get_error(), (text *)name, (sb4)strlen(name), (dvoid*)buf, len + 1, (ub2)SQLT_STR, (is_null) ? is_null: 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = 1;
}

void COCIStatement::bind(int pos, char* valuep, ub4 elem_len, ub4 no_of_elem)
{
  CHECK(m_Session.get_error(), OCIBindByPos(stmthp, &m_pBind, m_Session.get_error(), (ub4)pos, (dvoid*)valuep, (sb4)elem_len + 1, (ub2)SQLT_STR, 0,0,0,0,0, OCI_DEFAULT));
  m_Rows = no_of_elem;
}


void COCIStatement::define(int pos, COCIDate& date)
{
  CHECK(m_Session.get_error(), 
	  OCIDefineByPos(stmthp, &m_pDefine, 
	  m_Session.get_error(), 
	  (ub4)pos, date.get_instance(), 
	  (sb4)sizeof(OCIDate), (ub2)SQLT_ODT, (void*)date.get_indicator(),0,0,OCI_DEFAULT));
}

void COCIStatement::bind(int pos, COCIDate& date)
{
	OCIDate * this_date = date.get_oci_date();
	bool isnull = date.is_null();

	CHECK(
		m_Session.get_error(), 
		OCIBindByPos(
			stmthp, 
			&m_pBind, 
			m_Session.get_error(), 
			(ub4)pos, 
			(OCIDate*)this_date, 
			(ub4)sizeof(OCIDate), 
			(ub2)SQLT_ODT, 
			(date.is_null() ? (void*)date.get_indicator() : 0),
			0,0,0,0, OCI_DEFAULT));
}

void COCIStatement::bind(char* name, COCIDate& date)
{
  CHECK(m_Session.get_error(), 
	  OCIBindByName(
		stmthp, 
		&m_pBind, 
		m_Session.get_error(), 
		(text *)name, 
		(sb4)strlen(name), 
		(dvoid*)date.get_oci_date(), 
		(sb4)sizeof(OCIDate), 
		(ub2)SQLT_ODT, 
		(void*)date.get_indicator(),
		0,0,0,0, OCI_DEFAULT));
}


void COCIStatement::define(int pos, COCIString& str)
{
  define(pos, (char*)str.get_text(), str.get_alloc_size(), str.get_indicator());
}

void COCIStatement::bind(int pos, COCIString& str)
{
  bind(pos, (char*)str.get_text(), str.get_size(), str.get_indicator());
}

void COCIStatement::bind(char* name, COCIString& str)
{
  bind(name, (char*)str.get_text(), str.get_size(), str.get_indicator());
}

void COCIStatement::define(int pos, std::string& str, signed short* is_null)
{
  define(pos, (char*)str.c_str(), (ub4)str.size(), is_null);
}

void COCIStatement::bind(int pos, std::string& str)
{
  bind(pos, (char*)str.c_str(), (ub4)str.size());
}

