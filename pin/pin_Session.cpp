// InfranetSession.cpp: implementation of the pin_Session class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <pin/pin_Session.h>
#include <cm_fm.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



#if defined(WIN32) && defined(RUNASDLL)
__declspec(dllimport) extern pcm_context_t *_admin_ctxp;
#endif



/****************************************************************************
 * NAME 
 *    __
 *
 * PRE:
 *    __
 *
 * POST:
 *    __
 *
 * ERRORS
 *    __
 *        
 ****************************************************************************/
pin_Session::pin_Session()
{
	/*
	 * Houskeeping
	 */
	this->IsMyConnection = false;
	this->database = 0;
	ebufp				= &ebuf;
	this->ctxp = NULL;
	this->pin_debug = false;

}

/****************************************************************************
 * NAME 
 *    __
 *
 * PRE:
 *    __
 *
 * POST:
 *    __
 *
 * ERRORS
 *    __
 *        
 ****************************************************************************/
pin_Session::~pin_Session()
{
	if (this->IsMyConnection)
		this->CloseConnection();


}

/****************************************************************************
 * NAME 
 *    __
 *
 * PRE:
 *    __
 *
 * POST:
 *    __
 *
 * ERRORS
 *    __
 *        
 ****************************************************************************/
bool pin_Session::OpenConnection()
{
	string mstr = "pin_Session::OpenConnection";

	//-- If wev'e already been set - return true
	if ( isConnected()	) {
		return true;
	}

	if ( this->ctxp != NULL ) {
		free(this->ctxp);
	}

//-- Try to use the Adminlib connection when running as dll ext
#if defined(WIN32) && defined(_RUNASDLL)
	this->SetConnection( _admin_ctxp );
	if ( isConnected()	) {
		this->IsMyConnection = false;
		return true;
	} else
		return false;
#endif

	//-- if we have gone thorough the trouble of setting - use it
	if ( ! this->m_sPortalUser.empty() 
		&& ! this->m_sPortalPassword.empty()
		&& ! this->m_sPortalHostname.empty()
		&& ! this->m_sPortalPort.empty() )
	{
		if (  OpenInternalConnection() )
			return true;
	}

	if ( access("pin.conf",0) == 0 )
        {
		if ( OpenLocalConnection() )
			return true;
        }

	//-- Check one mo' time
	if ( isConnected()	) {
		return true;
	}
	return false;
}

/************************************************************
 * OpenLocalConnection
 *
 * Input:
 * Output:
 ************************************************************/
bool pin_Session::OpenLocalConnection()
{
	/*
	 * Houskeeping
	 */
	int64 localdb	=	1;  //-- for November release


	PIN_ERR_CLEAR_ERR(ebufp);
	
	/*
	 * Open the connection
	 */
	ctxp = NULL;
	PCM_CONNECT(&ctxp, &localdb, &ebuf);

	if ( isConnected() )
		return true;
		
	return false;
}

/************************************************************
 * OpenInternalConnection
 *
 * Input:
 * Output:
 ************************************************************/
