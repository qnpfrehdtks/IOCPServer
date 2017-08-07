#include "stdafx.h"
#include "cplayerData.h"



cplayerData::~cplayerData()
{
}

int cplayerData::checkOrder()
{
	if (m_EnemyPlayer->GetOrder() < GetOrder())
	{
		return m_pID;
	}
	else if(m_EnemyPlayer->GetOrder() > GetOrder())
	{
		return m_EnemyPlayer->m_pID;
	}
	else
	{
		if (m_EnemyPlayer->GetRating() < m_Rating)
		{
			return m_pID;
		}
		else if(m_EnemyPlayer->GetRating() > m_Rating)
		{
			return m_EnemyPlayer->GetpID();
		}
	}
}

BOOL cplayerData::CheckMatchingPlayer(cplayerData * player)
{
	if (m_EnemyPlayer->GetMatchingPlayerID() == player->GetMatchingPlayerID())
	{
		printf("%d Yes My Enemey and your %d Enemy Same Same", m_EnemyPlayer->GetMatchingPlayerID(),
			player->GetMatchingPlayerID());
		return TRUE;
	}
	else
	{
		return FALSE;
	}



	return 0;
}
