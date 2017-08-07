// IOCPServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"


int main()
{
	
	DATABASEMANAGER->init("root","0909qq");
	

	NETWORKMANGER->init(5001);
	NETWORKMANGER->AcceptProcess();
	



    return 0;
}

