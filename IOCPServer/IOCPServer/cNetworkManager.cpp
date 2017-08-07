#include "stdafx.h"
#include "cNetworkManager.h"

unsigned int __stdcall CompletionThread(LPVOID pComPort);
BOOL Recv(LPPER_IO_DATA PerIoData, DWORD DataByte, byte* packetstr, cTCPPacket** packet);
BOOL Send(int ID, int size, char* packet);
void packetClassify(unsigned char* buf, int buffSize, PACKET_KEY* packetState, int* ID, char* DataBuffer, int* outSize);
char* packetProcess(PACKET_KEY key, int playerID, char Data[], int DataSize, int PlayerKey);
void createPacketFrame(int size, PACKET_KEY PacketKey, int PlayerID, char* outbuffer);

mutex mtx;

cNetworkManager::cNetworkManager() : m_ClientIdx(0)
{
}


cNetworkManager::~cNetworkManager()
{
}

BOOL cNetworkManager::init(int PORT)
{
	WSADATA wsaData;
	

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) /* Load Winsock 2.2 DLL */
		ErrorHandling("WSAStartup() error!");

	m_ListenSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (m_ListenSock == INVALID_SOCKET)
	    ErrorHandling("socket() error");

	memset(&m_servAdr, 0, sizeof(m_servAdr));
	m_servAdr.sin_family = AF_INET;
	m_servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_servAdr.sin_port = htons((short)PORT);

	// thread 헤더 와 충돌이 있어서 ::bind로 썻다.  (원인을 찾기 어려웠음...)
	// Stack overflow 에서는 using namespace std 가 그래서 좋지 않은 버릇이라고 한다.... ㄷㄷ
	::bind(m_ListenSock, (SOCKADDR*)&m_servAdr, sizeof(m_servAdr));


	int retval = listen(m_ListenSock, 1);

	if (retval == SOCKET_ERROR)
	{
		int err_no = WSAGetLastError();
		printf("Listen() %d error\n", err_no);
		ErrorHandling("Listen T^T \n");
	}

	ZeroMemory(&m_clients, sizeof(PER_CLNT_DATA) * 1000);
	printf("Client Init Success");

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	/*if (CreateIOCP() == FALSE)
	{
		return FALSE;
	}*/
	SYSTEM_INFO sys;
	GetSystemInfo(&sys);

	for (int i = 0; i < sys.dwNumberOfProcessors * 2; i++)
	{
		_beginthreadex(NULL, 0, CompletionThread, (LPVOID)m_hIOCP, 0, NULL);
	}
	//CreateThreadPool();

	return TRUE;

}

