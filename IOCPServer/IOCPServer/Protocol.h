#pragma once
#include "stdafx.h"

struct STcpPacketHeader {
	STcpPacketHeader() { memset(this, 0, sizeof(STcpPacketHeader)); }

	UINT32	key;			// 32bit 송신자 구별용 key

	UINT32	size : 11;		// 사용자 헤더를 포함한 패킷 전체 크기 (min: 8, max: 2047)
	UINT32	padding : 21;
};

struct SCommandHeader {
	SCommandHeader() { memset(this, 0, sizeof(SCommandHeader)); }

	UINT16	order : 10;		// 명령 (max: 1023)
	UINT16	mission : 6;		// 추가 정보 (max: 63)
	BYTE	extra;			// 결과 처리 (max: 255)
	BYTE	option;			// 추가 정보 (max: 255)
};