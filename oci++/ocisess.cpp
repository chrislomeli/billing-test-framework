#ifndef _COCISession
#include "ocisess.h"
#endif

#ifndef _INC_STRING
#include <string.h>
#endif


bool COCISession::connect(
		const char* _username, 
		const char* _password, 
		const char* _server,
        ub4 init_mode )
{
	unsigned char* username = (unsigned char*)_username; 
	unsigned char* password = (unsigned char*)_password; 
	unsigned char* server = (unsigned char*)_server;

	try {
		init_handles(init_mode);
		attach_server(server);
		log_on(username,password);
	}
	catch(CBaseException& exc) {
			char* buff = exc.GetError();
			cout << buff;
			m_bConnected = false;
			return m_bConnected;
	}
	strcpy(m_server_name, (char *)server);
	strcpy(m_username, (char *)username);
	m_bConnected = true;
	return m_bConnected;
}

void COCISession::connect(char* uid, ub4 init_mode)
{
  char uid_copy[100];
  strcpy(uid_copy, uid);
  char seps[]="/@";
  char* token;
  char* components[3] = {NULL,NULL,NULL};
  token = strtok((char*)uid_copy, seps);
  int component_cnt = 0;
  while(token != NULL)
  {
	  components[component_cnt] = token;
	  component_cnt++;
	  token = strtok(NULL, seps);
  }
  connect((const char*)components[0],(const char*)components[1], (const char*)components[2] ? (const char*)components[2] : (const char*)"",init_mode);
}


void COCISession::disconnect()
{
  if (m_bConnected)
	CHECK(errhp,OCILogoff(svchp, errhp));
  m_bConnected = false;
}

sword COCISession::init_handles(ub4 init_mode)
{
   CHECK(errhp,OCIInitialize(init_mode,  
     (dvoid *)0, 
	(dvoid * (*)(dvoid *, size_t)) 0,
	(dvoid * (*)(dvoid *, dvoid *, size_t)) 0,
	(void (*)(dvoid *, dvoid *)) 0 )); 


/* cjl : remove all of this crap
(dvoid *)0,
                    (dvoid * (*)(dvoid *, size_t)) 0,
                    (dvoid * (*)(dvoid *, dvoid *, size_t))0,
                    (void (*)(dvoid *, dvoid *)) 0 ));
*/

  /* Inititialize the OCI Environment */
  CHECK(errhp,OCIEnvInit((OCIEnv **) &envhp, (ub4) OCI_DEFAULT,
                 (size_t) 0, (dvoid **) 0 ));
 
  /* Allocate a service handle */
  CHECK(errhp,OCIHandleAlloc((dvoid *) envhp, (dvoid **) &svchp,
                     (ub4) OCI_HTYPE_SVCCTX, (size_t) 0, (dvoid **) 0));
 
  /* Allocate an error handle */
  CHECK(errhp,OCIHandleAlloc((dvoid *) envhp, (dvoid **) &errhp,
                     (ub4) OCI_HTYPE_ERROR, (size_t) 0, (dvoid **) 0));
 
  /* Allocate a server handle */
  CHECK(errhp,OCIHandleAlloc((dvoid *) envhp, (dvoid **) &srvhp,
                     (ub4) OCI_HTYPE_SERVER, (size_t) 0, (dvoid **) 0));
 
  /* Allocate a authentication handle */
  CHECK(errhp,OCIHandleAlloc((dvoid *) envhp, (dvoid **) &authp,
                     (ub4) OCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0));
 
  return OCI_SUCCESS;

}

sword COCISession::attach_server(unsigned char* server)
{
  CHECK(errhp,OCIServerAttach(srvhp, errhp, (unsigned char *) server,
                     (sb4) strlen((char *)server), (ub4) OCI_DEFAULT));


  /* Set the server handle in the service handle */
  CHECK(errhp,OCIAttrSet((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX,
                 (dvoid *) srvhp, (ub4) 0, (ub4) OCI_ATTR_SERVER, errhp));
 
  return OCI_SUCCESS;

}

sword COCISession::detach_server()
{
  CHECK(errhp,OCIServerDetach ( srvhp, errhp, OCI_DEFAULT)); 
  return OCI_SUCCESS;

}

void COCISession::end()
{
  CHECK(errhp,OCISessionEnd(svchp, errhp, authp, OCI_DEFAULT));
}

sword COCISession::log_on(unsigned char* username, unsigned char* password,
                          ub4 credt, ub4 mode)
{
    /* Set attributes in the authentication handle */
  CHECK(errhp,OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION,
                 (dvoid *) username, (ub4) strlen((char *) username),
                 (ub4) OCI_ATTR_USERNAME, errhp));
  CHECK(errhp,OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION,
                 (dvoid *) password, (ub4) strlen((char *) password),
                 (ub4) OCI_ATTR_PASSWORD, errhp));
 

  CHECK(errhp,OCISessionBegin(svchp, errhp, authp, credt, mode));
 
  /* Set the authentication handle in the Service handle */
  CHECK(errhp,OCIAttrSet((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX,
                 (dvoid *) authp, (ub4) 0, (ub4) OCI_ATTR_SESSION, errhp));
 
  return OCI_SUCCESS;	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          

}

COCISession& COCISession::operator = (const COCISession& src)
{
  if(this != &src)
  {
    authp = src.authp;
    srvhp = src.srvhp;
    svchp = src.svchp;
    envhp = src.envhp;
    errhp = src.errhp;
  }
  return *this;
}

std::string COCISession::get_server_version() const
{

  std::string tmp;
  tmp.resize(512);

  CHECK( errhp, OCIServerVersion ( svchp,
                                   errhp,
                                   (unsigned char*)tmp.c_str(), 512,
                                   OCI_HTYPE_SVCCTX));
  return tmp;
}


std::string uppercase(const char* str)
{
  std::string tmp = str;
  for(int i = 0; i < (int)strlen(str); i++)
  {
    // Is the character lowercase?
    if(tmp[i] >= 'a' && tmp[i] <= 'z')
      tmp[i] = tmp[i]-'a'+'A';
  }
  return tmp;
}