BOOL cNetworkManager::AcceptProcess()
{


	while (1)
	{
		SOCKET hClntsock;
		SOCKADDR_IN clntAddr;
		int addrSize = sizeof(clntAddr);

		hClntsock = accept(m_ListenSock, (SOCKADDR*)&clntAddr, &addrSize);

		printf("ACCEPT CLIENT!!");

		if (hClntsock == INVALID_SOCKET)
		{
			// 리슨 소켓을 클로즈 하면 이 에러가 나오므로
			// 이 에러시에 Accept 루프를 빠져나간다.
			if (WSAGetLastError() == WSAEINTR)
			{
				return FALSE;
			}

			ErrorHandling("accept Error");
		}

		//int addrLen = sizeof(clntAddr);
		//LPPER_HANDLE_DATA hHandleData = new PER_HANDLE_DATA();
		//hHandleData->clntSock = hClntsock;
		//memcpy(&(hHandleData->clntAdr), &clntAddr, addrLen);

		m_ClientIdx += 1;
		m_clients[m_ClientIdx].clntSock = hClntsock;
		m_clients[m_ClientIdx].connected = true;
		m_clients[m_ClientIdx].packet_size = 0;
		m_clients[m_ClientIdx].previoust_size = 0;
		m_clients[m_ClientIdx].recv_overlap.rwmode = IO_RECV;
		m_clients[m_ClientIdx].recv_overlap.wsaBuf.len = BUFF_MAX;
		m_clients[m_ClientIdx].recv_overlap.wsaBuf.buf = reinterpret_cast<char*>( m_clients[m_ClientIdx].recv_overlap.buffer);
		ZeroMemory(&m_clients[m_ClientIdx].recv_overlap.ov, sizeof(m_clients[m_ClientIdx].recv_overlap.ov));


	//	CreateIoCompletionPort((HANDLE)hClntsock, m_hIOCP, (DWORD)hHandleData, 0);

		if (!ConnectToIOCP((HANDLE)m_clients[m_ClientIdx].clntSock,(DWORD)m_ClientIdx))
		{
			printf("Connect To IOCP Error: %d\n", GetLastError());
			continue;
		}

		////LPPER_IO_DATA ioData = new PER_IO_DATA();
		////memset(&(ioData->ov), 0, sizeof(OVERLAPPED));
		////ioData->wsaBuf.len = BUFF_MAX;
		////ioData->wsaBuf.buf = reinterpret_cast<char*>(ioData->buffer);
		////ioData->rwmode = IO_RECV;

		DWORD dwRecvBytes = 0;
		DWORD dwFlags = 0;


		if (m_clients[m_ClientIdx].clntSock == INVALID_SOCKET || m_clients[m_ClientIdx].clntSock == NULL)
		{
			printf("Clnt Sock is Connectless");
			closesocket(m_clients[m_ClientIdx].clntSock);
		}

		/*for (uint32_t i = 0; i < m_ClientIdx + 1; i++)
		{
			if (m_clients[i].connected = false) continue;
			if ( i == m_ClientIdx) continue;

			cTCPPacket packet;

			Send(i, packet);
			
		}*/


		WSARecv(m_clients[m_ClientIdx].clntSock, // 데이터 입력소켓.
			&(m_clients[m_ClientIdx].recv_overlap.wsaBuf),  // 데이터 입력 버퍼포인터.
			1,       // 데이터 입력 버퍼의 수.
			(LPDWORD)&dwRecvBytes,
			(LPDWORD)&dwFlags,
			&(m_clients[m_ClientIdx].recv_overlap.ov), // OVERLAPPED 구조체 포인터.
			NULL
			);

		

	}
	return 0;
}


BOOL cNetworkManager::CreateIOCP()
{
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_hIOCP == NULL )
	{
		printf(" IOCP Create Error %d",GetLastError());
	}

	return (m_hIOCP != NULL);
}



BOOL cNetworkManager::CreateThreadPool()
{
	SYSTEM_INFO sys;
	GetSystemInfo(&sys);

	for (int i = 0; i < sys.dwNumberOfProcessors * 2; i++)
	{
		_beginthreadex(NULL, 0, CompletionThread, (LPVOID)m_hIOCP, 0, NULL);
	}


	return TRUE;

}

BOOL cNetworkManager::ConnectToIOCP(HANDLE hDevice, DWORD CompletionKey)
{
	HANDLE retHandle = CreateIoCompletionPort(hDevice, m_hIOCP, CompletionKey, 0);
	if (retHandle != m_hIOCP )
	{
		printf("Connect To ICOP Error  : %d", GetLastError());
	}

	return (retHandle == m_hIOCP);
}



void cNetworkManager::CloseSocket(int IDX)
{
	closesocket(m_clients[IDX].clntSock);
	m_clients[IDX].connected = false;
}

