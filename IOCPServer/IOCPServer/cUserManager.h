#pragma once
#include "cplayerData.h"
#include "stdafx.h"

using PlayerRating_pair = pair<int, cplayerData*>;
auto my_comp = [](const PlayerRating_pair& e1, const PlayerRating_pair& e2) { return e1.first > e2.first; };

class cUserManager : public singletonBase<cUserManager>
{
	// 플레이어를 담디 위한 테이블
	unordered_map <int, cplayerData*> m_mPlayer;

	// 플레이어 대기하기 위한 Q로 만들긴 했지만 안쓸듯..
	vector<cplayerData*> m_WaitPlayerQ;

	// 처음에 바보 같이 map으로 하고서 왜 같은 레이팅이 안들어가지...? 고민함... 
	// multimap으로 수정...
	multimap<int, cplayerData*> m_WaitingPlayerMap;
	multimap<int, cplayerData*>::iterator miter;
//	priority_queue<PlayerRating_pair, vector<PlayerRating_pair>, decltype(my_comp)> m_heapWaitingQ;

public:
	cUserManager();
	~cUserManager();

	int addPlayerData(INT pid, INT Key);
	BOOL insertToWaitingRoom(cplayerData* player);

	BOOL matchingPlayer(cplayerData* player);
	BOOL matchingPlayer(int playerID);
	cplayerData* matchPlayer(cplayerData* Searchingplayer, cplayerData* Searchedplayer);


	cplayerData* getPlayer(INT pid);
	cplayerData* getAllPlayer();

};

