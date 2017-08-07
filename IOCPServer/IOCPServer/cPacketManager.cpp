#include "stdafx.h"
#include "Packet.h"

cPacketManager::cPacketManager()
{
}


cPacketManager::~cPacketManager()
{
}

void cPacketManager::packetProcess(PACKET_KEY key, int ID, char * Data)
{

	


}

cTCPPacket* cPacketManager::packetClassfy(unsigned char* buf, int buffSize)
{
	char tempBuf[3000];
	memcpy(tempBuf, buf, buffSize);

	PACKET_KEY packetState;
	int PlayerId = 0;
	int buffCurPos = 0;

	buffCurPos += 4;
	memcpy(&packetState, tempBuf + buffCurPos, sizeof(PACKET_KEY));
	buffCurPos += 4;
	memcpy(&PlayerId, tempBuf + buffCurPos, sizeof(int));
	buffCurPos += 4;

	packetProcess(packetState, PlayerId, tempBuf + buffCurPos);

	//cTCPPacket* tcppacket = NULL;

	//switch (packet.GetKey())
	//{
	//case PK_LOGIN_REQUEST :
	//	if (DATABASEMANAGER->QueryUpdate("select * from user"))
	//	{
	//		int feildNum = DATABASEMANAGER->FieldCount();
	//		vector<char*> uservec = DATABASEMANAGER->GetDBData();
	//		int vecSize = uservec.size() / feildNum;

	//		for (int i = 0; i < vecSize; i++) {
	//			USERMANAGER->addPlayerData(
	//				atoi(uservec[i * feildNum]),
	//				uservec[1 + (i * feildNum)],
	//				atoi(uservec[2 + (i * feildNum)]));
	//		}

	//		tcppacket = createPacket(PK_LOGIN_OK, 0, sizeof(packet), " " );

	//	//	USERMANAGER->getPlayer(packet.GetData())
	//	}
	//	else
	//	{

	//	}
	//	break;
	//case PK_PLAYERINFO:
	//	if (DATABASEMANAGER->QueryUpdate("select * from user"))
	//	{
	//		int feildNum = DATABASEMANAGER->FieldCount();
	//		vector<char*> uservec = DATABASEMANAGER->GetDBData();
	//		int vecSize = uservec.size() / feildNum;

	//		for (int i = 0; i < vecSize; i++) {
	//			USERMANAGER->addPlayerData(
	//				atoi(uservec[i * feildNum]),
	//				uservec[1 + (i * feildNum)],
	//				atoi(uservec[2 + (i * feildNum)]));
	//		}

	//		tcppacket = createPacket(PK_LOGIN_OK, 0, sizeof(packet), " ");

	//		//	USERMANAGER->getPlayer(packet.GetData())
	//	}
	//	else
	//	{

	//	}
	//	break;





	//}

	return NULL;
}

void PacketFactory()
{

}




cTCPPacket* cPacketManager::createPacket(PACKET_KEY key, int ID, char* Data)
{
	cTCPPacket* packet = new cTCPPacket;
	packet->GetPacketHeader().pkKey = key;

	/*packet->GetPacketHeader().pkClassID = ID;
	packet->GetPacketHeader().pkPlayerID = 143;
	packet->GetPacketHeader().pkSize = sizeof(cTCPPacket);*/
//	packet->setData(Data, 0);

	return packet;
}