unsigned int __stdcall CompletionThread(LPVOID pComPort)
{
	HANDLE hCompletionPort = (HANDLE)pComPort;
	int required = 0;
	/*
	인자로 받은 pComPort 에는 main의 hCompletionPort 가 전달 된다.
	이는 쓰레드를 생성시 main 의 hCompletionPort 를 인자로 전달 했기 때문이다.
	*/
	bool isStartPacket = true;
	DWORD BytesTransferred;
	DWORD Key;
	LPPER_IO_DATA PerIoData;

	DWORD flags;
	int test_data = 0;

	while (1)
	{
		// 5. 입출력이 완료된 소켓의 정보 얻음.
		BOOL isSucce = GetQueuedCompletionStatus(hCompletionPort,    // Completion Port
			&BytesTransferred,   // 전송된 바이트수
			&Key,
			(LPOVERLAPPED*)&PerIoData, // OVERLAPPED 구조체 포인터.
			INFINITE
			);

		/*
		첫번째 인자로 전달된 hCompletionPort 의연결된 소켓들 중에
		입출력을 완료한 소켓이 있다면 리턴 한다.
		그렇지 않다면 입출력의 완료가 될때까지 기다린다.
		이때 세번째 인자로 전달된 PerHandleData 은입출력이 완료된 소켓이
		hCompletionPort 와 연결 할때 같이 세번째 인자로 전달했던 클라이언트 정보가 전달 된다.
		위에가서 다시보고 오도록 하자.
		네번째 인자로 전달된 PerIoData 에는Send 나Recv 시에 전달했던
		overapped 구조체 변수의 포인터를 얻기 위해사용된다.

		위로 올라가 Recv 함수 호출시 어떻게 overlapped 구조체 변수의 주소를 전달했는지 보고 오도록하자.
		여기서 우리가 LPPER_IO_DATA 구조체로 overlapped 구조체 정보를 얻어올때 Recv 시 전달했던
		데이터 정보들도 같이 받아오게 된다.
		이는 받아오게 되는 overlapped 구조체 주소가 실제로는 LPPER_IO_DATA 구조체의 주소이기 때문이다.
		LPPER_IO_DATA 구조체를 만들어 줄때 가장 먼저 포함 시킨 맴버가 무엇인지 보고 오도록 하자.
		LPPER_IO_DATA 구조체의 시작주소 와 overlapped 구조체의 시작주소는 같기 때문에 가능하다.
		*/

		if (BytesTransferred == 0 || FALSE == isSucce) {
			printf("- Clinet Connect Exit -\n");
			//delete PerIoData;
			NETWORKMANGER->CloseSocket(Key);
			/*
			클라이언트의 연결 종료시 처리를 해준다.
			free 가 이해가 안된다면 동적할당을 배우고 오도록..
			*/
			continue;
		}
		
		PER_CLNT_DATA clnt = NETWORKMANGER->GetCLNT(Key);
		
		if (PerIoData->rwmode == IO_RECV)
		{
			int remain = BytesTransferred;
			Packet *buf_ptr = clnt.recv_overlap.buffer;

			while (0 < remain)
			{
				if (isStartPacket) {
					memcpy(&clnt.packet_size, buf_ptr, sizeof(int));
				}
				required = clnt.packet_size - clnt.previoust_size;

				if (remain >= required)
				{
					// 패킷 완성~ 
					memcpy(&clnt.packet_buff + clnt.previoust_size, buf_ptr, required);
					buf_ptr += required;
					remain -= required;

					int pID; // 플레이어 ID
					PACKET_KEY PacketKey;  // Packet의 key를 담을 변수
					int size = sizeof(clnt.packet_buff); // 패킷의 사이즈 좀 알자.
					char DataBuffer[3000] = { 0 };


					int dataSize = 0; // 제대로 된 패킷 사이즈

				


					// 들어온 패킷 이놈이 무슨 놈인지 알기 위해 처리하는 함수.
					packetClassify(clnt.packet_buff, size, &PacketKey, &pID, DataBuffer, &dataSize);
					printf("\n%d 사잊, \n", dataSize);

				    // 위에서 얻은 패킷 정보를 바탕으로 Data를 처리하자~~~
					// 스레드간 동기화거 필요.
					// 이 부분 동기화가 되지 않는다면... 끔찍한 일이 벌어짐.
					// 왜냐면 heap 영역을 건드는 코드가 많다보니...
					 mtx.lock();
					 packetProcess(PacketKey, pID, DataBuffer, dataSize - 12, Key);
					 mtx.unlock();
				
					// 버퍼 정보 초기화
					clnt.packet_size = 0;
					clnt.previoust_size = 0;
					isStartPacket = true;
	
					break;

				}
				else // 패킷을 완성하지 못했으니 계속 정보를 받자.
				{
					memcpy(&clnt.packet_buff + clnt.previoust_size, buf_ptr, required);
					buf_ptr += remain;
					clnt.previoust_size += remain;
					remain = 0;
					isStartPacket = false;
				}
			}

			flags = 0;
			//IOCP의 특성/... 주고 받고를 무한 반복하는 운명임.
			if (WSARecv(clnt.clntSock, &(clnt.recv_overlap.wsaBuf), 1, NULL,
				&flags, &(clnt.recv_overlap.ov), NULL) == SOCKET_ERROR)
			{
				int error_np = WSAGetLastError();
				if(error_np != WSA_IO_PENDING)
				  printf("WorkerThread::WSARecv Error");

				continue;
			}
		}
		else if (PerIoData->rwmode == IO_SEND)
		{
			delete PerIoData;
		}
		else
		{
			NETWORKMANGER->ErrorHandling("another Command ");
		}
		
	
		}
	
}

