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

	// thread ��� �� �浹�� �־ ::bind�� ����.  (������ ã�� �������...)
	// Stack overflow ������ using namespace std �� �׷��� ���� ���� �����̶�� �Ѵ�.... ����
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
			// ���� ������ Ŭ���� �ϸ� �� ������ �����Ƿ�
			// �� �����ÿ� Accept ������ ����������.
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


		WSARecv(m_clients[m_ClientIdx].clntSock, // ������ �Է¼���.
			&(m_clients[m_ClientIdx].recv_overlap.wsaBuf),  // ������ �Է� ����������.
			1,       // ������ �Է� ������ ��.
			(LPDWORD)&dwRecvBytes,
			(LPDWORD)&dwFlags,
			&(m_clients[m_ClientIdx].recv_overlap.ov), // OVERLAPPED ����ü ������.
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
	���ڷ� ���� pComPort ���� main�� hCompletionPort �� ���� �ȴ�.
	�̴� �����带 ������ main �� hCompletionPort �� ���ڷ� ���� �߱� �����̴�.
	*/
	bool isStartPacket = true;
	DWORD BytesTransferred;
	DWORD Key;
	LPPER_IO_DATA PerIoData;

	DWORD flags;
	int test_data = 0;

	while (1)
	{
		// 5. ������� �Ϸ�� ������ ���� ����.
		BOOL isSucce = GetQueuedCompletionStatus(hCompletionPort,    // Completion Port
			&BytesTransferred,   // ���۵� ����Ʈ��
			&Key,
			(LPOVERLAPPED*)&PerIoData, // OVERLAPPED ����ü ������.
			INFINITE
			);

		/*
		ù��° ���ڷ� ���޵� hCompletionPort �ǿ���� ���ϵ� �߿�
		������� �Ϸ��� ������ �ִٸ� ���� �Ѵ�.
		�׷��� �ʴٸ� ������� �Ϸᰡ �ɶ����� ��ٸ���.
		�̶� ����° ���ڷ� ���޵� PerHandleData ��������� �Ϸ�� ������
		hCompletionPort �� ���� �Ҷ� ���� ����° ���ڷ� �����ߴ� Ŭ���̾�Ʈ ������ ���� �ȴ�.
		�������� �ٽú��� ������ ����.
		�׹�° ���ڷ� ���޵� PerIoData ����Send ��Recv �ÿ� �����ߴ�
		overapped ����ü ������ �����͸� ��� ���ػ��ȴ�.

		���� �ö� Recv �Լ� ȣ��� ��� overlapped ����ü ������ �ּҸ� �����ߴ��� ���� ����������.
		���⼭ �츮�� LPPER_IO_DATA ����ü�� overlapped ����ü ������ ���ö� Recv �� �����ߴ�
		������ �����鵵 ���� �޾ƿ��� �ȴ�.
		�̴� �޾ƿ��� �Ǵ� overlapped ����ü �ּҰ� �����δ� LPPER_IO_DATA ����ü�� �ּ��̱� �����̴�.
		LPPER_IO_DATA ����ü�� ����� �ٶ� ���� ���� ���� ��Ų �ɹ��� �������� ���� ������ ����.
		LPPER_IO_DATA ����ü�� �����ּ� �� overlapped ����ü�� �����ּҴ� ���� ������ �����ϴ�.
		*/

		if (BytesTransferred == 0 || FALSE == isSucce) {
			printf("- Clinet Connect Exit -\n");
			//delete PerIoData;
			NETWORKMANGER->CloseSocket(Key);
			/*
			Ŭ���̾�Ʈ�� ���� ����� ó���� ���ش�.
			free �� ���ذ� �ȵȴٸ� �����Ҵ��� ���� ������..
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
					// ��Ŷ �ϼ�~ 
					memcpy(&clnt.packet_buff + clnt.previoust_size, buf_ptr, required);
					buf_ptr += required;
					remain -= required;

					int pID; // �÷��̾� ID
					PACKET_KEY PacketKey;  // Packet�� key�� ���� ����
					int size = sizeof(clnt.packet_buff); // ��Ŷ�� ������ �� ����.
					char DataBuffer[3000] = { 0 };


					int dataSize = 0; // ����� �� ��Ŷ ������

				


					// ���� ��Ŷ �̳��� ���� ������ �˱� ���� ó���ϴ� �Լ�.
					packetClassify(clnt.packet_buff, size, &PacketKey, &pID, DataBuffer, &dataSize);
					printf("\n%d ����, \n", dataSize);

				    // ������ ���� ��Ŷ ������ �������� Data�� ó������~~~
					// �����尣 ����ȭ�� �ʿ�.
					// �� �κ� ����ȭ�� ���� �ʴ´ٸ�... ������ ���� ������.
					// �ֳĸ� heap ������ �ǵ�� �ڵ尡 ���ٺ���...
					 mtx.lock();
					 packetProcess(PacketKey, pID, DataBuffer, dataSize - 12, Key);
					 mtx.unlock();
				
					// ���� ���� �ʱ�ȭ
					clnt.packet_size = 0;
					clnt.previoust_size = 0;
					isStartPacket = true;
	
					break;

				}
				else // ��Ŷ�� �ϼ����� �������� ��� ������ ����.
				{
					memcpy(&clnt.packet_buff + clnt.previoust_size, buf_ptr, required);
					buf_ptr += remain;
					clnt.previoust_size += remain;
					remain = 0;
					isStartPacket = false;
				}
			}

			flags = 0;
			//IOCP�� Ư��/... �ְ� �ް� ���� �ݺ��ϴ� �����.
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

		// �� ��Ŷ�� ���� ó��

	//	*packet = PACKETMANAGER->packetDispatch(tcppacket);

		PerIoData->rwmode = IO_SEND; // �б���� �����͸� �о������, ���� ������� ������ �Ѵ�.

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
	header.Size = size;  //������
	header.pkKey = PacketKey; // ��Ŷ�� �뵵�� ���� key
	header.pkPlayerID = PlayerID; // �÷��̾� ID
	p.SetHeader(header);

	memcpy(outbuffer, (char*)&p, sizeof(p)); 
}


char* packetProcess(PACKET_KEY PacketKey, int playerID, char Data[], int DataSize,int PlayerKey)
{

	// playerID Ŭ���̾�Ʈ�� ��ȣ
	// PlayerKey clnt�� ��� indx; (��Ʈ��ũ��)

	// Packet ù ��° : ��Ŷ�� size
	// Packet �� ��° : ��Ŷ�� �뵵�� ����  Key
	// Pakcet �� ��° : ��� �÷��̾�� ������ ��������, �÷��̾� ID ����
	int SkillTypeSize = 4;
	int buffCurPos = 0;
	char* tempBuffer;
	char* sendoutBuffer;
	cplayerData* enemyData;
	cplayerData* user;
	const int packetHeaderSize = sizeof(cTCPPacket);
	int PlayerSize = sizeof(cplayerData);
	int PointSize = sizeof(sCoordinate);
	// �켱 ��Ŷ�� ��� ����� ��� ����~
	int enemyID = -1;
	switch (PacketKey)
	{
		// �÷��̾� �����Ͻð�, ��Ī�� ��ٸ��� ��� ��Ŷ�� ������!
	case PK_PLAYER_ENTER:
		if ((enemyID = USERMANAGER->addPlayerData(playerID, PlayerKey)) > -1) // �̰� TRUE�� ���̶� �ſϷ�~!  
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
		else  // ��Ī�� ���� ���� ���... �ȉ�ٰ� ��Ŷ�� ������
		{
			printf("No %d Player Doesn't Match a Player \n", playerID);
			char* sendoutBuffer = new char[packetHeaderSize];

			createPacketFrame(packetHeaderSize, PK_PLAYER_WAIT, playerID, sendoutBuffer);
			Send(PlayerKey, packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;
		}
		break;

	case PK_PLAYER_READY:

		// �� ���� ���¿���. �ϰ� ǥ��
		USERMANAGER->getPlayer(playerID)->SetReady(true);

		if (USERMANAGER->getPlayer(playerID)->IsEnemyReady()) // ����  Ready �������� Ȯ���Ѵ�.
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
		else  // ��Ī�� ���� ���� ���.. ���� ���� ������ �ƴ϶� Wait �϶�� ��Ŷ�� �������� ����.
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

		// ���� ������ ������ �Ѵ�!! �� ���� ����Ʈ�� ������ �����ϰ� ���� ��Ŷ�� ��ٸ���...~
	case PK_PLAYER_ATTACK:

	//	sendoutBuffer = new char[packetHeaderSize];
		 user = USERMANAGER->getPlayer(playerID);
		 enemyData = user->GetMatchingPlayer();
		 buffCurPos = 0;

		sPoint tempXY;

		memcpy(&tempXY.X, Data + buffCurPos, sizeof(int));

		buffCurPos += 4;
		
		memcpy(&tempXY.Y, Data + buffCurPos, sizeof(int));

		printf("\n %d %d ��� ", tempXY.X, tempXY.Y);


		user->SetAttacking(true);  // �� ���� �غ� ��ħ.
		user->SetAttackPt(tempXY.X, tempXY.Y, SK_BASE);  // ���� ��ġ�� �����ϰ� �������´�.
		 
		enemyData->SetAttacking(false);  // ���� ������ �׷� ó���� �ƴ�.


		break;

		// ���� ������ �����ߴٰ� �������� ��Ŷ�� ���� ���
	case PK_ENEMY_ATTACKSUCC:
		user = USERMANAGER->getPlayer(playerID);
		enemyData = user->GetMatchingPlayer();

		// ���� ������ ���������� �� ���� ������ �����ϴٴ� ���� ���.
		if (!user->IsMyTurn()         // ���� ���̰�
			&& enemyData->IsMyTurn()    // ������ �ƴѰ�
			&& !enemyData->IsAttacking())   // ���� ������ �׷� �ܰ谡 �ƴҶ�
		{
			// �׷� ���� ������ ������ �����ߴٰ� ������ ������.
			sendoutBuffer = new char[packetHeaderSize];
			createPacketFrame(packetHeaderSize, PK_PLAYER_DAMAGESUCC, enemyData->GetpID(), sendoutBuffer);

			Send(enemyData->GetpKey(), packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;

			// ���� �ٽ� ����.
			user->SetTurn(false);   // �� ���� �ƴ϶�
			enemyData->SetTurn(true);  // ���� ���̶��
		}
		else
		{
			printf(" �� ������ �ƴѵ� ������??");
		}

		break;
		// ���� ������ �����ߵ��� �������� ���� ���.
	case PK_ENEMY_ATTACKFAIL:
		sendoutBuffer = new char[packetHeaderSize];
		user = USERMANAGER->getPlayer(playerID);
		enemyData = user->GetMatchingPlayer();

		if (!user->IsMyTurn() 
			&& enemyData->IsMyTurn()
			&& !enemyData->IsAttacking())
		{
			// ������ ���� �����ߴٰ� ������.
			createPacketFrame(packetHeaderSize, PK_PLAYER_DAMAGEFAIL, enemyData->GetpID(), sendoutBuffer);

			Send(enemyData->GetpKey(), packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;

			// ���� �����ô�... 
			user->SetTurn(true);   // ���� �� ���̰�
			enemyData->SetTurn(false);   //  �� ���� �ƴ�.

		}
		else
		{
			printf(" �� ������ �ƴѵ� ������??");
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

		user->SetAttacking(true);  // �� ���� �غ� ��ħ.
		user->SetAttacksPt(tempXYs,DataSize - 4, skill);  // ���� ��ġ�� �����ϰ� �������´�.

		enemyData->SetAttacking(false);  // ���� ������ �׷� ó���� �ƴ�.


		break;



		// ���� Ŭ�󿡰� �˷��ִ� ����.
	case PK_TURN_CHECK:
		sendoutBuffer = new char[packetHeaderSize];
		user = USERMANAGER->getPlayer(playerID);
		enemyData = user->GetMatchingPlayer();
		printf("�ι��� �;���\n");
		if (!user->IsMyTurn() && enemyData->IsMyTurn())
		{
			printf("���� ���̱���\n");
			createPacketFrame(packetHeaderSize, PK_OPPO_TURN, playerID, sendoutBuffer);

			Send(PlayerKey, packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;

		}
		else if (user->IsMyTurn() && !enemyData->IsMyTurn())
		{
			printf("���� ���̱���\n");
			createPacketFrame(packetHeaderSize, PK_MY_TURN, playerID, sendoutBuffer);

			Send(PlayerKey, packetHeaderSize, sendoutBuffer);

			delete[] sendoutBuffer;
		}


		break;

	case PK_ENEMY_ATTACKWAIT:  // ���� ������ ��ٸ��� �츮 ����...
		user = USERMANAGER->getPlayer(playerID);
		enemyData = user->GetMatchingPlayer();

		// ���� ���� ��Ŷ�� �̹� ��ϵ� ��Ȳ.
		if (
			enemyData->GetAttackType() == SK_BASE
			&& enemyData->IsAttacking()  // ���� �����ҷ��� ���̰�
			&& enemyData->IsMyTurn()  // ���� ���� �����̰�
			&& !user->IsMyTurn()     // ������ ���� �ƴϴ�.
			)
		{
			sendoutBuffer = new char[packetHeaderSize + PointSize];
			createPacketFrame(packetHeaderSize + PointSize, PK_ENEMY_ATTACK, playerID, sendoutBuffer);

			// �Ѵ� ���� ���°� �ƴ� ���·� ������.
			enemyData->SetAttacking(false);
			user->SetAttacking(false);

			// �츮 �÷��̾ ������ ��ٸ��� ���°� �ƴѰɷ� �ٲٰ�
			user->SetAttackWait(false);

			tempBuffer = (char*)&enemyData->GetAttkPoint();
			memcpy(sendoutBuffer + packetHeaderSize, tempBuffer, PointSize);

			Send(PlayerKey, packetHeaderSize + PointSize, sendoutBuffer);

			delete[] sendoutBuffer;

		}
		// ���� ���� ��Ŷ�� �̹� ��ϵ� ��Ȳ.
		else if (
			enemyData->GetAttackType() != SK_BASE
			&& enemyData->IsAttacking()  // ���� �����ҷ��� ���̰�
			&& enemyData->IsMyTurn()  // ���� ���� �����̰�
			&& !user->IsMyTurn()     // ������ ���� �ƴϴ�.
			)
		{
			printf("�� ��� ��ų ���� �̱�����");
			sendoutBuffer = new char[packetHeaderSize + SkillTypeSize +enemyData->GetAttackSize()];
			createPacketFrame(packetHeaderSize + SkillTypeSize + enemyData->GetAttackSize(), PK_ENEMY_SKILLATTACK, playerID, sendoutBuffer);

			// �Ѵ� ���� ���°� �ƴ� ���·� ������.
			enemyData->SetAttacking(false);
			user->SetAttacking(false);

			// �츮 �÷��̾ ������ ��ٸ��� ���°� �ƴѰɷ� �ٲٰ�
			user->SetAttackWait(false);


			int skillType = enemyData->GetAttackType();
			tempBuffer = (char*)&skillType;
			memcpy(sendoutBuffer + packetHeaderSize, tempBuffer, SkillTypeSize);

			tempBuffer = (char*)enemyData->GetAttkPoints();
			memcpy(sendoutBuffer + SkillTypeSize + packetHeaderSize, tempBuffer, enemyData->GetAttackSize());

			Send(PlayerKey, packetHeaderSize + SkillTypeSize + enemyData->GetAttackSize(), sendoutBuffer);

			delete[] sendoutBuffer;

		}
		// ���� ���� ������ �� ����ħ....
		else if(
			!enemyData->IsAttacking()  // ���� ���� ���� �ƴϰų�
			|| ( !enemyData->IsMyTurn() && user->IsMyTurn() )     // ������ ���ε� ���� ���� ��ٸ��ٰ�?? 
			)
		{ 
			printf("��ٷ�;��\n");
			//  ������ �׳� ������ ��ٷ���....
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

			// ������ ���̴� ��� �뺸
			createPacketFrame(packetHeaderSize + 4, PK_PLAYER_RADERRESULT, enemyData->GetpID(), sendoutBuffer);

			tempBuffer = (char*)&RaderResult;
			memcpy(sendoutBuffer + packetHeaderSize, tempBuffer, DataSize);

			Send(enemyData->GetpKey(), packetHeaderSize + DataSize, sendoutBuffer);

			printf("\n%d�� ���̴� ����� ������ ������� %d \n", RaderResult, DataSize);

			delete[] sendoutBuffer;

			// ���� �����ô�... 
			user->SetTurn(true);   // ���� �� ���̰�
			enemyData->SetTurn(false);   //  �� ���� �ƴ�.

		}
		else
		{
			printf(" �� ������ �ƴѵ� ������??");
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
			// ������ ��ų ��� �뺸
			createPacketFrame(packetHeaderSize + (DataSize), PK_PLAYER_SKILLRESULT, enemyData->GetpID(), sendoutBuffer);

			tempBuffer = (char*)Data;
			memcpy(sendoutBuffer + packetHeaderSize, tempBuffer, DataSize);

			Send(enemyData->GetpKey(), packetHeaderSize + DataSize, sendoutBuffer);

			//printf("\n%d�� ���̴� ����� ������ ������� %d \n", RaderResult, DataSize);

			delete[] sendoutBuffer;

			// ���� �����ô�... 
			user->SetTurn(true);   // ���� �� ���̰�
			enemyData->SetTurn(false);   //  �� ���� �ƴ�.

		}
		else
		{
			printf(" �� ������ �ƴѵ� ������??");
		}
		break;

	}
	return NULL;
}


void cNetworkManager::ErrorHandling(char *message)
{
		fputs(message, stderr);
		fputc('\n', stderr);
		printf("���� Ƚ�� : %d\n");
		exit(1);
}

	



