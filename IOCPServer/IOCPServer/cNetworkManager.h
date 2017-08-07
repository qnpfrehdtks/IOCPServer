#pragma once
#include "stdafx.h"
#include "Packet.h"


#define CLINET_MAX 1000
#define BUFF_MAX 2000

using Packet = unsigned char;


enum IOCP_MODE {
	IO_RECV,
	IO_SEND
};

// overlap 확장
typedef struct overlapedIOContext
{
	OVERLAPPED ov;
	Packet buffer[BUFF_MAX];
	IOCP_MODE rwmode;
	WSABUF wsaBuf;

} PER_IO_DATA, *LPPER_IO_DATA;

// client 정보 함수.
typedef struct ClintInfo
{

	SOCKET clntSock;
	bool connected;
	int packet_size = 0;
	int previoust_size;
	PER_IO_DATA recv_overlap;
	SOCKADDR_IN clntAdr;
	Packet packet_buff[BUFF_MAX];

	ClintInfo() {}
	ClintInfo(SOCKET Sock, SOCKADDR_IN addr)
		:clntSock(Sock)
	{
		int addrLen = sizeof(addr);
		memcpy(&(this->clntAdr), &addr, addrLen);
	}
} PER_CLNT_DATA, *LPPER_CLNT_DATA;


class cNetworkManager : public singletonBase<cNetworkManager>
{
private:

	SOCKADDR_IN               m_servAdr;   // 서버의 socket 소켓 주소
	SOCKET                    m_ListenSock;   // 서버의 소켓
	HANDLE                    m_hIOCP;  // IOCP의 핸들을 담는다 
	PER_CLNT_DATA             m_clients[1000];  // 클라이언트의 정보를 담을 변수
	UINT                      m_ClientIdx;   // 클라이언트 수 , 아마 최대 3~4명 될듯

public:
	cNetworkManager();
	~cNetworkManager();

	BOOL init(int PORT);
	BOOL AcceptProcess();


	// IOCP 를 생성하여 IOCP 커널 객체에게 넘겨줌.
	BOOL CreateIOCP();
	BOOL CreateThreadPool();
	BOOL ConnectToIOCP(HANDLE hDevice, DWORD CompletionKey);

	void CloseSocket(int IDX);
	PER_CLNT_DATA GetCLNT(int IDX)        { return m_clients[IDX]; }

	UINT GetClntIDX()                     { return m_ClientIdx; }
	//unsigned int __stdcall CompletionThread(LPVOID pComPort);

	
	//BOOL Send(LPPER_IO_DATA PerIoData, LPPER_HANDLE_DATA PerHandleData,char* ol);
//	BOOL Recv(LPPER_IO_DATA PerIoData, DWORD DataByte);

	void ErrorHandling(char *message);
	
	
};