BOOL Send(int ID, int size, char* packet)
{
	//cTCPPacket* packet = PACKETMANAGER->createPacket(PK_LOGIN_REQUEST, 111, "DSDF");

	PER_CLNT_DATA clnt = NETWORKMANGER->GetCLNT(ID);

	
	//int size = sizeof(*packet);
	//char buffer[1616];
	PER_IO_DATA* PerIoData = new PER_IO_DATA;
	ZeroMemory(PerIoData, sizeof(PER_IO_DATA));

	memcpy(PerIoData->buffer, packet, size);
	PerIoData->rwmode = IO_SEND;
	PerIoData->wsaBuf.buf = reinterpret_cast<char*>(PerIoData->buffer);
	PerIoData->wsaBuf.len = size;


	printf("Send to %d Key Clnt Packet Size : %d \n", ID, size);
	DWORD flag = 0;
	//WSASend(clnt.clntSock, &(PerIoData->wsaBuf), 1, NULL, flag, &(PerIoData->ov), NULL);
	if (SOCKET_ERROR == WSASend(clnt.clntSock, &(PerIoData->wsaBuf), 1, NULL, flag, &(PerIoData->ov), NULL))
	{
		int err_no = WSAGetLastError();
	//	printf("SendPacket::WsaSend Error");
		if (err_no != WSA_IO_PENDING)
		{
			printf(" error %d", err_no);
			NETWORKMANGER->ErrorHandling("\nWSASend() error");
		}
	}

	return TRUE;
}



BOOL Recv(LPPER_IO_DATA PerIoData, DWORD DataByte , byte* packetstr, cTCPPacket** packet)
{
		//PerIoData->wsaBuf.buf[DataByte] = '\0';
		//memcpy(packet, PerIoData->buffer, DataByte);
		cTCPPacket tcppacket = (cTCPPacket&)(PerIoData->buffer);

		// 온 패킷에 따라 처리

	//	*packet = PACKETMANAGER->packetDispatch(tcppacket);

		PerIoData->rwmode = IO_SEND; // 읽기모드로 데이터를 읽어왔으니, 이제 쓰기모드로 변경을 한다.

		return TRUE;
}


void packetClassify(unsigned char* buf, int buffSize, PACKET_KEY* packetState, int* ID, char* DataBuffer, int* outSize )
{
	char tempBuf[3000];
	memcpy(tempBuf, buf, buffSize);

	int buffCurPos = 0;

	memcpy(outSize, tempBuf + buffCurPos, sizeof(int));
	buffCurPos += 4;
	memcpy(packetState, tempBuf + buffCurPos, sizeof(PACKET_KEY));
	buffCurPos += 4;
	memcpy(ID, tempBuf + buffCurPos, sizeof(int));
	buffCurPos += 4;

	if( buffSize > buffCurPos)
	memcpy(DataBuffer, tempBuf + buffCurPos, buffSize - buffCurPos);

	//(packetState, PlayerId, "LL");
}




void createPacketFrame(int size, PACKET_KEY PacketKey, int PlayerID, char* outbuffer )
{
	cTCPPacket p;
	sTCPHeader header;
	header.Size = size;  //사이즈
	header.pkKey = PacketKey; // 패킷의 용도가 뭔지 key
	header.pkPlayerID = PlayerID; // 플레이어 ID
	p.SetHeader(header);

	memcpy(outbuffer, (char*)&p, sizeof(p)); 
}


