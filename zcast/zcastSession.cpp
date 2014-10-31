#include <zcast/zcastSession.h>


zcastSession::zcastSession() 
	: ociTransp(NULL) 
	, ociSessionp(NULL)
	,  add_profile_stmtp(NULL)
	, m_connected(false)
{
	db_reset();
}

zcastSession::~zcastSession(void)
{
	if ( this->m_connected )
		this->db_disconnect();
	db_reset();
}

////////////////////////////////////////////////////////////////// 
// Inits
///////////////////////////////////////////////////////////////////
void zcastSession::db_disconnect(void)
{
	if(this->ociSessionp) {
		this->ociSessionp->disconnect();
	}
	db_reset();
}

//*********************************
// reset
	void zcastSession::db_reset()
	{
		if (ociTransp) { delete ociTransp, ociTransp = NULL; }
		if (ociSessionp) { 
			ociSessionp->disconnect();
			delete ociSessionp;
			ociSessionp = NULL; 
		}
		if (add_profile_stmtp) { delete add_profile_stmtp; add_profile_stmtp=NULL; }
	}



//////////////////////////////////////////////////////////////////
// Helpers
//////////////////////////////////////////////////////////////////


//*********************************
// db_connect
//
bool zcastSession::db_connect(
	string user, string password, string service)
{
		this->ociSessionp = new COCISession();
		bool rc = false;

		if (m_connected)
			db_disconnect();

		try {
			if ( ociSessionp->connect(user.c_str(),password.c_str(),service.c_str()) ) {
				rc = m_connected = true;
				this->last_unsuccessful_attempt=0;
				this->retry_count=0;
			}

		} catch (COCIException e) {
			rc=false;
		}

		if (!rc)
		{
			this->last_unsuccessful_attempt = time(NULL);
			this->retry_count++;
		}
		return rc;
}

//*********************************
// db_newStatement
//
COCIStatement*
	zcastSession::db_newStatement(COCISession * Sessionp, string sql_string)
{
	COCIStatement* stmtp = NULL;
	if (stmtp) { delete stmtp; stmtp = NULL; }

	if ( ! ociSessionp ){
		return NULL;
	}

	try {
		stmtp = new COCIStatement(*Sessionp);
		if (!sql_string.empty())
			*stmtp = (char*)sql_string.c_str();

	} catch (COCIException e) {
			return NULL;
	} catch (CDefineException e) {
			return NULL;
	}
	return stmtp;
}





//////////////////////////////////////////////
//*********************************
// sql_selectAccountData
//

COCIStatement*
zcastSession::addProfileStmt()
{
/*
	    Procedure MBR_PROFILE_UPDATE_PG.ADD_PROFILE(
                p_login in mbr_login.login_name%type,
                p_profile_string_in in varchar2, -- question_id1|answer_id1~question_id2|answer_id2... 
                p_error_num out nz_app_error.error_num%type,
                p_error_msg out nz_app_error.error_msg%type)

*/
	ostringstream os;
	os << "BEGIN ADD_PROFILE( :1, :2, :3, :4 ); END; ";

	COCIStatement* stmtp = NULL;
	if ((stmtp=db_newStatement(this->ociSessionp, os.str())) == NULL )
		return NULL;

	return stmtp;
}




int zcastSession::addProfile( 
			const char * login, const char * profile )
{
	int p_error_num = 0;
	char p_error_msg[255];
	char p_Login[255];
	char p_profile_string_in[255];
	short is_null = 1;

	bool rc;
	if (!p_Login || !p_profile_string_in || ! p_error_msg)
		return false;

	sprintf(p_Login, "%s\0", login );
	sprintf(p_profile_string_in, "%s\0", profile );
	memset(p_error_msg,0,255);

	//-- is the select statement prepared?-------
	if ( add_profile_stmtp == NULL ) {
		if ((add_profile_stmtp = this->addProfileStmt()) == NULL ) {
			return false;
		}
	}

	//-- execute the search----------------------
	try 
	{
		add_profile_stmtp->bind(1,p_Login,255, &is_null);
		add_profile_stmtp->bind(2,p_profile_string_in, 255, &is_null);
		add_profile_stmtp->bind(3,p_error_num);
		add_profile_stmtp->bind(4,p_error_msg, 255, &is_null);

		if ((rc=add_profile_stmtp->execute()) == false )
		{
			return -510;
		}
		else if (p_error_num == 0)
			return 1;
			
	}
	catch (COCIException e) {
		return e.GetErrorCode();
	}

	return (-1*p_error_num);
}


bool zcastSession::isConnected()
{
	return m_connected;

}

