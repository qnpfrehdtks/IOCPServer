#pragma once
#include "stdafx.h"

struct STcpPacketHeader {
	STcpPacketHeader() { memset(this, 0, sizeof(STcpPacketHeader)); }

	UINT32	key;			// 32bit �۽��� ������ key

	UINT32	size : 11;		// ����� ����� ������ ��Ŷ ��ü ũ�� (min: 8, max: 2047)
	UINT32	padding : 21;
};

struct SCommandHeader {
	SCommandHeader() { memset(this, 0, sizeof(SCommandHeader)); }

	UINT16	order : 10;		// ��� (max: 1023)
	UINT16	mission : 6;		// �߰� ���� (max: 63)
	BYTE	extra;			// ��� ó�� (max: 255)
	BYTE	option;			// �߰� ���� (max: 255)
};