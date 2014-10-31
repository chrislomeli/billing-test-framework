#ifndef _COCISession
#define _COCISession

#if defined(_WIN32)
#pragma warning(disable : 4786 4800 4666)
#endif

#if defined(_MSC_VER) && defined(_AFXDLL)
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#endif

#ifndef OCI_ORACLE
#include "oci.h"
#endif

#ifdef sparc
// used for bool definition
//#include <stl_config.h>
//#define std
#endif

#include <util/EnvironmentStd.h>
//#include "cerror.h"
#include <string>


#define ORACLE8i
#ifdef ORACLE8i
#define CHECK(errhp,Func)\
{\
	text errbuf[512];\
	int errcode;\
	sword sres = (Func);\
	if (sres != OCI_SUCCESS)\
	{\
		OCIErrorGet ((dvoid *) errhp, (ub4) 1, (text *) NULL, &errcode,\
		errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ERROR);\
			throw COCIException((char*)errbuf, errcode);\
	}\
}
#else
#define CHECK(errhp,Func)\
{\
	text errbuf[512];\
	long errcode;\
	sword sres = (Func);\
	if (sres != OCI_SUCCESS)\
	{\
		OCIErrorGet ((dvoid *) errhp, (ub4) 1, (text *) NULL, &errcode,\
		errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ERROR);\
		throw COCIException((char*)errbuf, errcode);\
	}\
}
#endif


class CBaseException
{
public:
  CBaseException(){}
  ~CBaseException(){}
 	char* GetError(){return m_error;}
  long  GetErrorCode(){return m_errorcode;}
protected:
	char* m_error;
  long  m_errorcode;

};

class COCIException : public CBaseException
{
public:
  COCIException(char* err, long errcode){m_error = err; m_errorcode = errcode;}
};

class CBindException : public CBaseException
{
public:
  CBindException(char* err = (char*)"Failed to bind"){m_error = err; m_errorcode = 0;}
};

class CDefineException : public CBaseException
{
public:
  CDefineException(char* err = (char*)"Failed to define"){m_error = err; m_errorcode = 0;}
};



class COCISession
{
public:
  COCISession() : authp(0)
, srvhp(0)
, svchp(0)
, envhp(0)
, errhp(0) {
};

virtual ~COCISession()
{
  if(m_bConnected)
  {
    end();
    detach_server();
	try {
    CHECK(errhp,OCIHandleFree((dvoid *)srvhp,(ub4)OCI_HTYPE_SERVER));
    CHECK(errhp,OCIHandleFree((dvoid *)svchp,(ub4)OCI_HTYPE_SVCCTX));
    CHECK(errhp,OCIHandleFree((dvoid *)authp,(ub4)OCI_HTYPE_SESSION));
    CHECK(errhp,OCIHandleFree((dvoid *)errhp,(ub4)OCI_HTYPE_ERROR));
    CHECK(errhp,OCIHandleFree((dvoid *)envhp,(ub4)OCI_HTYPE_ENV));
	} catch (...) {
	}
  }
}

  bool connect(
			const char* _username, 
			const char* _password, 
			const char* _server,
			ub4 init_mode  = OCI_DEFAULT );

  void connect(char* uid, ub4 init_mode = OCI_DEFAULT);
  void disconnect();

  OCISession*& get_session(void) {return authp;}
  OCIServer*& get_server(void) {return srvhp;}
  OCISvcCtx* get_svc(void) const {return svchp;}
  OCIEnv* get_env(void) const {return envhp;}
  OCIError* get_error(void) const {return errhp;}
  const char* get_server_name() const {return m_server_name;}
  const char* get_username() const{return m_username;}
  const char* get_schema_name() const {return m_username;}
  std::string get_server_version() const;

  sword detach_server();
  void end();

  COCISession& operator = (const COCISession& src);

private:
  sword init_handles(ub4 init_mode);
  sword attach_server(unsigned char* server);
  
  sword log_on(unsigned char* username, unsigned char* password,
               ub4 credt = OCI_CRED_RDBMS, ub4 mode = OCI_DEFAULT);

private:
  OCISession* authp;
  OCIServer* srvhp;
  OCISvcCtx* svchp;
  OCIEnv* envhp;
  OCIError* errhp;
  char m_server_name[100];
  char m_username[100];
  bool m_bConnected;
};

#include <string>
extern std::string uppercase(const char* str);



#endif
