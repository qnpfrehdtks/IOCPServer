#pragma once
#include "GameObject.h"


class LinkingContext
{
private:

	uint32_t mNetworkID;
	unordered_map<uint32_t, GameObject* > mmIDtoGameObject;
	unordered_map<GameObject*, uint32_t> mmGotoID;


public:
	LinkingContext();
	~LinkingContext();

	void AddGameObject(GameObject* object, uint32_t networkID)
	{
		mmIDtoGameObject[networkID] = object;
		mmGotoID[object] = networkID;
	}

	uint32_t GetNetworkId(GameObject* go, bool isFoundifNotFound)
	{
		auto it = mmGotoID.find(go);

		//Ã£¾Ò´Ù!!!
		if (it != mmGotoID.end())
		{
			return it->second;
		}
	   else if (isFoundifNotFound)
		{
			uint32_t newNetworkId = mNetworkID++;
			AddGameObject(go, newNetworkId);
			return newNetworkId;

		}
			else return 0;
	}







};

