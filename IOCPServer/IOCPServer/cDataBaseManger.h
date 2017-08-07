#pragma once
#include "stdafx.h"
#include <mysql.h>

#pragma comment(lib, "libmysql.lib")

class cDataBaseManger : public singletonBase<cDataBaseManger>
{
private:
	const char*  m_szUser;
	const char*	 m_szPassword;
	const char*  m_szHost;
	const char*	 m_szDatabase;

	bool		 m_bSecureAuth;
	bool		 m_bConnected;
	MYSQL        m_conn;
	MYSQL*       m_mySQL;
	MYSQL_ROW    m_mySQLRow;

public:


	cDataBaseManger();
	~cDataBaseManger();

	vector<char*>           GetDBData();
	BOOL                    init(char* user, char* password);
	BOOL                    ConnectToDB();
	BOOL                    QueryUpdate(char* query);

    UINT			        RowCount(void*);
	UINT			        FieldCount();
	UINT64                  AffectedRows();
	MYSQL_RES*		        StoreResult();
	MYSQL_ROW		        FetchRow(void*);
	bool			        NextResult();
	void			        FreeResult(void*);
  
};

