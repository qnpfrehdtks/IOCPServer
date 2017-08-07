// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <iostream>

#include <tchar.h>
#include <WinSock2.h>
#include <process.h>
#include <vector>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
//#include <functional>
//#include <thread>
//#include <process.h>
#include <string.h>
#include <string>
#include <mutex>
#include "GlobalDef.h"
#include "singleton.h"

#include <errmsg.h>
//#include <json\json.h>
//#include<windows.data.json.h>

using namespace std;



#include "cDataBaseManger.h"
#include "cNetworkManager.h"
#include "cPacketManager.h"
#include "cUserManager.h"

#define BUFF_MAX 1024
#define MAX_CLIENT 4

#define DATABASEMANAGER cDataBaseManger::getSingleton()
#define PACKETMANAGER cPacketManager::getSingleton()
#define NETWORKMANGER cNetworkManager::getSingleton()
#define USERMANAGER cUserManager::getSingleton()


#define MIN_RATING 50
#define MAX_RATING 50

#pragma comment(lib,"ws2_32.lib")

//#pragma comment(lib ,"json_vc71_libmtd")






// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