char* packetProcess(PACKET_KEY PacketKey, int playerID, char Data[], int DataSize,int PlayerKey)
{

	// playerID 클라이언트상 번호
	// PlayerKey clnt가 담긴 indx; (네트워크용)

	// Packet 첫 번째 : 패킷의 size
	// Packet 두 번째 : 패킷의 용도가 뭔지  Key
	// Pakcet 세 번째 : 어느 플레이어에게 보내고 받을건지, 플레이어 ID 정보
	int SkillTypeSize = 4;
	int buffCurPos = 0;
	char* tempBuffer;
	char* sendoutBuffer;
	cplayerData* enemyData;
	cplayerData* user;
	const int packetHeaderSize = sizeof(cTCPPacket);
	int PlayerSize = sizeof(cplayerData);
	int PointSize = sizeof(sCoordinate);
	// 우선 패킷의 헤더 사이즈를 담는 변수~
	int enemyID = -1;
	switch (PacketKey)
	{
		// 플레이어 입장하시고, 매칭을 기다린다 라는 패킷을 날린다!
	case PK_PLAYER_ENTER:
		if ((enemyID = USERMANAGER->addPlayerData(playerID, PlayerKey)) > -1) // 이거 TRUE면 적이랑 매완료~!  
		{
		/*	int enemyId = -1;
			if ((enemyId = USERMANAGER->matchingPlayer(playerID)) > -1)
			{*/			
				char* sendoutBuffer = new char[packetHeaderSize + PlayerSize];
				char* tempBuffer;

				createPacketFrame(packetHeaderSize + PlayerSize, PK_ENEMY_ENTER, playerID, sendoutBuffer);

				enemyData = USERMANAGER->getPlayer(enemyID);

				tempBuffer = (char*)enemyData;
				memcpy(sendoutBuffer + packetHeaderSize, tempBuffer, PlayerSize);
				Send(PlayerKey, packetHeaderSize + PlayerSize , sendoutBuffer);

				//Send(enemyData.GetpKey, packetHeaderSize + PlayerSize, sendoutBuffer);

				delete[] sendoutBuffer;
		}
		else  // 매칭이 되지 않은 경우... 안됬다고 패킷을 보내자
		{
			printf("No %d Player Doesn't Match a Player \n", playerID);
			char* sendoutBuffer = new char[packetHeaderSize];

			createPacketFrame(packetHeaderSize, PK_PLAYER_WAIT, playerID, sendoutBuffer);
			Send(PlayerKey, packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;
		}
		break;

	case PK_PLAYER_READY:

		// 저 ㄹㄷ 상태에요. 하고 표시
		USERMANAGER->getPlayer(playerID)->SetReady(true);

		if (USERMANAGER->getPlayer(playerID)->IsEnemyReady()) // 적도  Ready 상태인지 확인한다.
		{
			char* sendoutBuffer = new char[packetHeaderSize + PlayerSize];
			char* tempBuffer;

			createPacketFrame(packetHeaderSize + PlayerSize, PK_ENEMY_READY, playerID, sendoutBuffer);

			cplayerData* enemyData = USERMANAGER->getPlayer(playerID)->GetMatchingPlayer();

			tempBuffer = (char*)enemyData;
			memcpy(sendoutBuffer + packetHeaderSize, tempBuffer, PlayerSize);
			Send(PlayerKey, packetHeaderSize + PlayerSize, sendoutBuffer);

			delete[] sendoutBuffer;
		}
		else  // 매칭이 되지 않은 경우.. 적은 아직 ㄹㄷ가 아니라서 Wait 하라는 패킷을 보내도록 하자.
		{
			printf("No %d Player Doesn't Match a Player \n", playerID);
			char* sendoutBuffer = new char[packetHeaderSize];

			createPacketFrame(packetHeaderSize, PK_PLAYER_WAIT, playerID, sendoutBuffer);
			Send(PlayerKey, packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;
		}
		break;


	case PK_BATTLE_START:
	
			sendoutBuffer = new char[packetHeaderSize + PlayerSize];
		
			int m_Turn;
		     user = USERMANAGER->getPlayer(playerID);
			enemyData = user->GetMatchingPlayer();

			if (user->checkOrder() == playerID )
			{
				user->SetTurn(true);
				enemyData->SetTurn(false);
				createPacketFrame(packetHeaderSize + PlayerSize, PK_MY_TURN, playerID, sendoutBuffer);
			}
			else if(user->checkOrder() == enemyData->GetpID() )
			{
				user->SetTurn(false);
				enemyData->SetTurn(true);
				createPacketFrame(packetHeaderSize + PlayerSize, PK_OPPO_TURN, playerID, sendoutBuffer);
			}
			else
			{
				printf(" so Strange both isMyTurn is true...?? please exit server...");
			}

			tempBuffer = (char*)enemyData;
			memcpy(sendoutBuffer + packetHeaderSize, tempBuffer, PlayerSize);
			Send(PlayerKey, packetHeaderSize + PlayerSize, sendoutBuffer);

			delete[] sendoutBuffer;

		break;

		// 내가 적에게 공격을 한다!! 내 공격 포인트를 서버에 저장하고 적의 패킷을 기다리장...~
	case PK_PLAYER_ATTACK:

	//	sendoutBuffer = new char[packetHeaderSize];
		 user = USERMANAGER->getPlayer(playerID);
		 enemyData = user->GetMatchingPlayer();
		 buffCurPos = 0;

		sPoint tempXY;

		memcpy(&tempXY.X, Data + buffCurPos, sizeof(int));

		buffCurPos += 4;
		
		memcpy(&tempXY.Y, Data + buffCurPos, sizeof(int));

		printf("\n %d %d 우웅 ", tempXY.X, tempXY.Y);


		user->SetAttacking(true);  // 나 공격 준비 마침.
		user->SetAttackPt(tempXY.X, tempXY.Y, SK_BASE);  // 공격 위치를 설정하고 빠져나온다.
		 
		enemyData->SetAttacking(false);  // 적은 공격할 그런 처지가 아님.


		break;

		// 적이 공격을 성공했다고 서버에게 패킷을 보낸 경우
	case PK_ENEMY_ATTACKSUCC:
		user = USERMANAGER->getPlayer(playerID);
		enemyData = user->GetMatchingPlayer();

		// 적이 공격을 성공했으니 또 적의 공격이 가능하다는 것을 명심.
		if (!user->IsMyTurn()         // 적의 턴이고
			&& enemyData->IsMyTurn()    // 내턴이 아닌고
			&& !enemyData->IsAttacking())   // 적이 공격할 그런 단계가 아닐때
		{
			// 그럼 이제 적에게 공격을 성공했다고 정보를 보내자.
			sendoutBuffer = new char[packetHeaderSize];
			createPacketFrame(packetHeaderSize, PK_PLAYER_DAMAGESUCC, enemyData->GetpID(), sendoutBuffer);

			Send(enemyData->GetpKey(), packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;

			// 턴을 다시 설정.
			user->SetTurn(false);   // 내 턴이 아니라
			enemyData->SetTurn(true);  // 적의 턴이라고
		}
		else
		{
			printf(" 왜 내턴이 아닌데 공격을??");
		}

		break;
		// 적이 공격을 실패했따고 서버에게 보낸 경우.
	case PK_ENEMY_ATTACKFAIL:
		sendoutBuffer = new char[packetHeaderSize];
		user = USERMANAGER->getPlayer(playerID);
		enemyData = user->GetMatchingPlayer();

		if (!user->IsMyTurn() 
			&& enemyData->IsMyTurn()
			&& !enemyData->IsAttacking())
		{
			// 적에게 공격 실패했다고 보낸다.
			createPacketFrame(packetHeaderSize, PK_PLAYER_DAMAGEFAIL, enemyData->GetpID(), sendoutBuffer);

			Send(enemyData->GetpKey(), packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;

			// 턴을 돌립시다... 
			user->SetTurn(true);   // 이제 내 턴이고
			enemyData->SetTurn(false);   //  적 턴이 아님.

		}
		else
		{
			printf(" 왜 내턴이 아닌데 공격을??");
		}


		break;

	case PK_PLAYER_SKILLATTACK:

		SKILL skill;
		user = USERMANAGER->getPlayer(playerID);
		enemyData = user->GetMatchingPlayer();
        

		buffCurPos = 0;


		memcpy(&skill, Data + buffCurPos, sizeof(int));
		buffCurPos += 4;


		sPoint tempXYs[100];

		for (int i = 0; i < (DataSize - 4) / 8; i++)
		{
			memcpy(&tempXYs[i].X, Data + buffCurPos, sizeof(int));

			buffCurPos += 4;

			memcpy(&tempXYs[i].Y, Data + buffCurPos, sizeof(int));

			buffCurPos += 4;

			printf("\n %d  %d  SkillPos \n", tempXYs[i].X, tempXYs[i].Y);
		}

		user->SetAttacking(true);  // 나 공격 준비 마침.
		user->SetAttacksPt(tempXYs,DataSize - 4, skill);  // 공격 위치를 설정하고 빠져나온다.

		enemyData->SetAttacking(false);  // 적은 공격할 그런 처지가 아님.


		break;



		// 턴을 클라에게 알려주는 경우다.
	case PK_TURN_CHECK:
		sendoutBuffer = new char[packetHeaderSize];
		user = USERMANAGER->getPlayer(playerID);
		enemyData = user->GetMatchingPlayer();
		printf("두번은 와야재\n");
		if (!user->IsMyTurn() && enemyData->IsMyTurn())
		{
			printf("적의 턴이구용\n");
			createPacketFrame(packetHeaderSize, PK_OPPO_TURN, playerID, sendoutBuffer);

			Send(PlayerKey, packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;

		}
		else if (user->IsMyTurn() && !enemyData->IsMyTurn())
		{
			printf("나의 턴이구용\n");
			createPacketFrame(packetHeaderSize, PK_MY_TURN, playerID, sendoutBuffer);

			Send(PlayerKey, packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;
		}


		break;

	case PK_ENEMY_ATTACKWAIT:  // 적의 공격을 기다리는 우리 유저...
		user = USERMANAGER->getPlayer(playerID);
		enemyData = user->GetMatchingPlayer();

		// 적이 공격 패킷이 이미 등록된 상황.
		if (
			enemyData->GetAttackType() == SK_BASE
			&& enemyData->IsAttacking()  // 적이 공격할려는 중이고
			&& enemyData->IsMyTurn()  // 적의 턴인 상태이고
			&& !user->IsMyTurn()     // 유저의 턴이 아니다.
			)
		{
			sendoutBuffer = new char[packetHeaderSize + PointSize];
			createPacketFrame(packetHeaderSize + PointSize, PK_ENEMY_ATTACK, playerID, sendoutBuffer);

			// 둘다 공격 상태가 아닌 상태로 돌리자.
			enemyData->SetAttacking(false);
			user->SetAttacking(false);

			// 우리 플레이어도 공격을 기다리는 상태가 아닌걸로 바꾸고
			user->SetAttackWait(false);

			tempBuffer = (char*)&enemyData->GetAttkPoint();
			memcpy(sendoutBuffer + packetHeaderSize, tempBuffer, PointSize);

			Send(PlayerKey, packetHeaderSize + PointSize, sendoutBuffer);

			delete[] sendoutBuffer;

		}
		// 적이 공격 패킷이 이미 등록된 상황.
		else if (
			enemyData->GetAttackType() != SK_BASE
			&& enemyData->IsAttacking()  // 적이 공격할려는 중이고
			&& enemyData->IsMyTurn()  // 적의 턴인 상태이고
			&& !user->IsMyTurn()     // 유저의 턴이 아니다.
			)
		{
			printf("아 행님 스킬 공격 이구만여");
			sendoutBuffer = new char[packetHeaderSize + SkillTypeSize +enemyData->GetAttackSize()];
			createPacketFrame(packetHeaderSize + SkillTypeSize + enemyData->GetAttackSize(), PK_ENEMY_SKILLATTACK, playerID, sendoutBuffer);

			// 둘다 공격 상태가 아닌 상태로 돌리자.
			enemyData->SetAttacking(false);
			user->SetAttacking(false);

			// 우리 플레이어도 공격을 기다리는 상태가 아닌걸로 바꾸고
			user->SetAttackWait(false);


			int skillType = enemyData->GetAttackType();
			tempBuffer = (char*)&skillType;
			memcpy(sendoutBuffer + packetHeaderSize, tempBuffer, SkillTypeSize);

			tempBuffer = (char*)enemyData->GetAttkPoints();
			memcpy(sendoutBuffer + SkillTypeSize + packetHeaderSize, tempBuffer, enemyData->GetAttackSize());

			Send(PlayerKey, packetHeaderSize + SkillTypeSize + enemyData->GetAttackSize(), sendoutBuffer);

			delete[] sendoutBuffer;

		}
		// 적이 아직 공격을 못 끝마침....
		else if(
			!enemyData->IsAttacking()  // 적이 공격 중이 아니거나
			|| ( !enemyData->IsMyTurn() && user->IsMyTurn() )     // 유저의 턴인데 공격 오길 기다린다고?? 
			)
		{ 
			printf("기다령;ㅁ\n");
			//  유저야 그냥 공격을 기다려라....
			sendoutBuffer = new char[packetHeaderSize];
			createPacketFrame(packetHeaderSize, PK_ENEMY_ATTACKWAIT, playerID, sendoutBuffer);

			Send(PlayerKey, packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;
		}



		break;


	case PK_PLAYER_RADERRESULT:

		sendoutBuffer = new char[packetHeaderSize + DataSize];
		user = USERMANAGER->getPlayer(playerID);
		enemyData = user->GetMatchingPlayer();

		if (!user->IsMyTurn()
			&& enemyData->IsMyTurn()
			&& !enemyData->IsAttacking()
			&& enemyData->GetAttackType() == SK_RADER
			)
		{
			int RaderResult = 0;
			memcpy(&RaderResult, Data, sizeof(int));

			// 적에게 레이더 결과 통보
			createPacketFrame(packetHeaderSize + 4, PK_PLAYER_RADERRESULT, enemyData->GetpID(), sendoutBuffer);

			tempBuffer = (char*)&RaderResult;
			memcpy(sendoutBuffer + packetHeaderSize, tempBuffer, DataSize);

			Send(enemyData->GetpKey(), packetHeaderSize + DataSize, sendoutBuffer);

			printf("\n%d를 레이더 결과로 데이터 사이즈는 %d \n", RaderResult, DataSize);

			delete[] sendoutBuffer;

			// 턴을 돌립시다... 
			user->SetTurn(true);   // 이제 내 턴이고
			enemyData->SetTurn(false);   //  적 턴이 아님.

		}
		else
		{
			printf(" 왜 내턴이 아닌데 공격을??");
		}
		break;

	case PK_PLAYER_SKILLRESULT:

		sendoutBuffer = new char[packetHeaderSize + DataSize];
		user = USERMANAGER->getPlayer(playerID);
		enemyData = user->GetMatchingPlayer();

		if (!user->IsMyTurn()
			&& enemyData->IsMyTurn()
			&& !enemyData->IsAttacking()
			&& enemyData->GetAttackType() != SK_RADER
			)
		{
			// 적에게 스킬 결과 통보
			createPacketFrame(packetHeaderSize + (DataSize), PK_PLAYER_SKILLRESULT, enemyData->GetpID(), sendoutBuffer);

			tempBuffer = (char*)Data;
			memcpy(sendoutBuffer + packetHeaderSize, tempBuffer, DataSize);

			Send(enemyData->GetpKey(), packetHeaderSize + DataSize, sendoutBuffer);

			//printf("\n%d를 레이더 결과로 데이터 사이즈는 %d \n", RaderResult, DataSize);

			delete[] sendoutBuffer;

			// 턴을 돌립시다... 
			user->SetTurn(true);   // 이제 내 턴이고
			enemyData->SetTurn(false);   //  적 턴이 아님.

		}
		else
		{
			printf(" 왜 내턴이 아닌데 공격을??");
		}
		break;

	}
	return NULL;
}


void cNetworkManager::ErrorHandling(char *message)
{
		fputs(message, stderr);
		fputc('\n', stderr);
		printf("전송 횟수 : %d\n");
		exit(1);
}

	



