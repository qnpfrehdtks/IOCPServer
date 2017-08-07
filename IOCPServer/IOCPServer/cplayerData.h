#pragma once
#include "GameObject.h"

struct Point
{
	int X;
	int Y;

};


class cplayerData : public GameObject
{
private:
	

	
	int      m_pID;
	BOOL     m_isWaiting;
	BOOL     m_isFirstClass;
	int      m_Rating;

	int      m_pKey;
	BOOL     m_isMatched;
	BOOL     m_isGameReady;
	int      m_Order;
	BOOL    m_isMyTurn;

	// 공격을 기다리는 중인가...
	BOOL   m_isAttackWaiting;
	//현재 플레이어가 공격중인가
	BOOL   m_isAttacking;
	//공격 포인트는?
	sPoint  m_AttackPt;

	//공격 포인트는?
	sPoint  m_AttackPts[25];

	SKILL m_AttackType;
	int m_AttackSize;
	cplayerData* m_EnemyPlayer;

public:
	cplayerData(INT pID, INT Key) :
		m_pID(pID), m_isWaiting(false), m_isMatched(false), m_Rating(0), m_pKey(Key), m_isGameReady(false), m_Order(0), m_isFirstClass(false), m_isMyTurn(false), m_isAttackWaiting(false), m_AttackSize(0)
	{
		
	}

	~cplayerData();

	BOOL IsWaiting()                             { return m_isWaiting; }
	BOOL IsMatched()                             { return m_isMatched; }
	BOOL IsReady()                               { return m_isGameReady; }
	BOOL IsEnemyReady()                          { return m_EnemyPlayer->IsReady(); }
	BOOL IsEnemyAttackWait()                     { return m_EnemyPlayer->IsAttackedWaiting(); }
	BOOL IsMyTurn()                              { return m_isMyTurn;  }
	BOOL IsAttackedWaiting()                     { return m_isAttackWaiting; }
	BOOL IsAttacking()                           { return m_isAttacking; }

	INT GetpID()                                 { return m_pID; }
	int GetpKey()                                { return m_pKey; }
	int GetRating()                              { return m_Rating; }
	int GetOrder()                               { return m_Order;  }
	sPoint GetAttkPoint()                        { return m_AttackPt; }
	sPoint* GetAttkPoints()                      { return m_AttackPts; }
	SKILL GetAttackType()                        { return m_AttackType; }
	int        GetAttackSize()                   { return m_AttackSize; }


	cplayerData* GetMatchingPlayer()             { return m_EnemyPlayer; }
	int GetMatchingPlayerID()                    { return m_EnemyPlayer->GetpID(); }

	

	void SetWaiting(bool TF)                     { m_isWaiting = TF; }
	void SetMatched(bool TF)                     { m_isMatched = TF; }
	void SetReady(bool TF)                       { m_isGameReady = TF; }
	void SetTurn (bool TF)                       { m_isMyTurn = TF; }
	void SetAttackWait(bool TF)                  { m_isAttackWaiting = TF; }
	void SetAttacking(bool TF)                   { m_isAttacking = TF; }
	void SetAttackPt(int X, int Y, SKILL key)
	{ m_AttackPt.X = X, m_AttackPt.Y = Y;  m_AttackType = key;}
	void SetAttacksPt(sPoint pt[], int Size, SKILL key)
	{
		memcpy(m_AttackPts, pt, Size); m_AttackType = key; m_AttackSize = Size;
	}

	void ResetAttackPts() { m_AttackSize = 0; }
	void PlusOrder()                             { m_Order++; }

	int checkOrder();

	void MatchPlayer(cplayerData* enemyPlayer)   { m_EnemyPlayer = enemyPlayer; }

	BOOL CheckMatchingPlayer(cplayerData* player);

};