bool pin_Session::OpenInternalConnection()
{
	/*
	 * Houskeeping
	 */
	pin_flist_t		*flistp = NULL;
	pin_flist_t		*a_flistp = NULL;
	poid_t			*userid_pdp = NULL;
	char pcm[40] = "0.0.0.1 /service/pcm_client 1 0";
	char adm[40] = "0.0.0.1 /service/admin_client 1 0";
	char *			poid_str[2];
	poid_str[0] = (char*)pcm;
	poid_str[1] = (char*)adm;
 
	u_int			type = 1;


	PIN_ERR_CLEAR_ERR(ebufp);
	

	/*
	 * Internal private routine - assume that portal login strings are good
	 */
	string stlserv = "ip "+m_sPortalHostname+" "+m_sPortalPort;

	flistp = PIN_FLIST_CREATE(ebufp);
	int i;

	for (i=0; i < 2; i++ )
	{
		/*
		 * Fill the connection flist
		 */
		userid_pdp = PIN_POID_FROM_STR((char*)poid_str[i], NULL, ebufp );

		PIN_FLIST_FLD_PUT(flistp, PIN_FLD_POID, (void *)userid_pdp, ebufp);
		PIN_FLIST_FLD_SET(flistp, PIN_FLD_TYPE, (void *)&type, ebufp);
		if (type == 1) 
		{
			PIN_FLIST_FLD_SET(flistp,PIN_FLD_LOGIN,(void *)m_sPortalUser.c_str(), ebufp);
			PIN_FLIST_FLD_SET(flistp, PIN_FLD_PASSWD_CLEAR,(void *)m_sPortalPassword.c_str(), ebufp);
		}

		a_flistp = PIN_FLIST_ELEM_ADD(flistp, PIN_FLD_CM_PTRS, 1, ebufp);
			PIN_FLIST_FLD_SET(a_flistp, PIN_FLD_CM_PTR, (void *)stlserv.c_str(), ebufp);

		/*
		 * Open the connection
		 */
		PIN_ERR_CLEAR_ERR(ebufp);
		PCM_CONTEXT_OPEN(&this->ctxp, flistp, ebufp);

		/*
		 * Check results - err types not so important 
		 *  we either have a connection or not
		 */
		if (PIN_ERR_IS_ERR(ebufp)) 
		{

			 ;  // error
		} else {
			; // error
		}

		/*
		 * Check the actual connection
		 *  if we are good, return, otherwise try 
		 *  a different poid type (admin or pcm)
		 */
		if ( this->isConnected() )
		{
			PIN_FLIST_DESTROY (flistp, NULL); 
			return true;
		}

	} //-- for loop

	/*
	 * Cleanup & Exit
	 */
	PIN_FLIST_DESTROY (flistp, NULL); 
	if ( this->isConnected() ) 
		return true;
	return false;

}



/**************************************
 * Close Connection
 *
 **************************************/
void pin_Session::CloseConnection(pcm_context_t *myctxp)
{
	ctxp = myctxp;
	CloseConnection();
}

void pin_Session::CloseConnection()
{
#if defined(WIN32) && defined(RUNASDLL)
	return;
#endif
    PCM_CONTEXT_CLOSE(ctxp, 0, NULL);
	ctxp = NULL;
}

/****************************************************************************
 * NAME isConnected
 *    checks if a connection to Infranet is open.
 *
 * PARAMETERS
 *    none
 *
 * RETURN TYPE
 *    bool - 'true' if exists, 'false' if not
 *
 * ERRORS
 *    NOT_CONNECTED_TO_INFRANET
 *        Connection to Infranet doesn't exist and can't be opened
 *
 ****************************************************************************/
bool pin_Session::isConnected()
{
	poid_t *s_pdp = NULL;
	bool connected = false;

	if ( this->ctxp != NULL ) 
	{
		// Check the session object
		s_pdp = (poid_t*)
			pcm_get_session( ctxp );

		if ( PIN_POID_IS_NULL(s_pdp) ) 
			ctxp = NULL;
		else {
			connected = true;
			this->database = pin_poid_get_db(s_pdp);
			pin_poid_destroy(s_pdp, NULL);
		}
	}
	return connected;  // Ok context is good
}


/****************************************************************************
 * NAME op
 *    Calls an opcode in Infranet.  This is essentially a wrapper for the 
 *    Infranet opcode call.
 *
 * PARAMETERS
 *    opcode - the opcode to be called
 *    flags - opcode flags to be used
 *    inflistp - the input FLIST pointer
 *    outflistp - the pointer** of the FLIST that will receive output
 *    rc - The StatusObject containing status information
 *
 * RETURN TYPE
 *    none
 *
 * ERRORS
 *    NOT_CONNECTED_TO_INFRANET
 *        Infranet Connection not open
 *    RSP_DATA_VALIDATION_ERROR
 *        FLIST value failed the data validation
 *    RSP_DATA_MISSING_INFO
 *        Information missing in the FLIST
 *    
 *
 ****************************************************************************/
bool pin_Session::call_opcode(
							u_int opcode, 
							u_int flags,
							pin_flist_t *inflistp,
							pin_flist_t **outflistp
							)
{
	/*
	 * Houskeeping 
	 */
	PIN_ERR_CLEAR_ERR(this->ebufp);

	return (
		this->call_opcode( opcode, flags, inflistp, outflistp, this->ebufp )
	);
}

