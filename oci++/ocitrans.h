#ifndef _COCITransaction
#define _COCITransaction

#ifndef OCI_ORACLE
#include "oci.h"
#endif

class COCISession;

class COCITransaction
{
public:
  COCITransaction(const COCISession& Session);
  ~COCITransaction();
  void start();
  void commit();
  void suspend();
  void resume();
  void rollback();
  void prepare();
  void forget();

private:
  OCITrans* txnhp;
  const COCISession& m_Session;
  ub4 m_Savepoint;
};

#endif
