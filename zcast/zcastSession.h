#ifndef  __ZCAST_CONN_INTERFACE___
#define  __ZCAST_CONN_INTERFACE___

#include <util/EnvironmentStd.h>
#include <oci++/stdinc.h>

class zcastSession {
public:

	//*********************************
	// CTOR
	//
	zcastSession();

	virtual ~zcastSession(void);


	COCISession* ociSessionp;
	COCITransaction* ociTransp;
	COCIStatement* add_profile_stmtp;  // for the add_profile api
	bool m_connected;
	time_t last_unsuccessful_attempt;
	int retry_count;


	bool db_connect(string user, string password, string service);
	void db_disconnect(void);
	bool isConnected();
	void db_reset();

	COCIStatement*
		db_newStatement(COCISession * Sessionp, string sql_string);

	COCIStatement* addProfileStmt();

	int addProfile( 
			const char * login, const char * profile );


	string err_msg;
	int err_code;
	void reset_counters(void);
};

#endif


