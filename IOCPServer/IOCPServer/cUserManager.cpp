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

	// �ߺ��� ���� ��� �Ҵ��ϰ� unorederMap�� �־�����.
	if (m_mPlayer.end() == m_mPlayer.find(pid))
	{
		player = new cplayerData(pid, Key);
		m_mPlayer.insert(pair<INT, cplayerData*>(pid, player));
		printf("\n ������ �� %d ���� �÷��̾ ��ϵ�....\n", m_mPlayer.size());
		printf("\nno %d Player Data insert to Unorder Map Success\n", pid);
	}
	else
	{
		player = m_mPlayer[pid];
	    printf("No %d Player Data is already inserted But Still Go...\n", pid);
	}

	// �÷��̾ �����ϴ� Waiting Q �� �־�����.
	if (player)
	{
		if (player->IsMatched())
		{
			enemyID = player->GetMatchingPlayerID();
			printf("ALREADY no. %d VS %d Number Player  : Matching Complete...\n", pid, enemyID);
		}
		else
		{
			// ���� ���� �´� �÷��̾ ���� ��� FALSE�� ������,,,,
			if ((enemyID = matchingPlayer(player->GetpID())) <= -1) {
				player->PlusOrder();
				printf("Matching Player...\n");
			}
			else  // ���� �´� �÷��̾ �����ϴ� ����. */ 
			{
				printf("no. %d VS %d Number Player  : Matching Complete...\n", pid, enemyID);
			}
		}

		// �̷� ��찡 ��������... �ƹ�ư FALSE..
		return enemyID;
	}
	else
	{
		// �÷��̾ ����� �����Ҵ� ���� �ʴ´ٸ�... �� ����
		printf("\nCant insert a Player bcause a Player == NULL");
	}

	return -1;
}



int cUserManager::matchingPlayer(int playerID)
{

	//unordered_map <int, cplayerData*>::iterator iter = m_mPlayer[playerID];

	// �÷��̾ ��ϵǾ� �ִ��� Ȯ��.
	if (m_mPlayer[playerID])
	{ // �÷��̾ ��� �Ǿ����� ��Ī�Ϸ� ����.
		//return matchingPlayer((*iter).second);

	//	printf("%d %d ����!", playerID, m_mPlayer[playerID]->GetpID());
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

	// �ش� �÷��̾ ������� �ƴϸ�
	if (!player->IsWaiting())
	{
		printf("No. %d Player Go to a Waiting State\n", PlayerID);
		player->SetWaiting(true);
		//m_WaitingPlayerMap.insert(make_pair(player->GetRating(), player));
	}

		for (unordered_map<int, cplayerData*>::iterator iter = m_mPlayer.begin();
		iter != m_mPlayer.end();  iter++)
		{
			// ������� �ƴ� �༮�̶� ��Ī�Ǹ� �ȵǴϱ�
			if (!(*iter).second->IsWaiting() || (*iter).second->IsMatched()) continue;

			// �ڱ� �ڽ��̶� ��Ī �Ǵ°� ���� ����
			if (PlayerID == (*iter).second->GetpID()) continue;

			// ������ ���� �ڱ� �ڽ� ������  +- 50
			if (PlayerRating <= (*iter).second->GetRating() + MAX_RATING && PlayerRating >= (*iter).second->GetRating() - MIN_RATING)
			{

				cplayerData* SelectedPlayer = (*iter).second;

				printf(" Waiting ���... ��Ī ����  %d�� %d�� �ٴ±���.\n", PlayerID, (*iter).second->GetpID());

				// �÷��̾�� ��Ī�� ���� ��� ���¿��� Ǯ���ְ�
				player->SetWaiting(false);
				SelectedPlayer->SetWaiting(false);

				// ��Ī ����.
				player->SetMatched(true);
				SelectedPlayer->SetMatched(true);

				// ��Ī ��븦 �־���.
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
	    printf("%d Player Eixt :D ���� �÷��̾� �� : %d\n", PlayerID, m_mPlayer.size());

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