void pin_Session::setDebug( bool onoff ) {
	this->pin_debug = onoff;
}


/**************************************
 * Call Opcode Utility
 *
 **************************************/
bool 
pin_Session::call_opcode(
	u_int opcode, 
	u_int flags,
	pin_flist_t *inflistp,
	pin_flist_t **return_flistpp,
	pin_errbuf_t *localebufp
	)
{
	/*
	 * Declares 
	 */


	/*
	 * Houskeeping 
	 */
	PIN_ERR_CLEAR_ERR(localebufp);

	string instr(pcm_opcode_to_opname(opcode));
	string outstr(instr);

	instr.append("Input flist ...");
	outstr.append("Output flist ...");

	/*
	 * Preconditions 
	 */

	/*-- connection to Infranet --*/
	if ( isConnected() == false	) {
		if ( OpenConnection() == false ) {
			// error
			return false;
		}
	}

	/*-- flist storage --*/
	if ( inflistp == NULL ) {
		pin_set_err(ebufp, PIN_ERRLOC_APP, PIN_ERRCLASS_APPLICATION, PIN_ERR_NULL_PTR, 0,0,0 );
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, 
				(char*)"Panic: call_opcode recieved a NULL input flist", ebufp );	
		return false;
	} 
	/*-- flist storage --*/
	if ( return_flistpp == NULL ) {
		pin_set_err(ebufp, PIN_ERRLOC_APP, PIN_ERRCLASS_APPLICATION, PIN_ERR_NULL_PTR, 0,0,0 );
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, 
				(char*)"Panic: call_opcode recieved a NULL output flist pointer2pointer", ebufp );	
		return false;
	} 
	/*-- flist storage --*/
	if ( *return_flistpp != NULL ) {
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_WARNING, 
			(char*)"Memory leak : output flist has storage, Delelting it...", ebufp );	
		PIN_FLIST_DESTROY_EX(return_flistpp,NULL);
		*return_flistpp = NULL;
		return false;
	} 


	/*
	 * Call the opcode 
	 */
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,(char*)instr.c_str(),inflistp );

	PCM_OP (ctxp, opcode, flags, inflistp, return_flistpp, localebufp);

	if ( PIN_ERR_IS_ERR(localebufp) )
	{
			PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, (char*)pcm_opcode_to_opname(opcode), localebufp ); 
			PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_ERROR,(char*)"input flist:", inflistp ); 
			if (return_flistpp && *return_flistpp)
				PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_ERROR,(char*)"output flist:", *return_flistpp ); 

	}
	else if (return_flistpp && *return_flistpp)
	{
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, (char*)outstr.c_str(),*return_flistpp );

	}


	/*
	 * Check success / fail 
	 */
	return (!PIN_ERR_IS_ERR (localebufp) );
}



/**************************************
 * getConnection
 *
 **************************************/
pcm_context_t *pin_Session::getConnection()
{
	bool goodcon = false;

	if ( isConnected() ) 
		goodcon = true;
	else {
		if ( OpenConnection() )
			goodcon = true;
	}

	if ( goodcon )
		return ctxp;

	return NULL;
}

/**************************************
 * sets
 *
 **************************************/
void pin_Session::setPortalUser(const char * s)
{
	m_sPortalUser = s;
}

void pin_Session::setPortalPassword(const char * s)
{
	m_sPortalPassword = s;

}

void pin_Session::setPortalHostname(const char * s)
{
	m_sPortalHostname = s;
}

void pin_Session::setPortalPort(const char * s)
{
	m_sPortalPort = s;

}


pin_errbuf_t * pin_Session::getebufp()
{
	return (this->ebufp);
}

/*******************************************************************
 * pin_Session::transaction_open():
 *
 *	Attempts to open a transaction on the given ctxp.
 *
 *	Reads flags to dtermine whether the transaction is RO or RW.
 *
 *******************************************************************/
