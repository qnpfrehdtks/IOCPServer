#pragma once

#include "stdafx.h"



typedef struct sCoordinate
{
	int X;
	int Y;
} sPoint;


typedef struct sHeader {
	int Size;
	PACKET_KEY pkKey;
	int pkPlayerID;
} sTCPHeader;



class cTCPPacket
{
	

protected:
	sTCPHeader      m_Header;

public:
//	UINT32          GetSize()           { return m_Header.pkSize;  }
	PACKET_KEY      GetKey()            { return m_Header.pkKey; }
	//int             GetID()             { return m_Header.pkPlayerID; }
	//int             GetClassID()        { return m_Header.pkClassID; }
	//char*           GetData()           { return m_acData; }

	sTCPHeader&	    GetPacketHeader()   { return m_Header; }
	char*			GetHeader()         { return (char*)&m_Header; }


	void            SetHeader(sTCPHeader header) { m_Header = header; }
	//void           setData(char data[], UINT size) {  strcpy_s(m_acData, data);  }


	//void setData(void* type) { m_acData = (char*)type; }

	
	//char*			m_acData;
};