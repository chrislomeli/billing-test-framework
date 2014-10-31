/*====================================================================*/
/* FILE PINSESSION.h                                             */
/*                                                                    */
/* DESCRIPTION                                                        */
/* interface for the PINSESSION class                            */
/*                                                                    */
/* AUTHOR                                                             */
/* clomeli - 02/27/01                                         */
/*                                                                    */
/*                                                                    */
/*====================================================================*/
// Modified
// 07/30/01 : cjl : Remove account find function


#if !defined(AFX_PINSESSION_H__3EA6F50A_91E0_4669_833C_CD9B4B718F88__INCLUDED_)
#define AFX_PINSESSION_H__3EA6F50A_91E0_4669_833C_CD9B4B718F88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning (disable : 4786 4005)
#endif 

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include "pin_type.h"


class pin_Session  
{
public:
	int64 getDBNumber();

	/* Constructors/Destructors */
	pin_Session();
	virtual ~pin_Session();

	/* Connection routines */
	bool OpenConnection();	
	bool OpenInternalConnection();
	bool OpenLocalConnection();
	void CloseConnection();
	bool isConnected();
	void setDebug( bool onoff );

	void setPortalPort(const char * s);
	void setPortalHostname(const char * s);
	void setPortalPassword(const char * s);
	void setPortalUser(const char * s);

	/* External connection routines */
	void SetConnection(pcm_context_t * external_ctxp);
	void CloseConnection(pcm_context_t *);
	pcm_context_t *getConnection();


	/* Opcode calling */
	virtual bool call_opcode(
			u_int opcode, 
			u_int flags,
			pin_flist_t *inflistp,
			pin_flist_t **outflistp
			);

	virtual bool call_opcode(
			u_int opcode, 
			u_int flags,
			pin_flist_t *inflistp,
			pin_flist_t **outflistp,
			pin_errbuf_t *ebufp 
			);


	/* Transaction routines */
	int  transaction_open(u_int i_flags );
	void transaction_abort();
	void transaction_commit();

	/* Helper routines */
	pin_errbuf_t * getebufp();

	pin_flist_t * credit_profile_flistp;
	/* This method is used for reading /config/credit_profile
	 * object.
	 * 
	 * input: -
	 * output : If successful, reads the /config/credit_profile
	 *	    object into the flist credit_profile_flistp
	 *	    and returns true otherwise returns false.
	 */
	bool readCreditProfileObj();

protected:

	/* vars */
	string m_sPortalHostname;
	string m_sPortalPassword;
	string m_sPortalUser;
	string m_sPortalPort;
	string m_sPortalServiceStr;

	int64 database;
	bool IsMyConnection;

	pin_errbuf_t	ebuf;
	pin_errbuf_t *	ebufp;
	pcm_context_t *ctxp;
	bool pin_debug;
};

#endif // !defined(AFX_PINSESSION_H__3EA6F50A_91E0_4669_833C_CD9B4B718F88__INCLUDED_)
