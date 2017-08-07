#include "stdafx.h"
#include "cDataBaseManger.h"


cDataBaseManger::cDataBaseManger()
{
}


cDataBaseManger::~cDataBaseManger()
{
	



}

BOOL cDataBaseManger::init(char* user, char* password)
{
	m_bConnected = false;
	m_szHost = "localhost";
	m_szDatabase = "tcp";
	m_szUser = user;
	m_szPassword = password;

	
	if (false == m_bConnected) {
		mysql_init(&m_conn);
		printf("\nMySQL init success");
		ConnectToDB();
		return true;
	}
	return true;
}

BOOL cDataBaseManger::ConnectToDB()
{

	m_mySQL = mysql_real_connect(&m_conn, m_szHost, m_szUser, m_szPassword, m_szDatabase, 3306, (char*)NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS | CLIENT_REMEMBER_OPTIONS);

	if (m_mySQL != NULL) {
		printf("\nMySQL Connect Success\n");
		m_bConnected = true;
		mysql_select_db(&m_conn, m_szDatabase);
		return true;
	}
	m_bConnected = false;
	 return false;
}

BOOL cDataBaseManger::QueryUpdate(char* query)
{
	if (!m_mySQL)
	{
		cout << "Error MySQL is NULL" << endl;
		return FALSE;
	}

	INT iRet = mysql_query(m_mySQL, query);

	if (0 == iRet) {
		return true;
	}
	else {
		iRet = mysql_errno(m_mySQL);
		if ((CR_CONN_HOST_ERROR == iRet) || (CR_SERVER_LOST == iRet) || (CR_SERVER_GONE_ERROR == iRet)) {
			cout << "[" << iRet << "] server gone error: [" << query << "] : " << mysql_error(m_mySQL);
			mysql_close(m_mySQL);
			m_bConnected = false;
		}
		else {
			printf("error!! ");
		}
	}


	return false;
}


MYSQL_RES* cDataBaseManger::StoreResult()
{
	return mysql_store_result(m_mySQL);
}

UINT64 cDataBaseManger::AffectedRows() 
{
	return UINT64(mysql_affected_rows(m_mySQL));
}

UINT cDataBaseManger::RowCount(void* pvResults)
{
	return mysql_num_rows((MYSQL_RES*)pvResults);
}

UINT cDataBaseManger::FieldCount()
{
	return mysql_field_count(m_mySQL);
}

MYSQL_ROW cDataBaseManger::FetchRow(void* pvResults_) {
	if (pvResults_) 
	{
		return mysql_fetch_row((MYSQL_RES*)pvResults_);
	}
	else {
		printf("pvResults_ is null: ");
	}
	return NULL;
}

bool cDataBaseManger::NextResult() {
	INT iStatus = mysql_next_result(m_mySQL);
	if (0 == iStatus) {
		return true;
	}
	else if (0 < iStatus) {
		cout << "mysql_next_result: error: " << iStatus;
		return true;
	}
	return false;
}

void
cDataBaseManger::FreeResult(void* pvResults_) 
{
	if (pvResults_) 
	{
		mysql_free_result((MYSQL_RES*)pvResults_);
	}
}

vector<char*> cDataBaseManger::GetDBData()
{
	vector<char*>  datavector;
	MYSQL_RES* res = StoreResult();
	MYSQL_ROW row;
	int rowNum = RowCount(res);
	int fieldNum = FieldCount();
	
	while (NULL != (row = FetchRow(res))) {

		for (int i = 0; i < rowNum; i += fieldNum) {
			for (int j = i; j < fieldNum; j++) {
				datavector.push_back(row[j]);
			}
		}
	}

	FreeResult(res);

	if (datavector.size() <= 0) {
		printf("can't find Data");
	}


	return datavector;
}

