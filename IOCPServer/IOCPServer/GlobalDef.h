#pragma once
#include "stdafx.h"


enum SKILL
{
	SK_BASE,
	SK_REPAIR,
	SK_RADER,
	SK_SUPER_BOMB

};


enum PACKET_KEY
{
	PK_PLAYER_ENTER = 333,
	PK_ENEMY_ENTER,   // 플레이어가 입장하려고 패킷을 보낸 경우
	PK_PLAYER_WAIT,        // 플레이어 보고 기다려라 라고 패킷을 보낼 경우 // 플레이어가 드디더 상대방과 매칭을 마폈을 경우 보내는 경우 ACK임.
	PK_PLAYER_READY,    // 배치 완료.
	PK_ENEMY_READY,
	PK_BATTLE_START,
	PK_MY_TURN,
	PK_OPPO_TURN,

	PK_ENEMY_ATTACKWAIT,
	PK_ENEMY_ATTACKNOWAIT,

	PK_PLAYER_ATTACK,
	PK_PLAYER_ATTACKWAIT,
	PK_PLAYER_DAMAGESUCC,
	PK_PLAYER_DAMAGEFAIL,
	PK_PLAYER_MISSILEDEFNED,



	PK_PLAYER_RADER,
	PK_PLAYER_RADERWAIT,
	PK_PLAYER_RADERRESULT,
	PK_PLAYER_SKILLATTACK,
	PK_PLAYER_SKILLRESULT,

	PK_ENEMY_SKILLATTACK,
	PK_ENEMY_ATTACK,
	PK_ENEMY_RADER,
	PK_ENEMY_ATTACKSUCC,
	PK_ENEMY_ATTACKFAIL,

	PK_TURN_CHECK,
	PK_TURN_CHANGE,
	PK_TURN_NOCHANGE,

	PK_PLAYER_DIE,
	PK_ENEMY_DIE,

	PK_PLAYER_EXIT,
	PK_ENEMY_EXIT
};


const INT iIP_HEAD_SIZE = 24;
const INT iTCP_HEAD_SIZE = 24;

const INT iMTU_SIZE = 1500 - (iIP_HEAD_SIZE + iTCP_HEAD_SIZE);

const INT iMAX_PACKET_SIZE = iMTU_SIZE;

const INT iDEFAULT_SO_SNDBUF_SIZE = 256 * 1024;
const INT iDEFAULT_SO_RCVBUF_SIZE = 256 * 1024;

const INT iTCP_MAX_SYN_BACKLOG = 1280;

const INT iMAX_CONNECTION = 1048;

const INT iLOG_BUFFER_LEN = 2048;

const INT iFILE_NAME_LEN = 255;
const INT iFILE_PATH_LEN = 1023;

const INT iPLAYER_NAME_LEN = 32;
const INT iPLAYER_MAX_LIFE = 100;
const INT iPLAYER_HIT_POWER = 10;
const INT iPLAYER_STEAMPACK_POWER = 10;
const INT iPLAYER_CHAT_LEN = 1024;
const INT iPLAYER_CHAT_LEN_LINE = 100;

const INT iTEXT_LEN = 255;

const INT iMAX_ROOM_MEMBERS = 16;
const INT iROOM_NAME_LEN = 64;

const INT iSECTION_DISTANCE = 10;
const INT iGAME_MAP_WIDTH = 600;
const INT iGAME_MAP_HEIGHT = 600;

const INT iLOGIN_ID_LEN = 32;
const INT iLOGIN_PWD_LEN = 32;



