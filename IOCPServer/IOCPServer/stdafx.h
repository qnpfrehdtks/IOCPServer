// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
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






// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
