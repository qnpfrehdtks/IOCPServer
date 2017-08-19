#include "stdafx.h"
#include "cUserManager.h"


cUserManager::cUserManager()
{


}


cUserManager::~cUserManager()
{
	map<INT, cplayerData*>::iterator iter;


	m_mPlayer.clear();
}

int cUserManager::addPlayerData(INT pid , INT Key)
{

//	pair<unordered_map<INT, cplayerData*>::iterator, bool> miter;
	cplayerData* player = NULL;
	int enemyID = 0;

	// 중복이 없는 경우 할당하고 unorederMap에 넣어주자.
	if (m_mPlayer.end() == m_mPlayer.find(pid))
	{
		player = new cplayerData(pid, Key);
		m_mPlayer.insert(pair<INT, cplayerData*>(pid, player));
		printf("\n 서버에 총 %d 명이 플레이어에 등록됨....\n", m_mPlayer.size());
		printf("\nno %d Player Data insert to Unorder Map Success\n", pid);
	}
	else
	{
		player = m_mPlayer[pid];
	    printf("No %d Player Data is already inserted But Still Go...\n", pid);
	}

	// 플레이어가 존재하니 Waiting Q 에 넣어주자.
	if (player)
	{
		if (player->IsMatched())
		{
			enemyID = player->GetMatchingPlayerID();
			printf("ALREADY no. %d VS %d Number Player  : Matching Complete...\n", pid, enemyID);
		}
		else
		{
			// 만약 나랑 맞는 플레이어가 없을 경우 FALSE를 내뱉자,,,,
			if ((enemyID = matchingPlayer(player->GetpID())) <= -1) {
				player->PlusOrder();
				printf("Matching Player...\n");
			}
			else  // 나랑 맞는 플레이어가 존재하니 뱉어내자. */ 
			{
				printf("no. %d VS %d Number Player  : Matching Complete...\n", pid, enemyID);
			}
		}

		// 이런 경우가 있으려나... 아무튼 FALSE..
		return enemyID;
	}
	else
	{
		// 플레이어가 제대로 동적할당 되지 않는다면... 걍 종료
		printf("\nCant insert a Player bcause a Player == NULL");
	}

	return -1;
}



int cUserManager::matchingPlayer(int playerID)
{

	//unordered_map <int, cplayerData*>::iterator iter = m_mPlayer[playerID];

	// 플레이어에 등록되어 있는지 확인.
	if (m_mPlayer[playerID])
	{ // 플레이어에 등록 되었으니 매칭하러 가자.
		//return matchingPlayer((*iter).second);

	//	printf("%d %d 들어간다!", playerID, m_mPlayer[playerID]->GetpID());
		return matchingPlayer(m_mPlayer[playerID]);
	}
	else return -1;
}


int cUserManager::matchingPlayer(cplayerData* player)
{
	int PlayerID = player->GetpID();
	int PlayerRating = player->GetRating();

	if (player->IsMatched())
		return player->GetMatchingPlayerID();

	// 해당 플레이어가 대기중이 아니면
	if (!player->IsWaiting())
	{
		printf("No. %d Player Go to a Waiting State\n", PlayerID);
		player->SetWaiting(true);
		//m_WaitingPlayerMap.insert(make_pair(player->GetRating(), player));
	}

		for (unordered_map<int, cplayerData*>::iterator iter = m_mPlayer.begin();
		iter != m_mPlayer.end();  iter++)
		{
			// 대기중이 아닌 녀석이랑 매칭되면 안되니깐
			if (!(*iter).second->IsWaiting() || (*iter).second->IsMatched()) continue;

			// 자기 자신이랑 매칭 되는걸 막기 위해
			if (PlayerID == (*iter).second->GetpID()) continue;

			// 레이팅 차이 자기 자신 레이팅  +- 50
			if (PlayerRating <= (*iter).second->GetRating() + MAX_RATING && PlayerRating >= (*iter).second->GetRating() - MIN_RATING)
			{

				cplayerData* SelectedPlayer = (*iter).second;

				printf(" Waiting 대기... 매칭 ㅊㅋ  %d와 %d와 붙는군요.\n", PlayerID, (*iter).second->GetpID());

				// 플레이어와 매칭된 적이 대기 상태에서 풀어주고
				player->SetWaiting(false);
				SelectedPlayer->SetWaiting(false);

				// 매칭 됬음.
				player->SetMatched(true);
				SelectedPlayer->SetMatched(true);

				// 매칭 상대를 넣어줌.
				player->MatchPlayer(SelectedPlayer);
				SelectedPlayer->MatchPlayer(player);

				return SelectedPlayer->GetpID();
			}
		}
		return -1;
	
}


cplayerData * cUserManager::GetPlayerFromKey(int PlayerKey)
{
	for (unordered_map<int, cplayerData*>::iterator iter = m_mPlayer.begin();
	iter != m_mPlayer.end();  iter++)
	{
		if ((*iter).second->GetpKey() == PlayerKey)
		{
			return (*iter).second;
		}

	}
}

void cUserManager::RemovePlayerFromKey(int Key)
{
	for (unordered_map<int, cplayerData*>::iterator iter = m_mPlayer.begin();
	iter != m_mPlayer.end();  iter++)
	{
		if ((*iter).second->GetpKey() == Key)
		{
			RemovePlayer((*iter).second->GetpID());
			return;
		}

	}

}

void cUserManager::RemovePlayer(int PlayerID)
{
	   delete getPlayer(PlayerID);
	   m_mPlayer.erase(PlayerID);
	    printf("%d Player Eixt :D 남은 플레이어 수 : %d\n", PlayerID, m_mPlayer.size());

}


//cplayerData * cUserManager::matchPlayer(cplayerData * Searchingplayer, cplayerData * Searchedplayer)
//{
//	
//	return NULL;
//
//}

cplayerData* cUserManager::getPlayer(INT pid)
{
	cplayerData* objectedPlayer = m_mPlayer[pid];

	return objectedPlayer;
	
}

cplayerData* cUserManager::getAllPlayer()
{
	return NULL;
}






