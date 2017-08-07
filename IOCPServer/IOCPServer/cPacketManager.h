#pragma once
#include "stdafx.h"
#include "Packet.h"


class cPacketManager : public singletonBase<cPacketManager>
{
    



public:
	cPacketManager();
	~cPacketManager();

	void packetProcess(PACKET_KEY key, int ID, char* Data);
	cTCPPacket* packetClassfy(unsigned char* buff, int buffSize);
	cTCPPacket* createPacket(PACKET_KEY key, int ID, char* Data);

};

