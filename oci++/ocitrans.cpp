#ifndef _COCITransaction
#include "ocitrans.h"
#endif

#ifndef _COCISession
#include "ocisess.h"
#endif

#ifndef _COCIStatement
#include "ocistmt.h"
#endif

COCITransaction::COCITransaction(const COCISession& Sess)
: m_Session(Sess)
, txnhp(0)
, m_Savepoint(0)
{

  CHECK(m_Session.get_error(),OCIHandleAlloc((dvoid *)m_Session.get_env(), (dvoid **)&txnhp,  OCI_HTYPE_TRANS, 0, 0));

  CHECK(m_Session.get_error(),OCIAttrSet((dvoid *)m_Session.get_svc(), OCI_HTYPE_SVCCTX, (dvoid *)txnhp, 0,
                          OCI_ATTR_TRANS, m_Session.get_error()));

}


COCITransaction::~COCITransaction()
{
  CHECK(m_Session.get_error(),OCIHandleFree(txnhp,OCI_HTYPE_TRANS));
}

void COCITransaction::start()
{
  if(m_Savepoint == 0)
    CHECK(m_Session.get_error(),OCITransStart(m_Session.get_svc(), m_Session.get_error(), 2, OCI_TRANS_READWRITE));

  if(m_Savepoint > 0)
  {
    COCIStatement s(m_Session);
    char buf[100];
    sprintf(buf,"savepoint s%d",m_Savepoint);
    s = buf;
    s.execute();
  }
  m_Savepoint++;
}

void COCITransaction::commit()
{
  if(m_Savepoint > 0)
    m_Savepoint--;

  if(m_Savepoint == 0)
    CHECK(m_Session.get_error(),OCITransCommit(m_Session.get_svc(), m_Session.get_error(), (ub4)0));


}

void COCITransaction::suspend()
{
  CHECK(m_Session.get_error(), OCITransDetach(m_Session.get_svc(), m_Session.get_error(), OCI_DEFAULT));
}

void COCITransaction::resume()
{
  CHECK(m_Session.get_error(),OCITransStart(m_Session.get_svc(), m_Session.get_error(), 60, OCI_TRANS_RESUME));
}

void COCITransaction::rollback()
{
  if(m_Savepoint > 1)
  {
    m_Savepoint--;
    COCIStatement s(m_Session);
    char buf[100];
    sprintf(buf,"rollback to savepoint s%d",m_Savepoint);
    s = buf;
    s.execute();
  }
  else
  {
    CHECK(m_Session.get_error(),OCITransRollback(m_Session.get_svc(), m_Session.get_error(), (ub4)0));
  }
  

}

void COCITransaction::prepare()
{
  CHECK(m_Session.get_error(),OCITransPrepare(m_Session.get_svc(), m_Session.get_error(), (ub4)OCI_DEFAULT));
}

void COCITransaction::forget()
{
  CHECK(m_Session.get_error(),OCITransForget(m_Session.get_svc(), m_Session.get_error(), OCI_DEFAULT));
}