// IOCPServer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"


int main()
{
	
	DATABASEMANAGER->init("root","0909qq");
	

	NETWORKMANGER->init(5001);
	NETWORKMANGER->AcceptProcess();
	



    return 0;
}