int
pin_Session::transaction_open(u_int i_flags )
{
	pin_flist_t	*flistp = NULL;
	pin_flist_t	*r_flistp = NULL;
	u_int		flags;
	int		result = PIN_BOOLEAN_FALSE;
	char  acctstr[10] = "/account\0";

	if (PIN_ERR_IS_ERR(ebufp))
		return(result);
	PIN_ERR_CLEAR_ERR(ebufp);

	/*
	 * Create the flistp for opening the transaction.
	 */
	int64 db = 1;
	poid_t *acct_pdp = 
		PIN_POID_CREATE(db, acctstr, 1, ebufp);

	flistp = PIN_FLIST_CREATE(ebufp);
	PIN_FLIST_FLD_PUT(flistp, PIN_FLD_POID, (void *)acct_pdp, ebufp);

	/*
	 * What are our flags?
	 */
	flags = PCM_TRANS_OPEN_READWRITE;
	if (i_flags & PCM_OPFLG_CALC_ONLY)
		flags = PCM_TRANS_OPEN_READONLY;
	if (i_flags & PCM_TRANS_OPEN_LOCK_OBJ)
		flags |= PCM_TRANS_OPEN_LOCK_OBJ;

	/*
	 * Use pcm_op to open the trans.
	 */
	PCM_OP(this->ctxp, PCM_OP_TRANS_OPEN, flags, flistp, &r_flistp, ebufp);

	/*
	 * What do we return?
	 */
	switch (ebufp->pin_err) {
	case PIN_ERR_NONE:
		result = PIN_BOOLEAN_TRUE;
		break;
	case PIN_ERR_TRANS_ALREADY_OPEN:
		result = PIN_BOOLEAN_FALSE;
		PIN_ERR_CLEAR_ERR(ebufp);
		break;
	default:
		result = PIN_BOOLEAN_FALSE;
		break;
	}

	/*
	 * Clean up.
	 */
	PIN_FLIST_DESTROY_EX(&flistp, NULL);
	PIN_FLIST_DESTROY_EX(&r_flistp, NULL);

	return(result);
}

/*******************************************************************
 * pin_Session::transaction_abort():
 *******************************************************************/
void
pin_Session::transaction_abort()
{
	pin_flist_t	*flistp = NULL;
	pin_errbuf_t	*foo;
	pin_errbuf_t	local_ebuf;

	foo = &local_ebuf;

	PIN_ERR_CLEAR_ERR(foo);

	/***********************************************************
	 * Abort the open (locked) transaction.
	 ***********************************************************/
	PCM_OP(this->ctxp, PCM_OP_TRANS_ABORT, 0, (pin_flist_t *)0, &flistp, foo);

	/***********************************************************
	 * Clean up.
	 ***********************************************************/
	PIN_FLIST_DESTROY(flistp, NULL);

	/***********************************************************
	 * Return with existing errror.
	 ***********************************************************/
	return;
}

/*******************************************************************
 * pin_Session::transaction_commit():
 *******************************************************************/
void
pin_Session::transaction_commit()
{
	pin_flist_t	*flistp = NULL;
	pin_errbuf_t	*foo;
	pin_errbuf_t	local_ebuf;

	/***********************************************************
	 * Which ebufp?
	 ***********************************************************/
	foo = &local_ebuf;

	PIN_ERR_CLEAR_ERR(foo);

	/***********************************************************
	 * Commit the open (locked) transaction.
	 ***********************************************************/
	PCM_OP(ctxp, PCM_OP_TRANS_COMMIT, 0, (pin_flist_t *)0, &flistp, foo);

	/***********************************************************
	 * Clean up.
	 ***********************************************************/
	PIN_FLIST_DESTROY(flistp, NULL);

	/***********************************************************
	 * Return with existing errror.
	 ***********************************************************/
	return;
}



/************************************************************
 * SetConnection
 *
 * Input:
 * Output:
 ************************************************************/
void pin_Session::SetConnection(pcm_context_t * external_ctxp)
{
	/*
	 * Houskeeping
	 */
	this->ctxp = external_ctxp;
}



int64 pin_Session::getDBNumber()
{
	if ( !this->isConnected() )
		return 1;
	poid_t *routing_pdp = pcm_get_session( this->getConnection() );
	if (!PIN_POID_IS_NULL(routing_pdp))
	{
		return PIN_POID_GET_DB(routing_pdp);
	}
	return 1;
}
