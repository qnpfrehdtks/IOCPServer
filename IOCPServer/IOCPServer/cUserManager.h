#pragma once
#include "cplayerData.h"
#include "stdafx.h"

using PlayerRating_pair = pair<int, cplayerData*>;
auto my_comp = [](const PlayerRating_pair& e1, const PlayerRating_pair& e2) { return e1.first > e2.first; };

class cUserManager : public singletonBase<cUserManager>
{
	// �÷��̾ ��� ���� ���̺�
	unordered_map <int, cplayerData*> m_mPlayer;

	// �÷��̾� ����ϱ� ���� Q�� ����� ������ �Ⱦ���..
	vector<cplayerData*> m_WaitPlayerQ;

	// ó���� �ٺ� ���� map���� �ϰ� �� ���� �������� �ȵ���...? �����... 
	// multimap���� ����...
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

